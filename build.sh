#!/bin/bash

# Check if the first argument is empty
if [ -z "$1" ]; then
  # Print help message if no action is provided
  echo
  echo "Enter './build.sh action' where action is one of the following:"
  echo
  echo "  clean            Remove all binaries and intermediate binaries and project files."
  echo "  gmake2           Generate Makefiles."
  echo "  vs2022           Generate Visual Studio 2022 project files (only applicable on Windows)."
  exit 1
fi

# Run premake with the provided action
./vendor/premake5 $1