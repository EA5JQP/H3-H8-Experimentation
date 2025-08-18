#ifndef EEPROM_H
#define EEPROM_H    

#include "TA3782F.h"
#include "types.h"
#include "i2c.h"

void eeprom_init(const u16 addr);
void eeprom_start(void);
void eeprom_read(const u16 addr, u8* destination, const u8 size);
void eeprom_write(u16 addr, const __xdata u8 *data, u8 size);
void eeprom_check_all_addresses(void);


#endif // EEPROM_H