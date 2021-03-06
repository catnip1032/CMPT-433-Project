/*
 * The shell module allows for abstract execution of Linux commands
 * typically entered in a shell. NOTE: functions are blocking.
 * Command line executable file paths and arguments are necessary.
 * Provides error messages for failures.
 */

#include <stddef.h>

#ifndef SHELL_GAURD
#define SHELL_GAURD

// blocks and returns NULL if successful, a string pointer with an error message
// if not.
const char *Shell_execCommand(const char *_pCommandNameIn,
                              const char *_pArgsIn[], size_t _numArgs);

#endif
