#ifndef KEYPAD_H
#define KEYPAD_H

#include "TA3782F.h"
#include "H8.h"
#include "types.h"
#include "delay.h"

#define KEY_FLSH    0x16

#define KEY_PTTA    0x13
#define KEY_SIDE1   0x11
#define KEY_SIDE2   0x12

#define KEY_VFO     0x17
#define KEY_BLANK   0x18
#define KEY_AB      0x19

#define KEY_MENU    0x0B
#define KEY_UP      0x0C
#define KEY_DOWN    0x0D
#define KEY_EXIT    0x0E

#define KEY_1       0x02
#define KEY_2       0x03
#define KEY_3       0x04
#define KEY_STAR    0x0F

#define KEY_4       0x05
#define KEY_5       0x06
#define KEY_6       0x07
#define KEY_0       0x01

#define KEY_7       0x08
#define KEY_8       0x09
#define KEY_9       0x0A
#define KEY_HASH    0x10

#define KEY_MENU    0x0B
#define KEY_UP      0x0C
#define KEY_DOWN    0x0D
#define KEY_EXIT    0x0E


u8 keypad_scan(void);

#endif