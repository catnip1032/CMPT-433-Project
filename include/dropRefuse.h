/*
 * This drop refuse module acts as an abstraction layer to the pipe.h and gate.h
 * modules in that it may be used to simply state which refuse is being dropped
 * allowing pipe.h and gate.h to handle the details.
 **/

#ifndef _DROP_REFUSE_HEADER_h
#define _DROP_REFUSE_HEADER_h

// Drops the garbage (red ball) in the garbage shute.
void DROP_REFUSE_dropGarbage(void);

// Drops the compost (green ball) in the compost shute.
void DROP_REFUSE_dropCompost(void);

// Drops the recycling (blue ball) in the recycling shute.
void DROP_REFUSE_dropRecycling(void);

#endif
