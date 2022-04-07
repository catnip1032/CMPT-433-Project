/*
 * The timing module provides a method with a simplified interface
 * to easily pause the calling thread for an specified duration.
 */

#include <stdint.h>

#ifndef TIMING_H_GAURD
#define TIMING_H_GAURD

// Pause the thread execution for seconds + nanoseconds;
void Timing_nanoSleep(int64_t _seconds, int64_t _nanoseconds);

#endif
