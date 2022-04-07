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
#include <stdbool.h>
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
static void setI2cBusPinsToI2cMode(int busNum);
static void getDataAndClockPinsForI2cBusNum(int busNum, int *pPinNumsOut);
static void getBusNameFromBusNum(char *pBusNameBuffer, int bufferSize,
                                 int busNum);
static void setI2cBusPinToI2cMode(const char *p9DataPinStr);
static int setI2CDeviceToSlaveAddress(const char *pBusName, int deviceAddress);
static void writeRegAddrToI2cBus(int i2cFileDesc, unsigned char regAddr);
static void readBytesFromRegAddr(int i2cFileDesc, unsigned char *pValueOutput,
                                 int sizeofValueOutput);

int I2c_initI2cDevice(int busNum, int deviceAddress)
{
  setI2cBusPinsToI2cMode(busNum);

  const int BUFFER_SIZE = sizeof(I2CDRV_LINUX_BUS_FORMAT);
  char pBusName[BUFFER_SIZE];
  getBusNameFromBusNum(pBusName, BUFFER_SIZE, busNum);

  int i2cFileDesc = setI2CDeviceToSlaveAddress(pBusName, deviceAddress);

  return i2cFileDesc;
}

void I2c_closeI2cDevice(int i2cFileDesc) { close(i2cFileDesc); }

static void setI2cBusPinsToI2cMode(int busNum)
{
  const int NUM_BUS_PINS = 2;
  int pinNumsForBusNum[NUM_BUS_PINS];
  getDataAndClockPinsForI2cBusNum(busNum, pinNumsForBusNum);

  // Get P9 header pin strings for bus data and clock pins
  char p9DataPinStr[50];
  snprintf(p9DataPinStr, 50, "P9_%d", pinNumsForBusNum[0]);

  char p9ClockPinStr[50];
  snprintf(p9ClockPinStr, 50, "P9_%d", pinNumsForBusNum[1]);

  setI2cBusPinToI2cMode(p9DataPinStr);
  setI2cBusPinToI2cMode(p9ClockPinStr);
}

static void getDataAndClockPinsForI2cBusNum(int busNum, int *pPinNumsOut)
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

static void getBusNameFromBusNum(char *pBusNameBuffer, int bufferSize,
                                 int busNum)
{
  snprintf(pBusNameBuffer, bufferSize, I2CDRV_LINUX_BUS_FORMAT, busNum);
}

static int setI2CDeviceToSlaveAddress(const char *pBusName, int deviceAddress)
{
  int i2cFileDesc = open(pBusName, O_RDWR);
  int result = ioctl(i2cFileDesc, I2C_SLAVE, deviceAddress);
  if (result < 0) {
    perror("I2C: Unable to set I2C device to slave address.");
    exit(1);
  }

  return i2cFileDesc;
}

void I2c_writeI2cReg(int i2cFileDesc, unsigned char regAddr,
                     unsigned char value)
{
  unsigned char pBuff[2];
  pBuff[0] = regAddr;
  pBuff[1] = value;
  int res = write(i2cFileDesc, pBuff, 2);
  if (res != 2) {
    perror("I2C: Unable to write i2c register.");
    exit(1);
  }
}

void I2c_readI2cReg(int i2cFileDesc, unsigned char regAddr,
                    unsigned char *pBufferOut, int numBytesToRead)
{
  // To read a register, must first write the address to the I2C bus
  writeRegAddrToI2cBus(i2cFileDesc, regAddr);

  readBytesFromRegAddr(i2cFileDesc, pBufferOut, numBytesToRead);
}

static void setI2cBusPinToI2cMode(const char *p9DataPinStr)
{
  const char *argsData[] = {p9DataPinStr, "i2c"};
  const char *errMessage = Shell_execCommand(
      "/usr/bin/config-pin", argsData, sizeof(argsData) / sizeof(argsData[0]));
  if (errMessage != NULL) {
    printf("There was an error setting P9 header pins to I2C mode!: %s",
           errMessage);
    exit(-1);
  }
}

static void writeRegAddrToI2cBus(int i2cFileDesc, unsigned char regAddr)
{
  int res = write(i2cFileDesc, &regAddr, sizeof(regAddr));
  if (res != sizeof(regAddr)) {
    perror("I2C: Unable to write to i2c register.");
    exit(1);
  }
}

static void readBytesFromRegAddr(int i2cFileDesc, unsigned char *pValueOutput,
                                 int sizeofValueOutput)
{
  int res = read(i2cFileDesc, pValueOutput, sizeofValueOutput);
  if (res != sizeofValueOutput) {
    perror("I2C: Unable to read from i2c register");
    exit(1);
  }
}