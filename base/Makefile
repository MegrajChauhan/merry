# Variable definitions
CC = gcc
FLAGS = -Wall -Wextra -MMD -MP -g -fsanitize=address -fno-omit-frame-pointer -fPIC
DIRS = 	abs/hard \
		abs/soft \
		abs/os \
		components \
		core_defins \
		interface \
		lib_defs \
		lib \
		utils
SRC_DIR = ./
ASM_DIR = abs/hard/
INC_DIRS = ${addprefix -I, ${DIRS}}
FLAGS += ${flags}

OUTPUT_DIR = ../build/
OUTPUT_DEPS= ../build/

FILES_TO_COMPILE = ${foreach _D, ${SRC_DIR},${wildcard ${_D}*.c}}
ASM_FILES = ${wildcard ${ASM_DIR}*.S}
OUTPUT_FILES_NAME = ${patsubst %.c, ${OUTPUT_DIR}%.o, ${FILES_TO_COMPILE}}
OUTPUT_FILES_NAME += ${patsubst %.S, ${OUTPUT_DIR}%.o, ${ASM_FILES}}
DEPS=${patsubst %.c, ${OUTPUT_DEPS}%.d, ${FILES_TO_COMPILE}}

#....
all: directories ${OUTPUT_FILES_NAME}
	${CC} -shared ${OUTPUT_FILES_NAME} -o ${OUTPUT_DIR}/libmbase.so

${OUTPUT_DIR}%.o: %.c
	${CC} ${FLAGS} ${INC_DIRS} -c $< -o $@

${OUTPUT_DIR}%.o: %.S
	${CC}  -Wall -Wextra -g -fsanitize=address -fno-omit-frame-pointer -fPIC ${INC_DIRS} -c $< -o $@

# Create necessary directories
directories:
	mkdir -p ${OUTPUT_DIR}
	${foreach f, ${SRC_DIR}, ${shell mkdir -p ${OUTPUT_DIR}${f}}}
	${foreach f, ${ASM_FILES}, ${shell mkdir -p ${OUTPUT_DIR}${f}}}

clean:
	rm -rf ${OUTPUT_DIR}

.PHONY: all clean directories

-include $(DEPS)
