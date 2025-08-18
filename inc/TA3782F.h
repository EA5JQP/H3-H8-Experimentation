#ifndef _TA3782F_H_
#define _TA3782F_H_

// Standard 8051 Core Registers
__sfr __at(0x81) SP;        // Stack Pointer
__sfr __at(0x82) DPL;       // Data Pointer Low
__sfr __at(0x83) DPH;       // Data Pointer High
__sfr __at(0x84) DPL1;      // Data Pointer 1 Low
__sfr __at(0x85) DPH1;      // Data Pointer 1 High
__sfr __at(0x86) DPS;       // Data Pointer Select
__sfr __at(0xD0) PSW;       // Program Status Word
__sfr __at(0xE0) ACC;       // Accumulator
__sfr __at(0xE9) EXA0;      // Extended Accumulator 0
__sfr __at(0xEA) EXA1;      // Extended Accumulator 1
__sfr __at(0xEB) EXA2;      // Extended Accumulator 2
__sfr __at(0xEC) EXA3;      // Extended Accumulator 3
__sfr __at(0xED) EXBL;      // Extended B Register Low
__sfr __at(0xEE) EXBH;      // Extended B Register High
__sfr __at(0xF0) B;         // B Register

// Standard 8051 Port Registers - CORRECTED ADDRESSES
__sfr __at(0x80) P0;        // Port 0
__sfr __at(0x90) P1;        // Port 1
__sfr __at(0xA0) P2;        // Port 2
__sfr __at(0xB0) P3;        // Port 3
__sfr __at(0xC0) P4;        // Port 4 (extended)
__sfr __at(0xD8) P5;        // Port 5 (extended) - CORRECTED from 0xC8

// Port configuration registers - CORRECTED ADDRESSES based on Ghidra analysis
__sfr __at(0x9A) P0CON;     // Port 0 configuration - CONFIRMED
__sfr __at(0x91) P1CON;     // Port 1 configuration - CORRECTED from 0xA1
__sfr __at(0xA1) P2CON;     // Port 2 configuration - CORRECTED from 0xA2
__sfr __at(0xA3) P3CON;     // Port 3 configuration - CONFIRMED
__sfr __at(0xA4) P4CON;     // Port 4 configuration - CONFIRMED
__sfr __at(0xA5) P5CON;     // Port 5 configuration - CONFIRMED

// Port pull-up control registers - Added based on manufacturer specs
__sfr __at(0x9B) P0PH;      // Port 0 pull-up control
__sfr __at(0x92) P1PH;      // Port 1 pull-up control  
__sfr __at(0xA2) P2PH;      // Port 2 pull-up control
__sfr __at(0xB2) P3PH;      // Port 3 pull-up control
__sfr __at(0xC2) P4PH;      // Port 4 pull-up control
__sfr __at(0xDA) P5PH;      // Port 5 pull-up control

// IOH configuration registers
__sfr __at(0x96) IOHCON0;   // IOH0 configuration register
__sfr __at(0x97) IOHCON1;   // IOH1 configuration register

// Timer registers
__sfr __at(0x88) TCON;      // Timer control register - Added
__sfr __at(0x89) TMOD;      // Timer mode register
__sfr __at(0x8A) TL0;       // Timer 0 low byte
__sfr __at(0x8B) TL1;       // Timer 1 low byte - Added
__sfr __at(0x8C) TH0;       // Timer 0 high byte
__sfr __at(0x8D) TH1;       // Timer 1 high byte - Added
__sfr __at(0x8E) TMCON;     // Timer frequency control register

// Extended Timer registers - Added
__sfr __at(0xC8) TXCON;     // Timer 2/3/4 control register
__sfr __at(0xC9) TXMOD;     // Timer 2/3/4 mode register
__sfr __at(0xCA) RCAPXL;    // Timer 2/3/4 reload low
__sfr __at(0xCB) RCAPXH;    // Timer 2/3/4 reload high
__sfr __at(0xCC) TLX;       // Timer 2/3/4 low byte
__sfr __at(0xCD) THX;       // Timer 2/3/4 high byte
__sfr __at(0xCE) TXINX;     // Timer control register pointer

// Timer Control Bits - CORRECTED bit addresses
__sbit __at(0x8C) TR0;      // Timer 0 Run - CORRECTED
__sbit __at(0x8D) TF0;      // Timer 0 Overflow Flag - CORRECTED
__sbit __at(0x8E) TR1;      // Timer 1 Run - CORRECTED  
__sbit __at(0x8F) TF1;      // Timer 1 Overflow Flag - CORRECTED

