#!/bin/bash

SOURCE_FOLDER_PATH=$1
OUTPUT_DIR=$2

echo "[INFO] Fetching .cpp file names from $SOURCE_FOLDER_PATH"

SRC_FILES=$(ls $SOURCE_FOLDER_PATH | grep -E '\.cpp$' | tr '\n' ';' | sed 's/;$//')
FILE_COUNT=$(echo $SRC_FILES | tr ';' '\n' | wc -l)

echo "[INFO] Number of source files: $FILE_COUNT"
echo "[INFO] Found files: $SRC_FILES"
echo "[INFO] Building engine"

mkdir -p $OUTPUT_DIR
cd $OUTPUT_DIR
cmake .. -DSOURCE_FILES="$SRC_FILES" -DSOURCE_DIR="$SOURCE_FOLDER_PATH" -DCMAKE_VERBOSE_MAKEFILE=ON
