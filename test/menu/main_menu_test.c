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
#include "at1846s.h"
#include "font.h"
#include "i2c.h"
#include "eeprom.h"
#include "uart_test.h"
#include "menu.h"
#include "settings.h"

// Simple background pattern for non-menu display
void simple_background_pattern(void) {
    static u8 counter = 0;
    u8 x, y;
    
    // Simple pattern every 4th update to reduce CPU load
    if ((counter++ & 0x03) == 0) {
        for (y = 0; y < 8; y++) {
            for (x = 0; x < 20; x++) {
                lcd_set_window(x * 8, y * 16, (x * 8) + 7, (y * 16) + 15);
                spi_write_pixel_data((counter + x + y) & 0xFF, (counter * 2 + x - y) & 0xFF);
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
    send_uart_message("UART initialized - starting LCD init");
    lcd_init();
    clear_area(0, 0, DISPLAY_WIDTH - 1, DISPLAY_HEIGHT - 1);
    at1846s_init();

    delay_ms(6, 232);

    // Initialize I2C bus for EEPROM
    send_uart_message("=== H8 MENU TEST FIRMWARE ===");
    send_uart_message("Initializing I2C bus...");
    i2c_init();
    
    // Test basic EEPROM communication
    send_uart_message("Testing EEPROM...");
    EA = 0;
    i2c_start();
    __bit eeprom_present = i2c_send(0xA0);
    i2c_stop();
    EA = 1;
    
    if (eeprom_present) {
        send_uart_message("EEPROM detected - initializing menu system");
    } else {
        send_uart_message("EEPROM not detected - menu will use defaults");
    }
    
    // Initialize menu system and load settings
    menu_init();
    settings_init();
    
    // Initialize AT1846S with current settings
    send_uart_message("Applying saved settings to radio...");
    u16 freq = settings_get_frequency();
    u8 vol = settings_get_volume();
    u8 sql = settings_get_squelch();
    
    at1846s_set_frequency((u32)freq);
    at1846s_set_volume(vol);
    at1846s_set_squelch(sql);
    
    send_uart_message("Menu Test Firmware Ready");
    send_uart_message("Controls:");
    send_uart_message("  MENU - Enter/exit menu");
    send_uart_message("  UP/DOWN - Navigate");
    send_uart_message("  * - Edit values");
    send_uart_message("  # - Save changes");
    send_uart_message("  EXIT - Cancel");
    
    // Clear display and show initial screen
    clear_area(0, 0, DISPLAY_WIDTH - 1, DISPLAY_HEIGHT - 1);
    render_16x16_string(8, 16, "H8 Menu Test");
    render_16x16_string(8, 48, "Press MENU");
    
    // Show current frequency
    render_16x16_string(8, 80, "Freq:");
    render_16x16_number(80, 80, freq);

    u8 last_key = 0;  // Track last key to prevent repeats

    while (1) {
        watchdog_reset();
        
        // Check for key presses and handle menu system
        u8 current_key = keypad_scan();
        if (current_key != last_key) {
            if (current_key != 0) {
                send_uart_message("Key detected:");
                uart_pr_send_byte('0' + (current_key & 0x0F));
                uart_pr_send_byte('\r');
                uart_pr_send_byte('\n');
                if (menu_mode == MENU_MODE_MENU) {
                // In menu mode - process menu keys
                menu_process_key(current_key);
            } else {
                // In normal mode - check for menu entry key
                if (current_key == KEY_MENU) {
                    send_uart_message("Entering menu mode");
                    menu_enter();
                } else {
                    // Show key press feedback
                    send_uart_message("Key pressed:");
                    if (current_key >= KEY_1 && current_key <= KEY_9) {
                        uart_pr_send_byte('0' + current_key);
                    } else if (current_key == KEY_0) {
                        uart_pr_send_byte('0');
                    } else {
                        uart_pr_send_byte('0' + (current_key & 0x0F));
                        uart_pr_send_byte('(');
                        uart_pr_send_byte('0' + ((current_key >> 4) & 0x0F));
                        uart_pr_send_byte(')');
                    }
                    uart_pr_send_byte('\r');
                    uart_pr_send_byte('\n');
                    
                    // Update frequency display in normal mode
                    if (current_key >= KEY_1 && current_key <= KEY_9) {
                        clear_area(80, 80, 159, 95);
                        render_16x16_number(80, 80, settings_get_frequency());
                    }
                }
            }
            last_key = current_key;
        }
        
        // Update display based on mode
        if (menu_mode == MENU_MODE_MENU && menu_display_dirty) {
            menu_update_display();
        } else if (menu_mode == MENU_MODE_NORMAL) {
            // Simple background pattern in normal mode
            simple_background_pattern();
        }
        
        delay_ms(0, 10);  // 10ms key handling for stable debounce
    }
}