#include "H8.h"
#include "delay.h"
#include "watchdog.h"
#include "hardware.h"
#include "pwm.h"
#include "uart.h"
#include "lcd.h"
#include "uart_test.h"
#include "font.h"
#include "lcd_test.h"

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

    // Run only LCD tests
    send_uart_message("=== LCD TEST FIRMWARE ===");
    send_uart_message("Creating LCD background pattern...");
    create_background_pattern();
    send_uart_message("LCD background pattern complete");
    send_uart_message("=== LCD TESTS COMPLETE ===");

    // Simple loop with LCD updates
    while (1) {
        watchdog_reset();
        delay_ms(100, 0);
        create_background_pattern();
    }
}