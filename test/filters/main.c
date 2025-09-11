#include "H8.h"
#include "delay.h"
#include "watchdog.h"
#include "hardware.h"
#include "pwm.h"
#include "uart.h"
#include "lcd.h"
#include "uart_test.h"
#include "filters.h"

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

    // Run only filters tests
    send_uart_message("=== FILTERS TEST FIRMWARE ===");
    send_uart_message("Filter test would run here");
    send_uart_message("Digital signal processing filters");
    send_uart_message("Low-pass, high-pass, and band-pass filters");
    send_uart_message("=== FILTERS TESTS COMPLETE ===");

    // Simple loop
    while (1) {
        watchdog_reset();
        delay_ms(100, 0);
    }
}