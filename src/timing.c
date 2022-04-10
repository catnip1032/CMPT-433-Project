/*
 * The timing module wraps the nanosleep function from time.h for easier usage.
 */

#include "../include/timing.h"
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

void Timing_nanoSleep(int64_t _seconds, int64_t _nanoseconds)
{
  struct timespec delay = {_seconds, _nanoseconds};
  nanosleep(&delay, (struct timespec *)NULL);
}

void Timing_milliSleep(int64_t _seconds, int64_t _milliseconds)
{
  Timing_nanoSleep(_seconds, _milliseconds * 1000000);
}
