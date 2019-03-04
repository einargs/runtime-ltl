#!/usr/bin/env sh

PASS_SO_DIR=build/RuntimeLtlProject
TARGET_FILE=$1
OUTPUT_FILE=$2

PASS_SO="$PASS_SO_DIR/RuntimeLtlProject.so"

$clang7 -Xclang -load -Xclang $PASS_SO -O0 -emit-llvm $TARGET_FILE -S -o $OUTPUT_FILE

cat $OUTPUT_FILE
echo "Running..."
lli $OUTPUT_FILE
