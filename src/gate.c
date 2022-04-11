#include "../include/gate.h"
#include "../include/servo.h"
#include <stdlib.h>

void Gate_init(void)
{
	// Set gate 1 to raised position
	Gate_raisesGate(gate1);

	// Set gate 2 to raised position
	Gate_raisesGate(gate2);
}

void Gate_raisesGate(eGateNum gateToRaise)
{
	// Set gate to 1000000

}

void Gate_lowersGate(eGateNum gateToLower)
{
	
}
