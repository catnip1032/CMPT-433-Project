/*
 * This shell module contains the functionality to fork the process and
 * carry out shell commands.
 */

#include "../include/shell.h"
#include <errno.h>
#include <stddef.h>
#include <stdint.h>
#include <string.h>
#include <sys/wait.h>
#include <unistd.h>

// returns NULL if successful, a string pointer with an error message if not.
const char *Shell_execCommand(const char *_pCommandNameIn,
                              const char *_pArgsIn[], size_t _numArgs)
{
  // Generate null-terminated array from args with command name
  const char *argsTerminated[_numArgs + 2];
  memcpy(&argsTerminated[1], _pArgsIn, _numArgs * sizeof(_pArgsIn[0]));
  argsTerminated[0] = _pCommandNameIn;
  argsTerminated[_numArgs + 1] = (const char *)0;

  // Execute command-line program
  pid_t childPid = fork();
  const char *error = NULL;
  if (childPid == 0) {
    int32_t e = execv(_pCommandNameIn, (char *const *)argsTerminated);
    if (e == -1) {
      error = strerror(errno);
    }
  }

  // Wait for command to finish
  wait(&childPid);

  return error;
}
