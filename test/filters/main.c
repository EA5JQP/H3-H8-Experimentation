#include "H8.h"
#include "delay.h"
#include "watchdog.h"
#include "hardware.h"
#include "pwm.h"
#include "uart.h"
#include "lcd.h"
#include "uart_test.h"
#include "filters.h"
#include "filters_test.h"

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
    filters_simple_test();
    u8 result = filters_run_all_tests();
    if (result == FILTERS_SUCCESS) {
        send_uart_message("All filters tests passed!");
    } else {
        send_uart_message("Some filters tests failed!");
    }
    send_uart_message("=== FILTERS TESTS COMPLETE ===");

    // Simple loop
    while (1) {
        watchdog_reset();
        delay_ms(100, 0);
    }
}