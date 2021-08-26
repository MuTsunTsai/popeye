#if !defined(SOLVING_POST_MOVE_ITERATION_H)
#define SOLVING_POST_MOVE_ITERATION_H

#include "solving/machinery/solve.h"
#include "solving/observation.h"
#include "solving/ply.h"

/* This module provides functionality for coordinating "post move iterations"
 * (i.e. promotions, Supercirce rebirths etc.).
 */

void post_move_iteration_init_ply(void);
boolean post_move_iteration_ply_was_ended(void);

void post_move_iteration_end(void);

/* Solve the next pipe while post move iterating
 * @param si identifies the iterating slice
 */
void post_move_iteration_solve_delegate(slice_index si);
void post_move_iteration_solve_fork(slice_index si);

/* Recurse into the same pipe while post move iterating
 * @param si identifies the iterating slice
 */
void post_move_iteration_solve_recurse(slice_index si);

/* Cancel post move iteration while taking back a move.
 * Useful in rare situations where e.g. promotion to queen is enough.
 */
void post_move_iteration_cancel(void);

/* Is post move iteration locked by an inner iteration?
 * @param *lock our lock
 * @return true iff is post move iteration is locked
 */
boolean post_move_iteration_is_locked(void);

/* Is the post move iterator holding an specific id iterating in the current ply?
 * @return true iff he is
 */
boolean post_move_am_i_iterating(void);

/* Determine whether the current post move iteration participant has the lock
 * @param true iff it has
 */
boolean post_move_have_i_lock(void);

/* Try to solve in solve_nr_remaining half-moves.
 * @param si slice index
 * @note assigns solve_result the length of solution found and written, i.e.:
 *            previous_move_is_illegal the move just played is illegal
 *            this_move_is_illegal     the move being played is illegal
 *            immobility_on_next_move  the moves just played led to an
 *                                     unintended immobility on the next move
 *            <=n+1 length of shortest solution found (n+1 only if in next
 *                                     branch)
 *            n+2 no solution found in this branch
 *            n+3 no solution found in next branch
 *            (with n denominating solve_nr_remaining)
 */
void move_execution_post_move_iterator_solve(slice_index si);

/* Iterate if move generation uses a post-move iterating slice (e.g. for
 * MarsCirce Super)
 * @param si identifies move generator slice
 */
void move_generation_post_move_iterator_solve(slice_index si);

/* Iterate if square observation testing uses a post-move iterating slice (e.g.
 * for MarsCirce Super)
 * @param si identifies move generator slice
 */
void square_observation_post_move_iterator_solve(slice_index si);

/* Determine whether a square is observed by a side
 * @param side observing side
 * @param s the square
 * @param evaluate identifies the set of restrictions imposed on the observation
 * @return true iff the square is observed
 */
boolean is_square_observed_general_post_move_iterator_solve(Side side,
                                                            square s,
                                                            validator_id evaluate);

/* Instrument the solving machinery with post move iteration slices
 * @param si identifies the root of the solving machinery
 */
void solving_insert_post_move_iteration(slice_index si);

#endif
