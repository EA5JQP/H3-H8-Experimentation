#include "led_test.h"

// Helper function declaration - defined in main.c
extern void send_uart_message(char* message);

void test_backlight_led_blink(void) {
    send_uart_message("=== BACKLIGHT LED TEST ===");
    send_uart_message("Blinking backlight LED 5 times...");
    
    for (u8 i = 0; i < 5; i++) {
        BACKLED = 1;  // Turn on
        delay_ms(4, 0);
        BACKLED = 0;  // Turn off
        delay_ms(4, 0);
        
        send_uart_message("Backlight blink cycle complete");
    }
    
    // Leave backlight on
    BACKLED = 1;
    send_uart_message("Backlight LED test complete - left ON");
}

void test_bt_led_blink(void) {
    send_uart_message("=== BLUETOOTH LED TEST ===");
    send_uart_message("Blinking BT LED 5 times...");
    
    for (u8 i = 0; i < 5; i++) {
        BT_LED = 1;  // Turn on
        delay_ms(4, 0);
        BT_LED = 0;  // Turn off  
        delay_ms(4, 0);
    }
    
    send_uart_message("BT LED test complete - left OFF");
}

void test_tx_led_blink(void) {
    send_uart_message("=== TX LED TEST ===");
    send_uart_message("Blinking TX LED 5 times...");
    
    for (u8 i = 0; i < 5; i++) {
        TXLED = 1;  // Turn on
        delay_ms(4, 0);
        TXLED = 0;  // Turn off
        delay_ms(4, 0);
    }
    
    send_uart_message("TX LED test complete - left OFF");
}

void test_rx_led_blink(void) {
    send_uart_message("=== RX LED TEST ===");
    send_uart_message("Blinking RX LED 5 times...");
    
    for (u8 i = 0; i < 5; i++) {
        RXLED = 1;  // Turn on
        delay_ms(4, 0);
        RXLED = 0;  // Turn off
        delay_ms(4, 0);
    }
    
    send_uart_message("RX LED test complete - left OFF");
}

void test_all_leds_blink(void) {
    send_uart_message("=== ALL LEDS SIMULTANEOUS BLINK TEST ===");
    send_uart_message("Blinking all LEDs together 3 times...");
    
    for (u8 i = 0; i < 3; i++) {
        // Turn all LEDs on
        BACKLED = 1;
        BT_LED = 1;
        TXLED = 1;
        RXLED = 1;
        
        delay_ms(4, 0);
        
        // Turn all LEDs off
        BACKLED = 0;
        BT_LED = 0;
        TXLED = 0;
        RXLED = 0;
        
        delay_ms(4, 0);
        
        send_uart_message("All LEDs blink cycle complete");
    }
    
    // Restore backlight
    BACKLED = 1;
    send_uart_message("All LEDs test complete - backlight restored");
}

void test_lampow_blink(void) {
    send_uart_message("=== LAMPOW TEST ===");
    send_uart_message("Blinking LAMPOW 5 times...");
    
    for (u8 i = 0; i < 5; i++) {
        LAMPOW = 1;  // Turn on
        delay_ms(4, 0);
        LAMPOW = 0;  // Turn off
        delay_ms(4, 0);
    }
    
    send_uart_message("LAMPOW test complete - left OFF");
}

void test_beep_blink(void) {
    send_uart_message("=== BEEP TEST ===");
    send_uart_message("Testing BEEP 5 times (should make sound)...");
    
    for (u8 i = 0; i < 5; i++) {
        BEEP = 1;  // Turn on beeper
        delay_ms(4, 0);
        BEEP = 0;  // Turn off beeper
        delay_ms(4, 0);
        
        send_uart_message("Beep cycle complete");
    }
    
    send_uart_message("BEEP test complete - left OFF");
}