#include "lcd_test.h"

void create_background_pattern(void) {
    u8 row, col;

    lcd_set_window(0, 0, 159, 127);  // Full screen

    for (row = 0; row < 128; row++) {
        for (col = 0; col < 160; col++) {
            lcd_send_data(0x00);  // High byte
            lcd_send_data(0x00);  // Low byte
        }
    }
}