#include "stipulation/boolean/not.h"
#include "pyslice.h"
#include "pypipe.h"
#include "pyproc.h"
#include "pydata.h"
#include "trace.h"

#include <assert.h>

/* Allocate a not slice.
 * @return index of allocated slice
 */
slice_index alloc_not_slice(void)
{
  slice_index result;

  TraceFunctionEntry(__func__);
  TraceFunctionParamListEnd();

  result = alloc_pipe(STNot);

  TraceFunctionExit(__func__);
  TraceFunctionResult("%u",result);
  TraceFunctionResultEnd();
  return result;
}

/* Solve a slice
 * @param si slice index
 * @return whether there is a solution and (to some extent) why not
 */
has_solution_type not_solve(slice_index si)
{
  has_solution_type result;

  TraceFunctionEntry(__func__);
  TraceFunctionParam("%u",si);
  TraceFunctionParamListEnd();

  switch (slice_solve(slices[si].u.pipe.next))
  {
    case opponent_self_check:
      result = opponent_self_check;
      break;

    case has_solution:
      result = has_no_solution;
      break;

    case has_no_solution:
      result = has_solution;
      break;

    default:
      assert(0);
      result = false;
      break;
  }

  TraceFunctionExit(__func__);
  TraceEnumerator(has_solution_type,result,"");
  TraceFunctionResultEnd();
  return result;
}
