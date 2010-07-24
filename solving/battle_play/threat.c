#include "stipulation/battle_play/threat.h"
#include "pytable.h"
#include "pydata.h"
#include "pypipe.h"
#include "stipulation/branch.h"
#include "stipulation/proxy.h"
#include "stipulation/battle_play/branch.h"
#include "stipulation/battle_play/defense_play.h"
#include "stipulation/battle_play/attack_play.h"
#include "trace.h"

#include <assert.h>

/* Table where threats of the various move levels are collected
 */
static table threats[maxply+1];

/* Lengths of threats of the various move levels 
 */
static stip_length_type threat_lengths[maxply+1];

/* value of threat_lengths[N] that signals that no threats were found
 * in ply N (and that threats[N] may not even refer to a table)
 */
static stip_length_type const no_threats_found = UINT_MAX;

threat_activity threat_activities[maxply+1];

/* count threats not defeated by a defense while we are
 * threat_enforcing
 */
static unsigned int nr_threats_to_be_confirmed;

/* Allocate a STThreatEnforcer slice.
 * @return index of allocated slice
 */
static slice_index alloc_threat_enforcer_slice(void)
{
  slice_index result;

  TraceFunctionEntry(__func__);
  TraceFunctionParamListEnd();

  result = alloc_pipe(STThreatEnforcer);

  TraceFunctionExit(__func__);
  TraceFunctionResult("%u",result);
  TraceFunctionResultEnd();
  return result;
}

/* Allocate a STThreatCollector slice.
 * @return index of allocated slice
 */
static slice_index alloc_threat_collector_slice(void)
{
  slice_index result;

  TraceFunctionEntry(__func__);
  TraceFunctionParamListEnd();

  result = alloc_pipe(STThreatCollector);

  TraceFunctionExit(__func__);
  TraceFunctionResult("%u",result);
  TraceFunctionResultEnd();
  return result;
}

/* Determine whether there is a solution in n half moves, by trying
 * n_min, n_min+2 ... n half-moves.
 * @param si slice index
 * @param n maximum number of half moves until goal
 * @param n_min minimal number of half moves to try
 * @param n_max_unsolvable maximum number of half-moves that we
 *                         know have no solution
 * @return length of solution found, i.e.:
 *            n_min-2 defense has turned out to be illegal
 *            n_min..n length of shortest solution found
 *            n+2 no solution found
 */
stip_length_type
threat_enforcer_has_solution_in_n(slice_index si,
                                  stip_length_type n,
                                  stip_length_type n_min,
                                  stip_length_type n_max_unsolvable)
{
  stip_length_type result;
  slice_index const next = slices[si].u.pipe.next;

  TraceFunctionEntry(__func__);
  TraceFunctionParam("%u",si);
  TraceFunctionParam("%u",n);
  TraceFunctionParam("%u",n_min);
  TraceFunctionParam("%u",n_max_unsolvable);
  TraceFunctionParamListEnd();

  result = attack_has_solution_in_n(next,n,n_min,n_max_unsolvable);

  TraceFunctionExit(__func__);
  TraceFunctionResult("%u",result);
  TraceFunctionResultEnd();
  return result;
}

/* Solve a slice, by trying n_min, n_min+2 ... n half-moves.
 * @param si slice index
 * @param n maximum number of half moves until goal
 * @param n_min minimum number of half-moves of interesting variations
 * @param n_max_unsolvable maximum number of half-moves that we
 *                         know have no solution
 * @return length of solution found and written, i.e.:
 *            n_min-2 defense has turned out to be illegal
 *            n_min..n length of shortest solution found
 *            n+2 no solution found
 */
