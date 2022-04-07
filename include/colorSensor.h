/* The color sensor module provides an abstract interface to read the
 * red, blue, and green color values from the TCS34725 color sensor.
 * I2c is used to communicate with the device. The I2c bus number
 * is provided by the client code, and the address of the
 * color sensor is hard-coded by the manufacturer. */

#include <stdint.h>

#ifndef COLORSENSOR_GAURD
#define COLORSENSOR_GAURD
typedef enum {
  COLOR_SENSOR_RED,
  COLOR_SENSOR_GREEN,
  COLOR_SENSOR_BLUE,
} eColorSensorColor;

// i2cBusNum is the i2c bus number that the color sensor is attached to
void ColorSensor_init(int32_t _i2cBusNum);
void ColorSensor_cleanup(void);

/* Output buffer must be 5 int32_t's in size. The first element will contain
 * the red luminance, second green, third blue, fourth ir luminance, and the
 * fifth element will be the ambient light luminance in Lux units */
void ColorSensor_getLuminanceValuesInLux(int32_t *_pLuminanceValsOut);

/* Output buffer must be 3 int32_t's in size. The first element will contain
 * the red value, second green, third blue. Values are in the range [0,255]. */
void ColorSensor_getRgbValues(int32_t *_pRgbValuesOut);

// return the color that the sensor is picking up
eColorSensorColor ColorSensor_getColor();
#endif
