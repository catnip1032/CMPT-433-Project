#include "../include/servo.h"
#include "../include/file.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

// Adapted from
// https://github.com/beagleboard/bb.org-overlays/blob/master/examples/cape-unversal-pwm.txt


// PWM file constants
// ----------------------------------------------------------------------------
static const char *PERIOD_FILE = "/period";
static const char *DUTY_CYCLE_FILE = "/duty_cycle";
static const char *ENABLE_FILE = "/enable";

// EHRPWM pin export
// ----------------------------------------------------------------------------
static const char P9_PIN_21 = '1';		// EHRPWM0B, 1 for B
static const char P9_PIN_14 = '0';		// EHRPWM1A, 0 for A
static const char P8_PIN_13 = '1';		// EHRPWM2B, 1 for B

// Servo period
// ----------------------------------------------------------------------------
static const char *SERVO_PERIOD = "20000000"; // 20ms as specified by servo docs

// Servo max and min duty cycles
// ----------------------------------------------------------------------------
// TODO: Move to gate and pipe design modules when created
// static const int MIN_TOWER_SERVO = 470000;		// clockwise
// static const int MAX_TOWER_SERVO = 2400000;		// counterclockwise

// static const int MIN_MICRO_SERVO = 1000000;		// clockwise
// static const int MAX_MICRO_SERVO = 20000000;	// counterclockwise

// List of servos and list length
// ----------------------------------------------------------------------------
static struct Servo servos[] = {
	{13, P8_PIN_13, "p8", "pwmchipN",	// p8_13
	"/sys/devices/platform/ocp/48304000.epwmss/48304200.pwm/pwm/", "tower"},
	{21, P9_PIN_21, "p9", "pwmchipN",	// p9_21
	"/sys/devices/platform/ocp/48300000.epwmss/48300200.pwm/pwm/", "micro"},
	{14, P9_PIN_14, "p9", "pwmchipN",	// p9_14
	"/sys/devices/platform/ocp/48302000.epwmss/48302200.pwm/pwm/", "micro"},
};

static const int SERVOS_LISTED = 3;

// Function prototype declarations
// ----------------------------------------------------------------------------
static void exportPWMChip(Servo _servo);

static void setPWMChip(Servo _servo);

static void setServoPeriod(Servo _servo, const char *_period);

static void unexportPWMChip(Servo _servo);

static void writeToServo(Servo _servo, const char *_fileToWrite,
						 const char *_pvalue);

// Constant pwmchip length for pwmchip buffers
// ----------------------------------------------------------------------------
static const int PWMCHIP_LENGTH = strlen("pwmchipN")+1;

// Public Functions
// ----------------------------------------------------------------------------

void Servo_init(void)
{
	for(int i = 0; i < SERVOS_LISTED ; i++){
		// Set the correct pwmchip according to pin path
		char *pinPwmChip = malloc(sizeof(char)*PWMCHIP_LENGTH);
		servos[i].pwmchip = pinPwmChip;
		setPWMChip(servos[i]);
		// Export EHRPWM pin
		exportPWMChip(servos[i]);
		// Set servo period
		setServoPeriod(servos[i], SERVO_PERIOD);
	}
}

void Servo_cleanup(void)
{
	for(int i = 0; i < SERVOS_LISTED ; i++){
		// Unenable pin
		Servo_enableSignal(servos[i], "0");
		// Set duty cycle to 0
		Servo_changeDutyCycle(servos[i], "0");
		// Set period to 0
		setServoPeriod(servos[i], 0);
		// Unexport EHRPWM pin
		unexportPWMChip(servos[i]);
		// Free pwmchip
		free(servos[i].pwmchip);
	}
}

void Servo_changeDutyCycle(Servo _servo, char *_newDutyCycle)
{
	writeToServo(_servo, DUTY_CYCLE_FILE, _newDutyCycle);
}

// Enables/disables the signal of the servo
void Servo_enableSignal(Servo _servo, char *_newSignal) 
{
	writeToServo(_servo, ENABLE_FILE, _newSignal);
}

// Private Functions
// ----------------------------------------------------------------------------

