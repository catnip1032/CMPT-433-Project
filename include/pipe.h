/*
 * The pipe module acts an abstraction layer for the servo API, sepecified in
 * servo.h, so as to manipulate the movement of the TowerPro SG-5010 Servo in
 * order to physically turn the pipe on the Ball Sorter project.*/

#ifndef _PIPE_HEADER_H_
#define _PIPE_HEADER_H_

// Initialization/Termination functions
// ----------------------------------------------------------------------------
// Initializes the pipe by setting the position of the arm on the TowerPro
// SG-5010 to it's initial position.
void Pipe_init(void);
void Pipe_cleanup(void);

// Behavior functions
// ----------------------------------------------------------------------------
// Resets the pipe position by turning the arm attachment on the TowerPro
// SG-5010 clockwise until the arm reaches it's initial position.
void Pipe_resetPipePosition(void);

// Rotates the pipe position by turning the arm attachment on the TowerPro
// SG-5010 counterclockwise to it's most left position.
void Pipe_rotatePipeToDropBall(void);

#endif
