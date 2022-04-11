#ifndef _LIGHTS_H_
#define _LIGHTS_H_

// Initialization/termination functions
// ----------------------------------------------------------------------------
void Lights_init(void);
void Lights_cleanup(void);

// Light stages functions
// ----------------------------------------------------------------------------
void Lights_idle(void);
void Lights_recycling(void);
void Lights_recycled(void);
void Lights_returning(void);

#endif
