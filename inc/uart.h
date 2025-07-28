#ifndef UART_H
#define UART_H

#include "TA3782F.h"
#include "types.h"

// Reception buffer definitions
#define UART_RX_BUFFER_SIZE 64
#define UART_BT_RX_BUFFER_SIZE 64

void uart_bt_init(void);
void uart_pr_init(void);
void uart_pr_send_byte(const u8 byte);
void uart_bt_send_byte(const u8 byte);

u8 uart_pr_data_available(void);
u8 uart_bt_data_available(void);
u8 uart_pr_receive_byte(void);
u8 uart_bt_receive_byte(void);
void uart_pr_check_reception(void);
void uart_bt_check_reception(void);
void uart_pr_send_string(const char *str);
void uart_bt_send_string(const char *str);
void uart_pr_flush_rx_buffer(void);
void uart_bt_flush_rx_buffer(void);
void process_pr_uart_commands(void);


#endif