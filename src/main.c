#include "../include/gate.h"
#include "../include/lights.h"
#include "../include/pipe.h"

#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void Main_initialize(void);
void Main_cleanup(int _singal);
void Main_setupInterrupt(void);

void Main_stageIdle(void);
void Main_stageCategorizing(void);
void Main_stageSorting(void);
void Main_stageDisposing(void);
void Main_stageReturning(void);

int main() {}

// Initialization/Termination functions
// ----------------------------------------------------------------------------
void Main_initialize(void)
{
  printf("Initializing recycler.");

  Main_setupInterrupt();

  Gate_init();
  Pipe_init();
  Lights_init();
}

void Main_cleanup(int _signal)
{
  printf("Terminating recycler.");

  Gate_cleanup();
  Pipe_cleanup();
  Lights_cleanup();
}

void Main_setupInterrupt(void)
{
  struct sigaction action;
  struct sigaction oldAction;
  memset(&action, 0, sizeof(action));
  action.sa_handler = &Main_cleanup;
  sigaction(SIGINT, &action, &oldAction);
}

// Stage functions
// ----------------------------------------------------------------------------

// TODO: Implement
void Main_stageIdle(void) {}

// TODO: Implement
void Main_stageCategorizing(void) {}

// TODO: Implement
void Main_stageSorting(void) {}

// TODO: Implement
void Main_stageDisposing(void) {}

// TODO: Implement
void Main_stageReturning(void) {}
