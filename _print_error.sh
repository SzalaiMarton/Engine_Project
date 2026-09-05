#!/bin/bash

RUNTIME_ERROR_FILE=$1

if [ -f $RUNTIME_ERROR_FILE ]; then
    tac $RUNTIME_ERROR_FILE;
else
    echo "[ERROR] Missing error output file";
fi
