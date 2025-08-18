#ifndef BATTERY_H
#define BATTERY_H

#include "TA3782F.h"
#include "types.h"

// Constants
#define ADC_CHANNEL_12      0x0C
#define ADC_ENABLE          0x80
#define ADC_START           0x40
#define ADC_IF              0x20
#define CTM1_ENABLE_INT_REF 0x40  // OP_CTM1 bit 6

#define AIN12_ENABLE_MASK  0x10  // ADCCFG1 bit 4
#define ADC_REF_VOLTAGE    2.048f
#define ADC_MAX_VALUE      4095.0f  // 12-bit

u16 battery_read(void);
u16 battery_read_bnkreg(void);
u16 battery_read_voltage(void);



#endif // BATTERY_H