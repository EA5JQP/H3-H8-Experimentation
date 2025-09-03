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
#include "battery.h"
#include "font.h"
#include "i2c.h"
#include "eeprom.h"
#include "uart_test.h"
#include "menu.h"
#include "settings.h"

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
    send_uart_message("Initializing I2C bus...");
    i2c_init();
    
    // Initialize menu system and load settings
    send_uart_message("Initializing menu system...");
    menu_init();
    settings_init();
    
    // Display startup message
    send_uart_message("Menu Test Firmware Ready");
    send_uart_message("Press MENU key to enter menu system");
    
    // Clear display and show initial screen
    clear_area(0, 0, DISPLAY_WIDTH - 1, DISPLAY_HEIGHT - 1);
    render_16x16_string(8, 32, "H8 Menu Test");
    render_16x16_string(8, 64, "Press MENU");

    u8 last_key = 0;  // Track last key to prevent repeats
    u8 loop_counter = 0;  // Debug counter

    while (1) {
        watchdog_reset();
        
        // Debug: show loop is running every 128 iterations
        if (++loop_counter == 0) {
            send_uart_message("Loop running...");
        }
        
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
                    menu_enter();
                } else {
                    // Handle other normal mode keys here
                    send_uart_message("Key pressed in normal mode:");
                    uart_pr_send_byte('0' + (current_key & 0x0F));
                    uart_pr_send_byte('\r');
                    uart_pr_send_byte('\n');
                }
            }
            last_key = current_key;
        }
        
        // Update menu display if needed
        if (menu_mode == MENU_MODE_MENU && menu_display_dirty) {
            menu_update_display();
        }
        
        delay_ms(0, 10);  // 10ms key handling for stable debounce
    }
}