#!/bin/bash

REDIRECT_OUTPUT=$1
OUTPUT=$2
ERROR=$3
OUTPUT_DIR=$4

echo "[INFO] Compiling output and error are redirected by default to:"
echo "[INFO] \tOutput: $OUTPUT"
echo "[INFO] \tError: $ERROR"
echo "[INFO] To disable this use REDIRECT_RUNTIME_OUTPUT=0 when running 'run-engine' target"

if [ -d $OUTPUT_DIR ]; then
    echo "[INFO] Starting engine";
    if [ $REDIRECT_OUTPUT -eq 1 ]; then
        ./build/bin/engine
        tac $ERROR
    elif [ $REDIRECT_OUTPUT -eq 0 ]; then
        cd $OUTPUT_DIR && make;
    else
        echo "[ERROR] REDIRECT_RUNTIME_OUTPUT must be either 1 or 0";
        exit 1;
    fi
else
    echo "[INFO] Build directory $OUTPUT_DIR is missing";
    echo "[INFO] Build it first then try again";
    exit 0;
fi