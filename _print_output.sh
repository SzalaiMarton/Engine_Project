#!/bin/bash

RUNTIME_OUTPUT_FILE=$1

if [ -f $RUNTIME_OUTPUT_FILE ]; then
    tac $RUNTIME_OUTPUT_FILE;
else
    echo "[ERROR] Missing output file";
fi
