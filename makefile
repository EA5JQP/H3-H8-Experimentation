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
CFLAGS += --model-small          # Use small memory model
CFLAGS += --stack-auto           # Use automatic stack allocation
CFLAGS += --int-long-reent       # Reentrant functions for better memory usage
CFLAGS += --float-reent          # Reentrant float functions

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

# Test targets
TEST_FEATURES = beep led uart i2c eeprom lcd at1846s filters font menu keypad

# Build all test firmwares
test-all:
	@echo "Building all test firmwares..."
	@for feature in $(TEST_FEATURES); do \
		echo "=== Building $$feature test firmware ==="; \
		mkdir -p test/$$feature/build; \
		$(MAKE) -C test/$$feature all || exit 1; \
	done
	@echo "=== All test firmwares built successfully ==="

# Build individual test firmware
test-beep:
	@echo "Building beep test firmware..."
	@mkdir -p test/beep/build
	@$(MAKE) -C test/beep all

test-led:
	@echo "Building led test firmware..."
	@mkdir -p test/led/build
	@$(MAKE) -C test/led all

test-uart:
	@echo "Building uart test firmware..."
	@mkdir -p test/uart/build
	@$(MAKE) -C test/uart all

test-i2c:
	@echo "Building i2c test firmware..."
	@mkdir -p test/i2c/build
	@$(MAKE) -C test/i2c all

test-eeprom:
	@echo "Building eeprom test firmware..."
	@mkdir -p test/eeprom/build
	@$(MAKE) -C test/eeprom all

test-lcd:
	@echo "Building lcd test firmware..."
	@mkdir -p test/lcd/build
	@$(MAKE) -C test/lcd all

test-at1846s:
	@echo "Building at1846s test firmware..."
	@mkdir -p test/at1846s/build
	@$(MAKE) -C test/at1846s all

test-filters:
	@echo "Building filters test firmware..."
	@mkdir -p test/filters/build
	@$(MAKE) -C test/filters all

test-font:
	@echo "Building font test firmware..."
	@mkdir -p test/font/build
	@$(MAKE) -C test/font all

test-menu:
	@echo "Building menu test firmware..."
	@mkdir -p test/menu/build
	@$(MAKE) -C test/menu all

test-keypad:
	@echo "Building keypad test firmware..."
	@mkdir -p test/keypad/build
	@$(MAKE) -C test/keypad all

# Clean all test builds
clean-tests:
	@echo "Cleaning all test builds..."
	@for feature in $(TEST_FEATURES); do \
		echo "Cleaning $$feature test..."; \
		$(MAKE) -C test/$$feature clean 2>/dev/null || true; \
	done
	@echo "All test builds cleaned"

# Show available test targets
list-tests:
	@echo "Available test targets:"
	@echo "  test-all      - Build all test firmwares"
	@echo "  clean-tests   - Clean all test builds"
	@echo "  list-tests    - Show this help"
	@echo "Individual test targets:"
	@for feature in $(TEST_FEATURES); do \
		echo "  test-$$feature"; \
	done

# Utility targets
.PHONY: clean print pad all build test-all clean-tests list-tests test-beep test-led test-uart test-i2c test-eeprom test-lcd test-at1846s test-filters test-font

clean:
	rm -rf $(DIR_BUILD)/*
	rm -rf $(DIR_BIN)/*

print:
	@echo "Sources: $(SRCS)"
	@echo "Rel files: $(RELS)"
