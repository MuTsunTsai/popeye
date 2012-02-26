#include "solving/find_shortest.h"
#include "pypipe.h"
#include "stipulation/branch.h"
#include "stipulation/battle_play/branch.h"
#include "trace.h"

#include <assert.h>

/* Allocate a STFindShortest slice.
 * @param length maximum number of half moves until end of slice
 * @param min_length minimum number of half moves until end of slice
 * @return index of allocated slice
 */
slice_index alloc_find_shortest_slice(stip_length_type length,
                                      stip_length_type min_length)
{
  slice_index result;

  TraceFunctionEntry(__func__);
  TraceFunctionParam("%u",length);
  TraceFunctionParam("%u",min_length);
  TraceFunctionParamListEnd();

  result = alloc_branch(STFindShortest,length,min_length);

  TraceFunctionExit(__func__);
  TraceFunctionResult("%u",result);
  TraceFunctionResultEnd();
  return result;
}

/* Try to solve in n half-moves after a defense.
 * @param si slice index
 * @param n maximum number of half moves until goal
 * @return length of solution found and written, i.e.:
 *            slack_length-2 defense has turned out to be illegal
 *            <=n length of shortest solution found
 *            n+2 no solution found
 */
stip_length_type find_shortest_attack(slice_index si, stip_length_type n)
{
  stip_length_type result = n+2;
  slice_index const next = slices[si].u.branch.next;
  stip_length_type const length = slices[si].u.branch.length;
  stip_length_type const min_length = slices[si].u.branch.min_length;
  stip_length_type const n_min = (min_length>=(length-n)+slack_length
                                  ? min_length-(length-n)
                                  : min_length);
  stip_length_type n_current;

  TraceFunctionEntry(__func__);
  TraceFunctionParam("%u",si);
  TraceFunctionParam("%u",n);
  TraceFunctionParamListEnd();

  assert(length>=n);

  for (n_current = n_min+(n-n_min)%2; n_current<=n; n_current += 2)
  {
    result = attack(next,n_current);
    if (result<=n_current)
      break;
  }

  TraceFunctionExit(__func__);
  TraceFunctionResult("%u",result);
  TraceFunctionResultEnd();
  return result;
}

/* Solve in a number of half-moves
 * @param si identifies slice
 * @param n exact number of half moves until end state has to be reached
 * @return length of solution found, i.e.:
 *         n+4 the move leading to the current position has turned out
 *             to be illegal
 *         n+2 no solution found
 *         n   solution found
 */
stip_length_type find_shortest_help(slice_index si, stip_length_type n)
{
  stip_length_type result = n+2;
  slice_index const next = slices[si].u.branch.next;
  stip_length_type const length = slices[si].u.branch.length;
  stip_length_type const min_length = slices[si].u.branch.min_length;
  stip_length_type const n_min = (min_length>=(length-n)+slack_length
                                  ? min_length-(length-n)
                                  : min_length);
  stip_length_type n_current;

  TraceFunctionEntry(__func__);
  TraceFunctionParam("%u",si);
  TraceFunctionParam("%u",n);
  TraceFunctionParamListEnd();

  assert(length>=n);

  for (n_current = n_min+(n-n_min)%2; n_current<=n; n_current += 2)
  {
    result = help(next,n_current);
    if (result<=n_current)
      break;
  }

  TraceFunctionExit(__func__);
  TraceFunctionResult("%u",result);
  TraceFunctionResultEnd();
  return result;
}

static void battle_insert_find_shortest(slice_index si)
{
  stip_length_type const length = slices[si].u.branch.length;
  stip_length_type const min_length = slices[si].u.branch.min_length;

  TraceFunctionEntry(__func__);
  TraceFunctionParam("%u",si);
  TraceFunctionParamListEnd();

  if (length>=min_length+2)
  {
    slice_index const defense = branch_find_slice(STReadyForDefense,si);
    slice_index const attack = branch_find_slice(STReadyForAttack,defense);
    slice_index const proto = alloc_find_shortest_slice(length,min_length);
    assert(defense!=no_slice);
    assert(attack!=no_slice);
    attack_branch_insert_slices(attack,&proto,1);
  }

  TraceFunctionExit(__func__);
  TraceFunctionResultEnd();
}

static void insert_find_shortest_attack_adapter(slice_index si,
                                                stip_structure_traversal *st)
{
  TraceFunctionEntry(__func__);
  TraceFunctionParam("%u",si);
  TraceFunctionParamListEnd();

  stip_traverse_structure_children(si,st);
  battle_insert_find_shortest(si);

  TraceFunctionExit(__func__);
  TraceFunctionResultEnd();
}

static void insert_find_shortest_defense_adapter(slice_index si,
                                                 stip_structure_traversal *st)
{
  TraceFunctionEntry(__func__);
  TraceFunctionParam("%u",si);
  TraceFunctionParamListEnd();

  stip_traverse_structure_children(si,st);
  battle_insert_find_shortest(si);

  TraceFunctionExit(__func__);
  TraceFunctionResultEnd();
}

static structure_traversers_visitors const find_shortest_inserters[] =
{
  { STAttackAdapter,  &insert_find_shortest_attack_adapter  },
  { STDefenseAdapter, &insert_find_shortest_defense_adapter }
};

enum
{
  nr_find_shortest_inserters = sizeof find_shortest_inserters / sizeof find_shortest_inserters[0]
};

/* Instrument the stipulation with slices that attempt the shortest
 * solutions/variations
 * @param si root slice of stipulation
 */
void stip_insert_find_shortest_solvers(slice_index si)
{
  stip_structure_traversal st;
  output_mode mode = output_mode_none;

  TraceFunctionEntry(__func__);
  TraceFunctionParam("%u",si);
  TraceFunctionParamListEnd();

  stip_structure_traversal_init(&st,&mode);
  stip_structure_traversal_override(&st,find_shortest_inserters,nr_find_shortest_inserters);
  stip_traverse_structure(si,&st);

  TraceFunctionExit(__func__);
  TraceFunctionResultEnd();
}
