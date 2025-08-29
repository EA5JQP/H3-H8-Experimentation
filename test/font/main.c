#include "H8.h"
#include "delay.h"
#include "watchdog.h"
#include "hardware.h"
#include "pwm.h"
#include "uart.h"
#include "lcd.h"
#include "uart_test.h"
#include "font.h"
#include "font_test.h"

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
    test_fonts_all();
    send_uart_message("=== FONT TESTS COMPLETE ===");

    // Simple loop
    while (1) {
        watchdog_reset();
        delay_ms(100, 0);
    }
}