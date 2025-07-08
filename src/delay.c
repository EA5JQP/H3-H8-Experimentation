#include "delay.h"

void delay_ms(u8 ms_high, u8 ms_low)
{
    /* Delay for (ms_high * 256 + ms_low) milliseconds.
       Uses watchdog-reset and ~1ms calibrated loop. */

    while (ms_high != 0 || ms_low != 0) {
        if (ms_low-- == 0) {
            ms_low = 0xFF;
            if (ms_high != 0)
                ms_high--;
        }

        watchdog_reset();  // Prevent watchdog timeout
        delay_loop();      // ~1ms calibrated delay
    }
}

void delay_loop(void)
{
    /* Approximately 1ms delay using nested loops.
       Inner loop: 65536 iterations
       Outer loop: 8 iterations */

    u8 inner_high = 0, inner_low = 0;
    u8 outer_high = 0, outer_low = 0;

    do {
        inner_high = 0;
        inner_low = 0;

        do {
            if (++inner_low == 0)
                ++inner_high;
        } while (inner_low != 0xFF || inner_high != 0x00);

        if (++outer_low == 0)
            ++outer_high;

    } while (outer_low != 8 || outer_high != 0);
}
