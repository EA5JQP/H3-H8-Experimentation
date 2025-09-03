#include "H8.h"
#include "delay.h"
#include "watchdog.h"
#include "hardware.h"
#include "pwm.h"
#include "types.h"
#include "TA3782F.h"
#include "uart.h"
#include "keypad.h"
#include "lcd.h"
#include "font.h"
#include "i2c.h"
#include "eeprom.h"
#include "uart_test.h"
#include "menu.h"
#include "settings.h"

// Minimal AT1846S functions (stubs for menu testing)
void at1846s_init(void) {
    send_uart_message("AT1846S init (minimal)");
}

void at1846s_set_frequency(u32 freq_khz) {
    send_uart_message("Set freq:");
    // Simple frequency display
    uart_pr_send_byte('0' + ((freq_khz / 100000) % 10));
    uart_pr_send_byte('0' + ((freq_khz / 10000) % 10));
    uart_pr_send_byte('0' + ((freq_khz / 1000) % 10));
    uart_pr_send_byte('.');
    uart_pr_send_byte('0' + ((freq_khz / 100) % 10));
    uart_pr_send_byte('0' + ((freq_khz / 10) % 10));
    uart_pr_send_byte('0' + (freq_khz % 10));
    uart_pr_send_byte('\r');
    uart_pr_send_byte('\n');
}

void at1846s_set_volume(u8 volume) {
    send_uart_message("Set volume:");
    uart_pr_send_byte('0' + (volume / 10));
    uart_pr_send_byte('0' + (volume % 10));
    uart_pr_send_byte('\r');
    uart_pr_send_byte('\n');
}

void at1846s_set_squelch(u8 squelch) {
    send_uart_message("Set squelch:");
    uart_pr_send_byte('0' + squelch);
    uart_pr_send_byte('\r');
    uart_pr_send_byte('\n');
}

u8 at1846s_set_ctcss_tx(u16 tone_freq) {
    send_uart_message("Set CTCSS:");
    uart_pr_send_byte('0' + (tone_freq % 10));
    uart_pr_send_byte('\r');
    uart_pr_send_byte('\n');
    return 1;
}

u16 at1846s_get_battery_voltage(void) {
    return 8400; // 8.4V dummy value
}

// Simple background pattern
void simple_background_pattern(void) {
    static u8 counter = 0;
    if ((counter++ & 0x0F) == 0) {
        u8 row, col;
        // Fill whole screen with black pixels
        lcd_set_window(0, 0, DISPLAY_WIDTH - 1, DISPLAY_HEIGHT - 1);
        
        for (row = 0; row < 128; row++) {
            for (col = 0; col < 160; col++) {
                lcd_send_data(0x00);
                lcd_send_data(0x00);
            }
        }
    }
}

// --- main ---
void main(void) {
    hardware_init();
    timer_init();
    pwm_init(0, 0xc);
    watchdog_init();
    watchdog_reset();
    watchdog_config();
    pwm_pin_setup();
    delay_ms(1, 0x2c);
    uart_pr_init();
    uart_bt_init();
    lcd_init();
    clear_area(0, 0, DISPLAY_WIDTH - 1, DISPLAY_HEIGHT - 1);

    delay_ms(6, 232);

    send_uart_message("=== H8 MINIMAL MENU TEST ===");
    send_uart_message("Initializing I2C...");
    i2c_init();
    
    // Initialize menu system
    menu_init();
    settings_init();
    
    send_uart_message("Menu Controls:");
    send_uart_message("  MENU - Enter/exit");
    send_uart_message("  UP/DOWN - Navigate");
    send_uart_message("  * - Edit, # - Save");
    
    // Initial display
    render_16x16_string(16, 32, "H8 MENU");
    render_16x16_string(16, 64, "TEST");

    while (1) {
        watchdog_reset();
        
        // Handle keys with 500ms debouncing
        u8 current_key = keypad_scan();
        static u8 last_processed_key = 0;
        static u16 key_debounce_timer = 0;
        
        if (current_key != 0) {
            if (current_key != last_processed_key || key_debounce_timer >= 25) {
                last_processed_key = current_key;
                key_debounce_timer = 0;
                
                if (menu_mode) {
                    menu_process_key(current_key);
                } else {
                    if (current_key == KEY_MENU) {
                        send_uart_message("Entering menu");
                        menu_enter();
                    } else {
                        send_uart_message("Key:");
                        uart_pr_send_byte('0' + (current_key & 0x0F));
                        uart_pr_send_byte('\r');
                        uart_pr_send_byte('\n');
                    }
                }
            }
        } else {
            last_processed_key = 0;
        }
        
        key_debounce_timer++;
        
        // Update display
        if (menu_mode && menu_display_dirty) {
            menu_update_display();
        } else if (!menu_mode) {
            simple_background_pattern();
        }
        
        delay_ms(0, 10);
    }
}