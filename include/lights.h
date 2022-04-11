// A layer of abstraction upon the LEDs API that implements behavior specific
// to each stage in the execution of the recycler.
#ifndef _LIGHTS_H_
#define _LIGHTS_H_

// Initialization/termination functions
// ----------------------------------------------------------------------------
void Lights_init(void);
void Lights_cleanup(void);

// Light stages functions
// ----------------------------------------------------------------------------
void Lights_setIdle(void);
void Lights_setRecycling(void);
void Lights_setRecycled(void);
void Lights_setReturning(void);

#endif
