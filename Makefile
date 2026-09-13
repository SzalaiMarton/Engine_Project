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

# ========================== HARDCODED VARS ==========================

BUILD_SCRIPT						:= $(CURDIR)/_build.sh
RUN_SCRIPT							:= $(CURDIR)/_run.sh
RUNTIME_OUTPUT						:= $(CURDIR)/_runtime_output.log
RUNTIME_ERROR						:= $(CURDIR)/_runtime_error.log

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
	@echo "\tSOURCE_FOLDER_PATH \t\t\t || Folder with the source (.cpp) files - recursively scaned"
	@echo "\tOUTPUT_DIR \t\t\t\t || The build's output folder"
	@echo "\tREDIRECT_RUNTIME_OUTPUT \t\t || 1 -> Redirects the runtime output || 0 -> Output will be printed to the console"
	@echo "\tEXCEPT_SOURCE_FOLDERS \t\t\t || Ignores the specified folders content when collecting .cpp files"


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

run-engine:
	@${RUN_SCRIPT} ${REDIRECT_RUNTIME_OUTPUT} ${RUNTIME_OUTPUT} ${RUNTIME_ERROR} ${OUTPUT_DIR}

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
