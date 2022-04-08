/* The classifier module is responsible for classifying the type of refuse
 * that is currently waiting on the ramp, so that the item can subsequently be
 * placed within the correct bin. */

#include <stdint.h>

#ifndef CLASSIFIER_MODULE_GAURD
#define CLASSIFIER_MODULE_GAURD

typedef enum {
  CLASSIFIER_MODULE_GARBAGE,
  CLASSIFIER_MODULE_COMPOST,
  CLASSIFIER_MODULE_RECYCLING,
} eClassifierModule_RefuseItemType;

void ClassifierModule_init(uint32_t _colorSensorI2cBusNumber);
void ClassifierModule_cleanup(void);

// Blocking function that returns once the refuse item is
// infront of the sensor on the ramp.
void ClassifierModule_waitUntilRefuseItemAppears(void);

// Returns the refuse type of the next refuse item waiting on the ramp.
eClassifierModule_RefuseItemType ClassifierModule_getRefuseItemType(void);

#endif