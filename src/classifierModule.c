/* The classifier module is responsible for classifying the type of recycling
 * that is currently waiting on the ramp, so that it can subsequently be
 * placed within the correct bin. */

#include "../include/classifierModule.h"
#include "../include/colorSensor.h"
#include "../include/timing.h"
#include <stdint.h>

static const uint32_t WAIT_UNTIL_RECYCLING_APPEARS_SLEEP_INTERVAL_NS =
    250000000; // 0.25 sec

void ClassifierModule_init(uint32_t _colorSensorI2cBusNumber)
{
  ColorSensor_init(_colorSensorI2cBusNumber);
}

void ClassifierModule_cleanup(void)
{
  ColorSensor_cleanup();
}

// Blocking function that returns once the recycling is infront of the sensor
// on the ramp.
void ClassifierModule_waitUntilRecyclingItemAppears(void)
{
  bool hasRecyclingAppeared;
  do {
    hasRecyclingAppeared = ColorSensor_isObjectInFrontOfSensor();
    Timing_nanoSleep(0, WAIT_UNTIL_RECYCLING_APPEARS_SLEEP_INTERVAL_NS);
  } while (!hasRecyclingAppeared);
}

// Returns the current color of the next recycling item waiting on the ramp.
eClassifierModule_RecyclingType ClassifierModule_getRecyclingType(void)
{
  eColorSensorColor recyclingColor = ColorSensor_getColor();

  switch (recyclingColor) {
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