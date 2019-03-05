#!/usr/bin/env bash

SO_NAME="RuntimeLtlPass.so"
TARGET_FILE=$1
BIN_OUT=$2
IR_OUT=$3

if [ -z "$TARGET_FILE" ]; then
  echo "Error: no target file"
  exit 1
fi

# Test to see if the passed file ends in `.c`. If it does, target C. Otherwise,
# target C++.
if [ ${TARGET_FILE:-2 == ".c"} ]; then
  TARGET_LANG="c"
elif [ ${TARGET_FILE:-4 == ".cpp"} ]; then
  TARGET_LANG="cpp"
else
  echo "Error: Unknown file extension on target file"
  exit 1
fi


# If `PASS_SO` is an environment variable, don't override it.
# This allows you to specify a different location for the script to find
# the plugin at.
if [ -z "$PASS_SO" ]; then 
  # location of the shared object in the local result of a `nix-build`
  RESULT_SO="result/$SO_NAME"
  # location of the shared object as a result of a normal CMake build
  BUILD_SO="build/RuntimeLtlPass/$SO_NAME"

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

echo "PASS_SO is $PASS_SO"

# If `nix-shell` has been run with the local `shell.nix`, use the `$clang7`
# variable to execute the code. Otherwise, locate the `clang` command in the
# user's environment.
#
# The reason this work around is neccessary is because currently in nix clang
# when build with libraries will break an environmental variable that GCC
# depends on.
#
# You can also pass a path to a clang executable in a `CLANG` environmental
# variable.
if [ -z "$CLANG" ]; then
  if [ $USING_NIX_SHELL == 1 ]; then
    if [ $TARGET_LANG == "c" ]; then
      CLANG="$clangxx7"
    elif [ TARGET_LANG == "cpp" ]; then
      CLANG="$clang7"
    fi
  else
    if [ $TARGET_CXX == "c" ]; then
      CLANG=$(/usr/bin/env clang)
    elif [ TARGET_LANG == "cpp" ]; then
      CLANG=$(/usr/bin/env clang++)
    fi
  fi
fi

# Make sure that a clang executable has been found.
if [ -z $CLANG ]; then
  echo "Error: could not find clang executable."
  exit 1
fi

# Echo the path of the `clang` executable being used.
echo "CLANG is $CLANG"

# Common options for the 
RUN_CLANG="$CLANG -Xclang -load -Xclang $PASS_SO -O0"

IR_TMP=$(mktemp)
BIN_TMP=$(mktemp)
# Load the shared object containing the pass into clang and emit the LLVM IR
# into the output file.
$RUN_CLANG -emit-llvm -S $TARGET_FILE -o $IR_TMP
$RUN_CLANG $TARGET_FILE -o $BIN_TMP

# Print the IR to stdout.
cat $IR_TMP

# Then execute the binary.
echo "Running..."
$BIN_TMP

# Copy the IR and binary if desired.
if [ "$IR_OUT" != "" ]; then
  echo "Outputing IR to $IR_OUT"
  cp $IR_TMP $IR_OUT
fi
rm $IR_TMP

if [ "$BIN_OUT" != "" ]; then
  echo "Outputing binary to $BIN_OUT"
  cp $BIN_TMP $BIN_OUT
fi
rm $BIN_TMP
