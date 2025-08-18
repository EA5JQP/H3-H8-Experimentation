#ifndef I2C_H
#define I2C_H

#include "TA3782F.h"
#include "types.h"
#include "H8.h"

// Initialization and basic delay functions
void i2c_init(void);
void i2c_bus_reset(void);
void i2c_delay(void);
void i2c_set_sda_high(void);
void i2c_set_sda_low(void);
void i2c_set_sda(u8 value);
void i2c_set_sck_high(void);
void i2c_set_sck_low(void);
void i2c_set_sda_input(void);
void i2c_set_sda_output(void);
__bit i2c_read_sda(void);

// I2C protocol functions
void i2c_start(void);
void i2c_stop(void);
__bit i2c_send(u8 byte);
u8 i2c_receive(__bit send_ack);

// High-level I2C functions
void i2c_write(const u8 *source, u8 length);
void i2c_read(u8* destination, u8 length);

#endif