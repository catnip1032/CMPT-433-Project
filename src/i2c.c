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
#include <stddef.h>
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
static void I2c_setI2cBusPinsToI2cMode(int32_t _busNum);
static void I2c_getP9DataAndClockPinsForI2cBusNum(int32_t _busNum,
                                                  int32_t *_pPinNumsOut);
static void I2c_getBusNameStringFromBusNum(char *_pBusNameBuffer,
                                           size_t _bufferSize, int32_t _busNum);
static void I2c_setI2cBusPinToI2cMode(const char *_p9DataPinStr);
static int I2c_setI2CDeviceToSlaveAddress(const char *_pBusName,
                                          int32_t _deviceAddress);
static void I2c_writeRegAddrToI2cBus(int32_t _i2cFileDesc, uint8_t _regAddr);
static void I2c_readBytesFromRegAddr(int32_t _i2cFileDesc,
                                     uint8_t *_pValueOutput,
                                     size_t _sizeofValueOutput);

int32_t I2c_initI2cDevice(int32_t _busNum, int32_t _deviceAddress) {
	I2c_setI2cBusPinsToI2cMode(_busNum);

	const size_t BUS_NAME_BUFFER_SIZE =
	    sizeof(I2CDRV_LINUX_BUS_FORMAT) / sizeof(I2CDRV_LINUX_BUS_FORMAT[0]);

	char busName[BUS_NAME_BUFFER_SIZE];
	I2c_getBusNameStringFromBusNum(busName, BUS_NAME_BUFFER_SIZE, _busNum);

	int32_t i2cFileDesc =
	    I2c_setI2CDeviceToSlaveAddress(busName, _deviceAddress);

	return i2cFileDesc;
}

static void I2c_setI2cBusPinsToI2cMode(int32_t _busNum) {
	const size_t NUM_I2C_BUS_PINS = 2;
	int32_t dataAndClockPins[NUM_I2C_BUS_PINS];
	I2c_getP9DataAndClockPinsForI2cBusNum(_busNum, dataAndClockPins);

	// Get P9 header pin strings for bus data and clock pins
	char p9DataPinStr[50];
	snprintf(p9DataPinStr, 50, "P9_%d", dataAndClockPins[0]);

	char p9ClockPinStr[50];
	snprintf(p9ClockPinStr, 50, "P9_%d", dataAndClockPins[1]);

	I2c_setI2cBusPinToI2cMode(p9DataPinStr);
	I2c_setI2cBusPinToI2cMode(p9ClockPinStr);
}

// Get bus name string from bus number like "/dev/i2c-1"
static void I2c_getBusNameStringFromBusNum(char *_pBusNameBufferOut,
                                           size_t _bufferSize,
                                           int32_t _busNum) {
	snprintf(_pBusNameBufferOut, _bufferSize, I2CDRV_LINUX_BUS_FORMAT, _busNum);
}

static int32_t I2c_setI2CDeviceToSlaveAddress(const char *_pBusName,
                                              int32_t _deviceAddress) {
	int32_t i2cFileDesc = open(_pBusName, O_RDWR);
	int32_t result = ioctl(i2cFileDesc, I2C_SLAVE, _deviceAddress);
	if (result < 0) {
		perror("I2C: Unable to set I2C device to slave address.");
		exit(1);
	}

	return i2cFileDesc;
}

void I2c_closeI2cDevice(int32_t _i2cFileDesc) { close(_i2cFileDesc); }

// Get data and clock pins for the bus number on the P9 header
static void I2c_getP9DataAndClockPinsForI2cBusNum(int32_t _busNum,
                                                  int32_t *_pPinNumsOut) {
	switch (_busNum) {
	case 1:
		_pPinNumsOut[0] = 18;
		_pPinNumsOut[1] = 17;
		break;
	case 2:
		_pPinNumsOut[0] = 20;
		_pPinNumsOut[1] = 19;
		break;
	default:
		assert(0);
	}
}

static void I2c_setI2cBusPinToI2cMode(const char *_p9DataPinStr) {
	const char *argsData[] = {_p9DataPinStr, "i2c"};
	Shell_execCommand("/usr/bin/config-pin", argsData,
	                  sizeof(argsData) / sizeof(argsData[0]));
}

void I2c_writeI2cReg(int32_t _i2cFileDesc, uint8_t _regAddr, uint8_t _value) {
	uint8_t buff[2];
	buff[0] = _regAddr;
	buff[1] = _value;

	size_t bytesWritten = write(_i2cFileDesc, buff, 2);

	// Failed to write 2 bytes
	if (bytesWritten != 2) {
		perror("I2C: Unable to write to i2c register.");
		exit(1);
	}
}

void I2c_readI2cReg(int32_t _i2cFileDesc, uint8_t _regAddr,
                    uint8_t *_pBufferOut, size_t _numBytesToRead) {
	// To read a register, must first write the address to the I2C bus
	I2c_writeRegAddrToI2cBus(_i2cFileDesc, _regAddr);

	// Read numBytesToRead bytes into pBufferOut
	I2c_readBytesFromRegAddr(_i2cFileDesc, _pBufferOut, _numBytesToRead);
}

static void I2c_writeRegAddrToI2cBus(int32_t _i2cFileDesc, uint8_t _regAddr) {
	size_t bytesWritten = write(_i2cFileDesc, &_regAddr, sizeof(_regAddr));

	// If failed to write from I2C register, terminate program
	if (bytesWritten != sizeof(_regAddr)) {
		perror("I2C: Unable to write to i2c register.");
		exit(1);
	}
}

static void I2c_readBytesFromRegAddr(int32_t _i2cFileDesc, uint8_t *_pValueOut,
                                     size_t _sizeofValueOutput) {
	size_t bytesRead = read(_i2cFileDesc, _pValueOut, _sizeofValueOutput);

	// If failed to read from I2C register, terminate program
	if (bytesRead != _sizeofValueOutput) {
		perror("I2C: Unable to read from i2c register");
		exit(1);
	}
}
