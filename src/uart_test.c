#include "uart_test.h"

void send_uart_message(char* message) {
    uart_pr_send_string(message);
    uart_pr_send_string("\r\n");
}

void send_uart_number(u16 number) {
    char buffer[6];
    u8 i = 0;
    u16 temp = number;

    if (number == 0) {
        uart_pr_send_byte('0');
        return;
    }

    while (temp > 0 && i < 5) {
        buffer[i++] = '0' + (temp % 10);
        temp /= 10;
    }

    while (i > 0) {
        uart_pr_send_byte(buffer[--i]);
    }
}

void send_uart_hex(u16 number) {
    uart_pr_send_byte('0');
    uart_pr_send_byte('x');
    
    // Send high nibble of high byte
    u8 nibble = (number >> 12) & 0x0F;
    uart_pr_send_byte(nibble > 9 ? 'A' + nibble - 10 : '0' + nibble);
    
    // Send low nibble of high byte  
    nibble = (number >> 8) & 0x0F;
    uart_pr_send_byte(nibble > 9 ? 'A' + nibble - 10 : '0' + nibble);
    
    // Send high nibble of low byte
    nibble = (number >> 4) & 0x0F;
    uart_pr_send_byte(nibble > 9 ? 'A' + nibble - 10 : '0' + nibble);
    
    // Send low nibble of low byte
    nibble = number & 0x0F;
    uart_pr_send_byte(nibble > 9 ? 'A' + nibble - 10 : '0' + nibble);
}

void send_uart_number_32(u32 number) {
    // For 8051, avoid complex 32-bit arithmetic
    // Just display as hex if it's a large number to avoid issues
    if (number == 0) {
        uart_pr_send_byte('0');
        return;
    }
    
    // If it's a small number that fits in 16-bit, use regular function
    if (number <= 65535) {
        send_uart_number((u16)number);
        return;
    }
    
    // For large numbers, display as hex to avoid 32-bit arithmetic issues
    uart_pr_send_byte('0');
    uart_pr_send_byte('x');
    
    // Display as 32-bit hex
    u16 high_word = (u16)(number >> 16);
    u16 low_word = (u16)(number & 0xFFFF);
    
    // High word
    u8 nibble = (high_word >> 12) & 0x0F;
    uart_pr_send_byte(nibble > 9 ? 'A' + nibble - 10 : '0' + nibble);
    nibble = (high_word >> 8) & 0x0F;
    uart_pr_send_byte(nibble > 9 ? 'A' + nibble - 10 : '0' + nibble);
    nibble = (high_word >> 4) & 0x0F;
    uart_pr_send_byte(nibble > 9 ? 'A' + nibble - 10 : '0' + nibble);
    nibble = high_word & 0x0F;
    uart_pr_send_byte(nibble > 9 ? 'A' + nibble - 10 : '0' + nibble);
    
    // Low word
    nibble = (low_word >> 12) & 0x0F;
    uart_pr_send_byte(nibble > 9 ? 'A' + nibble - 10 : '0' + nibble);
    nibble = (low_word >> 8) & 0x0F;
    uart_pr_send_byte(nibble > 9 ? 'A' + nibble - 10 : '0' + nibble);
    nibble = (low_word >> 4) & 0x0F;
    uart_pr_send_byte(nibble > 9 ? 'A' + nibble - 10 : '0' + nibble);
    nibble = low_word & 0x0F;
    uart_pr_send_byte(nibble > 9 ? 'A' + nibble - 10 : '0' + nibble);
}