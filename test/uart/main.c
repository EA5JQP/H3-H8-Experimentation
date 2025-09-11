#include "H8.h"
#include "delay.h"
#include "watchdog.h"
#include "hardware.h"
#include "pwm.h"
#include "uart.h"
#include "lcd.h"
#include "uart_test.h"

// Simple UART message function implementation
void send_uart_message(char* message) {
    uart_pr_send_string((u8*)message);
    uart_pr_send_string((u8*)"\r\n");
}

void send_uart_number(u16 number) {
    // Simple number to string conversion and send
    char buffer[6];
    u8 i = 0, j;
    if (number == 0) {
        uart_pr_send_byte('0');
        return;
    }
    while (number > 0) {
        buffer[i++] = '0' + (number % 10);
        number /= 10;
    }
    for (j = i; j > 0; j--) {
        uart_pr_send_byte(buffer[j-1]);
    }
}

void send_uart_hex(u16 number) {
    uart_pr_send_string((u8*)"0x");
    u8 nibble = (number >> 12) & 0x0F;
    uart_pr_send_byte((nibble > 9) ? ('A' + nibble - 10) : ('0' + nibble));
    nibble = (number >> 8) & 0x0F;
    uart_pr_send_byte((nibble > 9) ? ('A' + nibble - 10) : ('0' + nibble));
    nibble = (number >> 4) & 0x0F;
    uart_pr_send_byte((nibble > 9) ? ('A' + nibble - 10) : ('0' + nibble));
    nibble = number & 0x0F;
    uart_pr_send_byte((nibble > 9) ? ('A' + nibble - 10) : ('0' + nibble));
}

void send_uart_number_32(u32 number) {
    // Simple 32-bit number to string conversion
    char buffer[11];
    u8 i = 0, j;
    if (number == 0) {
        uart_pr_send_byte('0');
        return;
    }
    while (number > 0) {
        buffer[i++] = '0' + (number % 10);
        number /= 10;
    }
    for (j = i; j > 0; j--) {
        uart_pr_send_byte(buffer[j-1]);
    }
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