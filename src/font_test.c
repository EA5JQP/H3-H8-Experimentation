#include "font_test.h"
#include "uart_test.h"

// Helper function to clear the screen
void font_test_clear_screen(void) {
    // Clear entire screen with black pixels
    lcd_set_window(0, 0, DISPLAY_WIDTH - 1, DISPLAY_HEIGHT - 1);
    
    u16 total_pixels = (u16)DISPLAY_WIDTH * DISPLAY_HEIGHT;
    for (u16 i = 0; i < total_pixels; i++) {
        lcd_send_data(0x00);  // Black high byte
        lcd_send_data(0x00);  // Black low byte
    }
}

// Helper function to wait and show next screen message
void font_test_next_screen(void) {
    delay_ms(30, 0);  // 3 second delay to view current screen
    font_test_clear_screen();
    delay_ms(5, 0);   // Short delay after clearing to ensure it's complete
}

// Test all printable ASCII characters (32-126)
void test_font_all_characters(void) {
    send_uart_message("=== TESTING ALL ASCII CHARACTERS ===");
    
    u8 char_count = 0;
    u8 screen_count = 1;
    
    font_test_clear_screen();
    
    // ASCII printable characters: 0x20 (space) to 0x7E (tilde) = 95 characters
    for (u8 c = 0x20; c <= 0x7E; c++) {
        u8 line = char_count / FONT_TEST_CHARS_PER_LINE;
        u8 col = char_count % FONT_TEST_CHARS_PER_LINE;
        
        // Check if we need a new screen
        if (line >= FONT_TEST_LINES_PER_SCREEN) {
            send_uart_message("ASCII characters screen complete, showing next screen...");
            
            // Clear and prepare for next screen
            font_test_next_screen();
            
            char_count = 0;
            line = 0;
            col = 0;
            screen_count++;
        }
        
        // Render character at calculated position
        u8 x = col * FONT_TEST_CHAR_WIDTH + 5;
        u8 y = line * FONT_TEST_LINE_HEIGHT + 5;
        
        render_16x16_char(x, y, c);
        char_count++;
    }
    
    // Final screen complete
    
    send_uart_message("All ASCII characters test complete");
    delay_ms(20, 0);  // 2 second delay
}

// Test numbers 0-9 with different formatting
void test_font_numbers(void) {
    send_uart_message("=== TESTING NUMBERS ===");
    font_test_clear_screen();
    
    // Test individual digits
    render_16x16_string(10, 10, "Digits:");
    render_16x16_string(10, 35, "0123456789");
    
    // Test larger numbers
    render_16x16_string(10, 60, "Numbers:");
    render_16x16_number(10, 85, 12345);
    
    delay_ms(25, 0);  // 2.5 second delay
}

// Test alphabet letters (uppercase and lowercase)
void test_font_letters(void) {
    send_uart_message("=== TESTING LETTERS ===");
    font_test_clear_screen();
    
    // Uppercase alphabet
    render_16x16_string(5, 10, "ABCDEFGH");
    render_16x16_string(5, 35, "IJKLMNOP");
    render_16x16_string(5, 60, "QRSTUVWX");
    render_16x16_string(5, 85, "YZ");
    
    delay_ms(25, 0);  // 2.5 second delay
    
    font_test_clear_screen();
    
    // Lowercase alphabet  
    render_16x16_string(5, 10, "abcdefgh");
    render_16x16_string(5, 35, "ijklmnop");
    render_16x16_string(5, 60, "qrstuvwx");
    render_16x16_string(5, 85, "yz");
    
    delay_ms(25, 0);  // 2.5 second delay
}

// Test symbols and special characters
void test_font_symbols(void) {
    send_uart_message("=== TESTING SYMBOLS ===");
    font_test_clear_screen();
    
    // Common symbols
    render_16x16_string(5, 10, "!@#$%^&*");
    render_16x16_string(5, 35, "()_+-={}");
    render_16x16_string(5, 60, "[]|\\:;\"'");
    render_16x16_string(5, 85, "<>?,./ ~");
    
    delay_ms(25, 0);  // 2.5 second delay
}

// Test font positioning and spacing
void test_font_positioning(void) {
    send_uart_message("=== TESTING POSITIONING ===");
    font_test_clear_screen();
    
    // Test corners and edges
    render_16x16_char(0, 0, 'A');           // Top-left corner
    render_16x16_char(144, 0, 'B');         // Top-right corner  
    render_16x16_char(0, 112, 'C');         // Bottom-left corner
    render_16x16_char(144, 112, 'D');       // Bottom-right corner
    
    // Test center
    render_16x16_string(55, 55, "CENTER");
    
    delay_ms(25, 0);  // 2.5 second delay
}

// Test font scaling (32x32 vs 16x16)
void test_font_scaling(void) {
    send_uart_message("=== TESTING FONT SCALING ===");
    font_test_clear_screen();
    
    // Test 16x16 font
    render_16x16_string(5, 10, "16x16:");
    render_16x16_string(5, 30, "HELLO");
    render_16x16_number(5, 50, 123);
    
    // Test 32x32 font (2x scaled)
    render_32x32_string(5, 75, "32x32:");
    
    delay_ms(25, 0);  // 2.5 second delay
    
    font_test_clear_screen();
    
    // Show larger text examples
    render_32x32_string(5, 10, "BIG");
    render_32x32_number(5, 50, 456);
    
    delay_ms(25, 0);  // 2.5 second delay
}


// Main font test function that runs all tests
void test_fonts_all(void) {
    send_uart_message("=== STARTING COMPREHENSIVE FONT TESTS ===");
    
    // Clear screen at the very beginning
    font_test_clear_screen();
    
    // Test 1: All ASCII characters (multiple screens)
    test_font_all_characters();
    
    // Test 2: Numbers
    test_font_numbers();
    
    // Test 3: Letters (uppercase and lowercase)
    test_font_letters();
    
    // Test 4: Symbols and special characters
    test_font_symbols();
    
    // Test 5: Positioning tests
    test_font_positioning();
    
    // Test 6: Font scaling
    test_font_scaling();
    
    // Final screen
    font_test_clear_screen();
    render_16x16_string(15, 40, "FONT TESTS");
    render_16x16_string(25, 65, "COMPLETE!");
    
    send_uart_message("=== ALL FONT TESTS COMPLETE ===");
    
    delay_ms(25, 0);  // Final delay
}