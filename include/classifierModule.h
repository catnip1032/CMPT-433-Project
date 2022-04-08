/* The classifier module is responsible for classifying the color of ball
 * that is currently waiting on the ramp, so that the ball can subsequently be
 * placed within the correct bin. */

#include <stdint.h>

#ifndef CLASSIFIER_MODULE_GAURD
#define CLASSIFIER_MODULE_GAURD

typedef enum {
  CLASSIFIER_MODULE_RED_BALL,
  CLASSIFIER_MODULE_GREEN_BALL,
  CLASSIFIER_MODULE_BLUE_BALL,
} eClassifierModule_BallColor;

void ClassifierModule_init(uint32_t _colorSensorI2cBusNumber);
void ClassifierModule_cleanup(void);

// Blocking function that returns once the ball is infront of the color sensor
// on the ramp.
void ClassifierModule_waitUntilBallAppears(void);

// Returns the current color of the next ball waiting on the ramp.
eClassifierModule_BallColor ClassifierModule_getBallColor(void);

#endif