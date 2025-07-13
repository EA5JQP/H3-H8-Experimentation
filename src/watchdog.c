#include "watchdog.h"

void watchdog_init(void)
{
    EXA3 = 0x50;
    exWdT0 = 4;
    exWdT1 = 5;
    // TODO: Investigate what mode 3 actually does for P0.4/P0.5
    // These appear to be LCD-related but mechanism unclear
    exP04Mode = 3;  // P0.4 (LCDSCLK?) - mode 3 function unknown
    exP05Mode = 3;  // P0.5 (LCD_SDA?) - mode 3 function unknown
}

void watchdog_reset(void)
{
    // Unlock sequence for IOHCON protected registers
    IOHCON0 = 0xAA;
    IOHCON0 = 0x55;

    // Set bit 0 of IOHCON1 to reset/feed the watchdog timer
    IOHCON1 |= 0x01;
}

void watchdog_config(void)
{
    TMCON &= 0x1F;
    TMCON |= 0xE0;

    IOHCON0 = 0xAA;
    IOHCON0 = 0x55;
    IOHCON1 |= 2;
  
}