// Exports the EHRPWM pin to either a 1 or a 0 dependednt on pin export char.
static void exportPWMChip(Servo _servo)
{
	writeToServo(_servo, FILE_EXPORT_FILE, &_servo.pinExportChar);
}

// unexports the EHRPWM pin
static void unexportPWMChip(Servo _servo)
{
	// Get pwmchip path
	char *pwmchipPath = malloc(sizeof(char)*MAX_BUFFER_LEN);
	File_concatFilePath(_servo.pwmPath, _servo.pwmchip, pwmchipPath,
	 MAX_BUFFER_LEN);

	// Get pwmchip unexport path
	char *pwmchipUnexportPath = malloc(sizeof(char)*MAX_BUFFER_LEN);
	File_concatFilePath(pwmchipPath, FILE_UNEXPORT_FILE, pwmchipUnexportPath,
	 MAX_BUFFER_LEN);

	// Write to unexport file
	File_writeToFile(pwmchipUnexportPath, &_servo.pinExportChar);
	free(pwmchipPath);
	free(pwmchipUnexportPath);
}

// Finds and sets the pwmchip number for the servo.
static void setPWMChip(Servo _servo)
{
	// Create the ls command for the pwm path
	char lsCommand[MAX_BUFFER_LEN]; 
	snprintf(lsCommand, MAX_BUFFER_LEN, "/bin/ls %s", _servo.pwmPath);
	if (lsCommand[0] == '\0'){
		printf("Error: No pwmchip found.\n");
		exit(EXIT_FAILURE);
	}

	// Open a pipe to read the ls command

	FILE *pwmchipFile;
  char pwmchipBuffer[PWMCHIP_LENGTH];
  pwmchipFile = popen(lsCommand, "r");
  if (pwmchipFile == NULL) {
    printf("Error: Could not execute command, \"%s\"\n",
		 lsCommand);
    exit(EXIT_FAILURE);
  }
  // Read the pwmchip output
  if (fgets(pwmchipBuffer, PWMCHIP_LENGTH, pwmchipFile) == NULL) {
    printf("Error: Could not write to pwmBuffer. \n");
    exit(EXIT_FAILURE);
  }
  // Close the pipe file
  pclose(pwmchipFile);
	// Set pwmchip
	strncpy(_servo.pwmchip, pwmchipBuffer, PWMCHIP_LENGTH);
	_servo.pwmchip[PWMCHIP_LENGTH] = 0;
}

// Sets the period of the servo.
static void setServoPeriod(Servo _servo, const char* _period)
{
	writeToServo(_servo, PERIOD_FILE, _period);
}

// Writes a char * value to a servo file.
static void writeToServo(Servo _servo, const char *_fileToWrite,
						 const char *_pvalue)
{
 	// Get pwmchip path
	char *pwmchipPath = malloc(sizeof(char)*MAX_BUFFER_LEN);
	File_concatFilePath(_servo.pwmPath, _servo.pwmchip, pwmchipPath,
	 MAX_BUFFER_LEN);

	// Get servo file path
	char *servoFilePath = malloc(sizeof(char)*MAX_BUFFER_LEN);

	// Handles pwm export file being in different directory
	if (strcmp(_fileToWrite, FILE_EXPORT_FILE)){	// Not exporting a file
		// Get pwm 0 or 1
		char *pwm = malloc(sizeof(char)*MAX_BUFFER_LEN);
		snprintf(pwm, MAX_BUFFER_LEN,"/pwm%c", _servo.pinExportChar);

		// Get final pwm path
		char *pwmPath = malloc(sizeof(char)*MAX_BUFFER_LEN);
		File_concatFilePath(pwmchipPath, pwm, pwmPath, MAX_BUFFER_LEN);
		File_concatFilePath(pwmPath, _fileToWrite, servoFilePath, MAX_BUFFER_LEN);
		free(pwm);
		free(pwmPath);
	} else {	// Exporting a file
		File_concatFilePath(pwmchipPath, _fileToWrite, servoFilePath,
												MAX_BUFFER_LEN);
	}

	// Write to servo file
	File_writeToFile(servoFilePath, _pvalue);
	free(pwmchipPath);
	free(servoFilePath);
}
