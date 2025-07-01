#ifndef __H3_H__
#define __H3_H__

#include <TA3782F.h>  // Include the main header file for the H3 microcontroller
//#include <SC92F754x_C.h> // Include the SC92F754x_C header for additional definitions

// Pin definitions for the H3 microcontroller based on the provided pinout diagrams
// LED Control Signals
#define RX_VLED     P30     // Pin 30: P30/S3/C0 -> RX_VLED
#define TX_VLED     P31     // Pin 29: P31/S2/C1 -> TX_VLED  
#define KEY_LED     P32     // Pin 28: P32/S1/G2 -> KEY_LED

// Power Control
#define BATT_ADC    P34     // Pin 26: P34/AN8/G4 -> BATT_ADC
#define HI_LED      P35     // Pin 25: P35/AN0/G5 -> HI_LED

// Communication Interfaces
#define SCK8802     P10     // Pin 32: P10/S4 -> SCK8802
#define SDA8802     P11     // Pin 33: P11/M-SQ/T4K1/S5/TCK -> SDA8802
#define SCK8519     P12     // Pin 34: P12/SQI/S6 -> SCK8519
#define SDA8519     P13     // Pin 35: P13/SQI/SDM/TX1/S7/TD_O -> SDA8519

// Audio/Codec Control  
#define ADI_OSCA    P14     // Pin 36: P14/S8 -> ADI_OSCA
#define ADI_OSCK    P15     // Pin 37: P15/S9 -> ADI_OSCK
#define RX8622      P20     // Pin 38: P20/S10 -> RX8622
#define VRX_CTRL    P21     // Pin 39: P21/S11 -> VRX_CTRL
#define TX_C        P22     // Pin 40: P22/S12 -> TX_C
#define BEEP        P23     // Pin 41: P23/S13 -> BEEP
#define TX_V_C      P24     // Pin 42: P24/S14 -> TX_V_C
#define URX_CTRL    P25     // Pin 43: P25/S15 -> URX_CTRL

// External Interface
#define SCLK        P26     // SCLK signal
#define SDIN        P27     // SDIN signal
#define BT_TXD      P28     // BT_TXD signal  
#define BT_RXD      P29     // BT_RXD signal
#define BT_ON_OFF   P33     // Pin 27: P33/SQ/G3 -> BT_ON_OFF


// Display Interface
#define LCD_RST     P07     // LCD_RST signal
#define LCD_CD      P06     // LCD_CD signal
#define LCD_SDA     P05     // LCD_SDA signal
#define LCD_CLK     P04     // LCD_CLK signal


// Keypad Interface
#define KEY_R1      P03     // KEY_R1 signal (2)
#define KEY_R2      P02     // KEY_R2 signal (1) 
#define KEY_R3      P01     // KEY_R3 signal (48)
#define KEY_R4      P00     // KEY_R4 signal (47)
#define KEY_R5      P27     // KEY_R5 signal (46)
#define KEY_R6      P26     // KEY_R6 signal (45)   


// System Control
#define CS          P53     // CS signal
#define SO          P54     // SO signal
#define LCD_BLC_PWM P55     // LCD_BLC_PWM signal
#define PT7_RXD32   P56     // PT7_RXD32 signal
#define TXD32       P57     // TXD32 signal

// Function Control
#define APC         P60     // APC signal
#define BT_LED      P61     // BT_LED signal
#define DECODE      P62     // DECODE signal


#endif // __H3_H__