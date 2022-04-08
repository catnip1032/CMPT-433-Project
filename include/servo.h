#ifndef SERVO_GAURD
#define SERVO_GAURD

void changeDutyCycle(char *_pathToPwm, int _newDutyCycle);
void changePeriod(char *_pathToPwm, int _newPeriod);
void enableSignal(char *_pathToPwm, int _newPeriod);

#endif