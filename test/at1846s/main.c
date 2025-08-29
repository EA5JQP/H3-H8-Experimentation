#include "H8.h"
#include "delay.h"
#include "watchdog.h"
#include "hardware.h"
#include "pwm.h"
#include "uart.h"
#include "lcd.h"
#include "uart_test.h"
#include "at1846s.h"
#include "at1846s_test.h"

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

    // Run only AT1846S tests
    send_uart_message("=== AT1846S TEST FIRMWARE ===");
    
    u8 result = at1846s_run_safe_tests();
    if (result == AT1846S_TEST_SUCCESS) {
        send_uart_message("SUCCESS: All AT1846S tests passed!");
    } else {
        send_uart_message("FAILURE: Some AT1846S tests failed!");
    }
    
    send_uart_message("=== AT1846S TESTS COMPLETE ===");

    // Simple loop
    while (1) {
        watchdog_reset();
        delay_ms(100, 0);
    }
}