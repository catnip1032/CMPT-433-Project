/*  This API enables basic interfacing with the BB leds. Current functions are
    simplistic and rudimentary, but the API gives enough room for extension.
   */
#ifndef _LED_H_
#define _LED_H_

#include <stdbool.h>

// Type definitions
// ----------------------------------------------------------------------------
#define NUMS_OF_LEDS 4
typedef enum { LED_0 = 0, LED_1 = 1, LED_2 = 2, LED_3 = 3 } eLedNum;

// Initialization/Termination functions
// ----------------------------------------------------------------------------
void Led_init(void);
void Led_cleanup(void);

/* Initializes a specific LED for use during the program. It is NOT an
   alternative to Led_init() and/or Led_cleanup(). Those functions must be
   called after Led_init() and before Led_cleanup(). It is NOT necessary to
   call Led_termLed() before Led_cleanup if Led_initLed has been called as
   Led_cleanup will make that call during its execution. If one wishes to
   return the initialized led to its default configuration, then this function
   may be called.
 */
void Led_initLed(eLedNum _ledNum);
void Led_termLed(eLedNum _ledNum);

// Light functions
// ----------------------------------------------------------------------------
// Sets/get the led on (true) or off (false). Led must be initialized before
// calling Led_set() on it.
void Led_setLight(eLedNum _ledNum, bool _value);
bool Led_getLight(eLedNum _ledNum);

bool Led_isInitialized(eLedNum _ledNum);

#endif
