#include "output/plaintext/tree/end_of_solution_writer.h"
#include "pypipe.h"
#include "pydata.h"
#include "pymsg.h"
#include "stipulation/battle_play/defense_play.h"
#include "trace.h"

/* Allocate a STEndOfSolutionWriter slice.
 * @return index of allocated slice
 */
slice_index alloc_end_of_solution_writer_slice(void)
{
  slice_index result;

  TraceFunctionEntry(__func__);
  TraceFunctionParamListEnd();

  result = alloc_pipe(STEndOfSolutionWriter);

  TraceFunctionExit(__func__);
  TraceFunctionResult("%u",result);
  TraceFunctionResultEnd();
  return result;
}

/* Try to defend after an attacking move
 * When invoked with some n, the function assumes that the key doesn't
 * solve in less than n half moves.
 * @param si slice index
 * @param n maximum number of half moves until end state has to be reached
 * @return <slack_length_battle - no legal defense found
 *         <=n solved  - return value is maximum number of moves
 *                       (incl. defense) needed
 *         n+2 refuted - acceptable number of refutations found
 *         n+4 refuted - more refutations found than acceptable
 */
stip_length_type end_of_solution_writer_defend(slice_index si, stip_length_type n)
{
  stip_length_type result;
  slice_index const next = slices[si].u.pipe.next;

  TraceFunctionEntry(__func__);
  TraceFunctionParam("%u",si);
  TraceFunctionParam("%u",n);
  TraceFunctionParamListEnd();

  result = defend(next,n);
  if (slack_length_battle<=result && result<=n+2)
    Message(NewLine);

  TraceFunctionExit(__func__);
  TraceFunctionResult("%u",result);
  TraceFunctionResultEnd();
  return result;
}
