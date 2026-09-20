# ===============================================================================
# This Makefile was made for Linux
#
# If you are on Windows install WSL with cmake and g++
# Use this Makefile while WSL is activated
# ===============================================================================

export CXX							:= /usr/bin/g++
export OUTPUT_DIR_MAKE				:= build-make
export OUTPUT_DIR_NINJA				:= build-ninja


# =============================== ARGS ===============================

SOURCE_FOLDER_PATH 					?= $(CURDIR)/src
OUTPUT_DIR							?= $(CURDIR)/build
REDIRECT_RUNTIME_OUTPUT				?= 1
EXCEPT_SOURCE_FOLDERS				?= ""
USE_GDB								?= 0

# ========================== HARDCODED VARS ==========================

BUILD_SCRIPT						:= $(CURDIR)/_build.sh
RUN_SCRIPT							:= $(CURDIR)/_run.sh
RUNTIME_OUTPUT						:= $(CURDIR)/_runtime_output.log
RUNTIME_ERROR						:= $(CURDIR)/_runtime_error.log
GDB_COMMAND							:= gdb --args

.PHONY: build-engine run-engine print-runtime-error print-runtime-output follow-runtime-error follow-runtime-output help all 

help:
	@echo "Available targets:"
	@echo "\tbuild-engine \t\t\t\t || Build the engine"
	@echo "\truntime-engine \t\t\t\t || Run the engine"
	@echo "\tprint-runtime-error \t\t\t || Show runtime errors (reversed)"
	@echo "\tprint-runtime-output \t\t\t || Show runtime output (reversed)"
	@echo "\tfollow-runtime-output \t\t\t || Tail runtime output"
	@echo "\tfollow-runtime-error \t\t\t || Tail runtime errors"
	@echo "\tall \t\t\t\t\t || Build and run"
	@echo "\thelp \t\t\t\t\t || Show this help"
	@echo "\nAvailable ARGS:"
	@echo "\tSOURCE_FOLDER_PATH (str path) \t\t || Folder with the source (.cpp) files - recursively scaned"
	@echo "\tOUTPUT_DIR (str path) \t\t\t || The build's output folder"
	@echo "\tEXCEPT_SOURCE_FOLDERS (str path) \t || Ignores the specified folders content when collecting .cpp files"
	@echo "\tREDIRECT_RUNTIME_OUTPUT (boolean 1/0) \t || 1 -> Redirects the runtime output || 0 -> Output will be printed to the console"
	@echo "\tUSE_GDB (boolean 1/0) \t\t\t || If true 'run-engine' will run the binary with 'gdb --args'"


# ===============================================================================
#
#									BUILDING STAGE
#
# ===============================================================================

comma := ,
empty :=
space := $(empty) $(empty)

ifneq ($(strip $(EXCEPT_SOURCE_FOLDERS)),)
    EXCEPT_FOLDERS := $(subst $(comma),$(space),$(strip $(EXCEPT_SOURCE_FOLDERS)))
	
	EXCEPT_FIRST_WORD := $(firstword $(EXCEPT_FOLDERS))
	EXCEPT_REST := $(wordlist 2, $(words $(EXCEPT_FOLDERS)), $(EXCEPT_FOLDERS))
	
	EXCEPT_FIRST_WORD := $(foreach d, $(EXCEPT_FIRST_WORD), -path '*/$(d)')
	EXCEPT_REST := $(foreach d, $(EXCEPT_REST), -o -path '*/$(d)')
	EXCEPT_SUM := $(EXCEPT_FIRST_WORD) $(EXCEPT_REST)
    
	FIND_PRUNE := '(' $(EXCEPT_SUM) ')' -prune -o
else
    FIND_PRUNE :=
endif

build-engine:
	@${BUILD_SCRIPT} ${SOURCE_FOLDER_PATH} ${OUTPUT_DIR} ${FIND_PRUNE}



# ===============================================================================
#
#									RUN STAGE
#
# ===============================================================================

ifeq ($(USE_GDB), 0)
	GDB_COMMAND := 
endif

run-engine:
	@${RUN_SCRIPT} ${REDIRECT_RUNTIME_OUTPUT} ${RUNTIME_OUTPUT} ${RUNTIME_ERROR} ${OUTPUT_DIR} ${GDB_COMMAND}

print-runtime-error:
	@if [ -f ${RUNTIME_ERROR} ]; then \
		tac ${RUNTIME_ERROR}; \
	else \
		echo '[ERROR] Missing error output file'; \
	fi

print-runtime-output:
	@if [ -f ${RUNTIME_OUTPUT} ]; then \
		tac ${RUNTIME_OUTPUT}; \
	else \
		echo '[ERROR] Missing output file'; \
	fi

follow-runtime-output:
	@if [ -f ${RUNTIME_OUTPUT} ]; then \
		tail -f ${RUNTIME_OUTPUT}; \
	else \
		echo '[ERROR] Missing output file'; \
	fi

follow-runtime-error:
	@if [ -f ${RUNTIME_ERROR} ]; then \
		tail -f ${RUNTIME_ERROR}; \
	else \
		echo '[ERROR] Missing output file'; \
	fi
