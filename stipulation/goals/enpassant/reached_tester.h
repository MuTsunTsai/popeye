#if !defined(STIPULATION_GOAL_ENPASSANT_REACHED_TESTER_H)
#define STIPULATION_GOAL_ENPASSANT_REACHED_TESTER_H

#include "pyslice.h"

/* This module provides functionality dealing with slices that detect
 * whether a enpassant goal has just been reached
 */

/* Allocate a system of slices that tests whether enpassant has been reached
 * @return index of entry slice
 */
slice_index alloc_goal_enpassant_reached_tester_system(void);

/* Solve a slice
 * @param si slice index
 * @return whether there is a solution and (to some extent) why not
 */
has_solution_type goal_enpassant_reached_tester_solve(slice_index si);

#endif
