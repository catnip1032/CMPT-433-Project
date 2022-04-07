/*
 * This shell module contains the functionality to fork the process and
 * carry out shell commands.
 */

#include "../include/shell.h"
#include <errno.h>
#include <string.h>
#include <sys/wait.h>
#include <unistd.h>

// returns NULL if successful, a string pointer with an error message if not.
const char *Shell_execCommand(const char *pCommandName, const char *args[],
                              int numArgs)
{
  // Generate null-terminated array from args with command name
  const char *argsTerminated[numArgs + 2];
  memcpy(&argsTerminated[1], args, numArgs * sizeof(args[0]));
  argsTerminated[0] = pCommandName;
  argsTerminated[numArgs + 1] = (const char *)0;

  // Execute command-line program
  pid_t childPid = fork();
  const char *error = NULL;
  if (childPid == 0) {
    int e = execv(pCommandName, (char *const *)argsTerminated);
    if (e == -1) {
      error = strerror(errno);
    }
  }

  // Wait for command to finish
  wait(&childPid);

  return error;
}