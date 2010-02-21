#include "pyslice.h"
#include "pydata.h"
#include "trace.h"
#include "stipulation/battle_play/attack_play.h"
#include "stipulation/help_play/root.h"
#include "stipulation/help_play/play.h"
#include "stipulation/series_play/play.h"
#include "pyleaf.h"
#include "pyleafd.h"
#include "pyleaff.h"
#include "pyleafh.h"
#include "stipulation/battle_play/defense_move.h"
#include "pybrafrk.h"
#include "pyquodli.h"
#include "pyrecipr.h"
#include "pynot.h"
#include "pymovein.h"
#include "pyhash.h"
#include "pyreflxg.h"
#include "pydirctg.h"
#include "pyselfgd.h"
#include "pyselfcg.h"
#include "pykeepmt.h"
#include "pypipe.h"
#include "optimisations/maxsolutions/root_solvable_filter.h"
#include "optimisations/stoponshortsolutions/root_solvable_filter.h"

#include <assert.h>
#include <stdlib.h>


#define ENUMERATION_TYPENAME has_solution_type
#define ENUMERATORS                             \
  ENUMERATOR(defender_self_check),              \
    ENUMERATOR(has_solution),                   \
    ENUMERATOR(has_no_solution)

#define ENUMERATION_MAKESTRINGS

#include "pyenum.h"


/* Determine and write threats of a slice
 * @param threats table where to store threats
 * @param si index of branch slice
 */
void slice_solve_threats(table threats, slice_index si)
{
  TraceFunctionEntry(__func__);
  TraceFunctionParam("%u",si);
  TraceFunctionParamListEnd();

  TraceEnumerator(SliceType,slices[si].type,"\n");
  switch (slices[si].type)
  {
    case STAttackHashed:
    case STLeafDirect:
      attack_solve_threats(threats,si);
      break;

    case STHelpMove:
    case STHelpHashed:
      help_solve_threats(threats,si);
      break;

    case STSeriesMove:
    case STSeriesHashed:
      series_solve_threats(threats,si);
      break;
    
    case STQuodlibet:
      quodlibet_solve_threats(threats,si);
      break;

    case STReciprocal:
      reci_solve_threats(threats,si);
      break;

    case STNot:
      not_solve_threats(threats,si);
      break;

    default:
      assert(0);
      break;
  }

  TraceFunctionExit(__func__);
  TraceFunctionResultEnd();
}

/* Determine whether the defense just played defends against the threats.
 * @param threats table containing the threats
 * @param si slice index
 * @return true iff the defense defends against at least one of the
 *         threats
 */
boolean slice_are_threats_refuted(table threats, slice_index si)
{
  boolean result = false;

  TraceFunctionEntry(__func__);
  TraceFunctionParam("%u",table_length(threats));
  TraceFunctionParam("%u",si);
  TraceFunctionParamListEnd();

  TraceEnumerator(SliceType,slices[si].type,"\n");
  switch (slices[si].type)
  {
    case STAttackHashed:
    case STLeafDirect:
      result = attack_are_threats_refuted(threats,si);
      break;

    case STHelpMove:
    case STHelpHashed:
      result = help_are_threats_refuted(threats,si);
      break;

    case STSeriesMove:
    case STSeriesHashed:
      result = series_are_threats_refuted(threats,si);
      break;

    case STQuodlibet:
      result = quodlibet_are_threats_refuted(threats,si);
      break;

    case STReciprocal:
      result = reci_are_threats_refuted(threats,si);
      break;

    case STNot:
      result = true;
      break;

    default:
      assert(0);
      break;
  }

  TraceFunctionExit(__func__);
  TraceFunctionResult("%u",result);
  TraceFunctionResultEnd();
  return result;
}

/* Solve a slice
 * @param si slice index
 * @return true iff >=1 solution was found
 */
