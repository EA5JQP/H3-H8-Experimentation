#include "keypad.h"
#include "uart.h"

// Direct memory access (for embedded systems)
__xdata volatile u8 key_code;          // DAT_EXTMEM_0379
__xdata volatile u8 debounce_counter;  // DAT_EXTMEM_0302
__xdata volatile u8 prev_key_code;     // DAT_EXTMEM_034c
__xdata volatile u8 key_hold_counter;  // DAT_EXTMEM_0311

u8 keypad_scan(void) {
    // This function scans the keypad for pressed keys.

    //  Prepare for scanning by masking P1 upper bits 
    P1 &= 0xF; // Set P1.4 to P1.7 as LOW (KEYOUT1 to KEYOUT4)
    
    // Secondary ptt is likely considered in the original code:
    u8 no_keys_pressed = (P3 & 0x1F) == 0x1F;

    // Handle key release and reset debounce
    // When RXD232_PTT is not pressed is high, so it is resetting the key code
    if ((RXD232_PTT == 1) && no_keys_pressed) {
        if ((key_code == 0x00) || (debounce_counter < 5)) {
            debounce_counter = 0;
            key_code = 0;
            // key_repeat_active = 0;
            // auto_repeat_flag = 0;
        }
    }
    else {
        debounce_counter++;
        if (debounce_counter > 5){
            // Reset debounce counter
            debounce_counter = 0;

            if (key_code == 0x00) {
                // Set high nibble temporarily
                KEYOUT1 = 01; KEYOUT2 = 1; KEYOUT3 = 1; KEYOUT4 = 1;
                delay_short(10);
                if (RXD232_PTT != 0x01) key_code = 0x13;
                else if (!KEYIN1) key_code = 0x16;
                else if (!KEYIN2) key_code = 0x11;
                else if (!KEYIN3) key_code = 0x12;
            } 

            if (key_code == 0x00) {
                // No key pressed, reset key code
                KEYOUT1 = 0; KEYOUT2 = 1; KEYOUT3 = 1; KEYOUT4 = 1;
                delay_short(10);
                if (KEYIN1 != 0x01) key_code = 0x0B;
                else if (!KEYIN2) key_code = 0x02;
                else if (!KEYIN3) key_code = 0x05;
                else if (!KEYIN4) key_code = 0x08;
                else if (!KEYIN5) key_code = 0x17;        
            }
            
            if (key_code == 0x00) {
                KEYOUT1 = 1; KEYOUT2 = 0; KEYOUT3 = 1; KEYOUT4 = 1;
                delay_short(10);
                if (!KEYIN1) key_code = 0x0C;
                else if (!KEYIN2) key_code = 0x03;
                else if (!KEYIN3) key_code = 0x06;
                else if (!KEYIN4) key_code = 0x09;
                else if (!KEYIN5) key_code = 0x18;    
            }

            if (key_code == 0x00) {
                KEYOUT1 = 1; KEYOUT2 = 1; KEYOUT3 = 0; KEYOUT4 = 1;
                delay_short(10);
                if (!KEYIN1) key_code = 0x0D;
                else if (!KEYIN2) key_code = 0x04;
                else if (!KEYIN3) key_code = 0x07;
                else if (!KEYIN4) key_code = 0x0A;
                else if (!KEYIN5) key_code = 0x19;
            }
            if (key_code == 0x00) {
                KEYOUT1 = 1; KEYOUT2 = 1; KEYOUT3 = 1; KEYOUT4 = 0;
                delay_short(10);
                if (!KEYIN1) key_code = 0x0E;
                else if (!KEYIN2) key_code = 0x0F;
                else if (!KEYIN3) key_code = 0x01;
                else if (!KEYIN4) key_code = 0x10;
            }
        }
    }
    return key_code; // Return the detected key code
}


