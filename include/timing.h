/*
 * The timing module provides a method with a simplified interface
 * to easily pause the calling thread for an specified duration.
 */

#ifndef TIMING_H_GAURD
#define TIMING_H_GAURD

// Pause the thread execution for seconds + nanoseconds;
void Timing_nanoSleep(long seconds, long nanoseconds);

#endif