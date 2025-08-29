/*
 * H8 Radio Hardware Initialization
 * 
 * Basic MCU and GPIO initialization
 */

 #include "hardware.h"
 
void hardware_init(void) {
    // This is the main hardware initialization function for the H8 radio.
    // For the H3 it needs to be adapted since the H3 has a different pinout and some different hardware features.

    // Port Configuration Registers
    P0CON = 0xFC;

    // P1CON is not initialized in the decompiled code 
    // Looks like the address is assigned to P1CON in the datasheet incorrectly, since 0x96 is used for UART 1 Control 
    // The keypad configuration appears incomplete in the decompiled code because ports 17, 16, 15 and 14 must be set to OUTPUT mode
    // yet the radio obviously works, indicating there's configuration happening elsewhere that we haven't identified yet.
    P2CON = 0xFD; 

    // Port Pull-up/Pull-down Control  
    P2PH = 0x7E;
    P3CON = 0x60;
    P4CON = 0xFE;

    // Extended Pin Mode Configuration
    exP40Mode = 1;  

    P5CON = 0xFF;

    // Port Initial Values
    P0 = 0;
    P1 = 10;  // P1 = 10 (0x0A in hex = 00001010 binary), which sets P11 = 1 (UART RX), P13 = 1 (UART TX)
    P2 = 0;
    P3 = 0; 
    P4 = 0;
    P5 = 0;
 }

void timer_init(void)
 {
     /* Timer0 Initialization:
        - Mode 1 (16-bit timer mode)
        - Clears Timer0 registers
        - Enables Timer0 interrupt
        - Starts Timer0
     */
 
     TMCON &= ~0x08;     // Clear Timer0 control bit if needed (bit 3)
     TMOD  |= 0x01;      // Set Timer0 to mode 1 (16-bit) TL0 and TH0 are all valid
 
     TH0 = 0x00;         // High byte of Timer0 start value
     TL0 = 0x00;         // Low byte of Timer0 start value
 
     TR0 = 0;            // Stop Timer0
     ET0 = 1;            // Enable Timer0 interrupt
     TR0 = 1;            // Start Timer0
 }
 
