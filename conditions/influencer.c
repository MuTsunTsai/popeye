#include "conditions/influencer.h"
#include "position/effects/walk_change.h"
#include "position/position.h"
#include "pieces/walks/vectors.h"
#include "pieces/walks/pawns/promotion.h"
#include "position/effects/utils.h"
#include "stipulation/move.h"
#include "solving/pipe.h"

#include "debugging/trace.h"
#include "debugging/assert.h"

static void apply_influence(PieceIdType id_influencer, square sq_to)
{
  Side const side_playing = trait[nbply];
  Side const side_influenced = advers(side_playing);
  SquareFlags const sq_prom = side_influenced==White ? WhPromSq : BlPromSq;
  SquareFlags const sq_base = side_influenced==White ? WhBaseSq : BlBaseSq;

  TraceFunctionEntry(__func__);
  TraceFunctionParam("%u",id_influencer);
  TraceSquare(sq_to);
  TraceFunctionParamListEnd();

  {
    square const sq_arrival = move_effect_journal_follow_piece_through_other_effects(nbply,
                                                                                     id_influencer,
                                                                                     sq_to);
    piece_walk_type const walk_influencer = get_walk_of_piece_on_square(sq_arrival);
    vec_index_type k;

    for (k = vec_queen_start; k<=vec_queen_end; ++k)
    {
      square const sq_candidate = sq_arrival+vec[k];

      if (TSTFLAG(being_solved.spec[sq_candidate],side_influenced)
          && !TSTFLAG(being_solved.spec[sq_candidate],Royal))
      {
        boolean const prom_or_base = (TSTFLAG(sq_spec(sq_candidate),sq_prom)
                                      || TSTFLAG(sq_spec(sq_candidate),sq_base));
        piece_walk_type const walk_substitute = (prom_or_base
                                                 ? walk_influencer
                                                 : Pawn);

        move_effect_journal_do_walk_change(move_effect_reason_influencer,
                                           sq_candidate,
                                           walk_substitute);
      }
    }
  }

  TraceFunctionExit(__func__);
  TraceFunctionResultEnd();
}

/* Try to solve in solve_nr_remaining half-moves.
 * @param si slice index
 * @note assigns solve_result the length of solution found and written, i.e.:
 *            previous_move_is_illegal the move just played is illegal
 *            this_move_is_illegal     the move being played is illegal
 *            immobility_on_next_move  the moves just played led to an
 *                                     unintended immobility on the next move
 *            <=n+1 length of shortest solution found (n+1 only if in next
 *                                     branch)
 *            n+2 no solution found in this branch
 *            n+3 no solution found in next branch
 *            (with n denominating solve_nr_remaining)
 */
void influencer_walk_changer_solve(slice_index si)
{
  move_effect_journal_index_type const base = move_effect_journal_base[nbply];
  move_effect_journal_index_type const top = move_effect_journal_base[nbply+1];
  move_effect_journal_index_type const movement = base+move_effect_journal_index_offset_movement;
  move_effect_journal_index_type curr;

  TraceFunctionEntry(__func__);
  TraceFunctionParam("%u",si);
  TraceFunctionParamListEnd();

  for (curr = movement; curr!=top; ++curr)
    switch (move_effect_journal[curr].type)
    {
      case move_effect_piece_movement:
        if (!TSTFLAG(move_effect_journal[curr].u.piece_movement.movingspec,Royal))
          apply_influence(GetPieceId(move_effect_journal[curr].u.piece_movement.movingspec),
                          move_effect_journal[curr].u.piece_movement.to);
        break;

      case move_effect_piece_exchange:
        if (!TSTFLAG(move_effect_journal[curr].u.piece_exchange.fromflags,Royal))
          apply_influence(GetPieceId(move_effect_journal[curr].u.piece_exchange.fromflags),
                          move_effect_journal[curr].u.piece_exchange.to);

        if (!TSTFLAG(move_effect_journal[curr].u.piece_exchange.toflags,Royal))
          apply_influence(GetPieceId(move_effect_journal[curr].u.piece_exchange.toflags),
                          move_effect_journal[curr].u.piece_exchange.from);
        break;

      default:
        break;
    }

  pipe_solve_delegate(si);

  TraceFunctionExit(__func__);
  TraceFunctionResultEnd();
}

/* Instrument a stipulation
 * @param si identifies root slice of stipulation
 */
void solving_insert_influencer(slice_index si)
{
  TraceFunctionEntry(__func__);
  TraceFunctionParam("%u",si);
  TraceFunctionParamListEnd();

  stip_instrument_moves(si,STInfluencerWalkChanger);

  promotion_insert_slice_sequence(si,STInfluencerWalkChanger,&move_insert_slices);

  TraceFunctionExit(__func__);
  TraceFunctionResultEnd();
}
