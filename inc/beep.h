#ifndef __BEEP_H__
#define __BEEP_H__

#include "types.h"
#include "delay.h"
#include "TA3782F.h"  
#include "H8.h"

// Basic beep control functions
void beep_init(void);
void beep_off(void);
void beep_power_off(void);

// Core beep function
void beep_tone(const u8 duration, const u16 waveLength);


#endif // __BEEP_H__