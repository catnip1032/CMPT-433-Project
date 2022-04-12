#include "../include/colorSensor.h"
#include "../include/i2c.h"
#include "../include/timing.h"
#include <stdio.h>
#include <stdlib.h>

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
static const size_t LUMINANCE_OUTPUT_ARRAY_SIZE = 5;

// Function Prototype declarations
// ----------------------------------------------------------------------------
static double ColorSensor_getMaxValue(double _val1, double _val2);

static void
ColorSensor_regValsToRgbLuminanceValues(uint8_t *_pRegisterBytesIn,
                                        int32_t *_pLuminanceValuesOut);
static void
ColorSensor_regValsToIrLuminanceValue(uint8_t *_pRegisterBytesIn,
                                      int32_t *_pLuminanceValuesOut);
static void ColorSensor_RGBValsToAmbientLightLuminanceValue(
    int32_t _red, int32_t _green, int32_t _blue, int32_t *_pLuminanceValuesOut);

// Static Variables
// ----------------------------------------------------------------------------

// File descriptor to use color sensor I2C device
static int32_t m_i2cFileBusDescriptor;

// Baseline for detecting if no object is in front of the sensor
static int32_t m_baselineLuminance;

// Distance away from baseline to detect if object is in front
static uint32_t m_objectSensingThreshold;

// Number of readings to take for calibration
const size_t MAX_CALIBRATION_READINGS = 10;

// Interval between calibration reads
const uint64_t CALIBRATION_READ_INTERVAL_NS = 250000000; // 0.25 seconds

void ColorSensor_init(uint32_t _i2cBusNum, uint32_t _objectSensingThreshold)
{
  m_objectSensingThreshold = _objectSensingThreshold;
  m_i2cFileBusDescriptor =
      I2c_initI2cDevice(_i2cBusNum, COLOR_SENSOR_DEVICE_ADDRESS);
  I2c_writeI2cReg(m_i2cFileBusDescriptor, SELECT_ENABLE_REGISTER_ADDRESS,
                  POWER_ON_RGBC_ENABLE_WAIT_TIME_DISABLE);
  I2c_writeI2cReg(m_i2cFileBusDescriptor, SELECT_ALS_TIME_REGISTER_ADDRESS,
                  ATIME_700MS);
  I2c_writeI2cReg(m_i2cFileBusDescriptor, SELECT_WAIT_TIME_REGISTER_ADDRESS,
                  WAIT_TIME_2POINT4_MS);
  I2c_writeI2cReg(m_i2cFileBusDescriptor, SELECT_CONTROL_REGISTER_ADDRESS,
                  AGAIN_1_TIME);

  // Calibrate the color sensor based on its current environment
  ColorSensor_recalibrate();

  Timing_nanoSleep(1, 0);
}

void ColorSensor_recalibrate(void)
{
  int32_t readingsSum = 0;
  for (int i = 0; i < MAX_CALIBRATION_READINGS; ++i) {
    int32_t luminanceValuesOut[LUMINANCE_OUTPUT_ARRAY_SIZE];
    ColorSensor_getLuminanceValuesInLux(luminanceValuesOut);

    readingsSum += luminanceValuesOut[AMBIENT_LIGHT_LUMINANCE_OUT_INDEX];

    Timing_nanoSleep(0, CALIBRATION_READ_INTERVAL_NS);
  }

  m_baselineLuminance = readingsSum / MAX_CALIBRATION_READINGS;
}

void ColorSensor_cleanup(void)
{
  I2c_closeI2cDevice(m_i2cFileBusDescriptor);
}

