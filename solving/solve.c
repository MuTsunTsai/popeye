#include "solving/solve.h"
#include "stipulation/fork.h"
#include "pyproof.h"
#include "conditions/amu/mate_filter.h"
#include "conditions/amu/attack_counter.h"
#include "conditions/anticirce/circuit_special.h"
#include "conditions/anticirce/exchange_filter.h"
#include "conditions/anticirce/exchange_special.h"
#include "conditions/anticirce/target_square_filter.h"
#include "conditions/bgl.h"
#include "conditions/blackchecks.h"
#include "conditions/extinction.h"
#include "conditions/circe/circuit_by_rebirth_special.h"
#include "conditions/circe/exchange_by_rebirth_special.h"
#include "conditions/circe/steingewinn_filter.h"
#include "conditions/circe/assassin.h"
#include "conditions/circe/frischauf.h"
#include "conditions/circe/super.h"
#include "conditions/circe/april.h"
#include "conditions/circe/king_rebirth_avoider.h"
#include "conditions/exclusive.h"
#include "conditions/ohneschach/legality_tester.h"
#include "conditions/maff/immobility_tester.h"
#include "conditions/ohneschach/immobility_tester.h"
#include "conditions/owu/immobility_tester.h"
#include "conditions/ultraschachzwang/goal_filter.h"
#include "conditions/ultraschachzwang/legality_tester.h"
#include "conditions/singlebox/type1.h"
#include "conditions/singlebox/type2.h"
#include "conditions/singlebox/type3.h"
#include "conditions/patience.h"
#include "conditions/isardam.h"
#include "conditions/sat.h"
#include "conditions/dynasty.h"
#include "conditions/masand.h"
#include "conditions/oscillating_kings.h"
#include "conditions/messigny.h"
#include "conditions/actuated_revolving_centre.h"
#include "conditions/actuated_revolving_board.h"
#include "conditions/republican.h"
#include "conditions/circe/capture_fork.h"
#include "conditions/circe/rebirth_handler.h"
#include "conditions/circe/cage.h"
#include "conditions/circe/kamikaze.h"
#include "conditions/circe/parrain.h"
#include "conditions/circe/volage.h"
#include "conditions/circe/promotion.h"
#include "conditions/anticirce/rebirth_handler.h"
#include "conditions/anticirce/super.h"
#include "conditions/sentinelles.h"
#include "conditions/duellists.h"
#include "conditions/haunted_chess.h"
#include "conditions/ghost_chess.h"
#include "conditions/kobul.h"
#include "conditions/andernach.h"
#include "conditions/antiandernach.h"
#include "conditions/chameleon_pursuit.h"
#include "conditions/norsk.h"
#include "conditions/protean.h"
#include "conditions/einstein/einstein.h"
#include "conditions/einstein/reverse.h"
#include "conditions/einstein/anti.h"
#include "conditions/traitor.h"
#include "conditions/volage.h"
#include "conditions/magic_square.h"
#include "conditions/tibet.h"
#include "conditions/degradierung.h"
#include "conditions/phantom.h"
#include "conditions/marscirce/anti.h"
#include "conditions/line_chameleon.h"
#include "conditions/haan.h"
#include "conditions/castling_chess.h"
#include "conditions/exchange_castling.h"
#include "conditions/transmuting_kings/super.h"
#include "optimisations/hash.h"
#include "conditions/imitator.h"
#include "conditions/football.h"
#include "conditions/castling_chess.h"
#include "optimisations/keepmating.h"
#include "optimisations/count_nr_opponent_moves/opponent_moves_counter.h"
#include "optimisations/goals/castling/filter.h"
#include "optimisations/goals/enpassant/filter.h"
#include "optimisations/intelligent/duplicate_avoider.h"
#include "optimisations/intelligent/limit_nr_solutions_per_target.h"
#include "optimisations/intelligent/mate/filter.h"
#include "optimisations/intelligent/mate/goalreachable_guard.h"
#include "optimisations/intelligent/moves_left.h"
#include "optimisations/intelligent/proof.h"
#include "optimisations/intelligent/stalemate/filter.h"
#include "optimisations/intelligent/stalemate/goalreachable_guard.h"
#include "optimisations/intelligent/stalemate/immobilise_black.h"
#include "optimisations/killer_move/collector.h"
#include "optimisations/killer_move/final_defense_move.h"
#include "optimisations/orthodox_mating_moves/orthodox_mating_move_generator.h"
#include "options/maxsolutions/guard.h"
#include "options/maxsolutions/initialiser.h"
#include "options/maxtime.h"
#include "options/movenumbers.h"
#include "options/degenerate_tree.h"
#include "options/nontrivial.h"
#include "options/maxthreatlength.h"
#include "options/maxflightsquares.h"
#include "options/movenumbers/restart_guard_intelligent.h"
#include "options/no_short_variations/no_short_variations_attacker_filter.h"
#include "options/stoponshortsolutions/filter.h"
#include "options/stoponshortsolutions/initialiser.h"
#include "output/plaintext/end_of_phase_writer.h"
#include "output/plaintext/illegal_selfcheck_writer.h"
#include "output/plaintext/line/end_of_intro_series_marker.h"
#include "output/plaintext/tree/end_of_solution_writer.h"
#include "output/plaintext/line/line_writer.h"
#include "output/plaintext/move_inversion_counter.h"
#include "output/plaintext/tree/check_writer.h"
#include "output/plaintext/tree/goal_writer.h"
#include "output/plaintext/tree/key_writer.h"
#include "output/plaintext/tree/move_writer.h"
#include "output/plaintext/tree/refutation_writer.h"
#include "output/plaintext/tree/refuting_variation_writer.h"
#include "output/plaintext/tree/threat_writer.h"
#include "output/plaintext/tree/try_writer.h"
#include "output/plaintext/tree/zugzwang_writer.h"
#include "pieces/attributes/paralysing/mate_filter.h"
#include "pieces/attributes/paralysing/stalemate_special.h"
#include "pieces/attributes/neutral/initialiser.h"
#include "pieces/attributes/neutral/half.h"
#include "pieces/attributes/hurdle_colour_changing.h"
#include "pieces/attributes/magic.h"
#include "pieces/attributes/chameleon.h"
#include "pieces/attributes/kamikaze/kamikaze.h"
#include "solving/avoid_unsolvable.h"
#include "solving/battle_play/continuation.h"
#include "solving/battle_play/min_length_guard.h"
#include "solving/battle_play/min_length_optimiser.h"
#include "solving/battle_play/threat.h"
#include "solving/battle_play/try.h"
#include "solving/battle_play/threat.h"
#include "solving/castling.h"
#include "solving/capture_counter.h"
#include "solving/find_by_increasing_length.h"
#include "solving/find_move.h"
#include "solving/find_shortest.h"
#include "solving/for_each_move.h"
#include "solving/fork_on_remaining.h"
#include "solving/king_move_generator.h"
#include "solving/legal_move_counter.h"
#include "solving/move_generator.h"
#include "solving/non_king_move_generator.h"
#include "solving/play_suppressor.h"
#include "solving/single_move_generator.h"
#include "solving/single_move_generator_with_king_capture.h"
#include "solving/single_piece_move_generator.h"
#include "solving/trivial_end_filter.h"
#include "solving/en_passant.h"
#include "solving/moving_pawn_promotion.h"
#include "solving/selfcheck_guard.h"
#include "solving/post_move_iteration.h"
#include "solving/move_effect_journal.h"
#include "stipulation/has_solution_type.h"
#include "stipulation/move_inverter.h"
#include "stipulation/battle_play/attack_adapter.h"
#include "stipulation/battle_play/branch.h"
#include "stipulation/battle_play/defense_adapter.h"
#include "stipulation/boolean/and.h"
#include "stipulation/boolean/false.h"
#include "stipulation/boolean/not.h"
#include "stipulation/boolean/or.h"
#include "stipulation/boolean/true.h"
#include "stipulation/if_then_else.h"
#include "stipulation/constraint.h"
#include "stipulation/dead_end.h"
#include "stipulation/dummy_move.h"
#include "stipulation/end_of_branch_goal.h"
#include "stipulation/end_of_branch.h"
#include "stipulation/goals/any/reached_tester.h"
#include "stipulation/goals/capture/reached_tester.h"
#include "stipulation/goals/castling/reached_tester.h"
#include "stipulation/goals/check/reached_tester.h"
#include "stipulation/goals/chess81/reached_tester.h"
#include "stipulation/goals/circuit_by_rebirth/reached_tester.h"
#include "stipulation/goals/circuit/reached_tester.h"
#include "stipulation/goals/countermate/filter.h"
#include "stipulation/goals/countermate/reached_tester.h"
#include "stipulation/goals/doublemate/filter.h"
#include "stipulation/goals/doublemate/reached_tester.h"
#include "stipulation/goals/enpassant/reached_tester.h"
#include "stipulation/goals/exchange_by_rebirth/reached_tester.h"
#include "stipulation/goals/exchange/reached_tester.h"
#include "stipulation/goals/immobile/reached_tester.h"
#include "stipulation/goals/notcheck/reached_tester.h"
#include "stipulation/goals/prerequisite_optimiser.h"
#include "stipulation/goals/proofgame/reached_tester.h"
#include "stipulation/goals/reached_tester.h"
#include "stipulation/goals/steingewinn/reached_tester.h"
#include "stipulation/goals/target/reached_tester.h"
#include "stipulation/goals/doublemate/king_capture_avoider.h"
#include "stipulation/help_play/adapter.h"
#include "stipulation/move_player.h"
#include "stipulation/move_played.h"
#include "stipulation/setplay_fork.h"
#include "debugging/trace.h"
#include "debugging/measure.h"

