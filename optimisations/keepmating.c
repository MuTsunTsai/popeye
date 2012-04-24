#include "optimisations/keepmating.h"
#include "pydata.h"
#include "pypipe.h"
#include "stipulation/battle_play/branch.h"
#include "stipulation/help_play/branch.h"
#include "debugging/trace.h"

#include <assert.h>


/* Allocate a STKeepMatingFilter slice
 * @param mating mating side
 * @return identifier of allocated slice
 */
static slice_index alloc_keepmating_filter_slice(Side mating)
{
  slice_index result;

  TraceFunctionEntry(__func__);
  TraceEnumerator(Side,mating,"");
  TraceFunctionParamListEnd();

  result = alloc_pipe(STKeepMatingFilter);
  slices[result].u.keepmating_guard.mating = mating;

  TraceFunctionExit(__func__);
  TraceFunctionResult("%u",result);
  TraceFunctionResultEnd();
  return result;
}

/* Determine whether the mating side still has a piece that could
 * deliver the mate.
 * @return true iff the mating side has such a piece
 */
static boolean is_a_mating_piece_left(Side mating_side)
{
  boolean const is_white_mating = mating_side==White;

  piece p = roib+1;
  while (p<derbla && nbpiece[is_white_mating ? p : -p]==0)
    p++;

  return p<derbla;
}

/* Try to solve in n half-moves after a defense.
 * @param si slice index
 * @param n maximum number of half moves until goal
 * @return length of solution found and written, i.e.:
 *            slack_length-2 defense has turned out to be illegal
 *            <=n length of shortest solution found
 *            n+2 no solution found
 */
