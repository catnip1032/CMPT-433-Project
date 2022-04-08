#include <stdlib.h>
#include <stdio.h>

#include "../include/file.h"

// PWM path and file constants
static const char *PWM_PATH = "/sys/class/pwm";
static const char *PERIOD_FILE = "/period";
static const char *DUTY_CYCLE_FILE = "/duty_cycle";
static const char *ENABLE_FILE = "/enable";

// Length constants
static const int MAX_FILE_PATH_LEN = 10000;

// Servo paths
static char* towerServoPath;
static char* microServoGate1Path;
static char* microServoGate2Path;

int initializeServos(void)
{
	// Initialize tower servo
	// Find the pwmchip
	// Get path to the pwmchip

	// Initialize micro servos
	// Find the pwmchip
	// Get path to the pwmchip

}

void changeDutyCycle(char *_pathToPwm, int _newDutyCycle)
{
	// Get the path to the PWM duty cycle
	char *dutyCyclePath = malloc(sizeof(char)*MAX_FILE_PATH_LEN);
	int concatFileResult = File_concatFilePath(_pathToPwm, DUTY_CYCLE_FILE, 
	dutyCyclePath, MAX_FILE_PATH_LEN);
	// Write to the duty cycle
	int writeFileResult = File_writeToFile(dutyCyclePath, _newDutyCycle);
	free(dutyCyclePath);
}

void changePeriod(char *_pathToPwm, int _newPeriod) 
{
	// Get the path to PWM period file
	char *periodPath = malloc(sizeof(char)*MAX_FILE_PATH_LEN);
	int concatFileResult = File_concatFilePath(_pathToPwm, PERIOD_FILE, 
	periodPath, MAX_FILE_PATH_LEN);
	// Write to the duty cycle
	int writeFileResult = File_writeToFile(periodPath, _newPeriod);
	free(periodPath);
}

void enableSignal(char *_pathToPwm, int _newPeriod) 
{
	// Get the path to PWM enable file
	char *enablePath = malloc(sizeof(char)*MAX_FILE_PATH_LEN);
	int concatFileResult = File_concatFilePath(_pathToPwm, ENABLE_FILE, 
	enablePath, MAX_FILE_PATH_LEN);
	// Write to the duty cycle
	File_writeToFile(enablePath, _newPeriod);
	free(enablePath);
}
