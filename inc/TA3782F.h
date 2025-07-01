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

// Standard 8051 SFR registers
__sfr __at(0x80) P0;        // Port 0
__sfr __at(0x90) P1;        // Port 1
__sfr __at(0xA0) P2;        // Port 2
__sfr __at(0xB0) P3;        // Port 3
__sfr __at(0xC0) P4;        // Port 4 (extended)
__sfr __at(0xC8) P5;        // Port 5 (extended)

// Port configuration registers
__sfr __at(0x9A) P0CON;     // Port 0 configuration
__sfr __at(0xA1) P1CON;     // Port 1 configuration
__sfr __at(0xA2) P2CON;     // Port 2 configuration
__sfr __at(0xA3) P3CON;     // Port 3 configuration
__sfr __at(0xA4) P4CON;     // Port 4 configuration
__sfr __at(0xA5) P5CON;     // Port 5 configuration
__sfr __at(0x96) IOHCON0; 		//IOH0���üĴ���
__sfr __at(0x97) IOHCON1; 		//IOH1���üĴ���

// Timer registers
__sfr __at(0x89) TMOD;      // Timer mode register
__sfr __at(0x8A) TL0;       // Timer 0 low byte
__sfr __at(0x8C) TH0;       // Timer 0 high byte
__sfr __at(0x8E) TMCON;     // Timer control register (custom)

// Timer Control Bits
__sbit __at(0x8C) TR0;      // Timer 0 Run
__sbit __at(0x8D) TF0;      // Timer 0 Overflow Flag
__sbit __at(0x8E) TR1;      // Timer 1 Run
__sbit __at(0x8F) TF1;      // Timer 1 Overflow Flag

// Interrupt Control
__sfr __at(0xA8) IE;        // Interrupt Enable
__sfr __at(0xB8) IP;        // Interrupt Priority
__sbit __at(0xAF) EA;       // Global Interrupt Enable
__sbit __at(0xAB) ET0;      // Timer 0 Interrupt Enable
__sbit __at(0xAD) ET1;      // Timer 1 Interrupt Enable
__sbit __at(0xAC) EX0;      // External Interrupt 0 Enable
__sbit __at(0xAE) EX1;      // External Interrupt 1 Enable

// Confirmed by code analysis
#define exZrdMode (*((volatile unsigned char __xdata *) 0xF692));   // Extended Z-read mode  
#define exWdT0 (*((volatile unsigned char __xdata *) 0xF004));      // Extended watchdog timer 0
#define exWdT1 (*((volatile unsigned char __xdata *) 0xF005));      // Extended watchdog timer 1
#define exP34Mode (*((volatile unsigned char __xdata *) 0xF034));   // Extended P34 mode

// Core UART control registers
__sfr __at(0xEA) EXA1;     // External UART 1 control
__sfr __at(0x91) SFR91;    // Multi-UART control register
__sfr __at(0x98) SCON;     // Standard UART control  
__sfr __at(0x87) PCON;     // Power control
__sfr __at(0x99) SBUF;     // Standard UART buffer
__sfr __at(0xEB) CCAP1L;   // External UART 1 buffer (alias)

// External UART 1 (P11/P13)
#define exUart1Con (*((volatile unsigned char __xdata *)0xF5C0));    // UART 1 control
#define exUart1Baud_H (*((volatile unsigned char __xdata *)0xF5C1)); // UART 1 baud rate high
#define exUart1Baud_L (*((volatile unsigned char __xdata *)0xF5C2)); // UART 1 baud rate low  
#define exUart1Port (*((volatile unsigned char __xdata *)0xF69E));   // UART 1 port assignment
#define exP11Mode (*((volatile unsigned char __xdata *)0xF011));     // P1.1 pin mode
#define exP13Mode (*((volatile unsigned char __xdata *)0xF013));     // P1.3 pin mode


// Bluetooth UART (P20/P21) 
#define exUart2Con  (*((volatile unsigned char __xdata *)0xF5C4));    // UART 2 control
#define exUart2Baud_H  (*((volatile unsigned char __xdata *)0xF5C5)); // UART 2 baud rate high
#define exUart2Baud_L  (*((volatile unsigned char __xdata *)0xF5C6)); // UART 2 baud rate low
#define exP20Mode  (*((volatile unsigned char __xdata *)0xF020));     // P2.0 pin mode  
#define exP21Mode  (*((volatile unsigned char __xdata *)0xF021));     // P2.1 pin mode

// ADC control and configuration registers
__sfr __at(0xDF) ROMBNK;    // ROM Bank control register
__sfr __at(0xAB) ADCCFG0;   // ADC Configuration Register 0
__sfr __at(0xAC) ADCCFG1;   // ADC Configuration Register 1  
__sfr __at(0xAD) ADCCON;    // ADC Control Register
__sfr __at(0xAE) ADCVL;     // ADC Value Low Register
__sfr __at(0xAF) ADCVH;     // ADC Value High Register
__sfr __at(0xB5) ADCCFG2;   // ADC Configuration Register 2


// ******************* PWM ***************************
// External memory-mapped PWM registers - ACTUAL ADDRESSES from Ghidra analysis
// These are the registers actually used in your pwmInit() function

// Port configuration registers
// __sfr __at(0xF041) exP41Mode;  // Port P4.1 ? mode setting
// __sfr __at(0xF022) exP22Mode;  // Port P2.2 ? mode setting

// // PWM control and configuration registers
// __sfr __at(0xF120) exPwmInit;  // PWM initialization register
// __sfr __at(0xF121) exPwmCon1;  // PWM control register 1
// __sfr __at(0xF122) exPwmMask;  // PWM output mask register
// __sfr __at(0xF123) exPwmFlow;  // PWM flow control register
// __sfr __at(0xF126) exPwmCon2;  // PWM control register 2
// __sfr __at(0xF127) exPwmCon3;  // PWM control register 3
// __sfr __at(0xF129) exPwmCon4;  // PWM control register 4
// __sfr __at(0xF12A) exPwm1Clr;  // PWM1 clear register
// __sfr __at(0xF12B) exPwm2Clr;  // PWM2 clear register

// // PWM1 registers
// __sfr __at(0xF130) exPwm1Dc     // PWM1 duty cycle register
// __sfr __at(0xF131) exPwm1Per;   // PWM1 period register
// __sfr __at(0xF132) exPwm2Dc     // PWM2 duty cycle register
// __sfr __at(0xF133) exPwm2Per;   // PWM2 period register

// // Additional PWM registers (from decompiled code)
// __sfr __at( e0xF140)xPwmReg140; // Additional PWM register
// __sfr __at( e0xF141)xPwmReg141; // Additional PWM register
// __sfr __at( e0xF142)xPwmReg142; // Additional PWM register
// __sfr __at( e0xF143)xPwmReg143; // Additional PWM register


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
// P5 is at 0xC8, but bit-addresses go from 0xC8 to 0xCF
__sbit __at(0xCD) P55;
__sbit __at(0xCC) P54;
__sbit __at(0xCB) P53;
__sbit __at(0xCA) P52;
__sbit __at(0xC9) P51;
__sbit __at(0xC8) P50;


#endif