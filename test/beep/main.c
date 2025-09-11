#include "H8.h"
#include "delay.h"
#include "watchdog.h"
#include "hardware.h"
#include "pwm.h"
#include "uart.h"
#include "lcd.h"
#include "beep.h"

// Simple UART message function
void send_uart_message(char* message) {
    uart_pr_send_string((u8*)message);
    uart_pr_send_string((u8*)"\r\n");
}

void main(void) {
    // Minimal hardware initialization
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
    beep_init();

    delay_ms(6, 232);

    // Run basic beep tests
    send_uart_message("=== BEEP TEST FIRMWARE ===");
    send_uart_message("Testing beep functionality...");
    
    // Test short beep
    beep_tone(10, 1000);
    delay_ms(500, 0);
    
    // Test longer beep
    beep_tone(20, 500);
    delay_ms(500, 0);
    
    send_uart_message("Beep tests completed successfully!");
    send_uart_message("=== BEEP TESTS COMPLETE ===");

    // Simple loop
    while (1) {
        watchdog_reset();
        delay_ms(100, 0);
    }
}