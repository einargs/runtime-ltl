#!/usr/bin/env bash

TARGET_FILE=$1

if [ "$2" != "" ]; then
  OUTPUT_FILE=$2
else
  OUTPUT_FILE=$(mktemp)
  trap 'rm -f $OUTPUT_FILE' EXIT
fi

echo "OUTPUT_FILE is $OUTPUT_FILE"

if [ -f ./result/RuntimeLtlProject.so ]; then
  PASS_SO=./result/RuntimeLtlProject.so
else
  PASS_SO="./build/RuntimeLtlProject/RuntimeLtlProject.so"
fi


if [ $USING_NIX_SHELL == 1 ]; then
  CLANG="$clang7"
else
  CLANG=$(/usr/bin/env clang)
fi

echo "CLANG is $CLANG"

$CLANG -Xclang -load -Xclang $PASS_SO -O0 -emit-llvm $TARGET_FILE -S -o $OUTPUT_FILE

cat $OUTPUT_FILE
echo "Running..."
lli $OUTPUT_FILE

