#ifndef EEPROM_H
#define EEPROM_H    

#include "TA3782F.h"
#include "types.h"
#include "i2c.h"
#include "delay.h"
#include "uart.h"

__bit eeprom_init(const u16 addr);
void eeprom_start(void);
__bit eeprom_read(const u16 addr, u8* destination, const u8 size);
__bit eeprom_write(u16 addr, const __xdata u8 *data, u8 size);
void eeprom_check_all_addresses(void);
// Test functions moved to test_functions_reference.c



#endif // EEPROM_H