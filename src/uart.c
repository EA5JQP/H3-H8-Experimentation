#include "uart.h"

// Global buffers for reception (add these to your global variables)
u8 __xdata uart_pr_rx_buffer[UART_RX_BUFFER_SIZE];
u8 uart_pr_rx_write_idx = 0;
u8 uart_pr_rx_read_idx = 0;
u8 uart_pr_rx_count = 0;

u8 __xdata uart_bt_rx_buffer[UART_BT_RX_BUFFER_SIZE];
u8 uart_bt_rx_write_idx = 0;
u8 uart_bt_rx_read_idx = 0;
u8 uart_bt_rx_count = 0;


//=================================================
// Programming UART Reception Functions (P11 RX)
//=================================================

void uart_pr_init(void) {

    EXA1 &= 0x3F;
    EXA1 |= 0x40;
    // The following SFR is incorrectly named P1CON in the datasheet provided by TIDRADIO.
    P1CON &= 0x8F;
    P1CON |= 0x30;
    EXA1 |= 0x10;
    PCON |= 0x40;
    exUart1Baud_H = 0xB2;
    exUart1Baud_L = 0xFF;
    exUart1Con &= 0xF8u;
    exUart1Con |= 0x80;
    exUart1Port = 0x11;
    exP11Mode = 2;
    exP13Mode = 2;
}

void uart_pr_send_byte(const u8 byte) {
    
    EXA2 = byte;
    
    u8 timeout_low = 0x00;  // Initialize low byte of timeout counter
    u8 timeout_high = 0x00; // Initialize high byte of timeout counter

    do {
        // Check if transmit complete flag is set (bit 1 of EXA1)
        if (EXA1 & 0x02) break;  // TX complete flag
        // Timeout counter (prevents infinite loop)
        timeout_low++;              // Increment low byte
        if (timeout_low == '\0') {  // If low byte overflows
            timeout_high ++;        // Increment high byte
        }
    } while ((timeout_high != 0x4E) || (timeout_low != 0x20));

    // Clear transmit complete flag
    EXA1 &= 0xFD; // Clear bit 1 (TX complete flag) in EXA1
}

// Check for received data on Programming UART
u8 uart_pr_data_available(void) {
    return uart_pr_rx_count;
}

// Non-blocking receive function for Programming UART
u8 uart_pr_try_receive_byte(u8 *data) {
    if (uart_pr_rx_count == 0) {
        return 0;  // No data available
    }
    
    EA = 0;  // Disable interrupts
    
    *data = uart_pr_rx_buffer[uart_pr_rx_read_idx];
    uart_pr_rx_read_idx = (uart_pr_rx_read_idx + 1) % UART_RX_BUFFER_SIZE;
    uart_pr_rx_count--;
    
    EA = 1;  // Re-enable interrupts
    
    return 1;  // Data received successfully
}

// Blocking receive function for Programming UART
u8 uart_pr_receive_byte(void) {
    u8 data;
    
    // Wait for data
    while (uart_pr_rx_count == 0);
    
    EA = 0;  // Disable interrupts
    
    data = uart_pr_rx_buffer[uart_pr_rx_read_idx];
    uart_pr_rx_read_idx = (uart_pr_rx_read_idx + 1) % UART_RX_BUFFER_SIZE;
    uart_pr_rx_count--;
    
    EA = 1;  // Re-enable interrupts
    
    return data;
}

// Check for Programming UART reception (call this in main loop)
void uart_pr_check_reception(void) {
    // Check if receive flag is set (bit 0 of EXA1)
    if (EXA1 & 0x01) {
        // Data received - read from EXA2
        u8 received_byte = EXA2;
        
        // Store in circular buffer
        uart_pr_rx_buffer[uart_pr_rx_write_idx] = received_byte;
        uart_pr_rx_write_idx = (uart_pr_rx_write_idx + 1) % UART_RX_BUFFER_SIZE;
        
        if (uart_pr_rx_count < UART_RX_BUFFER_SIZE) {
            uart_pr_rx_count++;
        } else {
            // Buffer overflow - advance read pointer
            uart_pr_rx_read_idx = (uart_pr_rx_read_idx + 1) % UART_RX_BUFFER_SIZE;
        }
        
        // Clear receive flag (bit 0 of EXA1)
        EXA1 &= 0xFE;
    }
}

//=====================================
// Bluetooth UART Reception Functions
//=====================================

void uart_bt_init(void) {
    SCON &= 0x3F;
    SCON |= 0x40;
    // The following SFR is incorrectly named P1CON in the datasheet provided by TIDRADIO.
    P1CON &= 0xF8;
    P1CON |= 4;
    SCON |= 0x10;
    PCON |= 0x80;
    exUart1Baud_H = 200;
    exUart1Baud_L = 0xFE;
    exUart2Con &= 0xF8;
    exUart2Con |= 0x80;
    exP20Mode = 2;
    exP21Mode = 2;
    IP |= 0x10;
}

