/*
 * The File module provides a simplified interface to interact with files
 * on the filesystem. Reading and writing operations are permitted.
 * This module also exports useful file path concatenation to create file paths
 * that may interact with the BBG's GPIO and PWM via the Linux filesystem.
 */

#ifndef FILE_GAURD
#define FILE_GAURD

// GPIO pin manipulation file and folder MACROs
// TODO: make these constant in the GPIO API
#define FILE_GPIO_PATH "/sys/class/gpio"
#define FILE_GPIO_FOLDER "/gpio"
#define FILE_DIRECTION_FILE "/direction"
#define FILE_EDGE_FILE "/edge"
#define FILE_EXPORT_FILE "/export"
#define FILE_UNEXPORT_FILE "/unexport"
#define FILE_VALUE_FILE "/value"

// PWM pin manipulation file and folder MACROs
// TODO: make these constant in the PWM API
#define FILE_PWM_PATH "/sys/class/pwm"
#define FILE_PERIOD_FILE "/period"
#define FILE_DUTY_CYCLE_FILE "/duty_cycle"
#define FILE_ENABLE_FILE "/enable"

// LED manipulation files
#define FILE_LED_BASEPATH "/sys/class/leds"
#define FILE_LED_PATH FILE_LED_BASEPATH "/beaglebone:green:usr"
#define FILE_LED_TRIGGER "/trigger"
#define FILE_LED_BRIGHTNESS "/brightness"
#define FILE_LED_MAX_BRIGHTNESS "/max_brightness"

// Concatenates a two files paths together and stores the result in the in-out
// parameter _pConcatFilePath. Returns a 1 if successful.
int File_concatFilePath(const char *_pFilePathBegin, const char *_pFilePathEnd,
                        char *_pConcatFilePath, const int concatFilePathSize);

// Reads from file indicated at pFileName into the passed read buffer of read
// buffer size. Returns 1 if successful.
int File_readFromFile(const char *_pFilePath, char *_readBuffer,
                      const int _readBufferSize);

// Writes the value at pValue to the file indicated at pFilePath.
// Returns 1 if successful.
int File_writeToFile(const char *_pFilePath, const char *_pValue);

#endif