boolean slice_solve(slice_index si)
{
  boolean solution_found = false;

  TraceFunctionEntry(__func__);
  TraceFunctionParam("%u",si);
  TraceFunctionParamListEnd();

  TraceEnumerator(SliceType,slices[si].type,"\n");
  switch (slices[si].type)
  {
    case STLeafForced:
      solution_found = leaf_forced_solve(si);
      break;

    case STLeafHelp:
      solution_found = leaf_h_solve(si);
      break;

    case STQuodlibet:
      solution_found = quodlibet_solve(si);
      break;

    case STAttackMove:
    case STAttackHashed:
    case STDirectDefense:
    case STSelfDefense:
    case STReflexAttackerFilter:
    case STDegenerateTree:
    case STLeafDirect:
      solution_found = attack_solve(si);
      break;

    case STHelpMove:
    case STHelpHashed:
    case STStopOnShortSolutionsHelpFilter:
      solution_found = help_solve(si);
      break;

    case STSeriesMove:
    case STSeriesFork:
    case STSeriesHashed:
    case STStopOnShortSolutionsSeriesFilter:
      solution_found = series_solve(si);
      break;

    case STReciprocal:
      solution_found = reci_solve(si);
      break;

    case STNot:
      solution_found = not_solve(si);
      break;

    case STMoveInverterSolvableFilter:
      solution_found = move_inverter_solve(si);
      break;

    case STSelfCheckGuardSolvableFilter:
      solution_found = selfcheck_guard_solve(si);
      break;

    default:
      assert(0);
      break;
  }

  TraceFunctionExit(__func__);
  TraceFunctionResult("%u",solution_found);
  TraceFunctionResultEnd();
  return solution_found;
}

/* Solve a slice at root level
 * @param si slice index
 * @return true iff >=1 solution was found
 */
boolean slice_root_solve(slice_index si)
{
  boolean result;

  TraceFunctionEntry(__func__);
  TraceFunctionParam("%u",si);
  TraceFunctionParamListEnd();

  TraceEnumerator(SliceType,slices[si].type,"\n");
  switch (slices[si].type)
  {
    case STLeafForced:
      result = leaf_forced_root_solve(si);
      break;

    case STQuodlibet:
      result = quodlibet_root_solve(si);
      break;

    case STReciprocal:
      result = reci_root_solve(si);
      break;

    case STNot:
      result = not_root_solve(si);
      break;

    case STAttackRoot:
    case STDefenseRoot:
    case STLeafDirect:
    case STDirectDefense:
    case STSelfAttack:
    case STAttackHashed:
    case STMaxThreatLength:
    case STReflexAttackerFilter:
    case STReflexDefenderFilter:
      result = attack_root_solve(si);
      break;

    case STHelpRoot:
    case STHelpHashed:
    case STLeafHelp:
    case STReflexHelpFilter:
      result = help_root_solve(si);
      break;

    case STSeriesRoot:
    case STReflexSeriesFilter:
      result = series_root_solve(si);
      break;

    case STMoveInverterRootSolvableFilter:
      result = move_inverter_root_solve(si);
      break;

    case STSelfCheckGuardRootSolvableFilter:
      result = selfcheck_guard_root_solve(si);
      break;

    case STHelpFork:
    case STSeriesFork:
      result = branch_fork_root_solve(si);
      break;

    case STMaxSolutionsRootSolvableFilter:
      result = maxsolutions_root_solvable_filter_root_solve(si);
      break;

    case STStopOnShortSolutionsRootSolvableFilter:
      result = stoponshortsolutions_root_solvable_filter_root_solve(si);
      break;

    default:
      assert(0);
      result = false;
      break;
  }

  TraceFunctionExit(__func__);
  TraceFunctionResult("%u",result);
  TraceFunctionResultEnd();
  return result;
}

/* Determine whether a slice has a solution
 * @param si slice index
 * @return whether there is a solution and (to some extent) why not
 */
has_solution_type slice_has_solution(slice_index si)
{
  has_solution_type result = has_no_solution;

  TraceFunctionEntry(__func__);
  TraceFunctionParam("%u",si);
  TraceFunctionParamListEnd();

  TraceEnumerator(SliceType,slices[si].type,"\n");
  switch (slices[si].type)
  {
    case STLeafDirect:
      result = leaf_d_has_solution(si);
      break;

    case STLeafHelp:
      result = leaf_h_has_solution(si);
      break;

    case STLeafForced:
      result = leaf_forced_has_solution(si);
      break;

    case STQuodlibet:
      result = quodlibet_has_solution(si);
      break;

    case STReciprocal:
      result = reci_has_solution(si);
      break;

    case STNot:
      result = not_has_solution(si);
      break;

    case STAttackRoot:
    case STAttackMove:
    case STDirectDefense:
    case STSelfDefense:
    case STKeepMatingGuardRootDefenderFilter:
    case STKeepMatingGuardAttackerFilter:
    case STKeepMatingGuardDefenderFilter:
    case STKeepMatingGuardHelpFilter:
    case STKeepMatingGuardSeriesFilter:
    case STDegenerateTree:
    case STAttackHashed:
      result = attack_has_solution(si);
      break;

    case STHelpRoot:
      result = help_root_has_solution(si);
      break;

    case STHelpMove:
      result = help_has_solution(si);
      break;

    case STHelpHashed:
      result = slice_has_solution(slices[si].u.pipe.next);
      break;

    case STMoveInverterSolvableFilter:
      result = move_inverter_has_solution(si);
      break;

    case STSeriesMove:
    case STSeriesHashed:
    case STReflexSeriesFilter:
      result = series_has_solution(si);

    case STHelpFork:
    case STSeriesFork:
      result = branch_fork_has_solution(si);
      break;

    case STSelfCheckGuardRootSolvableFilter:
    case STSelfCheckGuardSolvableFilter:
      result = selfcheck_guard_has_solution(si);
      break;

    default:
      assert(0);
      break;
  }

  TraceFunctionExit(__func__);
  TraceEnumerator(has_solution_type,result,"");
  TraceFunctionResultEnd();
  return result;
}

