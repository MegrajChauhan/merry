# Variable definitions
CC = gcc
FLAGS = -Wall -Wextra -MMD -MP -g -fsanitize=address -fno-omit-frame-pointer
DIRS = merry/common \
       merry/comps \
       merry/core \
       merry/core/test_core \
       merry/core/regr_core \
       merry/defs \
       merry/graves \
       merry/graves/abs \
       merry/interface/hard \
       merry/interface/soft \
       merry/lib/defs \
       merry/lib \
       merry/use \
       merry/utils
SRC_DIR = merry/ merry/core/test_core/ merry/core/regr_core/
ASM_DIR = merry/interface/hard/
INC_DIRS = ${addprefix -I, ${DIRS}}
FLAGS += ${flags}

#ifeq (${OS}, Windows_NT)
	# most likey windows
#else
	# Something that is not Windows
	# For our makeshift makefile right now, this mess is good enough so the next most likely option is Linux
#	FLAGS += -luring
#endif

OUTPUT_DIR = build/
OUTPUT_DEPS= build/

FILES_TO_COMPILE = ${foreach _D, ${SRC_DIR},${wildcard ${_D}*.c}}
ASM_FILES = ${wildcard ${ASM_DIR}*.S}
OUTPUT_FILES_NAME = ${patsubst %.c, ${OUTPUT_DIR}%.o, ${FILES_TO_COMPILE}}
DEPS=${patsubst %.c, ${OUTPUT_DEPS}%.d, ${FILES_TO_COMPILE}}

all: directories ${OUTPUT_FILES_NAME}
	${CC} ${FLAGS} ${OUTPUT_FILES_NAME} ${ASM_FILES} mvm.c ${INC_DIRS} -o ${OUTPUT_DIR}mvm

WATCH_PROJECT: directories ${OUTPUT_FILES_NAME}

${OUTPUT_DIR}%.o: %.c
	${CC} ${FLAGS} ${INC_DIRS} -c $< -o $@

# Create necessary directories
directories:
	mkdir -p ${OUTPUT_DIR}
	${foreach f, ${SRC_DIR}, ${shell mkdir -p ${OUTPUT_DIR}${f}}}

clean:
	rm -rf ${OUTPUT_DIR}

.PHONY: all clean directories

-include $(DEPS)
