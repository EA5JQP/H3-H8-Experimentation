#include "beep.h"
#include "delay.h"
#include "watchdog.h"

// Private variables for beep state
static u8 beep_initialized = 0;

// Initialize the beep system
void beep_init(void) {
    // Enable power to TDA2822 amplifier (active high)
    POW2822 = 1;   // Enable power to beep circuit (active high)
    // Disable interrupts during beep operations
    EA = 0;
    // Start with beep OFF
    BEEP = 0;      
    // Delay to allow TDA2822 to power up properly
    delay_ms(0, 200);  // 200ms delay for amplifier startup

    beep_initialized = 1;
}



// Power off the beep system
void beep_power_off(void) {
    BEEP = 0;       // Turn off beep signal
    POW2822 = 0;    // Disable power to beep circuit (active high)
    EA = 1;         // Re-enable interrupts
    beep_initialized = 0;  // Mark as uninitialized
}

// Generate tone with specified duration and wavelength
// duration: number of toggle cycles, waveLength: delay between toggles
void beep_tone(const u8 duration, const u16 waveLength) {
    u16 i;
    u8 cycles;
    
    if (!beep_initialized) {
        beep_init();
    }
    
    // Disable interrupts during beep generation
    EA = 0;
    
    // Simple duration loop
    for (cycles = 0; cycles < duration; cycles++) {
        BEEP ^= 1;  // Toggle beep pin (XOR with 1)
        
        // Delay loop for wave length (frequency control)
        for (i = 0; i != waveLength; i++) {
            watchdog_reset();  // Feed the dog
        }
    }
    
    BEEP = 0;  // Ensure beep is off when done
    
    // Re-enable interrupts
    EA = 1;
}