void uart_bt_send_byte(const u8 byte) {
    u8 status;
    SBUF = byte;

    // Wait for transmission to complete (polling method)
    do {
        status = SCON;
    } while ((status & 0x02) == 0);  // Wait for TX complete flag (bit 1)
    
    // Clear transmit complete flag
    SCON &= 0xFD;  // Clear bit 1 (TX complete flag)
}

// Check for received data on Bluetooth UART
u8 uart_bt_data_available(void) {
    return uart_bt_rx_count;
}

// Non-blocking receive function for Bluetooth UART
u8 uart_bt_try_receive_byte(u8 *data) {
    if (uart_bt_rx_count == 0) {
        return 0;  // No data available
    }
    
    EA = 0;  // Disable interrupts
    
    *data = uart_bt_rx_buffer[uart_bt_rx_read_idx];
    uart_bt_rx_read_idx = (uart_bt_rx_read_idx + 1) % UART_BT_RX_BUFFER_SIZE;
    uart_bt_rx_count--;
    
    EA = 1;  // Re-enable interrupts
    
    return 1;  // Data received successfully
}

// Blocking receive function for Bluetooth UART
u8 uart_bt_receive_byte(void) {
    u8 data;
    
    // Wait for data
    while (uart_bt_rx_count == 0);
    
    EA = 0;  // Disable interrupts
    
    data = uart_bt_rx_buffer[uart_bt_rx_read_idx];
    uart_bt_rx_read_idx = (uart_bt_rx_read_idx + 1) % UART_BT_RX_BUFFER_SIZE;
    uart_bt_rx_count--;
    
    EA = 1;  // Re-enable interrupts
    
    return data;
}

// Check for Bluetooth UART reception (call this in main loop)
void uart_bt_check_reception(void) {
    // Check if receive flag is set (bit 0 of SCON)
    if (SCON & 0x01) {
        // Data received - read from SBUF
        u8 received_byte = SBUF;
        
        // Store in circular buffer
        uart_bt_rx_buffer[uart_bt_rx_write_idx] = received_byte;
        uart_bt_rx_write_idx = (uart_bt_rx_write_idx + 1) % UART_BT_RX_BUFFER_SIZE;
        
        if (uart_bt_rx_count < UART_BT_RX_BUFFER_SIZE) {
            uart_bt_rx_count++;
        } else {
            // Buffer overflow - advance read pointer
            uart_bt_rx_read_idx = (uart_bt_rx_read_idx + 1) % UART_BT_RX_BUFFER_SIZE;
        }
        
        // Clear receive flag (bit 0 of SCON)
        SCON &= 0xFE;
    }
}

// Utility Functions
//==================

// Flush Programming UART receive buffer
void uart_pr_flush_rx_buffer(void) {
    EA = 0;
    uart_pr_rx_read_idx = uart_pr_rx_write_idx;
    uart_pr_rx_count = 0;
    EA = 1;
}

// Flush Bluetooth UART receive buffer
void uart_bt_flush_rx_buffer(void) {
    EA = 0;
    uart_bt_rx_read_idx = uart_bt_rx_write_idx;
    uart_bt_rx_count = 0;
    EA = 1;
}

// Send string via Programming UART
void uart_pr_send_string(const u8 *str) {
    while (*str) {
        uart_pr_send_byte(*str++);
    }
}

// Send string via Bluetooth UART  
void uart_bt_send_string(const u8 *str) {
    while (*str) {
        uart_bt_send_byte(*str++);
    }
}

void process_pr_uart_commands(void) {
    static __xdata u8 cmd_buffer[32];
    static u8 cmd_index = 0;
    u8 received_byte;
    
    // Process all available data
    while (uart_pr_data_available()) {
        received_byte = uart_pr_receive_byte();
        
        // Echo received character (optional)
        uart_pr_send_byte(received_byte);
        
        if (received_byte == '\r' || received_byte == '\n') {
            // End of command
            cmd_buffer[cmd_index] = '\0';
            
            if (cmd_index > 0) {
                // Process command based on first character
                switch (cmd_buffer[0]) {
                    case 'E':  // Echo command
                        uart_pr_send_string("ECHO: ");
                        uart_pr_send_string(cmd_buffer + 1);
                        uart_pr_send_string("\r\n");
                        break;
                        
                    case 'V':  // Version
                        uart_pr_send_string("VERSION: 1.0\r\n");
                        break;
                        
                    case 'S':  // Status
                        uart_pr_send_string("STATUS: Ready\r\n");
                        break;
                        
                    case 'R':  // Reset/Restart
                        uart_pr_send_string("RESET: OK\r\n");
                        // Add your reset logic here
                        break;
                        
                    default:
                        uart_pr_send_string("ERROR: Unknown command\r\n");
                        break;
                }
            }
            cmd_index = 0;
        } 
        else if (received_byte == 8 || received_byte == 127) {
            // Backspace
            if (cmd_index > 0) {
                cmd_index--;
                uart_pr_send_string("\b \b");  // Erase character on terminal
            }
        }
        else if (cmd_index < sizeof(cmd_buffer) - 1) {
            // Add to command buffer
            cmd_buffer[cmd_index++] = received_byte;
        }
    }
}