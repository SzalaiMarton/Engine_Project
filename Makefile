# ===============================================================================
# This Makefile was made for Linux
#
# If you are on Windows install WSL with cmake and g++
# Use this Makefile while WSL is activated
# ===============================================================================

export CXX							:= /usr/bin/g++

CPP_NAME_PARSER_SCRIPT 				:= $(CURDIR)/parse_cpp_names.sh
SOURCE_FOLDER_PATH 					:= $(CURDIR)/src
TEMP_SOURCE_NAME_FILE				:= $(CURDIR)/_cpp_names.txt
OUTPUT_DIR							:= $(CURDIR)/build

BUILD_SCRIPT						:= $(CURDIR)/_build.sh
COMPILER_SCRIPT						:= $(CURDIR)/_compile.sh

REDIRECT_COMPILE_OUTPUT				?= 1
COMPILER_OUTPUT						:= $(CURDIR)/_compile_runtime_output.log
COMPILER_ERROR						:= $(CURDIR)/_compile_runtime_error.log



.PHONY: build-engine compile-engine print-compile-error print-compile-output follow-compile-runtime \
follow-compile-runtime-error

help:
	@echo "Available targets:"
	@echo "  build-engine                  - Build the engine"
	@echo "  compile-engine                - Compile the engine"
	@echo "  print-compile-error           - Show compile errors (reversed)"
	@echo "  print-compile-output          - Show compile output (reversed)"
	@echo "  follow-compile-runtime-output - Tail compile output"
	@echo "  follow-compile-runtime-error  - Tail compile errors"
	@echo "  all                           - Build and compile"
	@echo "  help                          - Show this help"


# ===============================================================================
#
#									BUILDING STAGE
#
# ===============================================================================


build-engine:
	@${BUILD_SCRIPT} ${SOURCE_FOLDER_PATH} ${OUTPUT_DIR}




# ===============================================================================
#
#									COMPILATION STAGE
#
# ===============================================================================

compile-engine:
	@${COMPILER_SCRIPT} ${REDIRECT_COMPILE_OUTPUT} ${COMPILER_OUTPUT} ${COMPILER_ERROR} ${OUTPUT_DIR}

print-compile-error:
	@if [ -f ${COMPILER_ERROR} ]; then \
		tac ${COMPILER_ERROR}; \
	else \
		echo '[ERROR] Missing error output file'; \
	fi

print-compile-output:
	@if [ -f ${COMPILER_OUTPUT} ]; then \
		tac ${COMPILER_OUTPUT}; \
	else \
		echo '[ERROR] Missing output file'; \
	fi

follow-compile-runtime-output:
	@if [ -f ${COMPILER_OUTPUT} ]; then \
		tail -f ${COMPILER_OUTPUT}; \
	else \
		echo '[ERROR] Missing output file'; \
	fi

follow-compile-runtime-error:
	@if [ -f ${COMPILER_ERROR} ]; then \
		tail -f ${COMPILER_ERROR}; \
	else \
		echo '[ERROR] Missing output file'; \
	fi
