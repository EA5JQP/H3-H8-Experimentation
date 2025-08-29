#include "beep_test.h"
#include "beep.h"
#include "delay.h"

// Helper function declaration - defined in main.c
extern void send_uart_message(char* message);

// Massive frequency testing with constant duration
void test_beep_sound(void) {
    send_uart_message("=== TESTING MASSIVE FREQUENCY RANGE ===");
    
    // Mid frequencies
    send_uart_message("450Hz"); beep_tone(2000, 1111); delay_ms(1, 0);
    send_uart_message("500Hz"); beep_tone(2000, 1000); delay_ms(1, 0);
    send_uart_message("550Hz"); beep_tone(2000, 909); delay_ms(1, 0);
    send_uart_message("600Hz"); beep_tone(2000, 833); delay_ms(1, 0);
    send_uart_message("700Hz"); beep_tone(2000, 714); delay_ms(1, 0);
    send_uart_message("800Hz"); beep_tone(2000, 625); delay_ms(1, 0);
    send_uart_message("900Hz"); beep_tone(2000, 556); delay_ms(1, 0);
    send_uart_message("1000Hz"); beep_tone(2000, 500); delay_ms(1, 0);
    
    // High-mid frequencies
    send_uart_message("1100Hz"); beep_tone(2000, 455); delay_ms(1, 0);
    send_uart_message("1200Hz"); beep_tone(2000, 417); delay_ms(1, 0);
    send_uart_message("1300Hz"); beep_tone(2000, 385); delay_ms(1, 0);
    send_uart_message("1400Hz"); beep_tone(2000, 357); delay_ms(1, 0);
    send_uart_message("1500Hz"); beep_tone(2000, 333); delay_ms(1, 0);
    send_uart_message("1600Hz"); beep_tone(2000, 313); delay_ms(1, 0);
    send_uart_message("1700Hz"); beep_tone(2000, 294); delay_ms(1, 0);
    send_uart_message("1800Hz"); beep_tone(2000, 278); delay_ms(1, 0);
    send_uart_message("1900Hz"); beep_tone(2000, 263); delay_ms(1, 0);
    send_uart_message("2000Hz"); beep_tone(2000, 250); delay_ms(1, 0);
    
}



// Simple comprehensive test
void test_beep_all(void) {
    send_uart_message("=== SIMPLE BEEP TEST ===");
    
    // Test basic tones
    test_beep_sound();
    
    send_uart_message("=== BEEP TEST COMPLETE ===");
}

// Individual test functions (for compatibility)
void test_beep_init(void) {
    send_uart_message("=== BEEP INITIALIZATION TEST ===");
    beep_init();
    send_uart_message("Beep system initialized");
    send_uart_message("Initialization test complete");
}