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

// Fill pattern based on working example structure
void fill_pattern_like_working(u8 fill_hi, u8 fill_lo, char* test_name) {
    u8 row, col;
    
    send_uart_message("--- FILL PATTERN ---");
    send_uart_message(test_name);
    
    lcd_set_window(0, 0, 159, 127);  // Full screen like working example

    for (row = 0; row < 128; row++) {
        for (col = 0; col < 160; col++) {
            lcd_send_data(fill_hi);
            lcd_send_data(fill_lo);
        }
    }
    
    send_uart_message("Fill pattern complete");
    send_uart_message("--- END PATTERN ---");
}

// Test different fill patterns using working example structure
void test_fill_patterns(void) {
    send_uart_message("Testing different fill patterns...");
    
    delay_ms(3, 232); // ~1 second delay (3*256 + 232 = 1000ms)
    
    // Test 1: Black fill
    send_uart_message("Test 1: Black fill (0x00, 0x00)");
    fill_pattern_like_working(0x00, 0x00, "BLACK_FILL");
    delay_ms(7, 208); // ~2 second delay (7*256 + 208 = 2000ms)
    
    // Test 2: White fill  
    send_uart_message("Test 2: White fill (0xFF, 0xFF)");
    fill_pattern_like_working(0xFF, 0xFF, "WHITE_FILL");
    delay_ms(7, 208); // ~2 second delay
    
    // Test 3: Red fill (RGB565: 0xF800)
    send_uart_message("Test 3: Red fill (0xF8, 0x00)");
    fill_pattern_like_working(0xF8, 0x00, "RED_FILL");
    delay_ms(7, 208); // ~2 second delay
    
    // Test 4: Green fill (RGB565: 0x07E0) 
    send_uart_message("Test 4: Green fill (0x07, 0xE0)");
    fill_pattern_like_working(0x07, 0xE0, "GREEN_FILL");
    delay_ms(7, 208); // ~2 second delay
    
    // Test 5: Blue fill (RGB565: 0x001F)
    send_uart_message("Test 5: Blue fill (0x00, 0x1F)");
    fill_pattern_like_working(0x00, 0x1F, "BLUE_FILL");
    delay_ms(7, 208); // ~2 second delay
    
    send_uart_message("Fill pattern tests complete!");
}

// Test the original clear_area function
void test_original_clear_area(void) {
    send_uart_message("Testing original clear_area function...");
    
    // Call original function
    send_uart_message("Calling clear_area(0, 0, 159, 127)...");
    clear_area(0, 0, DISPLAY_WIDTH-1, DISPLAY_HEIGHT-1);
    send_uart_message("Original clear_area call complete");
    
    delay_ms(7, 208); // ~2 second delay
}

// Create the working background pattern from the original code
void create_background_pattern(void) {
    u8 row, col;
    
    send_uart_message("Creating background pattern...");
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
    send_uart_message("Background pattern complete");
}

// Create alternating stripes pattern based on working example
void create_test_pattern(void) {
    u8 row, col;
    
    send_uart_message("Creating alternating stripe pattern...");
    lcd_set_window(0, 0, 159, 127);  // Full screen like working example

    for (row = 0; row < 128; row++) {
        for (col = 0; col < 160; col++) {
            u8 high_byte, low_byte;
            
            // Create alternating horizontal stripes
            if (row % 8 < 4) {
                // White stripes
                high_byte = 0xFF;
                low_byte = 0xFF;
            } else {
                // Black stripes
                high_byte = 0x00;
                low_byte = 0x00;
            }
            
            lcd_send_data(high_byte);
            lcd_send_data(low_byte);
        }
    }
    
    send_uart_message("Test pattern complete");
    delay_ms(11, 184); // ~3 second delay (11*256 + 184 = 3000ms)
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
    
    send_uart_message("=== LCD CLEAR_AREA DEBUG TEST ===");
    send_uart_message("Initializing LCD...");
    lcd_init();
    send_uart_message("LCD initialization complete");
    
    delay_ms(1, 244); // ~500ms delay (1*256 + 244 = 500ms)
    
    // Initial clear as recommended in CLAUDE.md
    send_uart_message("Initial clear after lcd_init()...");
    clear_area(0, 0, DISPLAY_WIDTH-1, DISPLAY_HEIGHT-1);
    send_uart_message("Initial clear complete");
    
    delay_ms(3, 232); // ~1 second delay
    
    // Run comprehensive tests
    send_uart_message("Starting comprehensive clear_area tests...");
    
    // Test 1: Working background pattern (known to work)
    send_uart_message("Test 1: Working background pattern from original code");
    create_background_pattern();
    delay_ms(15, 160); // ~4 second delay to observe
    
    // Test 2: Original clear function
    test_original_clear_area();
    
    // Test 3: Different fill patterns
    test_fill_patterns();
    
    // Test 4: Test pattern
    create_test_pattern();
    
    // Test 5: Background pattern again
    send_uart_message("Test 5: Background pattern again");
    create_background_pattern();
    delay_ms(7, 208); // ~2 second delay
    
    // Test 6: Final black clear
    send_uart_message("Final test: Black clear");
    fill_pattern_like_working(0x00, 0x00, "FINAL_BLACK_CLEAR");
    
    send_uart_message("=== ALL LCD TESTS COMPLETE ===");
    send_uart_message("Monitor display for visual changes during tests");
    
    // Main loop with periodic testing
    u16 test_counter = 0;
    while (1) {
        watchdog_reset();
        delay_ms(0, 100); // 100ms delay
        
        // Repeat tests every 30 seconds
        test_counter++;
        if (test_counter >= 300) { // 300 * 100ms = 30 seconds
            test_counter = 0;
            send_uart_message("Repeating working background pattern...");
            create_background_pattern();
        }
    }
}