#include "H8.h"
#include "delay.h"
#include "watchdog.h"
#include "hardware.h"
#include "pwm.h"
#include "uart.h"
#include "lcd.h"
#include "uart_test.h"

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

    // Run only UART tests
    send_uart_message("=== UART TEST FIRMWARE ===");
    send_uart_message("Testing UART communication...");
    send_uart_number(12345);
    send_uart_hex(0xABCD);
    send_uart_number_32(4294967295UL);
    send_uart_message("=== UART TESTS COMPLETE ===");

    // Simple loop with periodic messages
    u16 counter = 0;
    while (1) {
        watchdog_reset();
        delay_ms(100, 0);
        if (++counter >= 100) {
            send_uart_message("UART test running...");
            counter = 0;
        }
    }
}