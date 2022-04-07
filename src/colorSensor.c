#include "../include/colorSensor.h"
#include "../include/i2c.h"
#include "../include/timing.h"

// Adapted from
// http://www.beaglebone.net/code/c/beaglebone-and-tcs34725-color-sensor-example-in-c.php

// Color Sensor I2c Device Address, Register Addresses and Bit Flag Values
// ----------------------------------------------------------------------------
static const int32_t COLOR_SENSOR_DEVICE_ADDRESS = 0x29;

static const int32_t SELECT_ENABLE_REGISTER_ADDRESS = 0x80;
static const int32_t POWER_ON_RGBC_ENABLE_WAIT_TIME_DISABLE = 0x03;

static const int32_t SELECT_ALS_TIME_REGISTER_ADDRESS = 0x81;
static const int32_t ATIME_700MS = 0x00;

static const int32_t SELECT_WAIT_TIME_REGISTER_ADDRESS = 0x83;
static const int32_t WAIT_TIME_2POINT4_MS = 0xFF;

static const int32_t SELECT_CONTROL_REGISTER_ADDRESS = 0x8F;
static const int32_t AGAIN_1_TIME = 0x00;

static const int32_t CDATA_LSB_REGISTER_ADDRESS = 0x94;
static const int32_t NUM_BYTES_TO_READ_FROM_CDATA_REGISTER = 8;

// Value indices into luminance output buffer from getLuminanceValuesInLux
// ----------------------------------------------------------------------------
static const int32_t RED_LUMINANCE_OUT_INDEX = 0;
static const int32_t BLUE_LUMINANCE_OUT_INDEX = 1;
static const int32_t GREEN_LUMINANCE_OUT_INDEX = 2;
static const int32_t IR_LUMINANCE_OUT_INDEX = 3;
static const int32_t AMBIENT_LIGHT_LUMINANCE_OUT_INDEX = 4;
static const int32_t LUMINANCE_OUTPUT_ARRAY_SIZE = 5;

// Function Prototype declarations
// ----------------------------------------------------------------------------
static double ColorSensor_getMaxValue(double val1, double val2);

static void
ColorSensor_regValsToRgbLuminanceValues(uint8_t *pRegisterBytesIn,
                                        int32_t *pLuminanceValuesOut);
static void ColorSensor_regValsToIrLuminanceValue(uint8_t *pRegisterBytesIn,
                                                  int32_t *pLuminanceValuesOut);
static void ColorSensor_RGBValsToAmbientLightLuminanceValue(
    int32_t red, int32_t green, int32_t blue, int32_t *pLuminanceValuesOut);

// Static Variables
// ----------------------------------------------------------------------------
static int32_t i2cFileBusDescriptor;

void ColorSensor_init(int32_t i2cBusNum)
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

void ColorSensor_getRgbValues(int32_t *pRgbValuesOut)
{
  int32_t luminanceValuesOut[LUMINANCE_OUTPUT_ARRAY_SIZE];
  ColorSensor_getLuminanceValuesInLux(luminanceValuesOut);

  int32_t redLuminance = luminanceValuesOut[RED_LUMINANCE_OUT_INDEX];
  int32_t greenLuminance = luminanceValuesOut[GREEN_LUMINANCE_OUT_INDEX];
  int32_t blueLuminance = luminanceValuesOut[BLUE_LUMINANCE_OUT_INDEX];

  // Get maximum RGB value (conversion is not perfect)
  int32_t maxRgbVal = ColorSensor_getMaxValue(
      ColorSensor_getMaxValue(redLuminance, greenLuminance), blueLuminance);

  pRgbValuesOut[0] = ((double)redLuminance / maxRgbVal) * 255;
  pRgbValuesOut[1] = ((double)greenLuminance / maxRgbVal) * 255;
  pRgbValuesOut[2] = ((double)blueLuminance / maxRgbVal) * 255;
}

void ColorSensor_getLuminanceValuesInLux(int32_t *pLuminanceValsOut)
{
  uint8_t regOutBuf[NUM_BYTES_TO_READ_FROM_CDATA_REGISTER];
  I2c_readI2cReg(i2cFileBusDescriptor, CDATA_LSB_REGISTER_ADDRESS, regOutBuf,
                 NUM_BYTES_TO_READ_FROM_CDATA_REGISTER);
  ColorSensor_regValsToRgbLuminanceValues(regOutBuf, pLuminanceValsOut);
  ColorSensor_regValsToIrLuminanceValue(regOutBuf, pLuminanceValsOut);
  ColorSensor_RGBValsToAmbientLightLuminanceValue(
      pLuminanceValsOut[RED_LUMINANCE_OUT_INDEX],
      pLuminanceValsOut[GREEN_LUMINANCE_OUT_INDEX],
      pLuminanceValsOut[BLUE_LUMINANCE_OUT_INDEX], pLuminanceValsOut);
}

static double ColorSensor_getMaxValue(double val1, double val2)
{
  return val1 > val2 ? val1 : val2;
}

eColorSensorColor ColorSensor_getColor()
{
  int32_t luminanceValuesOut[LUMINANCE_OUTPUT_ARRAY_SIZE];
  ColorSensor_getLuminanceValuesInLux(luminanceValuesOut);

  int32_t redLuminance = luminanceValuesOut[RED_LUMINANCE_OUT_INDEX];
  int32_t greenLuminance = luminanceValuesOut[GREEN_LUMINANCE_OUT_INDEX];
  int32_t blueLuminance = luminanceValuesOut[BLUE_LUMINANCE_OUT_INDEX];

  if (redLuminance > greenLuminance && redLuminance > blueLuminance) {
    return COLOR_SENSOR_RED;
  }
  else if (greenLuminance > redLuminance && greenLuminance > blueLuminance) {
    return COLOR_SENSOR_GREEN;
  }
  else {
    return COLOR_SENSOR_BLUE;
  }
}

static void
ColorSensor_regValsToRgbLuminanceValues(uint8_t *pRegisterBytesIn,
                                        int32_t *pLuminanceValuesOut)
{
  // Convert the RGB values from the registers
  int32_t red = (pRegisterBytesIn[3] * 256 + pRegisterBytesIn[2]);
  int32_t green = (pRegisterBytesIn[5] * 256 + pRegisterBytesIn[4]);
  int32_t blue = (pRegisterBytesIn[7] * 256 + pRegisterBytesIn[6]);

  pLuminanceValuesOut[RED_LUMINANCE_OUT_INDEX] = red;
  pLuminanceValuesOut[GREEN_LUMINANCE_OUT_INDEX] = green;
  pLuminanceValuesOut[BLUE_LUMINANCE_OUT_INDEX] = blue;
}

static void ColorSensor_regValsToIrLuminanceValue(uint8_t *pRegisterBytesIn,
                                                  int32_t *pLuminanceValuesOut)
{
  int32_t cData = (pRegisterBytesIn[1] * 256 + pRegisterBytesIn[0]);

  pLuminanceValuesOut[IR_LUMINANCE_OUT_INDEX] = cData;
}

static void ColorSensor_RGBValsToAmbientLightLuminanceValue(
    int32_t red, int32_t green, int32_t blue, int32_t *pLuminanceValuesOut)
{
  // Calculate ambient light luminance
  double luminance =
      (-0.32466) * (red) + (1.57837) * (green) + (-0.73191) * (blue);
  if (luminance < 0) {
    luminance = 0;
  }

  pLuminanceValuesOut[AMBIENT_LIGHT_LUMINANCE_OUT_INDEX] = (int32_t)luminance;
}