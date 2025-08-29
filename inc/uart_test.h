#ifndef UART_TEST_H
#define UART_TEST_H

#include "uart.h"
#include "types.h"

// UART Helper Functions
void send_uart_message(char* message);
void send_uart_number(u16 number);
void send_uart_hex(u16 number);
void send_uart_number_32(u32 number);

#endif // UART_TEST_H