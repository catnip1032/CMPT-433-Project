/*
 * The gate module acts an abstraction layer for the servo API, sepecified in 
 * servo.h, so as to manipulate the movement of the 2 Micro Servo 98 SG90's in
 * order to physically lift and lower the gates on the pipe of the Ball Sorter
 * project.*/

#ifndef _GATE_HEADER_H_
#define _GATE_HEADER_H_

// Gate enumerator to specify between gates 1 and gate 2.
typedef enum gate {
  gate1,
  gate2
};

// Initializes the gates 1 and 2 by setting the position of the arms on the 
// Micro Servo 98 SG90's to their initial positions where the gates are up.
void Gate_init(void);

// Raises the gate respective of the gate number.
void Gate_raisesGate(gate gateToRaise);

// Lowers the gate respective of the gate number.
void Gate_lowersGate(gate gateToLower);

#endif
