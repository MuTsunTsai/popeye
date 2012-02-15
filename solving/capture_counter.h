#if !defined(SOLVING_LEGAL_CAPTURE_COUNTER_H)
#define SOLVING_LEGAL_CAPTURE_COUNTER_H

#include "stipulation/help_play/play.h"

/* This module provides functionality dealing with the attacking side
 * in STCaptureCounter stipulation slices.
 */

/* current value of the count */
extern unsigned int capture_counter_count;

/* stop the move iteration once capture_counter_count exceeds this number */
extern unsigned int capture_counter_interesting;

/* Allocate a STCaptureCounter slice.
 * @return index of allocated slice
 */
slice_index alloc_capture_counter_slice(void);

/* Solve in a number of half-moves
 * @param si identifies slice
 * @param n exact number of half moves until end state has to be reached
 * @return length of solution found, i.e.:
 *         n+4 the move leading to the current position has turned out
 *             to be illegal
 *         n+2 no solution found
 *         n   solution found
 */
stip_length_type capture_counter_help(slice_index si, stip_length_type n);

#endif
