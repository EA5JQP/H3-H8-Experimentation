#ifndef EEPROM_TEST_H
#define EEPROM_TEST_H

#include "eeprom.h"
#include "types.h"
#include "uart.h"
#include "delay.h"

// EEPROM Test Data
extern __xdata u8 eeprom_test_data[32];

// EEPROM Test Functions
void test_eeprom_read_initial(u16 addr);  // Check what's there before writing
void test_eeprom_write(u16 addr);
void test_eeprom_read(u16 addr);
void test_eeprom_verify(u16 addr);
void eeprom_write_read_test(void);
void eeprom_scan_all_content(void);


#endif // EEPROM_TEST_H