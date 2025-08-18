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

// u16 battery_read_bnkreg(void) {
//     u16 result;

//     UKSFR_DF |= 2;  // Use exact same bank as working code
    
//     // Method similar to: result = ((u16)ADC10) << 4; result |= ((ADC1>>4)&0xf);
//     result = ((u16)BANK1_R6) << 4;        // High 8 bits shifted left by 4
//     result |= (BANK1_R7 >> 4) & 0x0F;         // Low 4 bits from upper nibble

//     return result;
// }


u16 battery_read_voltage(void) {
    u16 raw_adc;
    u16 calibrated_adc;
    u16 voltage_b;

    // Step 1: Read raw ADC (your current code)
    UKSFR_DF |= 2;  // Bank switching
    raw_adc = ((u16)ADC10) << 4;        // High 8 bits shifted left by 4
    raw_adc |= (ADC1 >> 4) & 0x0F;     // Low 4 bits from upper nibble

    // Step 2: Apply calibration offset (from batteryRead function)
    // calibrated_adc = (raw_adc > 2828) ? raw_adc - 2828 : 0;
    calibrated_adc = raw_adc;
    // Step 3: Convert to voltage value (from batteryShow function)

    voltage_b = calibrated_adc / 48;    // Scale factor
    voltage_b += 58;                    // Voltage offset

    return voltage_b;  // This is the 'b' value used for display
}

// ✅ Final Formula:
// Vbatt=Vadc×(R3+R9)/R9≈Vadc×3.94
// R3 = 1.5kΩ, R9 = 51kΩ
// So multiply your ADC voltage by ≈ 3.94 to get the real battery voltage.
