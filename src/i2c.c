/*
 * The I2C Module contains an abstract interface to interact with I2C Devices
 * connected to an I2C bus. This includes intialization of
 * the I2C Bus Device, reading from and writing to the I2C Bus Device.
 */

#include "../include/i2c.h"
#include "../include/shell.h"
#include <assert.h>
#include <errno.h>
#include <fcntl.h>
#include <linux/i2c-dev.h>
#include <linux/i2c.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ioctl.h>
#include <sys/wait.h>
#include <unistd.h>

// Format for I2C bus node
static const char I2CDRV_LINUX_BUS_FORMAT[] = "/dev/i2c-%d";

// Static method prototypes
static void setI2cBusPinsToI2cMode(int32_t busNum);
static void getP9DataAndClockPinsForI2cBusNum(int32_t busNum,
                                              int32_t *pPinNumsOut);
static void getBusNameStringFromBusNum(char *pBusNameBuffer, int32_t bufferSize,
                                       int32_t busNum);
static void setI2cBusPinToI2cMode(const char *p9DataPinStr);
static int setI2CDeviceToSlaveAddress(const char *pBusName,
                                      int32_t deviceAddress);
static void writeRegAddrToI2cBus(int32_t i2cFileDesc, uint8_t regAddr);
static void readBytesFromRegAddr(int32_t i2cFileDesc, uint8_t *pValueOutput,
                                 int32_t sizeofValueOutput);

int32_t I2c_initI2cDevice(int32_t busNum, int32_t deviceAddress)
{
  setI2cBusPinsToI2cMode(busNum);

  const int32_t BUS_NAME_BUFFER_SIZE =
      sizeof(I2CDRV_LINUX_BUS_FORMAT) / sizeof(I2CDRV_LINUX_BUS_FORMAT[0]);

  char busName[BUS_NAME_BUFFER_SIZE];
  getBusNameStringFromBusNum(busName, BUS_NAME_BUFFER_SIZE, busNum);

  int32_t i2cFileDesc = setI2CDeviceToSlaveAddress(busName, deviceAddress);

  return i2cFileDesc;
}

static void setI2cBusPinsToI2cMode(int32_t busNum)
{
  const int32_t NUM_I2C_BUS_PINS = 2;
  int32_t dataAndClockPins[NUM_I2C_BUS_PINS];
  getP9DataAndClockPinsForI2cBusNum(busNum, dataAndClockPins);

  // Get P9 header pin strings for bus data and clock pins
  char p9DataPinStr[50];
  snprintf(p9DataPinStr, 50, "P9_%d", dataAndClockPins[0]);

  char p9ClockPinStr[50];
  snprintf(p9ClockPinStr, 50, "P9_%d", dataAndClockPins[1]);

  setI2cBusPinToI2cMode(p9DataPinStr);
  setI2cBusPinToI2cMode(p9ClockPinStr);
}

// Get bus name string from bus number like "/dev/i2c-1"
static void getBusNameStringFromBusNum(char *pBusNameBufferOut,
                                       int32_t bufferSize, int32_t busNum)
{
  snprintf(pBusNameBufferOut, bufferSize, I2CDRV_LINUX_BUS_FORMAT, busNum);
}

static int32_t setI2CDeviceToSlaveAddress(const char *pBusName,
                                          int32_t deviceAddress)
{
  int32_t i2cFileDesc = open(pBusName, O_RDWR);
  int32_t result = ioctl(i2cFileDesc, I2C_SLAVE, deviceAddress);
  if (result < 0) {
    perror("I2C: Unable to set I2C device to slave address.");
    exit(1);
  }

  return i2cFileDesc;
}

void I2c_closeI2cDevice(int32_t i2cFileDesc) { close(i2cFileDesc); }

// Get data and clock pins for the bus number on the P9 header
static void getP9DataAndClockPinsForI2cBusNum(int32_t busNum,
                                              int32_t *pPinNumsOut)
{
  switch (busNum) {
  case 1:
    pPinNumsOut[0] = 18;
    pPinNumsOut[1] = 17;
    break;
  case 2:
    pPinNumsOut[0] = 20;
    pPinNumsOut[1] = 19;
    break;
  default:
    assert(0);
  }
}

static void setI2cBusPinToI2cMode(const char *p9DataPinStr)
{
  const char *argsData[] = {p9DataPinStr, "i2c"};
  const char *errMessage = Shell_execCommand(
      "/usr/bin/config-pin", argsData, sizeof(argsData) / sizeof(argsData[0]));

  if (errMessage != NULL) {
    printf("There was an error setting P9 header pins to I2C mode!: %s",
           errMessage);
    exit(1);
  }
}

void I2c_writeI2cReg(int32_t i2cFileDesc, uint8_t regAddr, uint8_t value)
{
  uint8_t buff[2];
  buff[0] = regAddr;
  buff[1] = value;

  int32_t bytesWritten = write(i2cFileDesc, buff, 2);

  // Failed to write 2 bytes
  if (bytesWritten != 2) {
    perror("I2C: Unable to write to i2c register.");
    exit(1);
  }
}

void I2c_readI2cReg(int32_t i2cFileDesc, uint8_t regAddr, uint8_t *pBufferOut,
                    int32_t numBytesToRead)
{
  // To read a register, must first write the address to the I2C bus
  writeRegAddrToI2cBus(i2cFileDesc, regAddr);

  // Read numBytesToRead bytes into pBufferOut
  readBytesFromRegAddr(i2cFileDesc, pBufferOut, numBytesToRead);
}

static void writeRegAddrToI2cBus(int32_t i2cFileDesc, uint8_t regAddr)
{
  int32_t bytesWritten = write(i2cFileDesc, &regAddr, sizeof(regAddr));

  // If failed to write from I2C register, terminate program
  if (bytesWritten != sizeof(regAddr)) {
    perror("I2C: Unable to write to i2c register.");
    exit(1);
  }
}

static void readBytesFromRegAddr(int32_t i2cFileDesc, uint8_t *pValueOut,
                                 int32_t sizeofValueOutput)
{
  int32_t bytesRead = read(i2cFileDesc, pValueOut, sizeofValueOutput);

  // If failed to read from I2C register, terminate program
  if (bytesRead != sizeofValueOutput) {
    perror("I2C: Unable to read from i2c register");
    exit(1);
  }
}