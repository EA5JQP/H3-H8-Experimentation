#ifndef HARDWARE_H
#define HARDWARE_H
 
#include "TA3782F.h"
#include "types.h"
#include "battery.h"
#include "uart.h"
#include "watchdog.h"


// Function prototypes
void hardware_init(void);
void timer_init(void); 
 
#endif // HARDWARE_H