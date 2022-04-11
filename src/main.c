#include "../include/classifierModule.h"
#include "../include/gate.h"
#include "../include/lights.h"
#include "../include/pipe.h"
#include "../include/timing.h"

#include <getopt.h>
#include <signal.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void Main_initialize(void);
void Main_cleanup(int _singal);
void Main_setupInterrupt(void);
void Main_getOpts(int argc, char **argv);

void Main_stageIdle(void);
void Main_stageCategorizing(void);
void Main_stageSorting(void);
void Main_stageDisposing(void);
void Main_stageReturning(void);

eClassifierModule_RefuseItemType m_itemType;

static uint32_t m_colorSensorI2CNumber;
static bool m_colorSensorOptFlag = false;

static eGateNum m_gateToLower = gate1;
static bool m_closeGate = true;

// Main
// ----------------------------------------------------------------------------
int main(int argc, char **argv)
{
  Main_getOpts(argc, argv);
  if (!m_colorSensorOptFlag) {
    fprintf(stderr, "i2c bus number for color sensor not provided. Cannot \
continue execution.\n");
    exit(EXIT_FAILURE);
  }

  Main_initialize();

  while (true) {
    Main_stageIdle();
    Main_stageCategorizing();
    Main_stageSorting();
    Main_stageDisposing();
    Main_stageReturning();
  }
}

// Initialization/Termination functions
// ----------------------------------------------------------------------------
void Main_initialize(void)
{
  printf("Initializing recycler.");

  Main_setupInterrupt();

  Gate_init();
  Pipe_init();
  ClassifierModule_init(m_colorSensorI2CNumber);
  Lights_init();
}

void Main_cleanup(int _signal)
{
  printf("Terminating recycler.");

  Gate_cleanup();
  Pipe_cleanup();
  ClassifierModule_cleanup();
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

void Main_getOpts(int argc, char **argv)
{
  int opt;

  while ((opt = getopt(argc, argv, "i:h")) != -1) {
    switch (opt) {
    case 'i':
      m_colorSensorI2CNumber = atoi(optarg);
      m_colorSensorOptFlag = true;
      break;
    case 'h':
      printf("Call this program with '-i num', where num is the i2c bus \
number for the color sensor.");
      exit(EXIT_SUCCESS);
      break;
    case '?':
      printf("Unknown option %c.\n", optopt);
    case ':':
      printf("Missing argument for %c.\n", optopt);
    }
  }
}

// Stage functions
// ----------------------------------------------------------------------------

void Main_stageIdle(void)
{
  Lights_setIdle();
  ClassifierModule_waitUntilRefuseItemAppears();
}

void Main_stageCategorizing(void)
{
  Lights_setRecycling();
  m_itemType = ClassifierModule_getRefuseItemType();
}

void Main_stageSorting()
{
  Lights_setRecycling();

  m_closeGate = true;
  m_gateToLower = gate1;

  switch (m_itemType) {
  case CLASSIFIER_MODULE_RECYCLING:
    m_gateToLower = gate1;
    break;
  case CLASSIFIER_MODULE_GARBAGE:
    m_gateToLower = gate2;
    break;
  case CLASSIFIER_MODULE_COMPOST:
    m_closeGate = false;
    break;
  default:
    fprintf(stderr, "Uncaught classifier module type %d. Terminating.\n",
            m_itemType);
    abort();
  }

  if (m_closeGate) {
    Gate_lowersGate(m_gateToLower);
  }

  Timing_milliSleep(3, 0);
}

void Main_stageDisposing(void)
{
  Lights_setRecycled();
  Pipe_rotatePipeToDropBall();

  Timing_milliSleep(2, 0);
}

void Main_stageReturning(void)
{
  Lights_setReturning();
  Pipe_resetPipePosition();

  if (m_closeGate) {
    Gate_raisesGate(m_gateToLower);
  }
}
