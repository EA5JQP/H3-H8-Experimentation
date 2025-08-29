#include "H8.h"
#include "delay.h"
#include "watchdog.h"
#include "hardware.h"
#include "pwm.h"
#include "uart.h"
#include "lcd.h"
#include "uart_test.h"
#include "i2c.h"
#include "i2c_test.h"

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

    // Initialize I2C
    send_uart_message("=== I2C TEST FIRMWARE ===");
    send_uart_message("Initializing I2C bus...");
    i2c_init();
    
    // Run only I2C tests
    test_i2c_pins();
    test_i2c_communication();
    send_uart_message("=== I2C TESTS COMPLETE ===");

    // Simple loop
    while (1) {
        watchdog_reset();
        delay_ms(100, 0);
    }
}