#include "conditions/transmuting_kings/reflective_kings.h"
#include "conditions/transmuting_kings/transmuting_kings.h"
#include "solving/move_generator.h"
#include "solving/observation.h"
#include "solving/find_square_observer_tracking_back_from_target.h"
#include "solving/pipe.h"
#include "stipulation/stipulation.h"
#include "stipulation/proxy.h"
#include "stipulation/branch.h"
#include "debugging/trace.h"
#include "pieces/pieces.h"
#include "position/position.h"

#include "debugging/assert.h"

/* Generate moves for a single piece
 * @param identifies generator slice
 */
void reflective_kings_generate_moves_for_piece(slice_index si)
{
  square const sq_departure = move_generation_stack[current_generation].departure;
  Flags const mask = BIT(trait[nbply])|BIT(Royal);

  TraceFunctionEntry(__func__);
  TraceFunctionParam("%u",si);
  TraceFunctionParamListEnd();

  if (TSTFULLFLAGMASK(being_solved.spec[sq_departure],mask))
  {
    numecoup const base = CURRMOVE_OF_PLY(nbply);
    pipe_move_generation_differnt_walk_delegate(si,King);
    if (generate_moves_of_transmuting_king(si))
      remove_duplicate_moves_of_single_piece(base);
  }
  else
    pipe_move_generation_delegate(si);

  TraceFunctionExit(__func__);
  TraceFunctionResultEnd();
}

/* Validate an observation according to Reflective Kings
 * @param si identifies next slice
 * @return true iff observation is valid
 */
boolean reflective_kings_enforce_observer_walk(slice_index si)
{
  boolean result;
  square const sq_king = being_solved.king_square[trait[nbply]];

  TraceFunctionEntry(__func__);
  TraceFunctionParam("%u",si);
  TraceFunctionParamListEnd();

  if (transmuting_kings_testing_transmutation[advers(trait[nbply])])
    result = validate_observation_recursive(SLICE_NEXT1(si));
  else if (move_generation_stack[CURRMOVE_OF_PLY(nbply)].departure==sq_king)
  {
    if (validate_observation_recursive(SLICE_NEXT1(si)))
      result = true;
    else if (transmuting_kings_is_king_transmuting_as(observing_walk[nbply]))
    {
      piece_walk_type const save_walk = observing_walk[nbply];
      observing_walk[nbply] = get_walk_of_piece_on_square(sq_king);
      result = validate_observation_recursive(SLICE_NEXT1(si));
      observing_walk[nbply] = save_walk;
    }
    else
      result = false;
  }
  else
    result = validate_observation_recursive(SLICE_NEXT1(si));

  TraceFunctionExit(__func__);
  TraceFunctionResult("%u",result);
  TraceFunctionResultEnd();
  return result;
}

/* Inialise the solving machinery with reflective kings
 * @param si identifies root slice of solving machinery
 * @param side for whom
 */
void reflective_kings_initialise_solving(slice_index si, Side side)
{
  TraceFunctionEntry(__func__);
  TraceFunctionParam("%u",si);
   TraceEnumerator(Side,side,"");
  TraceFunctionParamListEnd();

  solving_instrument_move_generation(si,side,STReflectiveKingsMovesForPieceGenerator);

  stip_instrument_observation_validation(si,side,STReflectiveKingsEnforceObserverWalk);
  stip_instrument_check_validation(si,side,STReflectiveKingsEnforceObserverWalk);

  TraceFunctionExit(__func__);
  TraceFunctionResultEnd();
}