stip_length_type threat_enforcer_solve_in_n(slice_index si,
                                            stip_length_type n,
                                            stip_length_type n_min,
                                            stip_length_type n_max_unsolvable)
{
  stip_length_type result;
  slice_index const next = slices[si].u.pipe.next;
  ply const threats_ply = nbply+1;
  stip_length_type const len_threat = threat_lengths[threats_ply];

  TraceFunctionEntry(__func__);
  TraceFunctionParam("%u",si);
  TraceFunctionParam("%u",n);
  TraceFunctionParam("%u",n_min);
  TraceFunctionParam("%u",n_max_unsolvable);
  TraceFunctionParamListEnd();

  TraceValue("%u\n",len_threat);

  if (len_threat<=slack_length_battle)
    /* the attack has something stronger than threats (typically, it
     * delivers check)
     */
    result = attack_solve_in_n(next,n,n_min,n_max_unsolvable);
  else if (len_threat<=n)
  {
    /* there are >=1 threats - don't report variations shorter than
     * the threats or variations that don't refute any threat
     */
    table const threats_table = threats[threats_ply];
    stip_length_type len_test_threats;

    nr_threats_to_be_confirmed = table_length(threats_table);

    threat_activities[threats_ply] = threat_enforcing;
    len_test_threats = attack_has_solution_in_n(next,
                                                len_threat,n_max_unsolvable+2,
                                                n_max_unsolvable);
    threat_activities[threats_ply] = threat_idle;

    if (len_test_threats>len_threat)
      /* variation is longer than threat */
      result = attack_solve_in_n(next,n,n_min,n_max_unsolvable);
    else if (len_test_threats>len_threat-2 && nr_threats_to_be_confirmed>0)
      /* variation has same length as the threat(s), but it has
       * defeated at least one threat
       */
      result = attack_solve_in_n(next,n,n_min,n_max_unsolvable);
    else
        /* variation is shorter than threat */
      result = len_test_threats;
  }
  else
    /* zugzwang, or we haven't looked for threats yet */
    result = attack_solve_in_n(next,n,n_min,n_max_unsolvable);

  TraceFunctionExit(__func__);
  TraceFunctionResult("%u",result);
  TraceFunctionResultEnd();
  return result;
}

/* Allocate a STThreatSolver defender slice.
 * @param length maximum number of half-moves of slice (+ slack)
 * @param min_length minimum number of half-moves of slice (+ slack)
 * @param defense_move identifies the slice where attack play starts
 *                    after this slice
 * @return index of allocated slice
 */
