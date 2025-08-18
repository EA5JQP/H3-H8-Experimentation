# H3/H8 Radio Firmware Reverse Engineering Project

## Overview

This project contains reverse-engineered firmware for H8 handheld radios based on the TA3782F microcontroller and AT1846S radio transceiver chip.

## Project Status

### ✅ Working Components
- **UART Communication**: Serial interface working over cable (Bluetooth untested)
- **AT1846S Control**: Bit-banged I2C interface for radio chip communication
- **Keypad Input**: Matrix keypad scanning and input handling
- **LCD Display**: GLCD control and text rendering
- **Backlight Control**: LED backlight functionality
- **Hardware Initialization**: GPIO, timers, and peripheral setup

### 🔄 In Progress
- **Battery Voltage Reading**: Code extracted from Ghidra but not fully functional
- **I2C/EEPROM Implementation**: Pin configuration being refined for proper bidirectional operation

## Building the Firmware

### Prerequisites
- SDCC (Small Device C Compiler)
- Make utility
- objcopy (for binary conversion)
- packihx (Intel HEX packer)

### Build Commands
```bash
make all       # Clean, build, and pad firmware
make build     # Build only
make clean     # Clean build artifacts
```

The build system produces:
- `bin/firmware.ihx` - Intel HEX format
- `bin/firmware.bin` - Raw binary (23,767 bytes)
- `bin/firmware_padded.bin` - Padded to exact firmware size


## Acknowledgments

- Thanks to https://www.patreon.com/c/nicsure for publishing the NicFw code

## Important Notice

**USE AT YOUR OWN RISK** - This is experimental firmware based on reverse engineering. Some register addresses may be incorrect and could potentially damage hardware.

## Legal Notice

This project is for educational and research purposes only. Users are responsible for ensuring compliance with local regulations regarding radio operation and firmware modification.


