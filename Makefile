# Variable definitions
CC = gcc
FLAGS = -Wall -Wextra -MMD -MP -g -c -fPIC -fsanitize=address -fno-omit-frame-pointer
DIRS = merry/def \
       merry/abs \
       merry/core \
       merry/utils
SRC_DIR = merry/
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

FILES_TO_COMPILE = ${wildcard ${SRC_DIR}*.c}
OUTPUT_FILES_NAME = ${patsubst %.c, ${OUTPUT_DIR}%.o, ${FILES_TO_COMPILE}}
DEPS=${patsubst %.c, ${OUTPUT_DEPS}%.d, ${FILES_TO_COMPILE}}

all: directories ${OUTPUT_FILES_NAME} main
	${CC} -shared ${FLAGS} ${OUTPUT_DIR}merry.o ${OUTPUT_FILES_NAME} ${INC_DIRS} -o ${OUTPUT_DIR}merry.so

WATCH_PROJECT: directories ${OUTPUT_FILES_NAME}

main:
	${CC} ${FLAGS} ${INC_DIRS} -c merry.c -o ${OUTPUT_DIR}merry.o

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
