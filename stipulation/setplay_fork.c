#include "stipulation/setplay_fork.h"
#include "pybrafrk.h"
#include "stipulation/branch.h"

#include "trace.h"

#include <assert.h>

/* Allocate a STSetplayFork slice
 * @param set entry branch of set play
 * @return newly allocated slice
 */
slice_index alloc_setplay_fork_slice(slice_index set)
{
  slice_index result;

  TraceFunctionEntry(__func__);
  TraceFunctionParam("%u",set);
  TraceFunctionParamListEnd();

  result = alloc_branch_fork(STSetplayFork,set);

  TraceFunctionExit(__func__);
  TraceFunctionResult("%u",result);
  TraceFunctionResultEnd();
  return result;
}

/* Traverse a subtree
 * @param branch root slice of subtree
 * @param st address of structure defining traversal
 */
void stip_traverse_structure_setplay_fork(slice_index si,
                                          stip_structure_traversal *st)
{
  TraceFunctionEntry(__func__);
  TraceFunctionParam("%u",si);
  TraceFunctionParamListEnd();

  assert(st->level==structure_traversal_level_root);

  stip_traverse_structure_pipe(si,st);

  st->level = structure_traversal_level_setplay;
  stip_traverse_structure_next_branch(si,st);
  st->level = structure_traversal_level_root;

  TraceFunctionExit(__func__);
  TraceFunctionResultEnd();
}

/* Traversal of the moves
 * @param si identifies root of subtree
 * @param st address of structure representing traversal
 */
void stip_traverse_moves_setplay_fork(slice_index si, stip_moves_traversal *st)
{
  TraceFunctionEntry(__func__);
  TraceFunctionParam("%u",si);
  TraceFunctionParamListEnd();

  stip_traverse_moves_pipe(si,st);
  stip_traverse_moves_branch(slices[si].u.fork.fork,st);

  TraceFunctionExit(__func__);
  TraceFunctionResultEnd();
}

/* Solve a slice
 * @param si slice index
 * @return whether there is a solution and (to some extent) why not
 */
has_solution_type setplay_fork_solve(slice_index si)
{
  has_solution_type result;

  TraceFunctionEntry(__func__);
  TraceFunctionParam("%u",si);
  TraceFunctionParamListEnd();

  slice_solve(slices[si].u.fork.fork);
  result = slice_solve(slices[si].u.fork.next);

  TraceFunctionExit(__func__);
  TraceEnumerator(has_solution_type,result,"");
  TraceFunctionResultEnd();
  return result;
}

/* Determine whether a slice has a solution
 * @param si slice index
 * @return whether there is a solution and (to some extent) why not
 */
has_solution_type setplay_fork_has_solution(slice_index si)
{
  has_solution_type result;

  TraceFunctionEntry(__func__);
  TraceFunctionParam("%u",si);
  TraceFunctionParamListEnd();

  result = slice_has_solution(slices[si].u.fork.next);

  TraceFunctionExit(__func__);
  TraceEnumerator(has_solution_type,result,"");
  TraceFunctionResultEnd();
  return result;
}
