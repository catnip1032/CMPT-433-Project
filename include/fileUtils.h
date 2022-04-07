/*
 * The File module provides a simplified interface to interact with files
 * on the filesystem. Reading and writing operations are permitted, but
 * care should be taken to free any pointers to allocated memory returned
 * by this module's functions. This module also exports useful file path
 * constants to interact with the BBG's GPIO via the Linux filesystem.
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

// Returns a pointer to the concatenation of filePathBegin and filePathEnd.
// The returned pointer must be freed with free().
char *File_concatFilePath(const char *pFilePathBegin, const char *pFilePathEnd);

// Reads from file indicated at pFileName. Must free the returned read 
// value pointer once finished
char *File_readFromFile(const char *pFileName);

// Writes the value at pValue to the file indicated at pFilePath.
void File_writeToFile(const char *pFilePath, const char *pValue);

#endif
