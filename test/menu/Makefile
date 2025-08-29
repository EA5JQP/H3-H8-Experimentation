# Menu Test Firmware Configuration
PROJ_NAME = firmware_menu
FIRMWARE_SIZE = 23767

# Directories
DIR_INC = ../../inc
DIR_SRC = .
DIR_BUILD = ./build
DIR_BIN = ../../bin/test/menu
BIN_ORIG = ${DIR_BIN}/${PROJ_NAME}.bin
BIN_PADDED = ${DIR_BIN}/${PROJ_NAME}_padded.bin

# Compiler
CC = sdcc
CFLAGS = -I${DIR_INC} --code-size ${FIRMWARE_SIZE}
CFLAGS += --model-small
CFLAGS += --stack-auto
CFLAGS += --int-long-reent
CFLAGS += --float-reent

# Core sources (minimal set for menu testing)
CORE_SRCS = main_menu_minimal.c \
           ../../src/hardware.c \
           ../../src/delay.c \
           ../../src/watchdog.c \
           ../../src/pwm.c \
           ../../src/uart.c \
           ../../src/uart_test.c \
           ../../src/keypad.c \
           ../../src/lcd.c \
           ../../src/font.c \
           ../../src/i2c.c \
           ../../src/eeprom.c \
           ../../src/menu.c \
           ../../src/settings.c

RELS = $(patsubst %.c,${DIR_BUILD}/%.rel,$(notdir ${CORE_SRCS}))

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

# Build rules for each source file
$(DIR_BUILD)/main_menu_minimal.rel: main_menu_minimal.c | $(DIR_BUILD)
	$(CC) $(CFLAGS) -c $< -o $@

$(DIR_BUILD)/%.rel: ../../src/%.c | $(DIR_BUILD)
	$(CC) $(CFLAGS) -c $< -o $@

# Ensure build directories exist
$(DIR_BUILD):
	mkdir -p $(DIR_BUILD)

$(DIR_BIN):
	mkdir -p $(DIR_BIN)

clean:
	rm -rf $(DIR_BUILD)
	rm -rf $(DIR_BIN)

print:
	@echo "Menu test firmware:"
	@echo "  Binary: $(BIN_PADDED)"
	@echo "  Size target: $(FIRMWARE_SIZE) bytes"
	@if [ -f $(BIN_ORIG) ]; then \
		echo "  Actual size: $$(stat -c %s $(BIN_ORIG)) bytes"; \
	fi

.PHONY: all build clean print pad