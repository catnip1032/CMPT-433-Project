#include "../include/classifierModule.h"
#include "../include/gate.h"
#include "../include/lights.h"
#include "../include/pipe.h"
#include "../include/servo.h"
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
static uint32_t m_objectSensingThreshold;

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
  printf("Initializing recycler.\n");

  Main_setupInterrupt();

  Servo_init();
  Gate_init();
  Pipe_init();
  ClassifierModule_init(m_colorSensorI2CNumber, m_objectSensingThreshold);
  Lights_init();
}

void Main_cleanup(int _signal)
{
  printf("Terminating recycler.");

  Gate_cleanup();
  Pipe_cleanup();
  Servo_cleanup();
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

  while ((opt = getopt(argc, argv, "t:i:h")) != -1) {
    switch (opt) {
    case 'i':
      m_colorSensorI2CNumber = atoi(optarg);
      m_colorSensorOptFlag = true;
      break;
    case 'h':
      printf("Call this program with '-i num', where num is the i2c bus \
      number for the color sensor. Use the '-t num' to set the object sensing \
			threshold.");
      exit(EXIT_SUCCESS);
      break;
		case 't':
			m_objectSensingThreshold = atoi(optarg);
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
  printf("\nEntering idle stage.\n");
  Lights_setIdle();
  ClassifierModule_waitUntilRefuseItemAppears();
  printf("Object detected!\n");

	// Wait until the ball is in place to get a better color reading
	Timing_milliSleep(0, 700);
}

void Main_stageCategorizing(void)
{
  printf("\nEntering sorting stage.\n");
  Lights_setRecycling();
  m_itemType = ClassifierModule_getRefuseItemType();

  char *objectTypeStr;
  switch (m_itemType) {
  case CLASSIFIER_MODULE_GARBAGE:
    objectTypeStr = "garbage";
    break;
  case CLASSIFIER_MODULE_COMPOST:
    objectTypeStr = "compost";
    break;
  case CLASSIFIER_MODULE_RECYCLING:
    objectTypeStr = "recycling";
    break;
  default:
    objectTypeStr = "unknown";
  }

  printf("Object of type %s detected.\n", objectTypeStr);
}

void Main_stageSorting()
{
  printf("\nEntering sorting stage\n");
  Lights_setRecycling();

  switch (m_itemType) {
  case CLASSIFIER_MODULE_COMPOST:
    printf("Compost, lowering gate %d.\n", gate1);
    Gate_lowersGate(gate1);
    break;

  case CLASSIFIER_MODULE_RECYCLING: // Should be lowering both gates
    printf("Recycling, lowering gate %d and %d.\n", gate1, gate2);
    Gate_lowersGate(gate1);
    Gate_lowersGate(gate2);
    break;

  case CLASSIFIER_MODULE_GARBAGE:
    printf("Garbage, not lowering any gates.\n");
    break;

  default:
    fprintf(stderr, "Uncaught classifier module type %d. Terminating.\n",
            m_itemType);
    abort();
  }

  Timing_milliSleep(3, 0);
}

void Main_stageDisposing(void)
{
  printf("\nEntering disposal stage.\n");
  Lights_setRecycled();

  printf("Rotating pipe to drop the object.\n");
  Pipe_rotatePipeToDropBall();

  Timing_milliSleep(2, 0);
}

void Main_stageReturning(void)
{
  printf("\nEntering return stage.\n");
  Lights_setReturning();

  printf("Rotating the pipe to original position.\n");
  Pipe_resetPipePosition();

	// Wait for pipe to return before fully
	Timing_milliSleep(1, 500);

	printf("Rising gates to their original positions.\n");
	Gate_raisesGate(gate1);
	Gate_raisesGate(gate2);
}
