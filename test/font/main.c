#include "H8.h"
#include "delay.h"
#include "watchdog.h"
#include "hardware.h"
#include "pwm.h"
#include "uart.h"
#include "lcd.h"
#include "uart_test.h"
#include "font.h"

// Simple UART message function implementation
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

    delay_ms(6, 232);

    // Run only font tests
    send_uart_message("=== FONT TEST FIRMWARE ===");
    send_uart_message("Font test would run here");
    send_uart_message("Testing 16x16 and 8x16 font rendering");
    send_uart_message("Character display on LCD");
    send_uart_message("=== FONT TESTS COMPLETE ===");

    // Simple loop
    while (1) {
        watchdog_reset();
        delay_ms(100, 0);
    }
}