#include <assert.h>

/* Try to solve in n half-moves.
 * @param si slice index
 * @param n maximum number of half moves
 * @return length of solution found and written, i.e.:
 *            slack_length-2 the move just played or being played is illegal
 *            <=n length of shortest solution found
 *            n+2 no solution found
 */
stip_length_type solve(slice_index si, stip_length_type n)
{
  stip_length_type result;

  TraceFunctionEntry(__func__);
  TraceFunctionParam("%u",si);
  TraceFunctionParam("%u",n);
  TraceFunctionParamListEnd();

  TraceEnumerator(slice_type,slices[si].type,"\n");
  switch (slices[si].type)
  {
    case STThreatSolver:
      result = threat_solver_solve(si,n);
      break;

    case STDummyMove:
      result = dummy_move_solve(si,n);
      break;

    case STThreatCollector:
      result = threat_collector_solve(si,n);
      break;

    case STThreatEnforcer:
      result = threat_enforcer_solve(si,n);
      break;

    case STThreatDefeatedTester:
      result = threat_defeated_tester_solve(si,n);
      break;

    case STThreatWriter:
      result = threat_writer_solve(si,n);
      break;

    case STZugzwangWriter:
      result = zugzwang_writer_solve(si,n);
      break;

    case STKeyWriter:
      result = key_writer_solve(si,n);
      break;

    case STTryWriter:
      result = try_writer_solve(si,n);
      break;

    case STRefutationsAllocator:
      result = refutations_allocator_solve(si,n);
      break;

    case STRefutationsSolver:
      result = refutations_solver_solve(si,n);
      break;

    case STRefutationsIntroWriter:
      result = refutations_intro_writer_solve(si,n);
      break;

    case STRefutationsAvoider:
      result = refutations_avoider_solve(si,n);
      break;

    case STRefutationsFilter:
      result = refutations_filter_solve(si,n);
      break;

    case STRefutingVariationWriter:
      result = refuting_variation_writer_solve(si,n);
      break;

    case STMoveWriter:
      result = move_writer_solve(si,n);
      break;

    case STTrivialEndFilter:
      result = trivial_end_filter_solve(si,n);
      break;

    case STNoShortVariations:
      result = no_short_variations_solve(si,n);
      break;

    case STOr:
      result = or_solve(si,n);
      break;

    case STFindShortest:
      result = find_shortest_solve(si,n);
      break;

    case STMoveGenerator:
      result = move_generator_solve(si,n);
      break;

    case STForEachAttack:
      result = for_each_attack_solve(si,n);
      break;

    case STFindAttack:
      result = find_attack_solve(si,n);
      break;

    case STForEachDefense:
      result = for_each_defense_solve(si,n);
      break;

    case STFindDefense:
      result = find_defense_solve(si,n);
      break;

    case STNullMovePlayer:
      result = null_move_player_solve(si,n);
      break;

    case STPostMoveIterationInitialiser:
      result = post_move_iteration_initialiser_solve(si,n);
      break;

    case STMoveEffectJournalUndoer:
      result = move_effect_journal_undoer_solve(si,n);
      break;

    case STMessignyMovePlayer:
      result = messigny_move_player_solve(si,n);
      break;

    case STCastlingPlayer:
      result = castling_player_solve(si,n);
      break;

    case STMovePlayer:
      result = move_player_solve(si,n);
      break;

    case STEnPassantAdjuster:
      result = en_passant_adjuster_solve(si,n);
      break;

    case STMovingPawnPromoter:
      result = moving_pawn_promoter_solve(si,n);
      break;

    case STPhantomChessEnPassantAdjuster:
      result = phantom_en_passant_adjuster_solve(si,n);
      break;

    case STAntiMarsCirceEnPassantAdjuster:
      result = antimars_en_passant_adjuster_solve(si,n);
      break;

    case STKamikazeCapturingPieceRemover:
      result = kamikaze_capturing_piece_remover_solve(si,n);
      break;

    case STHaanChessHoleInserter:
      result = haan_chess_hole_inserter_solve(si,n);
      break;

    case STCastlingChessMovePlayer:
      result = castling_chess_move_player_solve(si,n);
      break;

    case STExchangeCastlingMovePlayer:
      result = exchange_castling_move_player_solve(si,n);
      break;

    case STSuperTransmutingKingTransmuter:
      result = supertransmuting_kings_transmuter_solve(si,n);
      break;

    case STAMUAttackCounter:
      result = amu_attack_counter_solve(si,n);
      break;

    case STMutualCastlingRightsAdjuster:
      result = mutual_castling_rights_adjuster_solve(si,n);
      break;

    case STImitatorMover:
      result = imitator_mover_solve(si,n);
      break;

    case STMovingPawnToImitatorPromoter:
      result = moving_pawn_to_imitator_promoter_solve(si,n);
      break;

    case STAttackPlayed:
      result = attack_played_solve(si,n);
      break;

#if defined(DOTRACE)
    case STMoveTracer:
      result = move_tracer_solve(si,n);
      break;
#endif

#if defined(DOMEASURE)
    case STMoveCounter:
      result = move_counter_solve(si,n);
      break;
#endif

    case STOrthodoxMatingMoveGenerator:
      result = orthodox_mating_move_generator_solve(si,n);
      break;

    case STDeadEnd:
    case STDeadEndGoal:
      result = dead_end_solve(si,n);
      break;

    case STMinLengthOptimiser:
      result = min_length_optimiser_solve(si,n);
      break;

    case STForkOnRemaining:
      result = fork_on_remaining_solve(si,n);
      break;

    case STAttackHashed:
      result = attack_hashed_solve(si,n);
      break;

    case STEndOfBranch:
    case STEndOfBranchForced:
    case STEndOfBranchTester:
      result = end_of_branch_solve(si,n);
      break;

    case STEndOfBranchGoal:
    case STEndOfBranchGoalImmobile:
    case STEndOfBranchGoalTester:
      result = end_of_branch_goal_solve(si,n);
      break;

    case STGoalReachedTester:
      result = goal_reached_tester_solve(si,n);
      break;

    case STAvoidUnsolvable:
      result = avoid_unsolvable_solve(si,n);
      break;

    case STResetUnsolvable:
      result = reset_unsolvable_solve(si,n);
      break;

    case STLearnUnsolvable:
      result = learn_unsolvable_solve(si,n);
      break;

    case STConstraintSolver:
    case STConstraintTester:
    case STGoalConstraintTester:
      result = constraint_solve(si,n);
      break;

    case STSelfCheckGuard:
      result = selfcheck_guard_solve(si,n);
      break;

    case STKeepMatingFilter:
      result = keepmating_filter_solve(si,n);
      break;

    case STOutputPlaintextTreeCheckWriter:
      result = output_plaintext_tree_check_writer_solve(si,n);
      break;

    case STRefutationWriter:
      result = refutation_writer_solve(si,n);
      break;

    case STDoubleMateFilter:
      result = doublemate_filter_solve(si,n);
      break;

    case STCounterMateFilter:
      result = countermate_filter_solve(si,n);
      break;

    case STEnPassantFilter:
      result = enpassant_filter_solve(si,n);
      break;

    case STCastlingFilter:
      result = castling_filter_solve(si,n);
      break;

    case STPrerequisiteOptimiser:
      result = goal_prerequisite_optimiser_solve(si,n);
      break;

    case STOutputPlaintextTreeGoalWriter:
      result = output_plaintext_tree_goal_writer_solve(si,n);
      break;

    case STOutputPlaintextLineLineWriter:
      result = output_plaintext_line_line_writer_solve(si,n);
      break;

    case STBGLFilter:
      result = bgl_filter_solve(si,n);
      break;

    case STMasandRecolorer:
      result = masand_recolorer_solve(si,n);
      break;

    case STActuatedRevolvingCentre:
      result = actuated_revolving_centre_solve(si,n);
      break;

    case STActuatedRevolvingBoard:
      result = actuated_revolving_board_solve(si,n);
      break;

    case STRepublicanKingPlacer:
      result = republican_king_placer_solve(si,n);
      break;

    case STRepublicanType1DeadEnd:
      result = republican_type1_dead_end_solve(si,n);
      break;

    case STCirceKingRebirthAvoider:
      result = circe_king_rebirth_avoider_solve(si,n);
      break;

    case STCirceCaptureFork:
      result = circe_capture_fork_solve(si,n);
      break;

    case STCirceRebirthHandler:
      result = circe_rebirth_handler_solve(si,n);
      break;

    case STAprilAprilFork:
      result = april_chess_fork_solve(si,n);
      break;

    case STSuperCirceNoRebirthFork:
      result = supercirce_no_rebirth_fork_solve(si,n);
      break;

    case STSuperCirceRebirthHandler:
      result = supercirce_rebirth_handler_solve(si,n);
      break;

    case STCirceRebirthPromoter:
      result = circe_promoter_solve(si,n);
      break;

    case STCirceVolageRecolorer:
      result = circe_volage_recolorer_solve(si,n);
      break;

    case STCirceParrainRebirthHandler:
      result = circe_parrain_rebirth_handler_solve(si,n);
      break;

    case STCirceKamikazeRebirthHandler:
      result = circe_kamikaze_rebirth_handler_solve(si,n);
      break;

    case STCirceCageNoCageFork:
      result = circe_cage_no_cage_fork_solve(si,n);
      break;

    case STCirceCageCageTester:
      result = circe_cage_cage_tester_solve(si,n);
      break;

    case STSentinellesInserter:
      result = sentinelles_inserter_solve(si,n);
      break;

    case STMagicViewsInitialiser:
      result = magic_views_initialiser_solve(si,n);
      break;

    case STMagicPiecesRecolorer:
      result = magic_pieces_recolorer_solve(si,n);
      break;

    case STHauntedChessGhostSummoner:
      result = haunted_chess_ghost_summoner_solve(si,n);
      break;

    case STHauntedChessGhostRememberer:
      result = haunted_chess_ghost_rememberer_solve(si,n);
      break;

    case STGhostChessGhostRememberer:
      result = ghost_chess_ghost_rememberer_solve(si,n);
      break;

    case STAndernachSideChanger:
      result = andernach_side_changer_solve(si,n);
      break;

    case STAntiAndernachSideChanger:
      result = antiandernach_side_changer_solve(si,n);
      break;

    case STChameleonPursuitSideChanger:
      result = chameleon_pursuit_side_changer_solve(si,n);
      break;

    case STNorskArrivingAdjuster:
      result = norsk_arriving_adjuster_solve(si,n);
      break;

    case STProteanPawnAdjuster:
      result = protean_pawn_adjuster_solve(si,n);
      break;

    case STEinsteinArrivingAdjuster:
      result = einstein_moving_adjuster_solve(si,n);
      break;

    case STReverseEinsteinArrivingAdjuster:
      result = reverse_einstein_moving_adjuster_solve(si,n);
      break;

    case STAntiEinsteinArrivingAdjuster:
      result = anti_einstein_moving_adjuster_solve(si,n);
      break;

    case STTraitorSideChanger:
      result = traitor_side_changer_solve(si,n);
      break;

    case STVolageSideChanger:
      result = volage_side_changer_solve(si,n);
      break;

    case STMagicSquareSideChanger:
      result = magic_square_side_changer_solve(si,n);
      break;

    case STTibetSideChanger:
      result = tibet_solve(si,n);
      break;

    case STDoubleTibetSideChanger:
      result = double_tibet_solve(si,n);
      break;

    case STDegradierungDegrader:
      result = degradierung_degrader_solve(si,n);
      break;

    case STPromoteMovingIntoChameleon:
      result = chameleon_promote_moving_into_solve(si,n);
      break;

    case STPromoteCirceRebornIntoChameleon:
      result = chameleon_promote_circe_reborn_into_solve(si,n);
      break;

    case STPromoteAnticirceRebornIntoChameleon:
      result = chameleon_promote_anticirce_reborn_into_solve(si,n);
      break;

    case STChameleonArrivingAdjuster:
      result = chameleon_arriving_adjuster_solve(si,n);
      break;

    case STLineChameleonArrivingAdjuster:
      result = line_chameleon_arriving_adjuster_solve(si,n);
      break;

    case STFrischaufPromoteeMarker:
      result = frischauf_promotee_marker_solve(si,n);
      break;

    case STPiecesHalfNeutralRecolorer:
      result = half_neutral_recolorer_solve(si,n);
      break;

    case STKobulKingSubstitutor:
      result = kobul_king_substitutor_solve(si,n);
      break;

    case STDuellistsRememberDuellist:
      result = duellists_remember_duellist_solve(si,n);
      break;

    case STSingleboxType2LatentPawnSelector:
      result = singlebox_type2_latent_pawn_selector_solve(si,n);
      break;

    case STSingleboxType2LatentPawnPromoter:
      result = singlebox_type2_latent_pawn_promoter_solve(si,n);
      break;

    case STAnticirceRebirthHandler:
      result = anticirce_rebirth_handler_solve(si,n);
      break;

    case STAnticirceRebornPromoter:
      result = anticirce_reborn_promoter_solve(si,n);
      break;

    case STAntisupercirceRebirthHandler:
      result = antisupercirce_rebirth_handler_solve(si,n);
      break;

    case STFootballChessSubsitutor:
      result = football_chess_substitutor_solve(si,n);
      break;

    case STRefutationsCollector:
      result = refutations_collector_solve(si,n);
      break;

    case STMinLengthGuard:
      result = min_length_guard_solve(si,n);
      break;

    case STAttackHashedTester:
      result = attack_hashed_tester_solve(si,n);
      break;

    case STDegenerateTree:
      result = degenerate_tree_solve(si,n);
      break;

    case STMaxNrNonTrivialCounter:
      result = max_nr_nontrivial_counter_solve(si,n);
      break;

    case STKillerDefenseCollector:
      result = killer_defense_collector_solve(si,n);
      break;

    case STFindByIncreasingLength:
      result = find_by_increasing_length_solve(si,n);
      break;

    case STHelpMovePlayed:
      result = help_move_played_solve(si,n);
      break;

    case STHelpHashed:
      result = help_hashed_solve(si,n);
      break;

    case STIntelligentMovesLeftInitialiser:
      result = intelligent_moves_left_initialiser_solve(si,n);
      break;

    case STRestartGuardIntelligent:
      result = restart_guard_intelligent_solve(si,n);
      break;

    case STIntelligentTargetCounter:
      result = intelligent_target_counter_solve(si,n);
      break;

    case STIntelligentMateFilter:
      result = intelligent_mate_filter_solve(si,n);
      break;

    case STIntelligentStalemateFilter:
      result = intelligent_stalemate_filter_solve(si,n);
      break;

    case STIntelligentProof:
      result = intelligent_proof_solve(si,n);
      break;

    case STIntelligentLimitNrSolutionsPerTargetPos:
      result = intelligent_limit_nr_solutions_per_target_position_solve(si,n);
      break;

    case STGoalReachableGuardFilterMate:
      result = goalreachable_guard_mate_solve(si,n);
      break;

    case STGoalReachableGuardFilterStalemate:
      result = goalreachable_guard_stalemate_solve(si,n);
      break;

    case STGoalReachableGuardFilterProof:
      result = goalreachable_guard_proofgame_solve(si,n);
      break;

    case STGoalReachableGuardFilterProofFairy:
      result = goalreachable_guard_proofgame_fairy_solve(si,n);
      break;

    case STRestartGuard:
      result = restart_guard_solve(si,n);
      break;

    case STMaxTimeGuard:
      result = maxtime_guard_solve(si,n);
      break;

    case STMaxSolutionsCounter:
      result = maxsolutions_counter_solve(si,n);
      break;

    case STMaxSolutionsGuard:
      result = maxsolutions_guard_solve(si,n);
      break;

    case STStopOnShortSolutionsFilter:
      result = stoponshortsolutions_solve(si,n);
      break;

    case STIfThenElse:
      result = if_then_else_solve(si,n);
      break;

    case STHelpHashedTester:
      result = help_hashed_tester_solve(si,n);
      break;

    case STFlightsquaresCounter:
      result = flightsquares_counter_solve(si,n);
      break;

    case STKingMoveGenerator:
      result = king_move_generator_solve(si,n);
      break;

    case STNonKingMoveGenerator:
      result = non_king_move_generator_solve(si,n);
      break;

    case STLegalMoveCounter:
    case STAnyMoveCounter:
      result = legal_move_counter_solve(si,n);
      break;

    case STCaptureCounter:
      result = capture_counter_solve(si,n);
      break;

    case STOhneschachCheckGuard:
      result = ohneschach_check_guard_solve(si,n);
      break;

    case STExclusiveChessUnsuspender:
      result = exclusive_chess_unsuspender_solve(si,n);
      break;

    case STSingleMoveGeneratorWithKingCapture:
      result = single_move_generator_with_king_capture_solve(si,n);
      break;

    case STSinglePieceMoveGenerator:
      result = single_piece_move_generator_solve(si,n);
      break;

    case STCastlingIntermediateMoveGenerator:
      result = castling_intermediate_move_generator_solve(si,n);
      break;

    case STCastlingRightsAdjuster:
      result = castling_rights_adjuster_solve(si,n);
      break;

    case STSingleMoveGenerator:
      result = single_move_generator_solve(si,n);
      break;

    case STOpponentMovesCounter:
      result = opponent_moves_counter_solve(si,n);
      break;

    case STIntelligentImmobilisationCounter:
      result = intelligent_immobilisation_counter_solve(si,n);
      break;

    case STIntelligentDuplicateAvoider:
      result = intelligent_duplicate_avoider_solve(si,n);
      break;

    case STIntelligentSolutionsPerTargetPosCounter:
      result = intelligent_nr_solutions_per_target_position_counter_solve(si,n);
      break;

    case STSetplayFork:
      result = setplay_fork_solve(si,n);
      break;

    case STAttackAdapter:
      result = attack_adapter_solve(si,n);
      break;

    case STDefenseAdapter:
      result = defense_adapter_solve(si,n);
      break;

    case STHelpAdapter:
      result = help_adapter_solve(si,n);
      break;

    case STAnd:
      result = and_solve(si,n);
      break;

    case STNot:
      result = not_solve(si,n);
      break;

    case STMoveInverter:
      result = move_inverter_solve(si,n);
      break;

    case STMaxSolutionsInitialiser:
      result = maxsolutions_initialiser_solve(si,n);
      break;

    case STStopOnShortSolutionsInitialiser:
      result = stoponshortsolutions_initialiser_solve(si,n);
      break;

    case STIllegalSelfcheckWriter:
      result = illegal_selfcheck_writer_solve(si,n);
      break;

    case STEndOfPhaseWriter:
      result = end_of_phase_writer_solve(si,n);
      break;

    case STOutputPlaintextMoveInversionCounter:
      result = output_plaintext_move_inversion_counter_solve(si,n);
      break;

    case STOutputPlaintextLineEndOfIntroSeriesMarker:
      result = output_plaintext_line_end_of_intro_series_marker_solve(si,n);
      break;

    case STPiecesParalysingMateFilter:
      result = paralysing_mate_filter_solve(si,n);
      break;

    case STPiecesParalysingStalemateSpecial:
      result = paralysing_stalemate_special_solve(si,n);
      break;

    case STAmuMateFilter:
      result = amu_mate_filter_solve(si,n);
      break;

    case STUltraschachzwangGoalFilter:
      result = ultraschachzwang_goal_filter_solve(si,n);
      break;

    case STCirceSteingewinnFilter:
      result = circe_steingewinn_filter_solve(si,n);
      break;

    case STCirceCircuitSpecial:
      result = circe_circuit_special_solve(si,n);
      break;

    case STCirceExchangeSpecial:
      result = circe_exchange_special_solve(si,n);
      break;

    case STAnticirceTargetSquareFilter:
      result = anticirce_target_square_filter_solve(si,n);
      break;

    case STAnticirceCircuitSpecial:
      result = anticirce_circuit_special_solve(si,n);
      break;

    case STAnticirceExchangeSpecial:
      result = anticirce_exchange_special_solve(si,n);
      break;

    case STAnticirceExchangeFilter:
      result = anticirce_exchange_filter_solve(si,n);
      break;

    case STTemporaryHackFork:
      result = solve(slices[si].next1,length_unspecified);
      break;

    case STGoalTargetReachedTester:
      result = goal_target_reached_tester_solve(si,n);
      break;

    case STGoalCheckReachedTester:
      result = goal_check_reached_tester_solve(si,n);
      break;

    case STGoalCaptureReachedTester:
      result = goal_capture_reached_tester_solve(si,n);
      break;

    case STGoalSteingewinnReachedTester:
      result = goal_steingewinn_reached_tester_solve(si,n);
      break;

    case STGoalEnpassantReachedTester:
      result = goal_enpassant_reached_tester_solve(si,n);
      break;

    case STGoalDoubleMateReachedTester:
      result = goal_doublemate_reached_tester_solve(si,n);
      break;

    case STGoalCounterMateReachedTester:
      result = goal_countermate_reached_tester_solve(si,n);
      break;

    case STGoalCastlingReachedTester:
      result = goal_castling_reached_tester_solve(si,n);
      break;

    case STGoalCircuitReachedTester:
      result = goal_circuit_reached_tester_solve(si,n);
      break;

    case STGoalExchangeReachedTester:
      result = goal_exchange_reached_tester_solve(si,n);
      break;

    case STGoalCircuitByRebirthReachedTester:
      result = goal_circuit_by_rebirth_reached_tester_solve(si,n);
      break;

    case STGoalExchangeByRebirthReachedTester:
      result = goal_exchange_by_rebirth_reached_tester_solve(si,n);
      break;

    case STGoalProofgameReachedTester:
    case STGoalAToBReachedTester:
      result = goal_proofgame_reached_tester_solve(si,n);
      break;

    case STGoalImmobileReachedTester:
      result = goal_immobile_reached_tester_solve(si,n);
      break;

    case STImmobilityTester:
      result = immobility_tester_solve(si,n);
      break;

    case STMaffImmobilityTesterKing:
      result = maff_immobility_tester_king_solve(si,n);
      break;

    case STOWUImmobilityTesterKing:
      result = owu_immobility_tester_king_solve(si,n);
      break;

    case STGoalNotCheckReachedTester:
      result = goal_notcheck_reached_tester_solve(si,n);
      break;

    case STGoalAnyReachedTester:
      result = goal_any_reached_tester_solve(si,n);
      break;

    case STGoalChess81ReachedTester:
      result = goal_chess81_reached_tester_solve(si,n);
      break;

    case STPiecesParalysingMateFilterTester:
      result = paralysing_mate_filter_tester_solve(si,n);
      break;

    case STBlackChecks:
      result = blackchecks_solve(si,n);
      break;

    case STExtinctionRememberThreatened:
      result = extinction_remember_threatened_solve(si,n);
      break;

    case STExtinctionTester:
      result = extinction_tester_solve(si,n);
      break;

    case STSingleBoxType1LegalityTester:
      result = singlebox_type1_legality_tester_solve(si,n);
      break;

    case STSingleBoxType2LegalityTester:
      result = singlebox_type2_legality_tester_solve(si,n);
      break;

    case STSingleBoxType3LegalityTester:
      result = singlebox_type3_legality_tester_solve(si,n);
      break;

    case STSingleBoxType3PawnPromoter:
      result = singlebox_type3_pawn_promoter_solve(si,n);
      break;

    case STExclusiveChessLegalityTester:
      result = exclusive_chess_legality_tester_solve(si,n);
      break;

    case STOhneschachLegalityTester:
      result = ohneschach_legality_tester_solve(si,n);
      break;

    case STUltraschachzwangLegalityTester:
      result = ultraschachzwang_legality_tester_solve(si,n);
      break;

    case STIsardamLegalityTester:
      result = isardam_legality_tester_solve(si,n);
      break;

    case STCirceAssassinRebirth:
      result = circe_assassin_rebirth_solve(si,n);
      break;

    case STKingCaptureAvoider:
      result = king_capture_avoider_solve(si,n);
      break;

    case STPatienceChessLegalityTester:
      result = patience_chess_legality_tester_solve(si,n);
      break;

    case STPiecesNeutralInitialiser:
      result = neutral_initialiser_solve(si,n);
      break;

    case STPiecesNeutralRetractingRecolorer:
      result = neutral_retracting_recolorer_solve(si,n);
      break;

    case STPiecesNeutralReplayingRecolorer:
      result = neutral_replaying_recolorer_solve(si,n);
      break;

    case STSATFlightMoveGenerator:
      result = sat_flight_moves_generator_solve(si,n);
      break;

    case STStrictSATUpdater:
      result = strict_sat_updater_solve(si,n);
      break;

    case STDynastyKingSquareUpdater:
      result = dynasty_king_square_updater_solve(si,n);
      break;

    case STHurdleColourChanger:
      result = hurdle_colour_changer_solve(si,n);
      break;

    case STKingOscillator:
      result = king_oscillator_solve(si,n);
      break;

    case STPlaySuppressor:
      result = play_suppressor_solve(si,n);
      break;

    case STContinuationSolver:
      result = continuation_solver_solve(si,n);
      break;

    case STDefensePlayed:
      result = defense_played_solve(si,n);
      break;

    case STMaxFlightsquares:
      result = maxflight_guard_solve(si,n);
      break;

    case STMaxNrNonTrivial:
      result = max_nr_nontrivial_guard_solve(si,n);
      break;

    case STEndOfSolutionWriter:
      result = end_of_solution_writer_solve(si,n);
      break;

    case STKillerMoveFinalDefenseMove:
      result = killer_move_final_defense_move_solve(si,n);
      break;

    case STMaxThreatLength:
      result = maxthreatlength_guard_solve(si,n);
      break;

    case STKillerAttackCollector:
      result = killer_attack_collector_solve(si,n);
      break;

    case STTrue:
      result = slack_length;
      break;

    case STFalse:
      result = n+2;
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