#include "H8.h"
#include "delay.h"
#include "watchdog.h"
#include "hardware.h"
#include "pwm.h"
#include "uart.h"
#include "lcd.h"
#include "at1846s.h"

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
    at1846s_init();

    delay_ms(6, 232);

    // Run basic AT1846S tests
    send_uart_message("=== AT1846S TEST FIRMWARE ===");
    
    u8 result = at1846s_self_test();
    if (result == 1) {
        send_uart_message("SUCCESS: AT1846S self test passed!");
    } else {
        send_uart_message("FAILURE: AT1846S self test failed!");
    }
    
    send_uart_message("=== AT1846S TESTS COMPLETE ===");

    // Simple loop
    while (1) {
        watchdog_reset();
        delay_ms(100, 0);
    }
}