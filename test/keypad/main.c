#include "H8.h"
#include "delay.h"
#include "watchdog.h"
#include "hardware.h"
#include "pwm.h"
#include "types.h"
#include "TA3782F.h"
#include "uart.h"
#include "keypad.h"
#include "uart_test.h"
#include "keypad_test.h"

// Simple diagnostic function
void show_basic_status(void) {
    send_uart_message("Basic port status:");
    
    uart_pr_send_byte('P');
    uart_pr_send_byte('1');
    uart_pr_send_byte(':');
    u8 high = (P1 >> 4) & 0x0F;
    u8 low = P1 & 0x0F;
    uart_pr_send_byte((high > 9) ? ('A' + high - 10) : ('0' + high));
    uart_pr_send_byte((low > 9) ? ('A' + low - 10) : ('0' + low));
    uart_pr_send_byte(' ');
    
    uart_pr_send_byte('P');
    uart_pr_send_byte('3');
    uart_pr_send_byte(':');
    high = (P3 >> 4) & 0x0F;
    low = P3 & 0x0F;
    uart_pr_send_byte((high > 9) ? ('A' + high - 10) : ('0' + high));
    uart_pr_send_byte((low > 9) ? ('A' + low - 10) : ('0' + low));
    uart_pr_send_byte('\r');
    uart_pr_send_byte('\n');
}

// --- main ---
void main(void) {
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

    delay_ms(6, 232);

    send_uart_message("=== H8 KEYPAD TEST ===");
    send_uart_message("Testing keypad.c functions");
    send_uart_message("");
    
    // Show initial port status
    show_basic_status();
    send_uart_message("");
    
    // Run keypad test suite
    test_keypad_all();
    
    send_uart_message("");
    send_uart_message("=== KEYPAD TEST COMPLETE ===");

    // Simple loop for any additional manual testing
    while (1) {
        watchdog_reset();
        delay_ms(100, 0);
    }
}