// Interrupt Control
__sfr __at(0xA8) IE;        // Interrupt Enable
__sfr __at(0xA9) IE1;       // Interrupt Enable 1 - Added
__sfr __at(0xAA) IE2;       // Interrupt Enable 2 - Added
__sfr __at(0xB8) IP;        // Interrupt Priority
__sfr __at(0xB9) IP1;       // Interrupt Priority 1 - Added
__sfr __at(0xBA) IP2;       // Interrupt Priority 2 - Added

// Interrupt Enable Bits - CORRECTED addresses
__sbit __at(0xAF) EA;       // Global Interrupt Enable
__sbit __at(0xAE) EADC;     // ADC Interrupt Enable - Added
__sbit __at(0xAD) ET2;      // Timer 2 Interrupt Enable - Added
__sbit __at(0xAC) EUART;    // UART Interrupt Enable - Added
__sbit __at(0xAB) ET1;      // Timer 1 Interrupt Enable
__sbit __at(0xAA) EINT1;    // External Interrupt 1 Enable - Added
__sbit __at(0xA9) ET0;      // Timer 0 Interrupt Enable - CORRECTED
__sbit __at(0xA8) EINT0;    // External Interrupt 0 Enable - Added

// Power Control
__sfr __at(0x87) PCON;      // Power control

// UART Control - CORRECTED and expanded
__sfr __at(0x98) SCON;      // Serial control register
__sfr __at(0x99) SBUF;      // Serial data buffer register

// USCI (Universal Serial Communication Interface) - CORRECTED addresses
__sfr __at(0x95) US0CON0;   // USCI0 control register 0
__sfr __at(0x9D) US0CON1;   // USCI0 control register 1
__sfr __at(0x9E) US0CON2;   // USCI0 control register 2
__sfr __at(0x9F) US0CON3;   // USCI0 control register 3

// USCI1 - CORRECTED addresses from manufacturer specs
__sfr __at(0xC1) US1CON0;   // USCI1 control register 0 - CORRECTED
__sfr __at(0xD9) US1CON1;   // USCI1 control register 1 - CORRECTED
__sfr __at(0xA6) US1CON2;   // USCI1 control register 2
__sfr __at(0xA7) US1CON3;   // USCI1 control register 3

// USCI2
__sfr __at(0xC4) US2CON0;   // USCI2 control register 0
__sfr __at(0xC5) US2CON1;   // USCI2 control register 1
__sfr __at(0xC6) US2CON2;   // USCI2 control register 2
__sfr __at(0xC7) US2CON3;   // USCI2 control register 3

// ADC control and configuration registers
__sfr __at(0xAB) ADCCFG0;   // ADC Configuration Register 0
__sfr __at(0xAC) ADCCFG1;   // ADC Configuration Register 1  
__sfr __at(0xAD) ADCCON;    // ADC Control Register
__sfr __at(0xAE) ADCVL;     // ADC Value Low Register
__sfr __at(0xAF) ADCVH;     // ADC Value High Register
__sfr __at(0xB5) ADCCFG2;   // ADC Configuration Register 2
__sfr __at (0xC2) OP_CTM1;  // Custom Option Register 1 


// PWM Control - Added from manufacturer specs
__sfr __at(0xD3) PWMCON;    // PWM control register
__sfr __at(0xD4) PWMCFG;    // PWM configuration register
__sfr __at(0xD5) PWMDFR;    // PWM dead time register
__sfr __at(0xD6) PWMFLT;    // PWM fault detection register
__sfr __at(0xD7) PWMMOD;    // PWM mode register

// Watchdog and Low Frequency Timer
__sfr __at(0xCF) WDTCON;    // Watchdog timer control
__sfr __at(0xFB) BTMCON;    // Low frequency timer control

// LCD/Display Control - Added
__sfr __at(0x8F) OTCON;     // Output control register
__sfr __at(0x93) DDRCON;    // Display drive control register
__sfr __at(0x94) P1VO;      // P1 port display driver output
__sfr __at(0x9C) P0VO;      // P0 port display driver output
__sfr __at(0xB1) P2VO;      // P2 port display driver output  
__sfr __at(0xB3) P3VO;      // P3 port display driver output

// External Interrupt Control - Added
__sfr __at(0xB4) INT0F;     // INT0 falling edge interrupt control
__sfr __at(0xBB) INT0R;     // INT0 rising edge interrupt control
__sfr __at(0xBC) INT1F;     // INT1 falling edge interrupt control
__sfr __at(0xBD) INT1R;     // INT1 rising edge interrupt control
__sfr __at(0xBE) INT2F;     // INT2 falling edge interrupt control
__sfr __at(0xBF) INT2R;     // INT2 rising edge interrupt control

// Analog Comparator - Added
__sfr __at(0xB6) CMPCFG;    // Analog comparator configuration
__sfr __at(0xB7) CMPCON;    // Analog comparator control

