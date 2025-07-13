#include "pwm.h"

void pwm_init(u8 duty_cycle_high, u8 duty_cycle_low)

{
    // Initialize PWM with specified duty cycle  
    UKSFR_DF = 0;
    if (((duty_cycle_high ^ 0x80) < 0x80U - (((duty_cycle_low < 0x10) << 7) >> 7)) << 7 < '\0') {
        UKSFR_DE = duty_cycle_low;
    }
    else {
        UKSFR_DF |= 0x80;
        UKSFR_DE = duty_cycle_low & 0xf;
    }

    UKSFR_DE |= 0x60;
    UKSFR_DE |= 0x80;
    exZrdMode = 0xa0;
    IE2 |= 0x10;
    EA = 1;
  
}


void pwm_pin_setup(void)
{
  pwmUkReg1 = 0;    // Clear PWM internal register 1 
  pwmUkReg2 = 0;    // Clear PWM internal register 2 
  
  exP02Mode = 5;    // Set P02 pin mode to PWM mode (5) 
  exP43Mode = 5;    // Set PWM flow control mode to 5 
  
  exPwmInit = 0;    // Initialize PWM module (clear initialization register)
  
  exPwmCon1 |= 2;   // Enable PWM Control 1 (set bit 1)
  exPwmCon2 |= 2;// Enable PWM Control 2 (set bit 1) 
  exPwmCon3 |= 2;   // Enable PWM Control 3 (set bit 1)              
  exPwmCon4 |= 2;   // Enable PWM Control 4 (set bit 1)                
                    
  exPwmMask &= 0xfd;// Clear bit 1 in PWM mask register
  exPwm1Dc = 200;// Set PWM Channel 2 duty cycle low byte to 200 (0xC8)               
  
  exPwm2Clr = 0;    // Clear PWM Channel 2 clear register                  
  exPwm2Per = 0;// Set PWM Channel 2 duty cycle high byte to 0               
  exPwm1Clr = 0;// Set PWM Channel 2 period high byte to 0              
  exPwm1Per = 0; // Set PWM Channel 2 period low byte to 0               
}
