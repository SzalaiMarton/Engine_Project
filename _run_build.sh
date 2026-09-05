#!/bin/bash

REDIRECT_RUNTIME=$1
RUNTIME_OUTPUT_FILE=$2
RUNTIME_ERROR_FILE=$3
OUTPUT_DIR=$4

echo "[INFO] Runtime output and error are redirected by default to:"
echo "[INFO] Output: $RUNTIME_OUTPUT_FILE"
echo "[INFO] Error: $RUNTIME_ERROR_FILE"
echo "[INFO] To disable this use REDIRECT_RUNTIME=0 when running 'run-engine' target"

if [ -d $OUTPUT_DIR ]; then
    echo "[INFO] Starting engine";
    if [ $REDIRECT_RUNTIME -eq 1 ]; then
        cd $OUTPUT_DIR && make > $RUNTIME_OUTPUT_FILE 2>$RUNTIME_ERROR_FILE;
        tac $RUNTIME_ERROR_FILE
    elif [ $REDIRECT_RUNTIME -eq 0 ]; then
        cd $OUTPUT_DIR && make;
    else
        echo "[ERROR] REDIRECT_RUNTIME must be either 1 or 0";
        exit 1;
    fi
else
    echo "[INFO] Build directory $OUTPUT_DIR is missing";
    echo "[INFO] Build it first then try again";
    exit 0;
fi