#!/usr/bin/env bash

SO_NAME="RuntimeLtlProject.so"
TARGET_FILE=$1

# If no argument is passed for the output, create a temporary file instead.
if [ "$2" != "" ]; then
  echo "Outputing IR to $2"
  OUTPUT_FILE=$2
else
  OUTPUT_FILE=$(mktemp)
  # delete the temp file when the script exits.
  trap 'rm -f $OUTPUT_FILE' EXIT
fi

# If `PASS_SO` is an environment variable, don't override it.
# This allows you to specify a different location for the script to find
# the plugin at.
if [ -z PASS_SO ]; then 
  # location of the shared object in the local result of a `nix-build`
  local RESULT_SO="./result/$SO_NAME"
  # location of the shared object as a result of a normal CMake build
  local BUILD_SO="./build/RuntimeLtlProject/$SO_NAME"

  if [ -f $RESULT_SO ]; then
    # If there is a result folder with a shared object in it, use that;
    PASS_SO=$RESULT_SO
  elif [ -f $BUILD_SO ]; then
    # Otherwise, check if a shared object exists as a result of a normal
    # CMake build.
    PASS_SO=$BUILD_SO
  else
    # If a shared object cannot be located, exit with an error.
    echo "Error: Could not find shared object."
    exit 1
  fi
fi

# If `nix-shell` has been run with the local `shell.nix`, use the `$clang7`
# variable to execute the code. Otherwise, locate the `clang` command in the
# user's environment.
#
# The reason this work around is neccessary is because currently in nix clang
# when build with libraries will break an environmental variable that GCC
# depends on.
if [ $USING_NIX_SHELL == 1 ]; then
  CLANG="$clang7"
else
  CLANG=$(/usr/bin/env clang)
fi

# Make sure that a clang executable has been found.
if [ -z $CLANG ]; then
  echo "Error: could not find clang executable."
  exit 1
fi

# Echo the path of the `clang` executable being used.
echo "CLANG is $CLANG"

# Load the shared object containing the pass into clang and emit the LLVM IR
# into the output file.
$CLANG -Xclang -load -Xclang $PASS_SO -O0 -emit-llvm $TARGET_FILE -S -o $OUTPUT_FILE

# Print the IR to stdout.
cat $OUTPUT_FILE

# Then execute the IR inside the IR interpreter.
echo "Running..."
lli $OUTPUT_FILE

