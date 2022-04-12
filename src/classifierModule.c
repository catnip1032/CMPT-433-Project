/* The classifier module is responsible for classifying the type of refuse
 * that is currently waiting on the ramp, so that it can subsequently be
 * placed within the correct bin. */

#include "../include/classifierModule.h"
#include "../include/colorSensor.h"
#include "../include/timing.h"
#include <stdint.h>

static const uint32_t WAIT_UNTIL_REFUSE_ITEM_APPEARS_SLEEP_INTERVAL_MS =
    150; // 0.15 sec

void ClassifierModule_init(uint32_t _colorSensorI2cBusNumber,
  uint32_t _objectSensingThreshold)
{
  ColorSensor_init(_colorSensorI2cBusNumber, _objectSensingThreshold);
}

void ClassifierModule_cleanup(void)
{
  ColorSensor_cleanup();
}

// Blocking function that returns once the refuse is in front of the sensor
// on the ramp.
void ClassifierModule_waitUntilRefuseItemAppears(void)
{
  bool hasRefuseAppeared;
  do {
    hasRefuseAppeared = ColorSensor_isObjectInFrontOfSensor();
    Timing_milliSleep(0, WAIT_UNTIL_REFUSE_ITEM_APPEARS_SLEEP_INTERVAL_MS);
  } while (!hasRefuseAppeared);
}

// Returns the current color of the next refuse item waiting on the ramp.
eClassifierModule_RefuseItemType ClassifierModule_getRefuseItemType(void)
{
  eColorSensorColor refuseColor = ColorSensor_getColor();

  switch (refuseColor) {
  case COLOR_SENSOR_RED:
    return CLASSIFIER_MODULE_GARBAGE;
  case COLOR_SENSOR_GREEN:
    return CLASSIFIER_MODULE_COMPOST;
  case COLOR_SENSOR_BLUE:
    return CLASSIFIER_MODULE_RECYCLING;
  default:
    return CLASSIFIER_MODULE_GARBAGE;
  }
}
