# H8 Radio Firmware Analysis Report

## Overview
Analysis of H8 handheld radio firmware with TA3782F MCU and AT1846S radio chip.

## Firmware Information
- **Version**: 20230923 (found in strings)
- **MCU**: TA3782F (8051-based with extended features)
- **Radio Chip**: AT1846S (I2C-controlled transceiver)
- **Features**: Ham radio, GMRS, Bluetooth, CTCSS seek, weather channels

## Key Findings

### Communication Interface
The radio uses a custom SPI/I2C-like interface to communicate with the AT1846S:

```c
// Hardware interface pins
P0_7 = chip_select/clock
P0_6 = enable/data
CCAP3L/CCAP4L = timer registers used for bit-banged communication
```

**Key Functions Identified:**
- `spi_write_cmd()` @ 0xEED6 - Writes command with P0_7 high
- `spi_write_data()` @ 0xEFA1 - Writes data 
- `spi_write_dual_data()` @ 0xED55 - Writes two data bytes
- `radio_command_interface()` @ 0xE488 - Main radio communication interface

### Main Control Logic
- **Main State Machine**: FUN_CODE_7889 @ 0x7889
  - Monitors P1_1 (button input)
  - Handles different system modes based on button sequences
  - Controls power, initialization, and mode switching

### System Modes Found
From strings and control logic:
- Ham radio mode
- GMRS mode  
- Unlock mode (wide frequency range)
- FM Radio mode
- Weather mode
- Bluetooth mode

### Memory Layout
- **CODE**: 0x0000-0xF050 (firmware code)
- **INTMEM**: 0x20-0xFF (internal RAM)
- **EXTMEM**: External memory for configuration/channel data
- **SFR**: 0x80-0xFF (special function registers)

### Radio Control Functions
Several functions appear to handle radio configuration:

1. **FUN_CODE_314D** @ 0x314D - Large frequency/tone lookup table
   - Contains CTCSS/DCS tone detection logic
   - Multiple frequency comparisons and decoding

2. **FUN_CODE_C196** @ 0xC196 - Serial data transmission  
   - Bit-by-bit data serialization
   - Likely sends multi-byte commands to AT1846S

3. **FUN_CODE_7C90** @ 0x7C90 - Configuration sequence
   - Multiple register programming steps
   - Calibration/initialization routine
   - Uses memory addresses 0x3D-0x44 range

### Hardware Configuration
Based on the TA3782F header file:

**GPIO Ports Available:**
- P0-P5 (48 total GPIO pins)
- Extended port configuration registers
- Multiple UART interfaces (standard + 2 extended)

**Communication Interfaces:**
- Standard UART (P3.0/P3.1)
- Extended UART1 (P1.1/P1.3) 
- Extended UART2 (P2.0/P2.1) - likely for Bluetooth
- SPI-like interface for AT1846S (P0.6/P0.7)

**Other Features:**
- PWM outputs
- ADC inputs
- Timers and interrupts

## AT1846S Integration

The AT1846S radio chip is controlled via the custom interface using:
- Register-based programming (typical for AT1846S)
- Frequency synthesis control
- Audio path configuration
- Power control and calibration

## Detailed Radio Control Analysis

### AT1846S Communication Protocol
The radio uses a sophisticated bit-banged interface with the AT1846S:

```c
// Communication sequence in radio_command_interface():
spi_write_cmd(0x2a);      // Write register address 
spi_write_data(register); // Send register value twice
spi_write_data(register);
spi_write_cmd(0x2b);      // Write data command
spi_write_data(data);     // Send data value twice  
spi_write_data(data);
spi_write_cmd(0x2c);      // Finalize command
spi_write_dual_data(0xff, value); // Write final value
```

### Key Radio Control Functions

1. **FUN_CODE_C196** @ 0xC196 - Serial Data Transmission
   - Serializes 8-byte data structures bit by bit
   - Uses bit masking and shifting for data encoding
   - Calls radio_command_interface for each bit

2. **FUN_CODE_BC14** @ 0xBC14 - Data Encoding Function
   - Complex bit manipulation with XOR operations (0xC, 0x75)
   - 12-iteration encoding loop with conditional XOR
   - Appears to implement error correction or encryption

3. **FUN_CODE_805A** @ 0x805A - Frequency/Channel Initialization
   - Accesses frequency lookup tables at CODE:479D area
   - Sets up dual frequency memories (RX/TX)
   - Handles frequency range validation

4. **FUN_CODE_666B** @ 0x666B - Channel Programming Logic
   - Complex frequency and tone programming
   - Handles CTCSS/DCS encoding
   - Sets various radio flags and states

### Memory Organization

**External Memory Map (EXTMEM):**
- 0x003D-0x0043: Primary frequency storage
- 0x00E7-0x00EA: Frequency buffer 1  
- 0x0106-0x0109: Frequency buffer 2
- 0x0144: Radio configuration flags
- 0x0162-0x0165: Encoded data storage

**Frequency Storage Format:**
- 4-byte frequency representation
- Appears to use BCD or custom encoding
- Separate RX/TX frequency storage

### Radio State Management
The firmware implements a complex state machine with:
- Multiple frequency memories
- RX/TX frequency switching
- CTCSS/DCS tone handling  
- Power level control
- Scanning functionality

### Next Steps for Analysis

1. **Decode Frequency Format**
   - Analyze frequency calculation in FUN_CODE_5b29
   - Map frequency encoding to MHz values

2. **Map AT1846S Registers**
   - Trace 0x2A, 0x2B, 0x2C command sequences
   - Identify register addresses and functions

3. **CTCSS/DCS Analysis**
   - Decode tone tables in FUN_CODE_314D
   - Map standard tone frequencies

4. **Audio Path Control**
   - Find microphone gain controls
   - Locate speaker/audio routing

5. **Power Management**
   - Identify PA control functions
   - Map power level settings

## Radio Features Implemented

From string analysis, the radio supports:
- Squelch control
- TX power settings
- VOX (voice activation)
- Bandwidth selection (wide/narrow)
- Scan functions
- Memory channels
- CTCSS seek
- Alarm modes
- Language selection
- Multiple frequency bands

This appears to be a full-featured amateur/GMRS radio with professional capabilities.


KEYIN1
KEYIN2
KEYIN3
KEYIN4
KEYIN5

P30 - P34


KEYOUT1
KEYOUT2
KEYOUT3
KEYOUT4

P14 - P17
