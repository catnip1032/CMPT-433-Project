/*
 * The timing module wraps the nanosleep function from time.h for easier usage.
 */

#include "../include/timing.h"
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

void Timing_nanoSleep(int64_t seconds, int64_t nanoseconds)
{
  struct timespec delay = {seconds, nanoseconds};
  nanosleep(&delay, (struct timespec *)NULL);
}