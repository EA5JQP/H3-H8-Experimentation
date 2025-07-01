#include "H3.h"
#include "delay.h"

#define led1 RX_VLED // Define led1 as RX_VLED for LED control
#define led2 TX_VLED // Define P31 as TX_VLED for LED control
#define led3 KEY_LED

void watchdog_reset(void) {
        // Reset the watchdog timer
        IOHCON0 = 0xaa;
        IOHCON0 = 0x55;
        IOHCON1 |= 1;
}

// Initialize P31 for LED output
void init_led(void) {
        // Configure P31 as output
        // Based on the original code: P3CON = 0xef
        // This means P31 is configured as output (bit 1 in P3CON)
        
        // Set P3CON - configure P31 as output
        // Bit pattern: 0xef = 11101111 (bit 4 = 0, others = 1)
        // For P31 output, we need bit 1 in P3CON to be configured appropriately
        // P3CON = 0xef;           // Configure P3 pins (same as original code)
        
        // Initialize P31 to 0 (LED off)
        // P0CON = 0xff;
        // P1CON = 0xdd;
        // P2CON = 0xc6;
        P3CON = 0xef;
        // P4CON = 0xfe;
        // P5CON = 0xfd;
        // exP34Mode = 1;
        // P0 = 0xf;
        // P1 = 0x02; //0xa;
        // P2 = 0x40;
        P3 = 0;
        // P4 = 0xb1;
        // P5 = 0x20;        

        watchdog_reset(); // Reset the watchdog timer
}

void main(void)
{
        init_led(); // Initialize LED control
        while (1) 
	{
                led1 = 0;
                led2 = 0; // Turn off TX_VLED
                led3 = 0; // Turn off KEY_LED
                watchdog_reset(); // Reset the watchdog timer
                delay(1000);
                led1 = 1;
                led2 = 1; // Turn on TX_VLED
                led3 = 1; // Turn on KEY_LED
                delay(1000);
        }
}
