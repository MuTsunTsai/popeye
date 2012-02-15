#if !defined(STIPULATION_BOOLEAN_OR_H)
#define STIPULATION_BOOLEAN_OR_H

#include "pyslice.h"

/* This module provides functionality dealing logical OR stipulation slices.
 */

/* Allocate a STOr slice.
 * @param op1 proxy to 1st operand
 * @param op2 proxy to 2nd operand
 * @return index of allocated slice
 */
slice_index alloc_or_slice(slice_index op1, slice_index op2);

/* Solve a slice
 * @param si slice index
 * @return whether there is a solution and (to some extent) why not
 */
has_solution_type or_solve(slice_index si);

/* Try to solve in n half-moves after a defense.
 * @param si slice index
 * @param n maximum number of half moves until goal
 * @return length of solution found and written, i.e.:
 *            slack_length_battle-2 defense has turned out to be illegal
 *            <=n length of shortest solution found
 *            n+2 no solution found
 */
stip_length_type or_attack(slice_index si, stip_length_type n);

#endif
