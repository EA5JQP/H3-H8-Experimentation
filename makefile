# Project Configuration
PROJ_NAME = firmware
FIRMWARE_SIZE = 23767

# Directories
DIR_INC = ./inc
DIR_SRC = ./src
DIR_BUILD = ./build
DIR_BIN = ./bin
BIN_ORIG = ${DIR_BIN}/${PROJ_NAME}.bin
BIN_PADDED = ${DIR_BIN}/${PROJ_NAME}_padded.bin

# Compiler
CC = sdcc
CFLAGS = -I${DIR_INC} --code-size ${FIRMWARE_SIZE}

# Sources and objects
SRCS = $(wildcard ${DIR_SRC}/*.c)
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
	@echo "Sources: $(SRCS)"
	@echo "Rel files: $(RELS)"
