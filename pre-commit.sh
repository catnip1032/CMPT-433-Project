#!/bin/bash

verify_commands () {
  if [ ! command -v git ] &> /dev/null
  then
    echo "git not installed. Please, install git."
    return 1
  fi

  if [ ! command -v llvm-format ] &> /dev/null
  then
    echo "llvm-format not installed. Please, install llvm."
    return 1
  fi
}

# Args:
#   1. Newline separated list of files to format
format_files () {
  echo "$1" | xargs -I {} clang-format -i {}
}

# Args:
#   1. Newline separated list of files to stage
commit_files () {
  echo "$1" | xargs -I {} git add {}
}

main () {
  STAGED_FILES="$(git diff --name-only --cached)"
  format_files $STAGED_FILES
  commit_files $STAGED_FILES
  echo "Files formatted, staged, and ready to commit!"
  return 0
}

main
