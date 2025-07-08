#include "watchdog.h"


void watchdog_reset(void)
{
    // Unlock sequence for IOHCON protected registers
    IOHCON0 = 0xAA;
    IOHCON0 = 0x55;

    // Set bit 0 of IOHCON1 to reset/feed the watchdog timer
    IOHCON1 |= 0x01;
}
