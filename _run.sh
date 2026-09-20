#!/bin/bash

REDIRECT_OUTPUT=$1
OUTPUT=$2
ERROR=$3
OUTPUT_DIR=$4
GDB_COMMAND=$5

echo "[INFO] Compiling output and error are redirected by default to:"
echo "[INFO]    Output: $OUTPUT"
echo "[INFO]    Error: $ERROR"
echo "[INFO] To disable this use REDIRECT_RUNTIME_OUTPUT=0 when running 'run-engine' target"
echo ""

if [ ! -f "$ERROR" ]; then
    touch $ERROR
fi

if [ ! -f "$OUTPUT" ]; then
    touch $OUTPUT
fi

if [ -d $OUTPUT_DIR ]; then
    echo "[INFO] Starting engine";
    if [ $REDIRECT_OUTPUT -eq 1 ]; then
        cd $OUTPUT_DIR && $GDB_COMMAND ./bin/engine
        tac $ERROR
    elif [ $REDIRECT_OUTPUT -eq 0 ]; then
        cd $OUTPUT_DIR && $GDB_COMMAND ./bin/engine
    else
        echo "[ERROR] REDIRECT_RUNTIME_OUTPUT must be either 1 or 0";
        exit 1;
    fi
else
    echo "[INFO] Build directory $OUTPUT_DIR is missing";
    echo "[INFO] Build it first then try again";
    exit 0;
fi
