#include "optimisations/intelligent/mate/filter.h"
#include "pystip.h"
#include "stipulation/proxy.h"
#include "stipulation/conditional_pipe.h"
#include "stipulation/branch.h"
#include "optimisations/intelligent/intelligent.h"
#include "optimisations/intelligent/duplicate_avoider.h"
#include "debugging/trace.h"

#include <assert.h>

/* Allocate a STIntelligentMateFilter slice.
 * @param goal_tester_fork fork into the goal goal_tester_fork branch
 * @return allocated slice
 */
slice_index alloc_intelligent_mate_filter(slice_index goal_tester_fork)
{
  slice_index result;

  TraceFunctionEntry(__func__);
  TraceFunctionParam("%u",goal_tester_fork);
  TraceFunctionParamListEnd();

  {
    slice_index const proxy = alloc_proxy_slice();
    link_to_branch(proxy,stip_deep_copy(goal_tester_fork));
    result = alloc_conditional_pipe(STIntelligentMateFilter,proxy);
  }

  TraceFunctionExit(__func__);
  TraceFunctionResult("%u",result);
  TraceFunctionResultEnd();
  return result;
}

/* Impose the starting side on a stipulation.
 * @param si identifies slice
 * @param st address of structure that holds the state of the traversal
 */
void impose_starter_intelligent_mate_filter(slice_index si,
                                            stip_structure_traversal *st)
{
  Side * const starter = st->param;

  TraceFunctionEntry(__func__);
  TraceFunctionParam("%u",si);
  TraceFunctionParam("%u",*starter);
  TraceFunctionParamListEnd();

  slices[si].starter = *starter;
  stip_traverse_structure_children_pipe(si,st);

  /* in duplexes, the colors swapped when looking for the "black solutions".
   * we thus have to make sure that the legal move finder always tests for
   * Black.
   */
  *starter = Black;
  stip_traverse_structure(slices[si].next2,st);
  *starter = slices[si].starter;

  TraceFunctionExit(__func__);
  TraceFunctionResultEnd();
}

static boolean Intelligent(slice_index si, stip_length_type n)
{
  boolean result;

  TraceFunctionEntry(__func__);
  TraceFunctionParam("%u",si);
  TraceFunctionParam("%u",n);
  TraceFunctionParamListEnd();

  solutions_found = false;
  current_start_slice = si;
  intelligent_duplicate_avoider_init();
  IntelligentRegulargoal_types(n);
  intelligent_duplicate_avoider_cleanup();
  current_start_slice = no_slice;
  result = solutions_found;

  TraceFunctionExit(__func__);
  TraceFunctionResult("%u",result);
  TraceFunctionResultEnd();
  return result;
}

/* Try to solve in n half-moves after a defense.
 * @param si slice index
 * @param n maximum number of half moves until end state has to be reached
 * @return length of solution found and written, i.e.:
 *            slack_length-2 defense has turned out to be illegal
 *            <=n length of shortest solution found
 *            n+2 no solution found
 */
stip_length_type intelligent_mate_filter_attack(slice_index si,
                                                stip_length_type n)
{
  stip_length_type result;

  TraceFunctionEntry(__func__);
  TraceFunctionParam("%u",si);
  TraceFunctionParam("%u",n);
  TraceFunctionParamListEnd();

  result = Intelligent(si,n) ? n : n+2;

  TraceFunctionExit(__func__);
  TraceFunctionResult("%u",result);
  TraceFunctionResultEnd();
  return result;
}
