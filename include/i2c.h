/*
 * The I2C Module allows provides an abstracted interface
 * when dealing with I2C devices connected to an I2C bus.
 * The operations allowed are writing and reading from
 * I2C device registers.
 */

#include <stdbool.h>
#include <stdint.h>

#ifndef I2C_GAURD
#define I2C_GAURD

// Returns i2c bus file descriptor as int to be used with other I2c functions.
// Must close the i2c bus file descriptor once finished with I2c_closeI2cBus().
int I2c_initI2cDevice(int busNum, int deviceAddress);

// Given a register address, I2C file destination, and a value, write the value
// to the register address at the file destination.
void I2c_writeI2cReg(int i2cFileDesc, unsigned char regAddr,
                     unsigned char value);

/* Given a register address, I2C file destination, an output buffer, and number
 * of bytes to read, read the number of bytes starting at the register address
 * into pBufferOut */
void I2c_readI2cReg(int i2cFileDesc, unsigned char regAddr,
                    unsigned char *pBufferOut, int numBytesToRead);

// Closes the I2C device at the I2C file destination, given its I2C File Bus
// Descriptor that was returned from I2c_initI2cDevice().
void I2c_closeI2cDevice(int i2cFileDesc);

#endif