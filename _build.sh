#!/bin/bash
set -e

SOURCE_FOLDER_PATH=$1
OUTPUT_DIR=$2
shift 2
# $@ -> find prune rule

echo "[INFO] Fetching .cpp file names from $SOURCE_FOLDER_PATH"

SRC_FILES=$(find "${SOURCE_FOLDER_PATH}" "$@" -name '*.cpp' | tr '\n' ';' | sed 's/;$//')
FILE_COUNT=$(echo "$SRC_FILES" | tr ';' '\n' | wc -l)
PRINT_SRC_FILES=$(echo ${SRC_FILES} | tr ';' '\n')

echo "[INFO] Number of source files: $FILE_COUNT"
echo "[INFO] Found files: "
echo "$PRINT_SRC_FILES"
echo ""
echo "[INFO] Building engine"

mkdir -p "$OUTPUT_DIR"
cd "$OUTPUT_DIR"

cmake .. \
  -G Ninja \
  -DSOURCE_FILES="$SRC_FILES" \
  -DCMAKE_BUILD_TYPE=Debug \
  -DCMAKE_EXPORT_COMPILE_COMMANDS=ON \
  -DCMAKE_CXX_COMPILER_LAUNCHER=ccache

cmake --build . --parallel