#include "../include/gate.h"
#include "../include/pipe.h"
#include "../include/lights.h"

#include <stdio.h>
#include <signal.h>

int main()
{

}

void _initialize(void) {
  printf("Initializing recycler.");

  _setupInterrupt();

  Gate_init(); 
  Pipe_init();
  Lights_init();
}

void _terminate(void) {
  printf("Terminating recycler.");
  
  Gate_cleanup();
  Pipe_cleanup();
  Lights_cleanup();
}


void _setupInterrupt(void) {
  struct sigaction action;
  struct sigaction oldAction;
  memset(&action, 0, sizeof(action));
  action.sa_handler = &_terminate;
  sigaction(SIGINT, &action, &oldAction);
}
