#include "battery.h"
#include "uart.h"
#include "watchdog.h"
#include "delay.h"

unsigned int ADCValue = 0x0000;
u8 AdcFlag = 0;


u16 battery_read(void) {
    u16 result;

    UKSFR_DF |= 2;  // Use exact same bank as working code
    
    // Method similar to: result = ((u16)ADC10) << 4; result |= ((ADC1>>4)&0xf);
    result = ((u16)ADC10) << 4;        // High 8 bits shifted left by 4
    result |= (ADC1 >> 4) & 0x0F;         // Low 4 bits from upper nibble

    return result;
}

u16 battery_read_bnkreg(void) {
    u16 result;

    UKSFR_DF |= 2;  // Use exact same bank as working code
    
    // Method similar to: result = ((u16)ADC10) << 4; result |= ((ADC1>>4)&0xf);
    result = ((u16)BANK1_R6) << 4;        // High 8 bits shifted left by 4
    result |= (BANK1_R7 >> 4) & 0x0F;         // Low 4 bits from upper nibble

    return result;
}


// ✅ Final Formula:
// Vbatt=Vadc×(R3+R9)/R9≈Vadc×3.94
// R3 = 1.5kΩ, R9 = 51kΩ
// So multiply your ADC voltage by ≈ 3.94 to get the real battery voltage.
