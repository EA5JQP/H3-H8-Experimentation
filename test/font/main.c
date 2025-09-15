#include "H8.h"
#include "delay.h"
#include "watchdog.h"
#include "hardware.h"
#include "pwm.h"
#include "uart.h"
#include "lcd.h"
#include "font.h"

// Simple UART message function implementation
void send_uart_message(char* message) {
    uart_pr_send_string((u8*)message);
    uart_pr_send_string((u8*)"\r\n");
}

// Test 16x8 font rendering
void test_16x8_font(void) {
    send_uart_message("=== Testing 16x8 Font ===");
    
    // Clear screen with working pattern
    u8 row, col;
    lcd_set_window(0, 0, 159, 127);
    for (row = 0; row < 128; row++) {
        for (col = 0; col < 160; col++) {
            lcd_send_data(0x00);  // Black background
            lcd_send_data(0x00);
        }
    }
    
    delay_ms(1, 0); // Small delay
    
    // Test basic strings
    send_uart_message("Rendering basic strings...");
    render_16x8_string(5, 5, "16x8 Font Test");
    render_16x8_string(5, 15, "ABCDEFGHIJKLMNOP");
    render_16x8_string(5, 25, "abcdefghijklmnop");
    render_16x8_string(5, 35, "0123456789!@#$%");
    render_16x8_string(5, 45, "Hello H8 Radio!");
    
    delay_ms(7, 208); // ~2 second delay
    
    // Test numbers - simplified debug version
    send_uart_message("Testing number rendering...");
    render_16x8_string(5, 60, "Numbers:");
    
    // Test individual digits first
    render_16x8_char(5, 70, '0');
    render_16x8_char(25, 70, '1'); 
    render_16x8_char(45, 70, '7');
    render_16x8_char(65, 70, '9');
    
    // Test simple numbers
    render_16x8_number(5, 85, 0);
    render_16x8_number(25, 85, 7);
    render_16x8_number(45, 85, 99);
    render_16x8_number(85, 85, 123);
    render_16x8_number(5, 100, 12345);
    
    delay_ms(7, 208); // ~2 second delay
    
    // Test special characters
    send_uart_message("Testing special characters...");
    render_16x8_string(5, 115, "()[]{}+-=<>?/*");
    
    send_uart_message("16x8 font test complete!");
}

void main(void) {
    // Hardware initialization
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
    
    send_uart_message("=== 16x8 FONT TEST FIRMWARE ===");
    send_uart_message("Initializing LCD...");
    lcd_init();
    send_uart_message("LCD initialization complete");
    
    delay_ms(1, 244); // ~500ms delay
    
    // Run font tests
    test_16x8_font();
    
    send_uart_message("=== ALL TESTS COMPLETE ===");
    send_uart_message("Font test running in continuous loop");
    
    // Main loop
    u16 counter = 0;
    while (1) {
        watchdog_reset();
        delay_ms(0, 100); // 100ms delay
        
        counter++;
        if (counter >= 100) { // Every 10 seconds
            counter = 0;
            test_16x8_font();
        }
    }
}