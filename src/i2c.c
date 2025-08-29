
#include "i2c.h"

#define __nop() __asm nop __endasm

// --- Bus Initialization ---

void i2c_init(void) {
    // Initialize I2C bus to a known good state
    // Both SDA and SCL should be high (idle state) with proper pin configuration
    
    // Set pins to output mode first
    i2c_set_sda_output();
    
    // Ensure both lines are high (bus idle state)
    i2c_set_sda_high();
    i2c_set_sck_high();
    
    // Brief delay to stabilize
    i2c_delay();
    i2c_delay();
    
    // Reset bus in case it was stuck in a bad state
    i2c_bus_reset();
}

// --- Core Protocol Functions ---

void i2c_start(void) {
    // A START condition is a HIGH-to-LOW transition of SDA while SCL is HIGH.
    // Note: Interrupt control is handled at the transaction level, not per function
    i2c_set_sda_output();
    i2c_set_sda_high();
    i2c_set_sck_high();
    i2c_delay();
    i2c_set_sda_low();
    i2c_set_sck_low();
}

void i2c_stop(void) {
    // A STOP condition is a LOW-to-HIGH transition of SDA while SCL is HIGH.
    // Note: Interrupt control is handled at the transaction level, not per function
    i2c_set_sda_output();
    i2c_set_sda_low();
    i2c_set_sck_low(); // Ensure SCL is low before raising SDA
    i2c_delay();    
    i2c_set_sck_high();
    i2c_set_sda_high();
}

__bit i2c_send(u8 byte) {
    u8 i;
    __bit ack;

    i2c_set_sda_output(); // Master drives the bus to send data.

    // Send 8 bits, MSB first.
    for (i = 0; i < 8; i++) {
        if (byte & 0x80) {
            i2c_set_sda_high();
        } else {
            i2c_set_sda_low();
        }
        byte <<= 1;
        i2c_set_sck_high(); // Pulse clock
        i2c_set_sck_low();
    }

    // Check for ACK from the slave.
    i2c_set_sda_input(); // Release the bus for the slave to respond.
    i2c_set_sck_high();
    ack = !i2c_read_sda(); // Read ACK bit. ACK is when SDA is LOW.
    i2c_set_sck_low();

    return ack; // Return 1 for ACK (success), 0 for NACK (failure).
}

u8 i2c_receive(__bit send_nack) {
    u8 i, received_byte = 0;

    i2c_set_sda_input(); // Release the bus for the slave to send data.

    // Read 8 bits, MSB first.
    for (i = 0; i < 8; i++) {
        received_byte <<= 1;
        i2c_set_sck_high();
        if (i2c_read_sda()) {
            received_byte |= 1;
        }
        i2c_set_sck_low();
    }

    // --- FIXED: Send ACK or NACK from Master ---
    i2c_set_sda_output(); // Master takes control to send ACK/NACK.
    if (send_nack) {
        i2c_set_sda_high(); // NACK: Master leaves SDA high.
    } else {
        i2c_set_sda_low();  // ACK: Master pulls SDA low.
    }
    i2c_set_sck_high(); // Pulse clock for the slave to read the ACK/NACK.
    i2c_set_sck_low();
    i2c_set_sda_high(); // Release the bus after sending ACK/NACK.

    return received_byte;
}

// --- High-Level Read/Write Functions ---

__bit i2c_write(const u8 *source, u8 length) {
    while (length--) {
        if (!i2c_send(*source++)) {
            return 0; // NACK received, abort
        }
    }
    return 1; // All bytes sent successfully
}

void i2c_read(u8* destination, u8 length) {
    for (u8 i = 0; i < length; i++) {
        // Send NACK only on the very last byte to be read.
        destination[i] = i2c_receive(i == (length - 1));
    }
}

// --- Low-Level Pin and Delay Control ---

void i2c_delay(void) {
    // This provides a software delay. The exact duration depends on the MCU clock speed.
    // For a 12T 8051 at 12MHz, this is roughly 50us.
    u8 counter = 0x2d;
    while (counter--) {
        __nop();
    }
}

void i2c_set_sda_high(void) {
    SDA24 = 1;
    i2c_delay();
}

void i2c_set_sda_low(void) {
    SDA24 = 0;
    i2c_delay();
}

void i2c_set_sck_high(void) {
    SCK24 = 1;
    i2c_delay();
}

void i2c_set_sck_low(void) {
    SCK24 = 0;
    i2c_delay();
}

// --- Hardware-Specific Port Configuration ---

void i2c_set_sda_input(void) {
    // Sets the SDA pin to input mode.
    // The value 0xDE is specific to your microcontroller's port configuration register.
    // 11011110 = Pin 45 is deactivated => 0: Pxy is the input mode (initial value at power-on)
    P4CON = 0xDE;
    i2c_delay();
}

void i2c_set_sda_output(void) {
    // Sets the SDA pin to push-pull output mode.
    // The value 0xFE is specific to your microcontroller's port configuration register.
    // 11111110 = Pin 45 is activated => 1: Pxy is a strong push-pull output mode
    P4CON = 0xFE;
    i2c_delay();
}

__bit i2c_read_sda(void) {
    return SDA24;
}

// --- Diagnostic functions moved to test_functions_reference.c ---

// --- Bus Management ---

void i2c_bus_reset(void) {
    // This function attempts to recover a stuck I2C bus.
    i2c_set_sda_output();
    i2c_set_sda_high();
    i2c_set_sck_high();

    // Send 9 clock pulses to force any slave holding SDA low to release it.
    for (u8 i = 0; i < 9; i++) {
        i2c_set_sck_low();
        i2c_set_sck_high();
    }
    // Send a proper STOP condition to fully reset the bus state.
    i2c_stop();
}