// IAP (In-Application Programming) - Added
__sfr __at(0xF1) IAPKEY;    // IAP protection register
__sfr __at(0xF2) IAPADL;    // IAP address low register
__sfr __at(0xF3) IAPADH;    // IAP address high register
__sfr __at(0xF4) IAPADE;    // IAP extended address register
__sfr __at(0xF5) IAPDAT;    // IAP data register
__sfr __at(0xF6) IAPCTL;    // IAP control register

// Extended Control Registers - Added
__sfr __at(0xEF) OPERCON;   // Arithmetic control register
__sfr __at(0xF7) EXADH;     // Extended SRAM address high
__sfr __at(0xFC) CRCINX;    // CRC index register
__sfr __at(0xFD) CRCREG;    // CRC register
__sfr __at(0xFE) OPINX;     // Option index register
__sfr __at(0xFF) OPREG;     // Option register

__sfr __at(0xDE) UKSFR_DE; // Unknown SFR at 0xDE - Added based on Ghidra analysis
__sfr __at(0xDF) UKSFR_DF; // Unknown SFR at 0xDF - Added based on Ghidra analysis

__data __at (0x0E) volatile unsigned char BANK1_R6; // ADC result low byte (or voltage reading low)
__data __at (0x0F) volatile unsigned char BANK1_R7; // ADC result high byte (or voltage reading high)
__data __at (0x0C) volatile unsigned char BANK1_R4; // ADC result high byte (or voltage reading high)
__data __at (0x03) volatile unsigned char BANK0_R3; // ADC result high byte (or voltage reading high)

__data __at (0x0E) volatile unsigned char ADC10; // ADC 10-bit result low byte
__data __at (0x0F) volatile unsigned char ADC1;  // ADC 10-bit result high byte

// Extended memory-mapped registers confirmed by Ghidra analysis
#define exWdT0 (*((volatile unsigned char __xdata *) 0xF698))      // Extended watchdog timer 0
#define exWdT1 (*((volatile unsigned char __xdata *) 0xF699))      // Extended watchdog timer 1
#define exZrdMode (*((volatile unsigned char __xdata *) 0xF692))   // Extended Z-read mode  

#define exP34Mode (*((volatile unsigned char __xdata *) 0xF034))   // Extended P34 mode (P3.4 is used for Battery Adc in H3)
#define exP40Mode (*((volatile unsigned char __xdata *) 0xF040))   // Extended P40 mode (P4.0 is used for Battery Adc in H8)
#define exP04Mode (*((volatile unsigned char __xdata *) 0xF004))   // Extended P04 mode (P0.4 is used for LCD SCLK in H3)
#define exP05Mode (*((volatile unsigned char __xdata *) 0xF005))   // Extended P05 mode (P0.5 is used for LCD SDA in H3)
#define exP02Mode (*((volatile unsigned char __xdata *) 0xF002))   // Extended P06 mode (P0.2 is used for APC in H8)
#define exP11Mode (*((volatile unsigned char __xdata *) 0xF011))   // P1.1 pin mode
#define exP13Mode (*((volatile unsigned char __xdata *) 0xF013))   // P1.3 pin mode
#define exP43Mode (*((volatile unsigned char __xdata *) 0xF123))   // P1.2 pin mode (used for UART 1 RX in H3)
#define exP20Mode (*((volatile unsigned char __xdata *) 0xF020))   // P2.0 pin mode  
#define exP21Mode (*((volatile unsigned char __xdata *) 0xF021))   // P2.1 pin mode


// External UART 1 (P11/P13) - Validated addresses
#define exUart1Con (*((volatile unsigned char __xdata *)0xF5C0))    // UART 1 control
#define exUart1Baud_H (*((volatile unsigned char __xdata *)0xF5C1)) // UART 1 baud rate high
#define exUart1Baud_L (*((volatile unsigned char __xdata *)0xF5C2)) // UART 1 baud rate low  
#define exUart1Port (*((volatile unsigned char __xdata *)0xF69E))   // UART 1 port assignment

// Bluetooth UART (P20/P21) - Validated addresses
#define exUart2Con  (*((volatile unsigned char __xdata *)0xF5C4))    // UART 2 control
#define exUart2Baud_H  (*((volatile unsigned char __xdata *)0xF5C5)) // UART 2 baud rate high
#define exUart2Baud_L  (*((volatile unsigned char __xdata *)0xF5C6)) // UART 2 baud rate low


// H8 PWM Register Definitions
// Internal Memory Registers (IDATA space)
#define pwmUkReg1    (*((volatile unsigned char __idata *) 0x30))   // DAT_INTMEM_30
#define pwmUkReg2    (*((volatile unsigned char __idata *) 0x31))   // DAT_INTMEM_31


