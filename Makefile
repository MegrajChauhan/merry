# Variable definitions
CC = gcc
FLAGS = -Wall -Wextra -MMD -MP -g -fsanitize=address -fno-omit-frame-pointer
DIRS = merry/cores \
       merry/defs \
       merry/graves \
       merry/interface \
       merry/req \
       base/abs/hard \
       base/abs/soft \
       base/abs/os \
       base/components \
       base/core_defins \
       base/interface \
       base/lib \
       base/lib_defs \
       base/utils \
       merry/utils
SRC_DIR = merry/
INC_DIRS = ${addprefix -I, ${DIRS}}
FLAGS += ${flags}

OUTPUT_DIR = build/
OUTPUT_DEPS= build/

FILES_TO_COMPILE = ${foreach _D, ${SRC_DIR},${wildcard ${_D}*.c}}
OUTPUT_FILES_NAME = ${patsubst %.c, ${OUTPUT_DIR}%.o, ${FILES_TO_COMPILE}}
DEPS=${patsubst %.c, ${OUTPUT_DEPS}%.d, ${FILES_TO_COMPILE}}

all: directories ${OUTPUT_FILES_NAME}
	make -C base
	${CC} ${FLAGS} ${OUTPUT_FILES_NAME} mvm.c -Lbuild -lmbase ${INC_DIRS} -o ${OUTPUT_DIR}merry

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
