#include "../include/colorSensor.h"
#include "../include/i2c.h"
#include "../include/timing.h"

// Adapted from
// http://www.beaglebone.net/code/c/beaglebone-and-tcs34725-color-sensor-example-in-c.php

// Color Sensor I2c Device Address, Register Addresses and Bit Flag Values
// ----------------------------------------------------------------------------
static const int COLOR_SENSOR_DEVICE_ADDRESS = 0x29;

static const int SELECT_ENABLE_REGISTER_ADDRESS = 0x80;
static const int POWER_ON_RGBC_ENABLE_WAIT_TIME_DISABLE = 0x03;

static const int SELECT_ALS_TIME_REGISTER_ADDRESS = 0x81;
static const int ATIME_700MS = 0x00;

static const int SELECT_WAIT_TIME_REGISTER_ADDRESS = 0x83;
static const int WAIT_TIME_2POINT4_MS = 0xFF;

static const int SELECT_CONTROL_REGISTER_ADDRESS = 0x8F;
static const int AGAIN_1_TIME = 0x00;

static const int CDATA_LSB_REGISTER_ADDRESS = 0x94;
static const int NUM_BYTES_TO_READ_FROM_CDATA_REGISTER = 8;

// Value indices into luminance output buffer from getLuminanceValuesInLux
// ----------------------------------------------------------------------------
static const int RED_LUMINANCE_OUT_INDEX = 0;
static const int BLUE_LUMINANCE_OUT_INDEX = 1;
static const int GREEN_LUMINANCE_OUT_INDEX = 2;
static const int IR_LUMINANCE_OUT_INDEX = 3;
static const int AMBIENT_LIGHT_LUMINANCE_OUT_INDEX = 4;

// Constants used to convert from lux to rgb
// ----------------------------------------------------------------------------
static const int MAX_RED_LUMINANCE = 1000;
static const int MAX_GREEN_LUMINANCE = 1000;
static const int MAX_BLUE_LUMINANCE = 1000;

// Function Prototype declarations
// ----------------------------------------------------------------------------
static double ColorSensor_getMinValue(double val1, double val2);

static void
ColorSensor_regValsToRgbLuminanceValues(unsigned char *pRegisterBytesIn,
                                        int *pLuminanceValuesOut);
static void
ColorSensor_regValsToIrLuminanceValue(unsigned char *pRegisterBytesIn,
                                      int *pLuminanceValuesOut);
static void
ColorSensor_RGBValsToAmbientLightLuminanceValue(int red, int green, int blue,
                                                int *pLuminanceValuesOut);

// Static Variables
// ----------------------------------------------------------------------------
static int i2cFileBusDescriptor;

void ColorSensor_init(int i2cBusNum)
{
  i2cFileBusDescriptor =
      I2c_initI2cDevice(i2cBusNum, COLOR_SENSOR_DEVICE_ADDRESS);
  I2c_writeI2cReg(i2cFileBusDescriptor, SELECT_ENABLE_REGISTER_ADDRESS,
                  POWER_ON_RGBC_ENABLE_WAIT_TIME_DISABLE);
  I2c_writeI2cReg(i2cFileBusDescriptor, SELECT_ALS_TIME_REGISTER_ADDRESS,
                  ATIME_700MS);
  I2c_writeI2cReg(i2cFileBusDescriptor, SELECT_WAIT_TIME_REGISTER_ADDRESS,
                  WAIT_TIME_2POINT4_MS);
  I2c_writeI2cReg(i2cFileBusDescriptor, SELECT_CONTROL_REGISTER_ADDRESS,
                  AGAIN_1_TIME);

  Timing_nanoSleep(1, 0);
}

void ColorSensor_cleanup() { I2c_closeI2cDevice(i2cFileBusDescriptor); }