stip_length_type keepmating_filter_attack(slice_index si, stip_length_type n)
{
  Side const mating = slices[si].u.keepmating_guard.mating;
  stip_length_type result;

  TraceFunctionEntry(__func__);
  TraceFunctionParam("%u",si);
  TraceFunctionParam("%u",n);
  TraceFunctionParamListEnd();

  TraceEnumerator(Side,mating,"\n");

  if (is_a_mating_piece_left(mating))
    result = attack(slices[si].next1,n);
  else
    result = n+2;

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
 * @return <slack_length - no legal defense found
 *         <=n solved  - <=acceptable number of refutations found
 *                       return value is maximum number of moves
 *                       (incl. defense) needed
 *         n+2 refuted - >acceptable number of refutations found
 */
stip_length_type keepmating_filter_defend(slice_index si, stip_length_type n)
{
  Side const mating = slices[si].u.keepmating_guard.mating;
  slice_index const next = slices[si].next1;
  stip_length_type result;

  TraceFunctionEntry(__func__);
  TraceFunctionParam("%u",si);
  TraceFunctionParam("%u",n);
  TraceFunctionParamListEnd();

  TraceEnumerator(Side,mating,"\n");

  if (is_a_mating_piece_left(mating))
    result = defend(next,n);
  else
    result = n+2;

  TraceFunctionExit(__func__);
  TraceFunctionResult("%u",result);
  TraceFunctionResultEnd();
  return result;
}

/* **************** Stipulation instrumentation ***************
 */

/* Data structure for remembering the side(s) that needs to keep >= 1
 * piece that could deliver mate
 */
typedef struct
{
  boolean for_side[nr_sides];
} insertion_state_type;

static slice_index alloc_appropriate_filter(insertion_state_type const *state)
{
  slice_index result;

  TraceFunctionEntry(__func__);
  TraceFunctionParamListEnd();

  if (state->for_side[White]+state->for_side[Black]==1)
  {
    Side const side = state->for_side[White] ? White : Black;
    result = alloc_keepmating_filter_slice(side);
  }
  else
    result = no_slice;

  TraceFunctionExit(__func__);
  TraceFunctionResult("%u",result);
  TraceFunctionResultEnd();
  return result;
}

static void keepmating_filter_inserter_goal(slice_index si,
                                            stip_structure_traversal *st)
{
  insertion_state_type * const state = st->param;

  TraceFunctionEntry(__func__);
  TraceFunctionParam("%u",si);
  TraceFunctionParamListEnd();

  state->for_side[advers(slices[si].starter)] = true;

  TraceFunctionExit(__func__);
  TraceFunctionResultEnd();
}

static void keepmating_filter_inserter_or(slice_index si,
                                          stip_structure_traversal *st)
{
  insertion_state_type * const state = st->param;
  insertion_state_type state1 = { { false, false } };
  insertion_state_type state2 = { { false, false } };

  TraceFunctionEntry(__func__);
  TraceFunctionParam("%u",si);
  TraceFunctionParamListEnd();

  st->param = &state1;
  stip_traverse_structure(slices[si].next1,st);

  st->param = &state2;
  stip_traverse_structure(slices[si].next2,st);

  state->for_side[White] = state1.for_side[White] && state2.for_side[White];
  state->for_side[Black] = state1.for_side[Black] && state2.for_side[Black];

  st->param = state;

  TraceFunctionExit(__func__);
  TraceFunctionResultEnd();
}

static void keepmating_filter_inserter_and(slice_index si,
                                           stip_structure_traversal *st)
{
  insertion_state_type * const state = st->param;
  insertion_state_type state1 = { { false, false } };
  insertion_state_type state2 = { { false, false } };

  TraceFunctionEntry(__func__);
  TraceFunctionParam("%u",si);
  TraceFunctionParamListEnd();

  st->param = &state1;
  stip_traverse_structure(slices[si].next1,st);

  st->param = &state2;
  stip_traverse_structure(slices[si].next2,st);

  state->for_side[White] = state1.for_side[White] || state2.for_side[White];
  state->for_side[Black] = state1.for_side[Black] || state2.for_side[Black];

  st->param = state;

  TraceFunctionExit(__func__);
  TraceFunctionResultEnd();
}

static
void keepmating_filter_inserter_end_of_branch(slice_index si,
                                              stip_structure_traversal *st)
{
  TraceFunctionEntry(__func__);
  TraceFunctionParam("%u",si);
  TraceFunctionParamListEnd();

  /* we can't rely on the (arbitrary) order stip_traverse_structure_children()
   * would use; instead make sure that we first traverse towards the
   * goal(s).
   */
  stip_traverse_structure_next_branch(si,st);
  stip_traverse_structure_children_pipe(si,st);

  TraceFunctionExit(__func__);
  TraceFunctionResultEnd();
}

static void keepmating_filter_inserter_battle(slice_index si,
                                              stip_structure_traversal *st)
{
  TraceFunctionEntry(__func__);
  TraceFunctionParam("%u",si);
  TraceFunctionParamListEnd();

  stip_traverse_structure_children(si,st);

  if (st->context!=stip_traversal_context_help)
  {
    insertion_state_type const * const state = st->param;
    slice_index const prototype = alloc_appropriate_filter(state);
    if (prototype!=no_slice)
    {
      slices[prototype].starter = slices[si].starter;
      if (st->context==stip_traversal_context_attack)
        attack_branch_insert_slices(si,&prototype,1);
      else
        defense_branch_insert_slices(si,&prototype,1);
    }
  }

  TraceFunctionExit(__func__);
  TraceFunctionResultEnd();
}

static void keepmating_filter_inserter_help(slice_index si,
                                            stip_structure_traversal *st)
{
  insertion_state_type const * const state = st->param;

  TraceFunctionEntry(__func__);
  TraceFunctionParam("%u",si);
  TraceFunctionParamListEnd();

  stip_traverse_structure_children(si,st);

  {
    slice_index const prototype = alloc_appropriate_filter(state);
    if (prototype!=no_slice)
    {
      slices[prototype].starter = advers(slices[si].starter);
      help_branch_insert_slices(si,&prototype,1);
    }
  }

  TraceFunctionExit(__func__);
  TraceFunctionResultEnd();
}

static structure_traversers_visitors keepmating_filter_inserters[] =
{
  { STNotEndOfBranchGoal,      &keepmating_filter_inserter_battle        },
  { STReadyForHelpMove,        &keepmating_filter_inserter_help          },
  { STAnd,                     &keepmating_filter_inserter_and           },
  { STOr,                      &keepmating_filter_inserter_end_of_branch },
  { STEndOfBranchGoal,         &keepmating_filter_inserter_end_of_branch },
  { STEndOfBranchGoalImmobile, &keepmating_filter_inserter_end_of_branch },
  { STEndOfBranchForced,       &keepmating_filter_inserter_end_of_branch },
  { STConstraintSolver,        &keepmating_filter_inserter_end_of_branch },
  { STConstraintTester,        &keepmating_filter_inserter_end_of_branch },
  { STGoalReachedTester,       &keepmating_filter_inserter_goal          }
};

enum
{
  nr_keepmating_filter_inserters = (sizeof keepmating_filter_inserters
                                    / sizeof keepmating_filter_inserters[0])
};

/* Instrument stipulation with STKeepMatingFilter slices
 * @param si identifies slice where to start
 */
void stip_insert_keepmating_filters(slice_index si)
{
  insertion_state_type state = { { false, false } };
  stip_structure_traversal st;

  TraceFunctionEntry(__func__);
  TraceFunctionParam("%u",si);
  TraceFunctionParamListEnd();

  TraceStipulation(si);

  stip_structure_traversal_init(&st,&state);
  stip_structure_traversal_override(&st,
                                    keepmating_filter_inserters,
                                    nr_keepmating_filter_inserters);
  stip_traverse_structure(si,&st);

  TraceFunctionExit(__func__);
  TraceFunctionResultEnd();
}