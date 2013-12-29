#if !defined(CONDITIONS_CIRCE_SYMMETRY_H)
#define CONDITIONS_CIRCE_SYMMETRY_H

/* This module implements Symmetry Circe */

#include "solving/solve.h"

/* Try to solve in n half-moves.
 * @param si slice index
 * @param n maximum number of half moves
 * @return length of solution found and written, i.e.:
 *            previous_move_is_illegal the move just played is illegal
 *            this_move_is_illegal     the move being played is illegal
 *            immobility_on_next_move  the moves just played led to an
 *                                     unintended immobility on the next move
 *            <=n+1 length of shortest solution found (n+1 only if in next
 *                                     branch)
 *            n+2 no solution found in this branch
 *            n+3 no solution found in next branch
 */
stip_length_type symmetry_circe_determine_rebirth_square_solve(slice_index si,
                                                               stip_length_type n);

/* Instrument the solving machinery with Symmetry Circe
 * @param si identifies root slice of solving machinery
 */
void symmetry_circe_initialise_solving(slice_index si);

#endif