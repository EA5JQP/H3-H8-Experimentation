#ifndef UART_H
#define UART_H

#include "TA3782F.h"
#include "types.h"

void uart_bt_init(void);
void uart_pr_init(void);
void uart_pr_send_byte(const u8 byte);
void uart_bt_send_byte(const u8 byte);

#endif