static slice_index alloc_threat_solver_slice(stip_length_type length,
                                             stip_length_type min_length,
                                             slice_index defense_move)
{
  slice_index result;

  TraceFunctionEntry(__func__);
  TraceFunctionParam("%u",length);
  TraceFunctionParam("%u",min_length);
  TraceFunctionParamListEnd();

  result = alloc_branch(STThreatSolver,length,min_length);
  slices[result].u.threat_writer.defense_move = defense_move;

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
 * @param n_min minimum number of half-moves of interesting variations
 *              (slack_length_battle <= n_min <= slices[si].u.branch.length)
 * @param n_max_unsolvable maximum number of half-moves that we
 *                         know have no solution
 * @return <=n solved  - return value is maximum number of moves
 *                       (incl. defense) needed
 *         n+2 refuted - acceptable number of refutations found
 *         n+4 refuted - more refutations found than acceptable
 */
stip_length_type threat_collector_defend_in_n(slice_index si,
                                              stip_length_type n,
                                              stip_length_type n_min,
                                              stip_length_type n_max_unsolvable)
{
  stip_length_type result;
  slice_index const next = slices[si].u.pipe.next;

  TraceFunctionEntry(__func__);
  TraceFunctionParam("%u",si);
  TraceFunctionParam("%u",n);
  TraceFunctionParam("%u",n_min);
  TraceFunctionParam("%u",n_max_unsolvable);
  TraceFunctionParamListEnd();

  result = defense_defend_in_n(next,n,n_min,n_max_unsolvable);

  TraceValue("%u\n",nbply);
  if (threat_activities[nbply]==threat_solving && result<=n)
    append_to_top_table();

  TraceFunctionExit(__func__);
  TraceValue("%u",result);
  TraceFunctionResultEnd();
  return result;
}

/* Determine whether there are defenses after an attacking move
 * @param si slice index
 * @param n maximum number of half moves until end state has to be reached
 * @param n_max_unsolvable maximum number of half-moves that we
 *                         know have no solution
 * @param max_nr_refutations how many refutations should we look for
 * @return <=n solved  - return value is maximum number of moves
                         (incl. defense) needed
           n+2 refuted - <=max_nr_refutations refutations found
           n+4 refuted - >max_nr_refutations refutations found
 */
stip_length_type
threat_collector_can_defend_in_n(slice_index si,
                                 stip_length_type n,
                                 stip_length_type n_max_unsolvable,
                                 unsigned int max_nr_refutations)
{
  stip_length_type result;
  slice_index const next = slices[si].u.pipe.next;

  TraceFunctionEntry(__func__);
  TraceFunctionParam("%u",si);
  TraceFunctionParam("%u",n);
  TraceFunctionParam("%u",n_max_unsolvable);
  TraceFunctionParam("%u",max_nr_refutations);
  TraceFunctionParamListEnd();

  result = defense_can_defend_in_n(next,n,n_max_unsolvable,max_nr_refutations);

  if (threat_activities[nbply]==threat_enforcing
      && n>=threat_lengths[nbply]-2)
  {
    if (is_current_move_in_table(threats[nbply]))
    {
      if (result<=n)
      {
        --nr_threats_to_be_confirmed;
        if (nr_threats_to_be_confirmed>0)
          /* threats tried so far still work (perhaps shorter than
           * before the current defense), but we haven't tried all
           * threats yet -> don't stop the iteration over the
           * attacking moves
           */
          result = n+2;
      }
      else if (n==threat_lengths[nbply]-1)
        /* we have found a defeated threat -> stop the iteration */
        result = n;
    }
    else
      /* not a threat -> don't stop the iteration */
      result = n+2;
  }

  TraceFunctionExit(__func__);
  TraceValue("%u",result);
  TraceFunctionResultEnd();
  return result;
}

/* Solve threats after an attacker's move
 * @param si slice index
 * @return length of threats
 *         <slack_length_battle if the attacker has something stronger
 *             than threats (i.e. has delivered check)
 *         n+2 if there is no threat
 */
static stip_length_type solve_threats(slice_index si, stip_length_type n)
{
  slice_index const defense_move = slices[si].u.threat_writer.defense_move;
  slice_index const attack_side = slices[defense_move].u.branch.next;
  stip_length_type const n_min = battle_branch_calc_n_min(si,n);
  stip_length_type const n_max_unsolvable = slack_length_battle;
  stip_length_type result;

  TraceFunctionEntry(__func__);
  TraceFunctionParam("%u",si);
  TraceFunctionParam("%u",n);
  TraceFunctionParamListEnd();

  /* insert an empty ply for the virtual defense played before the
   * threat
   */
  nextply(nbply);
  result = attack_solve_in_n(attack_side,n,n_min,n_max_unsolvable);
  finply();

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
 * @param n_min minimum number of half-moves of interesting variations
 *              (slack_length_battle <= n_min <= slices[si].u.branch.length)
 * @param n_max_unsolvable maximum number of half-moves that we
 *                         know have no solution
 * @return <=n solved  - return value is maximum number of moves
 *                       (incl. defense) needed
 *         n+2 refuted - acceptable number of refutations found
 *         n+4 refuted - more refutations found than acceptable
 */
stip_length_type threat_solver_defend_in_n(slice_index si,
                                           stip_length_type n,
                                           stip_length_type n_min,
                                           stip_length_type n_max_unsolvable)
{
  stip_length_type result;
  slice_index const next = slices[si].u.pipe.next;
  ply const threats_ply = nbply+2;

  TraceFunctionEntry(__func__);
  TraceFunctionParam("%u",si);
  TraceFunctionParam("%u",n);
  TraceFunctionParam("%u",n_min);
  TraceFunctionParam("%u",n_max_unsolvable);
  TraceFunctionParamListEnd();

  TraceValue("%u\n",threats_ply);
  threats[threats_ply] = allocate_table();

  threat_activities[threats_ply] = threat_solving;
  threat_lengths[threats_ply] = solve_threats(si,n-1);
  threat_activities[threats_ply] = threat_idle;

  result = defense_defend_in_n(next,n,n_min,n_max_unsolvable);

  assert(get_top_table()==threats[threats_ply]);
  free_table();
  threat_lengths[threats_ply] = no_threats_found;
  threats[threats_ply] = table_nil;

  TraceFunctionExit(__func__);
  TraceValue("%u",result);
  TraceFunctionResultEnd();
  return result;
}

/* Determine whether there are defenses after an attacking move
 * @param si slice index
 * @param n maximum number of half moves until end state has to be reached
 * @param n_max_unsolvable maximum number of half-moves that we
 *                         know have no solution
 * @param max_nr_refutations how many refutations should we look for
 * @return <=n solved  - return value is maximum number of moves
 *                       (incl. defense) needed
 *         n+2 refuted - <=max_nr_refutations refutations found
 *         n+4 refuted - >max_nr_refutations refutations found
 */
stip_length_type
threat_solver_can_defend_in_n(slice_index si,
                              stip_length_type n,
                              stip_length_type n_max_unsolvable,
                              unsigned int max_nr_refutations)
{
  stip_length_type result;
  slice_index const next = slices[si].u.pipe.next;

  TraceFunctionEntry(__func__);
  TraceFunctionParam("%u",si);
  TraceFunctionParam("%u",n);
  TraceFunctionParam("%u",n_max_unsolvable);
  TraceFunctionParam("%u",max_nr_refutations);
  TraceFunctionParamListEnd();

  result = defense_can_defend_in_n(next,n,n_max_unsolvable,max_nr_refutations);

  TraceFunctionExit(__func__);
  TraceValue("%u",result);
  TraceFunctionResultEnd();
  return result;
}


/* The following enumeration type represents the state of threat
 * handler insertion; this helps us avoiding unnecessary threat
 * handler slices (e.g. we only need a threat collector slice if we
 * have already inserted a threat writer and a threat enforcer slice).
 */
typedef enum
{
  threat_handler_inserted_none,
  threat_handler_inserted_solver,
  threat_handler_inserted_enforcer,
  threat_handler_inserted_collector
} threat_handler_insertion_state;

/* Prepend a threat writer slice to a defense move slice
 * @param si identifies slice around which to insert threat handlers
 * @param st address of structure defining traversal
 */
static void prepend_threat_solver(slice_index si, stip_structure_traversal *st)
{
  threat_handler_insertion_state * const state = st->param;
  threat_handler_insertion_state const save_state = *state;
  stip_length_type const length = slices[si].u.branch.length;

  TraceFunctionEntry(__func__);
  TraceFunctionParam("%u",si);
  TraceFunctionParamListEnd();

  if (length>slack_length_battle+1)
  {
    *state = threat_handler_inserted_solver;
    stip_traverse_structure_children(si,st);
    *state = save_state;

    {
      slice_index const prev = slices[si].prev;
      stip_length_type const min_length = slices[si].u.branch.min_length;
      pipe_append(prev,alloc_threat_solver_slice(length,min_length,si));
    }
  }
  else
    stip_traverse_structure_children(si,st);

  TraceFunctionExit(__func__);
  TraceFunctionResultEnd();
}

/* Append a threat collector slice to an attack move slice
 * @param si identifies slice around which to insert threat handlers
 * @param st address of structure defining traversal
 */
static void append_threat_enforcer(slice_index si, stip_structure_traversal *st)
{
  threat_handler_insertion_state * const state = st->param;
  threat_handler_insertion_state const save_state = *state;
  stip_length_type const length = slices[si].u.branch.length;

  TraceFunctionEntry(__func__);
  TraceFunctionParam("%u",si);
  TraceFunctionParamListEnd();

  if (length>slack_length_battle && *state==threat_handler_inserted_solver)
  {
    pipe_append(si,alloc_threat_enforcer_slice());
    *state = threat_handler_inserted_enforcer;
  }

  stip_traverse_structure_children(si,st);

  *state = save_state;

  TraceFunctionExit(__func__);
  TraceFunctionResultEnd();
}

/* Append a threat collector slice to an attack move slice
 * @param si identifies slice around which to insert threat handlers
 * @param st address of structure defining traversal
 */
static void append_threat_collector(slice_index si, stip_structure_traversal *st)
{
  threat_handler_insertion_state * const state = st->param;
  threat_handler_insertion_state const save_state = *state;
  stip_length_type const length = slices[si].u.branch.length;

  TraceFunctionEntry(__func__);
  TraceFunctionParam("%u",si);
  TraceFunctionParamListEnd();

  if (length>slack_length_battle && *state==threat_handler_inserted_enforcer)
  {
    pipe_append(si,alloc_threat_collector_slice());
    *state = threat_handler_inserted_collector;
  }

  stip_traverse_structure_children(si,st);

  *state = save_state;

  TraceFunctionExit(__func__);
  TraceFunctionResultEnd();
}

/* Reset the threat handler insertaion state for the subsequent slices
 * @param si identifies slice around which to insert threat handlers
 * @param st address of structure defining traversal
 */
static void threat_handler_reset_insertion_state(slice_index si,
                                                 stip_structure_traversal *st)
{
  threat_handler_insertion_state * const state = st->param;
  threat_handler_insertion_state const save_state = *state;

  TraceFunctionEntry(__func__);
  TraceFunctionParam("%u",si);
  TraceFunctionParamListEnd();

  *state = threat_handler_inserted_none;
  stip_traverse_structure_children(si,st);
  *state = save_state;

  TraceFunctionExit(__func__);
  TraceFunctionResultEnd();
}

static stip_structure_visitor const threat_handler_inserters[] =
{
  &stip_traverse_structure_children,     /* STProxy */
  &append_threat_collector,              /* STAttackMove */
  &prepend_threat_solver,                /* STDefenseMove */
  &stip_traverse_structure_children,     /* STDefenseMoveAgainstGoal */
  &stip_structure_visitor_noop,          /* STHelpMove */
  &stip_structure_visitor_noop,          /* STHelpMoveToGoal */
  &stip_structure_visitor_noop,          /* STHelpFork */
  &stip_structure_visitor_noop,          /* STSeriesMove */
  &stip_structure_visitor_noop,          /* STSeriesMoveToGoal */
  &stip_structure_visitor_noop,          /* STSeriesFork */
  &stip_structure_visitor_noop,          /* STGoalReachedTester */
  &stip_structure_visitor_noop,          /* STLeaf */
  &stip_traverse_structure_children,     /* STReciprocal */
  &stip_traverse_structure_children,     /* STQuodlibet */
  &threat_handler_reset_insertion_state, /* STNot */
  &stip_traverse_structure_children,     /* STMoveInverterRootSolvableFilter */
  &stip_traverse_structure_children,     /* STMoveInverterSolvableFilter */
  &stip_traverse_structure_children,     /* STMoveInverterSeriesFilter */
  &stip_traverse_structure_children,     /* STAttackRoot */
  &stip_traverse_structure_children,     /* STDefenseRoot */
  &stip_traverse_structure_children,     /* STPostKeyPlaySuppressor */
  &stip_traverse_structure_children,     /* STContinuationSolver */
  &stip_traverse_structure_children,     /* STContinuationWriter */
  &stip_traverse_structure_children,     /* STBattlePlaySolver */
  &stip_traverse_structure_children,     /* STBattlePlaySolutionWriter */
  &stip_traverse_structure_children,     /* STThreatSolver */
  &stip_traverse_structure_children,     /* STZugzwangWriter */
  &stip_traverse_structure_children,     /* STThreatEnforcer */
  &stip_traverse_structure_children,     /* STThreatCollector */
  &stip_traverse_structure_children,     /* STRefutationsCollector */
  &stip_traverse_structure_children,     /* STVariationWriter */
  &stip_traverse_structure_children,     /* STRefutingVariationWriter */
  &stip_traverse_structure_children,     /* STNoShortVariations */
  &stip_traverse_structure_children,     /* STAttackHashed */
  &stip_structure_visitor_noop,          /* STHelpRoot */
  &stip_structure_visitor_noop,          /* STHelpShortcut */
  &stip_traverse_structure_children,     /* STHelpHashed */
  &stip_structure_visitor_noop,          /* STSeriesRoot */
  &stip_structure_visitor_noop,          /* STSeriesShortcut */
  &stip_traverse_structure_children,     /* STParryFork */
  &stip_traverse_structure_children,     /* STSeriesHashed */
  &stip_traverse_structure_children,     /* STSelfCheckGuardRootSolvableFilter */
  &stip_traverse_structure_children,     /* STSelfCheckGuardSolvableFilter */
  &append_threat_enforcer,               /* STSelfCheckGuardAttackerFilter */
  &stip_traverse_structure_children,     /* STSelfCheckGuardDefenderFilter */
  &stip_traverse_structure_children,     /* STSelfCheckGuardHelpFilter */
  &stip_traverse_structure_children,     /* STSelfCheckGuardSeriesFilter */
  &stip_traverse_structure_children,     /* STDirectDefenderFilter */
  &stip_traverse_structure_children,     /* STReflexRootFilter */
  &stip_traverse_structure_children,     /* STReflexHelpFilter */
  &stip_traverse_structure_children,     /* STReflexSeriesFilter */
  &stip_traverse_structure_children,     /* STReflexAttackerFilter */
  &stip_traverse_structure_children,     /* STReflexDefenderFilter */
  &stip_traverse_structure_children,     /* STSelfDefense */
  &stip_traverse_structure_children,     /* STDefenseFork */
  &stip_traverse_structure_children,     /* STRestartGuardRootDefenderFilter */
  &stip_traverse_structure_children,     /* STRestartGuardHelpFilter */
  &stip_traverse_structure_children,     /* STRestartGuardSeriesFilter */
  &stip_traverse_structure_children,     /* STIntelligentHelpFilter */
  &stip_traverse_structure_children,     /* STIntelligentSeriesFilter */
  &stip_traverse_structure_children,     /* STGoalReachableGuardHelpFilter */
  &stip_traverse_structure_children,     /* STGoalReachableGuardSeriesFilter */
  &stip_traverse_structure_children,     /* STIntelligentDuplicateAvoider */
  &stip_traverse_structure_children,     /* STKeepMatingGuardAttackerFilter */
  &stip_traverse_structure_children,     /* STKeepMatingGuardDefenderFilter */
  &stip_traverse_structure_children,     /* STKeepMatingGuardHelpFilter */
  &stip_traverse_structure_children,     /* STKeepMatingGuardSeriesFilter */
  &stip_traverse_structure_children,     /* STMaxFlightsquares */
  &stip_traverse_structure_children,     /* STDegenerateTree */
  &stip_traverse_structure_children,     /* STMaxNrNonTrivial */
  &stip_traverse_structure_children,     /* STMaxNrNonTrivialCounter */
  &stip_traverse_structure_children,     /* STMaxThreatLength */
  &stip_traverse_structure_children,     /* STMaxTimeRootDefenderFilter */
  &stip_traverse_structure_children,     /* STMaxTimeDefenderFilter */
  &stip_traverse_structure_children,     /* STMaxTimeHelpFilter */
  &stip_traverse_structure_children,     /* STMaxTimeSeriesFilter */
  &stip_traverse_structure_children,     /* STMaxSolutionsRootSolvableFilter */
  &stip_traverse_structure_children,     /* STMaxSolutionsSolvableFilter */
  &stip_traverse_structure_children,     /* STMaxSolutionsRootDefenderFilter */
  &stip_traverse_structure_children,     /* STMaxSolutionsHelpFilter */
  &stip_traverse_structure_children,     /* STMaxSolutionsSeriesFilter */
  &stip_traverse_structure_children,     /* STStopOnShortSolutionsRootSolvableFilter */
  &stip_traverse_structure_children,     /* STStopOnShortSolutionsHelpFilter */
  &stip_traverse_structure_children,     /* STStopOnShortSolutionsSeriesFilter */
  &stip_traverse_structure_children,     /* STEndOfPhaseWriter */
  &stip_traverse_structure_children,     /* STEndOfSolutionWriter */
  &stip_traverse_structure_children,     /* STRefutationWriter */
  &stip_traverse_structure_children,     /* STOutputPlaintextTreeCheckDetectorAttackerFilter */
  &stip_traverse_structure_children,     /* STOutputPlaintextTreeCheckDetectorDefenderFilter */
  &stip_traverse_structure_children,     /* STOutputPlaintextLineLineWriter */
  &stip_traverse_structure_children,     /* STOutputPlaintextTreeGoalWriter */
  &stip_traverse_structure_children,     /* STOutputPlaintextTreeMoveInversionCounter */
  &stip_traverse_structure_children,     /* STOutputPlaintextLineMoveInversionCounter */
  &stip_traverse_structure_children      /* STOutputPlaintextLineEndOfIntroSeriesMarker */
};

/* Instrument the stipulation representation so that it can deal with
 * threats
 */
void stip_insert_threat_handlers(void)
{
  stip_structure_traversal st;
  threat_handler_insertion_state state = threat_handler_inserted_none;
  unsigned int i;

  TraceFunctionEntry(__func__);
  TraceFunctionParamListEnd();

  TraceStipulation(root_slice);

  stip_structure_traversal_init(&st,&threat_handler_inserters,&state);
  stip_traverse_structure(root_slice,&st);

  for (i = 0; i<=maxply; ++i)
  {
    threat_lengths[i] = no_threats_found;
    threats[i] = table_nil;
    threat_activities[i] = threat_idle;
  }

  TraceFunctionExit(__func__);
  TraceFunctionResultEnd();
}
