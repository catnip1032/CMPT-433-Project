#include "../include/pipe.h"
#include "../include/servo.h"
#include "../include/timing.h"
#include <stdlib.h>

// Servo max and min duty cycles
// ----------------------------------------------------------------------------
static const char *MIN_PIPE_SERVO = "470000";			// clockwise
static const char *MAX_PIPE_SERVO = "2300000";		// counterclockwise

// Servo index for pipe
//-----------------------------------------------------------------------------
static const int PIPE_SERVO_INDEX = 0;

// Function prototype declarations
// ----------------------------------------------------------------------------

static void enablePipe(void);

static void unenablePipe(void);

// Public Functions
// ----------------------------------------------------------------------------

void Pipe_init(void)
{
	// Enable gates
  Timing_nanoSleep(0, 500000000);
	enablePipe();
  Timing_nanoSleep(0, 500000000);
	// Ensure pipe is in initial position
	Pipe_resetPipePosition();
  Timing_nanoSleep(0, 500000000);
}

void Pipe_cleanup(void)
{
  Timing_nanoSleep(0, 500000000);
	// Ensure pipe goes back to initial position
	Pipe_resetPipePosition();
  Timing_nanoSleep(0, 500000000);
	unenablePipe();
}

void Pipe_resetPipePosition(void)
{
	// Gets the correct servo
	Servo servo = Servo_getServo(PIPE_SERVO_INDEX);
	// Set gate to 470000
	Servo_changeDutyCycle(servo, MIN_PIPE_SERVO);
}

void Pipe_rotatePipeToDropBall(void)
{
  // Gets the correct servo
	Servo servo = Servo_getServo(PIPE_SERVO_INDEX);
	// Set gate to 2300000
	Servo_changeDutyCycle(servo, MAX_PIPE_SERVO);
}

// Private Functions
// ----------------------------------------------------------------------------

static void enablePipe(void)
{
	// Enable pipe
	Servo servo1 = Servo_getServo(PIPE_SERVO_INDEX);
	Servo_enableSignal(servo1, "1");
}

static void unenablePipe(void)
{
	// Unenable pipe
	Servo servo1 = Servo_getServo(PIPE_SERVO_INDEX);
	Servo_enableSignal(servo1, "0");
}
