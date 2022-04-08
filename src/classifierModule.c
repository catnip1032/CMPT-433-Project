/* The classifier module is responsible for classifying the color of ball
 * that is currently waiting on the ramp, so that the ball can subsequently be
 * placed within the correct bin. */

#include "../include/classifierModule.h"
#include "../include/colorSensor.h"
#include "../include/timing.h"
#include <stdint.h>

static const uint32_t WAIT_UNTIL_BALL_APPEARS_SLEEP_INTERVAL_NS =
    250000000; // 0.25 sec

void ClassifierModule_init(uint32_t _colorSensorI2cBusNumber)
{
  ColorSensor_init(_colorSensorI2cBusNumber);
}

void ClassifierModule_cleanup(void)
{
  ColorSensor_cleanup();
}

// Blocking function that returns once the ball is infront of the color sensor
// on the ramp.
void ClassifierModule_waitUntilBallAppears(void)
{
  bool hasBallAppeared;
  do {
    hasBallAppeared = ColorSensor_isObjectInFrontOfSensor();
    Timing_nanoSleep(0, WAIT_UNTIL_BALL_APPEARS_SLEEP_INTERVAL_NS);
  } while (!hasBallAppeared);
}

// Returns the current color of the next ball waiting on the ramp.
eClassifierModule_BallColor ClassifierModule_getBallColor(void)
{
  eColorSensorColor ballColor = ColorSensor_getColor();

  switch (ballColor) {
  case COLOR_SENSOR_RED:
    return CLASSIFIER_MODULE_RED_BALL;
  case COLOR_SENSOR_GREEN:
    return CLASSIFIER_MODULE_GREEN_BALL;
  case COLOR_SENSOR_BLUE:
    return CLASSIFIER_MODULE_BLUE_BALL;
  default:
    return CLASSIFIER_MODULE_RED_BALL;
  }
}