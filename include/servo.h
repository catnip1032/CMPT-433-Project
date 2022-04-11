/* The servo module is responsible for handling the intialization and clean up
 * of 1 TowerPro SG-5010 Servo and 2 Micro Servo 98 SG90's. The module also 
 * reads and writes to the linux pwmchips enabled by the PWM overlays. The
 * period and duty cycle files for the servos may be written to for PWM control
 * of the motors and the servos may be enabled/unenabled and 
 * exported/unexported. */

#ifndef _SERVO_GAURD_H_
#define _SERVO_GAURD_H_

// Declaration of max buffer length for servo module
#define MAX_BUFFER_LEN 1024

// Structure to hold servo information
typedef struct Servo {
	int pin;
	char pinExportChar;
	char *header;
	char *pwmchip;
	char *pwmPath;
	char *type;
} Servo;

// Intializes the 1 TowerPro SG-5010 and 2 Micro Servo 98 SG90 servos by
// finding the pwmchip path, exporting the pwm for the pwmchip, and setting the 
// period.
void Servo_init(void);

// Cleans up the pwms by unenabling, resting the duty cycle and period, and
// unexporting the pwm form the pwmchip.
void Servo_cleanup(void);

// Can be used to move the servos in clockwise or counterclockwise direction by
// changing the duty cycle.
void Servo_changeDutyCycle(Servo _servo, const char *_newDutyCycle);

// Get servo in the servo list via index.
Servo Servo_getServo(int servoIndex);

// Sends a signal to the pwm 'enable' file to enable or disable access to the 
// pwm. 
void Servo_enableSignal(Servo _servo, char *_newSignal);

#endif
