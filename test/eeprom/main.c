#include "H8.h"
#include "delay.h"
#include "watchdog.h"
#include "hardware.h"
#include "pwm.h"
#include "uart.h"
#include "lcd.h"
#include "uart_test.h"
#include "i2c.h"
#include "eeprom.h"
#include "eeprom_test.h"

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

    // Initialize I2C for EEPROM
    send_uart_message("=== EEPROM TEST FIRMWARE ===");
    send_uart_message("Initializing I2C bus...");
    i2c_init();
    
    // Run only EEPROM tests
    u16 test_addr = 0x0000;
    test_eeprom_read_initial(test_addr);
    send_uart_message("=== EEPROM TESTS COMPLETE ===");

    // Simple loop
    while (1) {
        watchdog_reset();
        delay_ms(100, 0);
    }
}