// External Memory PWM Registers (XDATA space)
#define exPwmInit    (*((volatile unsigned char __xdata *) 0xF120)) // PWM Initialization
#define exPwmCon1    (*((volatile unsigned char __xdata *) 0xF121)) // PWM Control 1
#define exPwmMask    (*((volatile unsigned char __xdata *) 0xF122)) // PWM Mask Register
#define exPwmCon2    (*((volatile unsigned char __xdata *) 0xF126)) // PWM Control 2
#define exPwmCon3    (*((volatile unsigned char __xdata *) 0xF127)) // PWM Control 3
#define exPwmCon4    (*((volatile unsigned char __xdata *) 0xF129)) // PWM Control 4
#define exPwm2Clr    (*((volatile unsigned char __xdata *) 0xF12B)) // PWM Channel 2 Clear
#define exPwm1Dc     (*((volatile unsigned char __xdata *) 0xF132)) // PWM Channel 1 Duty Cycle (Low byte)
#define exPwm2Per    (*((volatile unsigned char __xdata *) 0xF133)) // PWM Channel 2 Period (High byte)
#define exPwm1Per    (*((volatile unsigned char __xdata *) 0xF142)) // PWM Channel 1 Period (Low byte)
#define exPwm1Clr    (*((volatile unsigned char __xdata *) 0xF143)) // PWM Channel 1 Clear (High byte)



// ******************* Port Bit Definitions - CORRECTED *********************

// ******************* P0 ***************************
__sbit __at(0x87) P07;
__sbit __at(0x86) P06;
__sbit __at(0x85) P05;
__sbit __at(0x84) P04;
__sbit __at(0x83) P03;
__sbit __at(0x82) P02;
__sbit __at(0x81) P01;
__sbit __at(0x80) P00;

// ******************* P1 ***************************
__sbit __at(0x97) P17;
__sbit __at(0x96) P16;
__sbit __at(0x95) P15;
__sbit __at(0x94) P14;
__sbit __at(0x93) P13;
__sbit __at(0x92) P12; 
__sbit __at(0x91) P11;
__sbit __at(0x90) P10;

// ******************* P2 ***************************
__sbit __at(0xA7) P27;
__sbit __at(0xA6) P26;
__sbit __at(0xA5) P25;
__sbit __at(0xA4) P24;
__sbit __at(0xA3) P23;
__sbit __at(0xA2) P22;
__sbit __at(0xA1) P21;
__sbit __at(0xA0) P20;

// ******************* P3 ***************************
__sbit __at(0xB7) P37;
__sbit __at(0xB6) P36;
__sbit __at(0xB5) P35;
__sbit __at(0xB4) P34;
__sbit __at(0xB3) P33;
__sbit __at(0xB2) P32;
__sbit __at(0xB1) P31;
__sbit __at(0xB0) P30;

// ******************* P4 ***************************
__sbit __at(0xC7) P47;
__sbit __at(0xC6) P46;
__sbit __at(0xC5) P45;
__sbit __at(0xC4) P44;
__sbit __at(0xC3) P43;
__sbit __at(0xC2) P42;
__sbit __at(0xC1) P41;
__sbit __at(0xC0) P40;

// ******************* P5 *************************** 
// P5 is at 0xD8 - CORRECTED from 0xC8
__sbit __at(0xDD) P55;  // CORRECTED bit addresses
__sbit __at(0xDC) P54;
__sbit __at(0xDB) P53;
__sbit __at(0xDA) P52;
__sbit __at(0xD9) P51;
__sbit __at(0xD8) P50;

// PSW (Program Status Word) bit definitions - Added
__sbit __at(0xD7) CY;       // Carry flag
__sbit __at(0xD6) AC;       // Auxiliary carry flag
__sbit __at(0xD5) F0;       // User flag 0
__sbit __at(0xD4) RS1;      // Register bank select 1
__sbit __at(0xD3) RS0;      // Register bank select 0
__sbit __at(0xD2) OV;       // Overflow flag
__sbit __at(0xD1) F1;       // User flag 1
__sbit __at(0xD0) P;        // Parity flag

// SCON (Serial Control) bit definitions - Added
__sbit __at(0x9F) SM0;      // Serial mode 0
__sbit __at(0x9E) SM1;      // Serial mode 1
__sbit __at(0x9D) SM2;      // Serial mode 2
__sbit __at(0x9C) REN;      // Receive enable
__sbit __at(0x9B) TB8;      // Transmit bit 8
__sbit __at(0x9A) RB8;      // Receive bit 8
__sbit __at(0x99) TI;       // Transmit interrupt flag
__sbit __at(0x98) RI;       // Receive interrupt flag

// TCON (Timer Control) bit definitions - Added
__sbit __at(0x8F) TF1;      // Timer 1 overflow flag
__sbit __at(0x8E) TR1;      // Timer 1 run control
__sbit __at(0x8D) TF0;      // Timer 0 overflow flag
__sbit __at(0x8C) TR0;      // Timer 0 run control



#endif