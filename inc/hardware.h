#ifndef HARDWARE_H
#define HARDWARE_H
 
#include "TA3782F.h"

#include "types.h"

#define BANK1_R6  battery_low   // ADC low byte
#define BANK1_R7  battery_high  // ADC high byte



// Function prototypes
void hardware_init(void);
void timer_init(void);
 
 
#endif // HARDWARE_H