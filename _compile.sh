#!/bin/bash

REDIRECT_COMPILE_OUTPUT=$1
COMPILER_OUTPUT=$2
COMPILER_ERROR=$3
OUTPUT_DIR=$4

echo "[INFO] Compiling output and error are redirected by default to:"
echo "[INFO] Output: $COMPILER_OUTPUT"
echo "[INFO] Error: $COMPILER_ERROR"
echo "[INFO] To disable this use REDIRECT_COMPILE_OUTPUT=0 when running 'compile-engine' target"

if [ -d $OUTPUT_DIR ]; then
    echo "[INFO] Starting engine";
    if [ $REDIRECT_COMPILE_OUTPUT -eq 1 ]; then
        cd $OUTPUT_DIR && make > $COMPILER_OUTPUT 2>$COMPILER_ERROR;
        tac $COMPILER_ERROR
    elif [ $REDIRECT_COMPILE_OUTPUT -eq 0 ]; then
        cd $OUTPUT_DIR && make;
    else
        echo "[ERROR] REDIRECT_COMPILE_OUTPUT must be either 1 or 0";
        exit 1;
    fi
else
    echo "[INFO] Build directory $OUTPUT_DIR is missing";
    echo "[INFO] Build it first then try again";
    exit 0;
fi