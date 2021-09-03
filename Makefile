CC := gcc
BIN := controladora

BUILD_DIR := build

FREERTOS_DIR_REL := $(FREERTOS_PATH)/FreeRTOS
FREERTOS_DIR := $(abspath $(FREERTOS_DIR_REL))

FREERTOS_PLUS_DIR_REL := $(FREERTOS_PATH)/FreeRTOS-Plus
FREERTOS_PLUS_DIR := $(abspath $(FREERTOS_PLUS_DIR_REL))

#INCLUDE_DIRS := -I.
INCLUDE_DIRS += -I./include
INCLUDE_DIRS += -I${FREERTOS_DIR}/Source/include
INCLUDE_DIRS += -I${FREERTOS_DIR}/Source/portable/ThirdParty/GCC/Posix
INCLUDE_DIRS += -I${FREERTOS_DIR}/Source/portable/ThirdParty/GCC/Posix/utils

LOCAL_SOURCE_FILES := $(wildcard ./src/*.c)
FREERTOS_SOURCE_FILES += $(wildcard ${FREERTOS_DIR}/Source/*.c)
# Memory manager (use malloc() / free() )
FREERTOS_SOURCE_FILES += ${FREERTOS_DIR}/Source/portable/MemMang/heap_3.c
# posix port
FREERTOS_SOURCE_FILES += ${FREERTOS_DIR}/Source/portable/ThirdParty/GCC/Posix/utils/wait_for_event.c
FREERTOS_SOURCE_FILES += ${FREERTOS_DIR}/Source/portable/ThirdParty/GCC/Posix/port.c


CFLAGS := -ggdb3 -O0
LDFLAGS := -ggdb3 -O0 -pthread -lpcap -lm

OBJ_FILES := $(LOCAL_SOURCE_FILES:%.c=$(BUILD_DIR)/%.o)
OBJ_FILES += $(FREERTOS_SOURCE_FILES:$(FREERTOS_PATH)/%.c=$(BUILD_DIR)/%.o)

DEP_FILE = $(OBJ_FILES:%.o=%.d)

${BIN} : $(BUILD_DIR)/$(BIN)

${BUILD_DIR}/${BIN} : ${OBJ_FILES}
	-mkdir -p ${@D}
	$(CC) $^ $(CFLAGS) $(INCLUDE_DIRS) ${LDFLAGS} -o $@


-include ${DEP_FILE}

${BUILD_DIR}/%.o : %.c
	-mkdir -p $(@D)
	echo
	echo $(OBJ_FILES)
	echo
	$(CC) $(CFLAGS) ${INCLUDE_DIRS} -MMD -c $< -o $@

${BUILD_DIR}/%.o : $(FREERTOS_PATH)/%.c
	-mkdir -p $(@D)
	echo
	echo $(OBJ_FILES)
	echo
	$(CC) $(CFLAGS) ${INCLUDE_DIRS} -MMD -c $< -o $@

.PHONY: clean

clean:
	-rm -rf $(BUILD_DIR)
