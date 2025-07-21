#ifndef I2C_H
#define I2C_H

#include "TA3782F.h"
#include "types.h"
#include "H8.h"

void delay_20_cycles(void);
void i2c_set_sda_high(void);
void i2c_set_sda_low(void);
void i2c_set_sck_high(void);
void i2c_set_sck_low(void);
void i2c_start(void);
void i2c_stop(void);
__bit i2c_send(u8 byte);
void i2c_set_sda_input(void);
void i2c_set_sda_output(void);
__bit i2c_read_sda(void);

#endif