void ColorSensor_getRgbValues(int *pRgbValuesOut)
{
  int luminanceValuesOut[5];
  ColorSensor_getLuminanceValuesInLux(luminanceValuesOut);

  int redLuminance = luminanceValuesOut[RED_LUMINANCE_OUT_INDEX];
  int greenLuminance = luminanceValuesOut[GREEN_LUMINANCE_OUT_INDEX];
  int blueLuminance = luminanceValuesOut[BLUE_LUMINANCE_OUT_INDEX];

  pRgbValuesOut[0] = ColorSensor_getMinValue(
      ((double)redLuminance / MAX_RED_LUMINANCE) * 255, 255);
  pRgbValuesOut[1] = ColorSensor_getMinValue(
      ((double)greenLuminance / MAX_GREEN_LUMINANCE) * 255, 255);
  pRgbValuesOut[2] = ColorSensor_getMinValue(
      ((double)blueLuminance / MAX_BLUE_LUMINANCE) * 255, 255);
}

void ColorSensor_getLuminanceValuesInLux(int *pLuminanceValsOut)
{
  unsigned char regOutBuf[NUM_BYTES_TO_READ_FROM_CDATA_REGISTER];
  I2c_readI2cReg(i2cFileBusDescriptor, CDATA_LSB_REGISTER_ADDRESS, regOutBuf,
                 NUM_BYTES_TO_READ_FROM_CDATA_REGISTER);
  ColorSensor_regValsToRgbLuminanceValues(regOutBuf, pLuminanceValsOut);
  ColorSensor_regValsToIrLuminanceValue(regOutBuf, pLuminanceValsOut);
  ColorSensor_RGBValsToAmbientLightLuminanceValue(
      pLuminanceValsOut[RED_LUMINANCE_OUT_INDEX],
      pLuminanceValsOut[GREEN_LUMINANCE_OUT_INDEX],
      pLuminanceValsOut[BLUE_LUMINANCE_OUT_INDEX], pLuminanceValsOut);
}

static double ColorSensor_getMinValue(double val1, double val2)
{
  return val1 < val2 ? val1 : val2;
}

eColorSensorColor ColorSensor_getColor()
{
  int rgbValues[3];
  ColorSensor_getRgbValues(rgbValues);

  if (rgbValues[0] > rgbValues[1] && rgbValues[0] > rgbValues[2]) {
    return COLOR_SENSOR_RED;
  }
  else if (rgbValues[1] > rgbValues[0] && rgbValues[1] > rgbValues[2]) {
    return COLOR_SENSOR_GREEN;
  }
  else {
    return COLOR_SENSOR_BLUE;
  }
}

static void
ColorSensor_regValsToRgbLuminanceValues(unsigned char *pRegisterBytesIn,
                                        int *pLuminanceValuesOut)
{
  // Convert the RGB values from the registers
  int red = (pRegisterBytesIn[3] * 256 + pRegisterBytesIn[2]);
  int green = (pRegisterBytesIn[5] * 256 + pRegisterBytesIn[4]);
  int blue = (pRegisterBytesIn[7] * 256 + pRegisterBytesIn[6]);

  pLuminanceValuesOut[RED_LUMINANCE_OUT_INDEX] = red;
  pLuminanceValuesOut[GREEN_LUMINANCE_OUT_INDEX] = green;
  pLuminanceValuesOut[BLUE_LUMINANCE_OUT_INDEX] = blue;
}

static void
ColorSensor_regValsToIrLuminanceValue(unsigned char *pRegisterBytesIn,
                                      int *pLuminanceValuesOut)
{
  int cData = (pRegisterBytesIn[1] * 256 + pRegisterBytesIn[0]);

  pLuminanceValuesOut[IR_LUMINANCE_OUT_INDEX] = cData;
}

static void
ColorSensor_RGBValsToAmbientLightLuminanceValue(int red, int green, int blue,
                                                int *pLuminanceValuesOut)
{
  // Calculate ambient light luminance
  float luminance =
      (-0.32466) * (red) + (1.57837) * (green) + (-0.73191) * (blue);
  if (luminance < 0) {
    luminance = 0;
  }

  pLuminanceValuesOut[AMBIENT_LIGHT_LUMINANCE_OUT_INDEX] = luminance;
}