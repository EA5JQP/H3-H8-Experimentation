#ifndef AT1846S_H
#define AT1846S_H

#include "TA3782F.h"
#include "types.h"
#include "H8.h"
#include "delay.h"

// Common AT1846S register addresses
#define AT1846S_REG_CHIP_ID       0x00  // Chip ID register (read-only)
#define AT1846S_REG_MAIN_CTRL     0x30  // Main control register
#define AT1846S_REG_FREQ_HIGH     0x29  // Frequency high word
#define AT1846S_REG_FREQ_LOW      0x2A  // Frequency low word
#define AT1846S_REG_PA_CTRL       0x0A  // PA control and bias
#define AT1846S_REG_VOICE_GAIN    0x41  // Voice gain control
#define AT1846S_REG_AUDIO_CTRL    0x44  // Audio control

#define spi_setup_delay()   __builtin_delay_cycles(5)
#define spi_hold_delay()    __builtin_delay_cycles(8)

void at1846s_init(void);
void at1846s_write_register(u8 data_low, u8 data_high, u8 reg_addr);
u8 at1846s_spi_read_byte(void);
void at1846s_spi_transceive(u8 spi_command, u8 *reg_low, u8 *reg_high);
void at1846s_set_frequency(u32 freq_khz);

#endif // AT1846S_H