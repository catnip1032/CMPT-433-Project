/*
 * The timing module wraps the nanosleep function for easier usage.
 */

#include "../include/timing.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

void Timing_nanoSleep(long seconds, long nanoseconds)
{
  struct timespec delay = {seconds, nanoseconds};
  nanosleep(&delay, (struct timespec *)NULL);
}