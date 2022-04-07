#!/bin/bash

verify_commands () {
  if [ ! command -v git ] &> /dev/null
  then
    echo "git not installed. Please, install git."
    exit 1
  fi

  if [ ! command -v llvm-format ] &> /dev/null
  then
    echo "llvm-format not installed. Please, install llvm."
    exit 1
  fi
}

# Args:
# variable number of filenames of files to format
# ex: $1: src/main.c, $2: include/colorSensor.h, ...
format_files () {
  echo "$@" | xargs clang-format -i
}

# Args:
# variable number of filenames of files to add to the git staging area
# ex: $1: Makefile, $2: src/main.c, ...
stage_files () {
  echo "$@" | xargs git add
}

main () {
  verify_commands

  # Only format .c and .h files with clang
  STAGED_FILES="$(git diff --name-only --cached '*.c' '*.h')"

  # No Staged Files Reminder
  if [ -z $STAGED_FILES ]; then
    echo "There are no staged files! Remember to add them with 'git add <file>'"
    return 1
  fi

  # Format the files with settings specified in .clang-format
  format_files $STAGED_FILES
  FORMAT_FILES_RESULT=$?
  if [ $FORMAT_FILES_RESULT -ne 0 ]; then
    echo "format_files() has failed."
    return 1
  fi

  # Stage the formatted files
  stage_files $STAGED_FILES
  STAGE_FILES_RESULT=$?
  if [ $STAGE_FILES_RESULT -ne 0 ]; then
    echo "stage_files() has failed."
    return 1
  fi
  
  return 0
}

main
