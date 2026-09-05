# ===============================================================================
# This Makefile was made for Linux
#
# If you are on Windows install WSL with cmake and g++
# Use this Makefile while WSL is activated
# ===============================================================================

export CXX					:= /usr/bin/gcc

CPP_NAME_PARSER_SCRIPT 		:= $(CURDIR)/parse_cpp_names.sh
SOURCE_FOLDER_PATH 			:= $(CURDIR)/src
TEMP_SOURCE_NAME_FILE		:= $(CURDIR)/_cpp_names.txt
OUTPUT_DIR					:= $(CURDIR)/build

BUILD_SCRIPT				:= $(CURDIR)/_build.sh
RUN_BUILD_SCRIPT			:= $(CURDIR)/_run_build.sh
ERROR_PRINT_SCRIPT			:= $(CURDIR)/_print_error.sh
OUTPUT_PRINT_SCRIPT			:= $(CURDIR)/_print_output.sh

REDIRECT_RUNTIME			?= 1
RUNTIME_OUTPUT_FILE			:= $(CURDIR)/_runtime_output.log
RUNTIME_ERROR_FILE			:= $(CURDIR)/_runtime_error.log

.PHONY: build-engine run-engine print-error print-output

build-engine:
	@${BUILD_SCRIPT} ${SOURCE_FOLDER_PATH} ${OUTPUT_DIR}

run-engine:
	@${RUN_BUILD_SCRIPT} ${REDIRECT_RUNTIME} ${RUNTIME_OUTPUT_FILE} ${RUNTIME_ERROR_FILE} ${OUTPUT_DIR}

print-error:
	@${ERROR_PRINT_SCRIPT} ${RUNTIME_ERROR_FILE}

print-output:
	@${OUTPUT_PRINT_SCRIPT} ${RUNTIME_OUTPUT_FILE}
