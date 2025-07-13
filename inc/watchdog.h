#ifndef WATCHDOG_H
#define WATCHDOG_H

#include "TA3782F.h"

void watchdog_init(void);
void watchdog_reset(void);
void watchdog_config(void);

#endif