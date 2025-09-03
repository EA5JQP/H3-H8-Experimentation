#include "lcd_test.h"

void create_background_pattern(void) {
    u8 row, col;

    lcd_set_window(0, 0, 159, 127);  // Full screen

    for (row = 0; row < 128; row++) {
        for (col = 0; col < 160; col++) {
            u8 high_byte, low_byte;
            
            // Create gradient pattern based on row and column
            if ((row + col) & 0x20) {
                // Blue areas
                high_byte = 0x00;
                low_byte = 0x1F;
            } else if ((row ^ col) & 0x10) {
                // Red areas  
                high_byte = 0xF8;
                low_byte = 0x00;
            } else {
                // Green areas
                high_byte = 0x07;
                low_byte = 0xE0;
            }
            
            lcd_send_data(high_byte);
            lcd_send_data(low_byte);
        }
    }
}