#ifndef DELAY_H
#define DELAY_H

#include "types.h"
#include "watchdog.h"

void delay_ms(u8 ms_high, u8 ms_low);
void delay_loop(void);

#endif
