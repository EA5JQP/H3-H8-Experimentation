# Common makefile rules for test firmwares
# Usage: include ../../shared/common.mk

# Get test name from current directory
TEST_NAME = $(notdir $(CURDIR))

# Project Configuration
PROJ_NAME = firmware_$(TEST_NAME)
FIRMWARE_SIZE = 23767

# Directories (relative to test subdirectory)
DIR_INC = ../../inc
DIR_SRC = ../../src
DIR_BUILD = ./build
DIR_BIN = ../../bin/test/$(TEST_NAME)
BIN_ORIG = ${DIR_BIN}/${PROJ_NAME}.bin
BIN_PADDED = ${DIR_BIN}/${PROJ_NAME}_padded.bin

# Compiler
CC = sdcc
CFLAGS = -I${DIR_INC} --code-size ${FIRMWARE_SIZE}
CFLAGS += --model-small          # Use small memory model
CFLAGS += --stack-auto           # Use automatic stack allocation
CFLAGS += --int-long-reent       # Reentrant functions for better memory usage
CFLAGS += --float-reent          # Reentrant float functions

# Core sources (always needed)
CORE_SRCS = delay.c watchdog.c hardware.c pwm.c uart.c keypad.c lcd.c battery.c font.c i2c.c eeprom.c at1846s.c at1846s_reg.c uart_test.c

# Test-specific main
TEST_MAIN = main.c

# Generate source paths and object files
SRCS = $(TEST_MAIN) $(addprefix ${DIR_SRC}/,${CORE_SRCS}) $(addprefix ${DIR_SRC}/,${TEST_SRCS})
RELS = $(patsubst %.c,${DIR_BUILD}/%.rel,$(notdir ${SRCS}))

# Targets
all: clean build pad

build: $(BIN_PADDED)

$(BIN_PADDED): $(BIN_ORIG)
	@size=$$(stat -c %s $(BIN_ORIG)); \
	pad=$$(( $(FIRMWARE_SIZE) - $$size )); \
	cp $(BIN_ORIG) $(BIN_PADDED); \
	if [ $$pad -gt 0 ]; then \
	  printf '\xFF%.0s' $$(seq 1 $$pad) >> $(BIN_PADDED); \
	fi

$(BIN_ORIG): $(DIR_BIN)/$(PROJ_NAME).hex
	objcopy -I ihex -O binary $< $@

$(DIR_BIN)/$(PROJ_NAME).hex: $(DIR_BIN)/$(PROJ_NAME).ihx
	packihx $< > $@

$(DIR_BIN)/$(PROJ_NAME).ihx: $(RELS) | $(DIR_BIN)
	$(CC) $^ -o $@

# Compile main.c from current directory
$(DIR_BUILD)/main.rel: main.c | $(DIR_BUILD)
	$(CC) $(CFLAGS) -c $< -o $@

# Compile source files from src directory
$(DIR_BUILD)/%.rel: $(DIR_SRC)/%.c | $(DIR_BUILD)
	$(CC) $(CFLAGS) -c $< -o $@

# Ensure build directories exist
$(DIR_BUILD):
	mkdir -p $(DIR_BUILD)

$(DIR_BIN):
	mkdir -p $(DIR_BIN)

# Utility targets
.PHONY: clean print pad all build

clean:
	rm -rf $(DIR_BUILD)/*
	rm -rf $(DIR_BIN)/*

print:
	@echo "Test Name: $(TEST_NAME)"
	@echo "Sources: $(SRCS)"
	@echo "Objects: $(RELS)"
	@echo "Test Sources: $(TEST_SRCS)"