# H3/H8 Radio Firmware Reverse Engineering Project

## Overview

This project contains reverse-engineered firmware for H8 handheld radios based on the TA3782F microcontroller and AT1846S radio transceiver chip.

## Project Status

### ✅ Working Components
- **UART Communication**: Serial interface working over cable (Bluetooth untested)
- **AT1846S Radio Control**: Comprehensive radio transceiver management
  - 70+ functions covering all major AT1846S features
  - Power management, frequency control, audio processing
  - Sub-audio support (CTCSS/CDCSS), VOX, DTMF functionality
  - Safe testing framework without TX operations
- **Keypad Input**: Matrix keypad scanning and input handling
- **LCD Display**: GLCD control and text rendering
- **Backlight Control**: LED backlight functionality
- **Hardware Initialization**: GPIO, timers, and peripheral setup
- **Battery Voltage Monitoring**: ADC-based voltage reading with interpretation
- **I2C/EEPROM Implementation**: Bidirectional I2C communication for external EEPROM

### 🔄 Recently Completed
- **AT1846S Communication Tests**: Comprehensive test suite for radio chip validation
  - Basic communication and chip ID verification
  - Register read/write operations
  - Power control (RX mode only for safety)
  - Read-only register access (RSSI, battery, status)
  - Frequency register accessibility testing
  - Audio control verification
- **Diagnostic Framework**: Enhanced debugging with proper 32-bit number display and battery voltage interpretation

### 📁 Project Structure
- **Core Files**:
  - `src/at1846s.c/.h` - Complete AT1846S driver implementation
  - `src/at1846s_test.c/.h` - Safe testing framework for radio functionality
  - `src/main.c` - Integration and test execution
- **Key Features**: 60+ register definitions, band-specific frequency control, comprehensive error handling

## Building the Firmware

### Prerequisites
- SDCC (Small Device C Compiler)
- Make utility
- objcopy (for binary conversion)
- packihx (Intel HEX packer)

### Build Commands

#### Main Firmware
```bash
make all       # Clean, build, and pad firmware (42K)
make build     # Build only
make clean     # Clean build artifacts
make print     # Show build information
```

The build system produces:
- `bin/firmware.ihx` - Intel HEX format
- `bin/firmware.bin` - Raw binary (42K)
- `bin/firmware_padded.bin` - Padded to exact firmware size

#### Individual Test Firmwares
Build memory-optimized test firmwares (24-29K each, ~40% smaller than main firmware):

```bash
make test-uart          # Build UART communication test (24K)
make test-beep          # Build audio/buzzer test (24K)
make test-led           # Build LED control test (24K)
make test-i2c           # Build I2C bus test (25K)
make test-eeprom        # Build EEPROM memory test (29K)
make test-lcd           # Build display test (24K)
make test-at1846s       # Build radio chip test (25K)
make test-filters       # Build signal filtering test (27K)
make test-font          # Build font display test (25K)
```

#### Batch Test Operations
```bash
make test-all           # Build ALL test firmwares at once
make clean-tests        # Clean all test builds
make list-tests         # Show available test targets
```

#### Multiple Tests
```bash
make test-uart test-led test-beep    # Build multiple tests in one command
```

### Generated Test Files
Individual test firmwares are generated in:
- `bin/test/uart/firmware_uart_padded.bin` (24K)
- `bin/test/beep/firmware_beep_padded.bin` (24K)
- `bin/test/led/firmware_led_padded.bin` (24K)
- `bin/test/i2c/firmware_i2c_padded.bin` (25K)
- `bin/test/eeprom/firmware_eeprom_padded.bin` (29K)
- `bin/test/lcd/firmware_lcd_padded.bin` (24K)
- `bin/test/at1846s/firmware_at1846s_padded.bin` (25K)
- `bin/test/filters/firmware_filters_padded.bin` (27K)
- `bin/test/font/firmware_font_padded.bin` (25K)

### Test Firmware Benefits
- **Memory optimized**: 40% smaller than main firmware
- **Isolated testing**: Each firmware tests only one feature
- **Faster flashing**: Smaller binaries = quicker upload
- **Easier debugging**: Focus on specific functionality


## Acknowledgments

- Thanks to https://www.patreon.com/c/nicsure for publishing the NicFw code

## Important Notice

**USE AT YOUR OWN RISK** - This is experimental firmware based on reverse engineering. Some register addresses may be incorrect and could potentially damage hardware.

## Legal Notice

This project is for educational and research purposes only. Users are responsible for ensuring compliance with local regulations regarding radio operation and firmware modification.


