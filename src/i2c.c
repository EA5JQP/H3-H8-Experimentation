#include "i2c.h"

#define __nop() __asm nop __endasm

// // Initialize I2C pins (call once at startup)
// void i2c_init(void) {
//     // Configure P44 (SCK) and P45 (SDA) for I2C
//     // Try quasi-bidirectional mode for both pins like original Ghidra code
//     // P44: quasi-bidirectional (11) - bits 3:2 = 11
//     // P45: quasi-bidirectional (11) - bits 5:4 = 11  
//     P4CON = (P4CON & 0xC3) | 0x3C;  // Clear bits 5:2, set P45=11, P44=11
//     SDA24 = 1;                      // Set both pins high (idle state)
//     SCK24 = 1;
// }

// Reset I2C bus to clean state
void i2c_bus_reset(void) {
    // Ensure we're in output mode
    i2c_set_sda_output();
    
    // Set both lines high (idle state)
    i2c_set_sda_high();
    i2c_set_sck_high();
    i2c_delay();
    
    // Generate several clock pulses to clear any stuck devices
    for (u8 i = 0; i < 9; i++) {
        i2c_set_sck_low();
        i2c_delay();
        i2c_set_sck_high(); 
        i2c_delay();
    }
    
    // Send proper STOP condition
    i2c_set_sda_low();
    i2c_delay();
    i2c_set_sck_high();
    i2c_delay();
    i2c_set_sda_high();
    i2c_delay();
}

// Basic delay function - match original delay_20_cycles()
void i2c_delay(void) {
    u8 counter = 0x2d;  
    while (counter--) {
        __nop();
    }
}

// Low-level pin control functions
void i2c_set_sda_high(void) {
    SDA24 = 1;
    i2c_delay();
}

void i2c_set_sda_low(void) {
    SDA24 = 0;
    i2c_delay();
}

// Combined SDA function (replaces missing i2c_set_sda)
void i2c_set_sda(u8 value) {
    if (value) {
        i2c_set_sda_high();
    } else {
        i2c_set_sda_low();
    }
}

void i2c_set_sck_high(void) {
    SCK24 = 1;
    i2c_delay();
}

void i2c_set_sck_low(void) {
    SCK24 = 0;
    i2c_delay();
}

void i2c_set_sda_input(void) {
    // Use exact Ghidra values - the original firmware expects these specific configurations
    P4CON = 0xDE;  // Original Ghidra value includes correct P43 settings
    i2c_delay();
}

void i2c_set_sda_output(void) {
    // Use exact Ghidra values - the original firmware expects these specific configurations  
    P4CON = 0xFE;  // Original Ghidra value includes correct P43 settings
    i2c_delay();  
}

__bit i2c_read_sda(void) {
    return SDA24; 
}

// I2C protocol functions
void i2c_start(void) {
    EA = 0;
    i2c_set_sda_high();
    i2c_set_sck_high();
    i2c_set_sda_low();
    i2c_set_sck_low();
}

void i2c_stop(void) {
    i2c_set_sda_low();
    i2c_set_sck_low();
    i2c_set_sck_high();
    i2c_set_sda_high();
    EA = 1;
}

__bit i2c_send(u8 byte) {
    u8 i;
    __bit ack = 1;   // default = success

    i2c_set_sck_low(); // Ensure SCL starts low

    // Send 8 bits (MSB first)
    for (i = 0; i < 8; i++) {
        // i2c_set_sda(byte & 0x80);
        byte & 0x80 ? i2c_set_sda_high() : i2c_set_sda_low();
        byte <<= 1;
        i2c_set_sck_high();
        i2c_set_sck_low();
    }

    // Release SDA and check ACK
    i2c_set_sda_input();
    i2c_set_sda_high();  // keep line idle
    i2c_set_sck_high();

    if (i2c_read_sda()) {
        // SDA stayed high → no ACK
        ack = 0;
    }

    i2c_set_sck_low();
    i2c_set_sda_output();
    i2c_set_sda_high();

    return ack; // 1 = success (ACK), 0 = fail (NACK)
}



u8 i2c_receive(__bit send_ack) {
    u8 r = 0;
    u8 i;
    
    i2c_set_sda_input();     // Set SDA as input like Ghidra (P4CON = 0xde)
    i2c_set_sck_low();       // Start with SCL low
    i2c_delay();
    i2c_set_sda_high();  // Release SDA to read data
    
    for (i = 0; i < 8; i++) {
        i2c_set_sck_high();   // SCL high first
        i2c_delay();          // Delay to allow SDA to stabilize
        
        r <<= 1;              // Shift left for next bit
        if (i2c_read_sda()) { // Read SDA during high clock
            r |= 1;           // Set LSB if SDA is high
        }
        
        i2c_set_sck_low();    // SCL low
        i2c_delay();          // Delay after clock
    }
    
    // Send ACK/NACK like Ghidra does
    i2c_delay();
    i2c_set_sda_output();     // Back to output mode (P4CON = 0xfe)
    i2c_set_sda_high();  // Release SDA to idle state
    // removed ACK/NACK logic from here to match Ghidra
    // i2c_set_sda(!send_ack);   // Send ACK (0) or NACK (1)
    // i2c_set_sck_high();       // SCL high for ACK/NACK
    // i2c_delay();
    // i2c_set_sck_low();        // SCL low
    // i2c_delay();
    
    return r;
}

// High-level functions
void i2c_write(const u8 *source, u8 length) {
    while (length--) {
        i2c_send(*source++);
    }
}

void i2c_read(u8* destination, u8 length) {
    for (u8 i = 0; i < length; i++) {
        destination[i] = i2c_receive(i == (length - 1));  // NACK on last byte
    }
}