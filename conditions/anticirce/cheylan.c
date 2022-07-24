#include "conditions/anticirce/cheylan.h"
#include "conditions/anticirce/anticirce.h"
#include "conditions/circe/circe.h"
#include "position/effects/utils.h"
#include "solving/pipe.h"
#include "stipulation/stipulation.h"
#include "stipulation/pipe.h"
#include "stipulation/move.h"
#include "debugging/trace.h"

#include "debugging/assert.h"

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
void anticirce_cheylan_filter_solve(slice_index si)
{
  TraceFunctionEntry(__func__);
  TraceFunctionParam("%u",si);
  TraceFunctionParamListEnd();

  {
    square const sq_rebirth = circe_rebirth_context_stack[circe_rebirth_context_stack_pointer].rebirth_square;

    move_effect_journal_index_type const base = move_effect_journal_base[nbply];
    move_effect_journal_index_type const movement = base+move_effect_journal_index_offset_movement;
    square const sq_arrival = move_effect_journal[movement].u.piece_movement.to;
    PieceIdType const moving_id = GetPieceId(move_effect_journal[movement].u.piece_movement.movingspec);
    square const pos = move_effect_journal_follow_piece_through_other_effects(nbply,
                                                                              moving_id,
                                                                              sq_arrival);

    pipe_this_move_illegal_if(si,pos==sq_rebirth);
  }

  TraceFunctionExit(__func__);
  TraceFunctionResultEnd();
}

/* Instrument a stipulation
 * @param si identifies root slice of stipulation
 */
void anticirce_cheylan_initialise_solving(slice_index si)
{
  TraceFunctionEntry(__func__);
  TraceFunctionParam("%u",si);
  TraceFunctionParamListEnd();

  circe_instrument_solving(si,
                           STAnticirceConsideringRebirth,
                           STCirceDeterminedRebirth,
                           alloc_pipe(STAnticirceCheylanFilter));

  TraceFunctionExit(__func__);
  TraceFunctionResultEnd();
}
