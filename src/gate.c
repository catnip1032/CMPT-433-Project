#include "../include/gate.h"
#include "../include/servo.h"
#include "../include/timing.h"

#include <stdlib.h>

// Servo max and min duty cycles
// ----------------------------------------------------------------------------
static const char *MIN_MICRO_SERVO = "1000000";		// clockwise
static const char *MAX_MICRO_SERVO = "2000000";	// counterclockwise

// Function prototype declarations
// ----------------------------------------------------------------------------
static void enableGates(void);

static void unenableGates(void);

// Public Functions
// ----------------------------------------------------------------------------

void Gate_init(void)
{
	// Enable gates
  Timing_nanoSleep(0, 500000000);
	enableGates();
  Timing_nanoSleep(0, 500000000);
	// Set gate 1 to raised position
	Gate_raisesGate(gate1);
	// Set gate 2 to raised position
	Gate_raisesGate(gate2);
  Timing_nanoSleep(0, 500000000);
}

void Gate_cleanup(void)
{
	// Set gate 1 to raised position
  Timing_nanoSleep(0, 500000000);
	Gate_lowersGate(gate1);
	// Set gate 2 to raised position
	Gate_lowersGate(gate2);
	// Unenable gates
  Timing_nanoSleep(0, 500000000);
	unenableGates();
}

void Gate_raisesGate(eGateNum _gateToRaise)
{
	// Gets the correct servo
	Servo servo = Servo_getServo(_gateToRaise);
	// Set gate to 1000000
	Servo_changeDutyCycle(servo, MIN_MICRO_SERVO);
}

void Gate_lowersGate(eGateNum _gateToLower)
{
	// Gets the correct servo
	Servo servo = Servo_getServo(_gateToLower);
	// Set gate to 2000000
	Servo_changeDutyCycle(servo, MAX_MICRO_SERVO);
}

// Private Functions
// ----------------------------------------------------------------------------

static void enableGates(void)
{
	// Enable gate 1
	Servo servo1 = Servo_getServo(gate1);
	Servo_enableSignal(servo1, "1");
	// Enable gate 1
	Servo servo2 = Servo_getServo(gate2);
	Servo_enableSignal(servo2, "1");
}

static void unenableGates(void)
{
	// Unenable gate 1
	Servo servo1 = Servo_getServo(gate1);
	Servo_enableSignal(servo1, "0");
	// Unenable gate 1
	Servo servo2 = Servo_getServo(gate2);
	Servo_enableSignal(servo2, "0");
}
