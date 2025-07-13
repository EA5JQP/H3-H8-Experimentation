#ifndef PWM_H
#define PWM_H

#include "types.h"
#include "TA3782F.h"

// Function prototypes
void pwm_init(u8 duty_cycle_high,u8 duty_cycle_low);
void pwm_pin_setup(void);

#endif // HARDWARE_H