#include "conditions/anticirce/circuit_special.h"
#include "pypipe.h"
#include "pydata.h"
#include "trace.h"

#include <assert.h>

/* This module provides slice type STAnticirceCircuitSpecial
 */

/* Allocate a STAnticirceCircuitSpecial slice.
 * @return index of allocated slice
 */
slice_index alloc_anticirce_circuit_special_slice(void)
{
  slice_index result;

  TraceFunctionEntry(__func__);
  TraceFunctionParamListEnd();

  result = alloc_pipe(STAnticirceCircuitSpecial);

  TraceFunctionExit(__func__);
  TraceFunctionResult("%u",result);
  TraceFunctionResultEnd();
  return result;
}

/* Solve a slice
 * @param si slice index
 * @return whether there is a solution and (to some extent) why not
 */
has_solution_type anticirce_circuit_special_solve(slice_index si)
{
  has_solution_type result;
  square const sq_rebirth = sq_rebirth_capturing[nbply];

  TraceFunctionEntry(__func__);
  TraceFunctionParam("%u",si);
  TraceFunctionParamListEnd();

  if (sq_rebirth!=initsquare && GetPositionInDiagram(spec[sq_rebirth])==sq_rebirth)
    result = slice_solve(slices[si].u.pipe.next);
  else
    result = has_no_solution;

  TraceFunctionExit(__func__);
  TraceEnumerator(has_solution_type,result,"");
  TraceFunctionResultEnd();
  return result;
}
