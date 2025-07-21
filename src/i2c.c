#include "i2c.h"

#define __nop() __asm nop __endasm

void delay_20_cycles(void) {
    u8 counter = 20;
    while (counter--) {
        __nop();
    }
}

void i2c_set_sda_high(void) {
    SDA24 = 1;
    delay_20_cycles(); // Ensure sufficient delay for clock high
}

void i2c_set_sda_low(void) {
    SDA24 = 0;
    delay_20_cycles(); // Ensure sufficient delay for clock low
}

void i2c_set_sck_high(void){
    SCK24 = 1;
    delay_20_cycles(); // Ensure sufficient delay for clock high
}

void i2c_set_sck_low(void) {
    SCK24 = 0;
    delay_20_cycles(); // Ensure sufficient delay for clock low
}

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
    i2c_set_sda_high();
    i2c_set_sck_high();
    EA = 1;
}

__bit i2c_send(u8 byte)
{
    u8 i;
    __bit ack_received = 0;

    i2c_set_sck_low();  // Ensure SCL starts low

    // Send 8 bits, MSB first
    for (i = 0; i < 8; i++) {
        if (byte & 0x80)
            i2c_set_sda_high();
        else
            i2c_set_sda_low();

        byte <<= 1;

        i2c_set_sck_high();  // Clock high
        i2c_set_sck_low();   // Clock low
    }

    // Release SDA, prepare to read ACK
    i2c_set_sda_high();   // May ensure pull-up if open-drain
    i2c_set_sda_input();  // Tri-state SDA
    i2c_set_sck_high();   // Clock high to read ACK

    // Read ACK (SDA low = ACK received)
    ack_received = !i2c_read_sda();

    i2c_set_sck_low();    // Finish ACK clock
    i2c_set_sda_output(); // Take back SDA control
    i2c_set_sda_high();   // Idle state

    return ack_received;
}

void i2c_set_sda_input(void){
    P4CON = 0xDE;
}

void i2c_set_sda_output(void){
    P4CON = 0xFE;
}

__bit i2c_read_sda(void) {
    return SDA24; 
}