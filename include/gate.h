/*
 * The gate module acts an abstraction layer for the servo API, sepecified in 
 * servo.h, so as to manipulate the movement of the 2 Micro Servo 98 SG90's in
 * order to physically lift and lower the gates on the pipe of the Ball Sorter
 * project.*/

#ifndef _GATE_HEADER_H_
#define _GATE_HEADER_H_

// Initializes the gates 1 and 2 by setting the position of the arms on the 
// Micro Servo 98 SG90's to their initial positions where the gates are up.
void Gate_init(void);

// Raises the gate respective of the gate number.
void Gate_raisesGate(const int gateNumber);

// Lowers the gate respective of the gate number.
void Gate_lowersGate(const int gateNumber);

#endif