void ColorSensor_getRgbValues(int32_t *_pRgbValuesOut)
{
  int32_t luminanceValuesOut[LUMINANCE_OUTPUT_ARRAY_SIZE];
  ColorSensor_getLuminanceValuesInLux(luminanceValuesOut);

  int32_t redLuminance = luminanceValuesOut[RED_LUMINANCE_OUT_INDEX];
  int32_t greenLuminance = luminanceValuesOut[GREEN_LUMINANCE_OUT_INDEX];
  int32_t blueLuminance = luminanceValuesOut[BLUE_LUMINANCE_OUT_INDEX];

  // Get maximum RGB value (conversion is not perfect)
  int32_t maxRgbVal = ColorSensor_getMaxValue(
      ColorSensor_getMaxValue(redLuminance, greenLuminance), blueLuminance);

  _pRgbValuesOut[0] = ((double)redLuminance / maxRgbVal) * 255;
  _pRgbValuesOut[1] = ((double)greenLuminance / maxRgbVal) * 255;
  _pRgbValuesOut[2] = ((double)blueLuminance / maxRgbVal) * 255;
}

void ColorSensor_getLuminanceValuesInLux(int32_t *_pLuminanceValsOut)
{
  uint8_t regOutBuf[NUM_BYTES_TO_READ_FROM_CDATA_REGISTER];
  I2c_readI2cReg(m_i2cFileBusDescriptor, CDATA_LSB_REGISTER_ADDRESS, regOutBuf,
                 NUM_BYTES_TO_READ_FROM_CDATA_REGISTER);
  ColorSensor_regValsToRgbLuminanceValues(regOutBuf, _pLuminanceValsOut);
  ColorSensor_regValsToIrLuminanceValue(regOutBuf, _pLuminanceValsOut);
  ColorSensor_RGBValsToAmbientLightLuminanceValue(
      _pLuminanceValsOut[RED_LUMINANCE_OUT_INDEX],
      _pLuminanceValsOut[GREEN_LUMINANCE_OUT_INDEX],
      _pLuminanceValsOut[BLUE_LUMINANCE_OUT_INDEX], _pLuminanceValsOut);
}

static double ColorSensor_getMaxValue(double _val1, double _val2)
{
  return _val1 > _val2 ? _val1 : _val2;
}

bool ColorSensor_isObjectInFrontOfSensor(void)
{
  int32_t luminanceValuesOut[LUMINANCE_OUTPUT_ARRAY_SIZE];
  ColorSensor_getLuminanceValuesInLux(luminanceValuesOut);

  return abs(m_baselineLuminance -
             luminanceValuesOut[AMBIENT_LIGHT_LUMINANCE_OUT_INDEX]) >=
         m_objectSensingThreshold;
}

eColorSensorColor ColorSensor_getColor(void)
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
ColorSensor_regValsToRgbLuminanceValues(uint8_t *_pRegisterBytesIn,
                                        int32_t *_pLuminanceValuesOut)
{
  // Convert the RGB values from the registers
  int32_t red = (_pRegisterBytesIn[3] * 256 + _pRegisterBytesIn[2]);
  int32_t green = (_pRegisterBytesIn[5] * 256 + _pRegisterBytesIn[4]);
  int32_t blue = (_pRegisterBytesIn[7] * 256 + _pRegisterBytesIn[6]);

  _pLuminanceValuesOut[RED_LUMINANCE_OUT_INDEX] = red;
  _pLuminanceValuesOut[GREEN_LUMINANCE_OUT_INDEX] = green;
  _pLuminanceValuesOut[BLUE_LUMINANCE_OUT_INDEX] = blue;
}

static void ColorSensor_regValsToIrLuminanceValue(uint8_t *_pRegisterBytesIn,
                                                  int32_t *_pLuminanceValuesOut)
{
  int32_t cData = (_pRegisterBytesIn[1] * 256 + _pRegisterBytesIn[0]);

  _pLuminanceValuesOut[IR_LUMINANCE_OUT_INDEX] = cData;
}

static void ColorSensor_RGBValsToAmbientLightLuminanceValue(
    int32_t _red, int32_t _green, int32_t _blue, int32_t *_pLuminanceValuesOut)
{
  // Calculate ambient light luminance
  double luminance =
      (-0.32466) * (_red) + (1.57837) * (_green) + (-0.73191) * (_blue);
  if (luminance < 0) {
    luminance = 0;
  }

  _pLuminanceValuesOut[AMBIENT_LIGHT_LUMINANCE_OUT_INDEX] = (int32_t)luminance;
}