/* Try to defend after an attempted key move at root level
 * @param si slice index
 * @return true iff the defending side can successfully defend
 */
boolean slice_root_defend(slice_index si, unsigned int max_number_refutations)
{
  boolean result = true;

  TraceFunctionEntry(__func__);
  TraceFunctionParam("%u",si);
  TraceFunctionParam("%u",max_number_refutations);
  TraceFunctionParamListEnd();

  TraceEnumerator(SliceType,slices[si].type,"\n");
  switch (slices[si].type)
  {
    case STLeafForced:
      result = leaf_forced_root_defend(si,max_number_refutations);
      break;

    case STQuodlibet:
      result = quodlibet_root_defend(si,max_number_refutations);
      break;

    default:
      assert(0);
      break;
  }

  TraceFunctionExit(__func__);
  TraceValue("%u",result);
  TraceFunctionResultEnd();
  return result;
}

/* Determine whether a slice.has just been solved with the just played
 * move by the non-starter
 * @param si slice identifier
 * @return true iff the non-starting side has just solved
 */
boolean slice_has_non_starter_solved(slice_index si)
{
  boolean result = false;

  TraceFunctionEntry(__func__);
  TraceFunctionParam("%u",si);
  TraceFunctionParamListEnd();

  TraceEnumerator(SliceType,slices[si].type,"\n");
  switch (slices[si].type)
  {
    case STLeafForced:
      result = leaf_forced_has_non_starter_solved(si);
      break;

    case STQuodlibet:
      result = quodlibet_has_non_starter_solved(si);
      break;

    default:
      assert(0);
      break;
  }

  TraceFunctionExit(__func__);
  TraceFunctionResult("%u",result);
  TraceFunctionResultEnd();
  return result;
}

/* Determine whether there are refutations
 * @param leaf slice index
 * @param max_result how many refutations should we look for
 * @return number of refutations found (0..max_result+1)
 */
unsigned int slice_count_refutations(slice_index si,
                                     unsigned int max_result)
{
  unsigned int result = max_result+1;

  TraceFunctionEntry(__func__);
  TraceFunctionParam("%u",si);
  TraceFunctionParamListEnd();

  TraceEnumerator(SliceType,slices[si].type,"\n");
  switch (slices[si].type)
  {
    case STLeafForced:
      result = leaf_forced_count_refutations(si,max_result);
      break;

    case STQuodlibet:
      result = quodlibet_count_refutations(si,max_result);
      break;

    default:
      assert(0);
      break;
  }

  TraceFunctionExit(__func__);
  TraceFunctionResult("%u",result);
  TraceFunctionResultEnd();
  return result;
}

/* Try to defend after an attempted key move at non-root level
 * @param si slice index
 * @return true iff the defending side can successfully defend
 */
boolean slice_defend(slice_index si)
{
  boolean result = true;

  TraceFunctionEntry(__func__);
  TraceFunctionParam("%u",si);
  TraceFunctionParamListEnd();

  TraceEnumerator(SliceType,slices[si].type,"\n");
  switch (slices[si].type)
  {
    case STLeafForced:
      result = leaf_forced_defend(si);
      break;

    case STLeafHelp:
      result = leaf_h_defend(si);
      break;

    case STQuodlibet:
      result = quodlibet_defend(si);
      break;

    default:
      assert(0);
      break;
  }

  TraceFunctionExit(__func__);
  TraceFunctionResult("%u",result);
  TraceFunctionResultEnd();
  return result;
}
