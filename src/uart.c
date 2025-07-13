#include "uart.h"

void uart_bt_init(void) {
    SCON &= 0x3F;
    SCON |= 0x40;
    // The following SFR is incorrectly named P1CON in the datasheet provided by TIDRADIO.
    P1CON &= 0xF8;
    P1CON |= 4;
    SCON |= 0x10;
    PCON = PCON |= 0x80;
    exUart1Baud_H = 200;
    exUart1Baud_L = 0xFE;
    exUart2Con &= 0xF8;
    exUart2Con |= 0x80;
    exP20Mode = 2;
    exP21Mode = 2;
    IP |= 0x10;
}

void uart_pr_init(void) {

    EXA1 &= 0x3F;
    EXA1 |= 0x40;
    // The following SFR is incorrectly named P1CON in the datasheet provided by TIDRADIO.
    P1CON &= 0x8F;
    P1CON |= 0x30;
    EXA1 |= 0x10;
    PCON |= 0x40;
    exUart1Baud_H = 0xB2;
    exUart1Baud_L = 0xFF;
    exUart1Con &= 0xF8u;
    exUart1Con |= 0x80;
    exUart1Port = 0x11;
    exP11Mode = 2;
    exP13Mode = 2;
}

void uart_pr_send_byte(const u8 byte) {
    
    EXA2 = byte;
    
    u8 timeout_low = 0x00;  // Initialize low byte of timeout counter
    u8 timeout_high = 0x00; // Initialize high byte of timeout counter

    do {
        // Check if transmit complete flag is set (bit 1 of EXA1)
        if (EXA1 & 0x02) break;  // TX complete flag
        // Timeout counter (prevents infinite loop)
        timeout_low++;              // Increment low byte
        if (timeout_low == '\0') {  // If low byte overflows
            timeout_high ++;        // Increment high byte
        }
    } while ((timeout_high != 0x4E) || (timeout_low != 0x20));

    // Clear transmit complete flag
    EXA1 &= 0xFD; // Clear bit 1 (TX complete flag) in EXA1
}

void uart_bt_send_byte(const u8 byte) {
    u8 status;
    SBUF = byte;

    // Wait for transmission to complete (polling method)
    do {
        status = SCON;
    } while ((status & 0x02) == 0);  // Wait for TX complete flag (bit 1)
    
    // Clear transmit complete flag
    SCON &= 0xFD;  // Clear bit 1 (TX complete flag)
}