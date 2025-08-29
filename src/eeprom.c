#include "eeprom.h"

// Forward declarations for functions used in diagnostics
void send_uart_message(char* message);

__xdata u8 eeprom_buffer[32];

__bit eeprom_init(const u16 addr) {
    // This function initializes the EEPROM by sending the start condition and the device address.
    i2c_delay();
    i2c_start();
    if (!i2c_send(0xa0)) {
        i2c_stop();
        return 0; // Device not responding
    }
    /*
    0xA0 = 10100000 in binary
    1010 = mandatory bits for all 2-wire EEPROMs
    000 = A2, A1, A0 address pins (all low/grounded)
    0 = write operation (R/W bit = 0 for write, 1 for read) 
    */
    if (!i2c_send((addr>>8) & 0xff)) {
        i2c_stop();
        return 0; // High address byte not acknowledged
    }
    if (!i2c_send(addr & 0xff)) {
        i2c_stop();
        return 0; // Low address byte not acknowledged
    }
    return 1; // Success
}

__bit eeprom_read(const u16 addr, u8* destination, const u8 size) {
    // Disable interrupts during I2C operation like original firmware
    EA = 0;
    
    // Step 1: Send address pointer (write mode)
    if (!eeprom_init(addr)) {
        EA = 1;
        return 0; // Failed to set address pointer
    }
    
    // Step 2: Repeated START to switch to read mode
    i2c_start();                 // Repeated START (don't send STOP)
    if (!i2c_send(0xA1)) {       // Device address + read bit
        i2c_stop();
        EA = 1;
        return 0; // Device not responding to read command
    }
    
    // Step 3: Read data
    i2c_read(destination, size);
    // Step 4: End transaction
    i2c_stop();
    
    // Re-enable interrupts
    EA = 1;
    return 1; // Success
}

__bit eeprom_write(u16 addr, const __xdata u8 *data, u8 size) {
    if ((size & 31) || (addr & 31)) {
        return 0; // must be 32-byte aligned
    }

    while (size) {
        // Read current EEPROM page
        if (!eeprom_read(addr, eeprom_buffer, 32)) {
            return 0; // Failed to read current page
        }

        // Compare all 32 bytes
        u8 diff = 0;
        for (u8 i = 0; i < 32; i++) {
            if (data[i] != eeprom_buffer[i]) {
                diff = 1;
                break;
            }
        }

        if (diff) {
            // Write page
            if (!eeprom_init(addr)) {
                return 0; // Failed to initialize write
            }
            if (!i2c_write(data, 32)) {
                i2c_stop();
                return 0; // Failed to write data
            }
            i2c_stop();

            // Wait until EEPROM finishes internal write (~5ms)
            // Safer than fixed delay: poll for ACK
            while (1) {
                i2c_start();
                if (i2c_send(0xA0)) {
                    i2c_stop();
                    break;
                }
                i2c_stop();
                i2c_delay();
            }
        }

        addr += 32;
        data += 32;
        size -= 32;
    }
    return 1; // Success
}

// --- Test functions moved to test_functions_reference.c ---

void eeprom_check_all_addresses(void) {
    static __xdata u8 read_buffer[32];
    u16 addr;
    u8 readable_count = 0;
    u16 total_addresses = 0;
    
    send_uart_message("EEPROM FULL SCAN:");
    
    // Scan from 0x0000 to 0x03FF (1KB) in 32-byte chunks
    for (addr = 0x0000; addr < 0x2000; addr += 32){
        total_addresses++;
        
        // Try to read 32 bytes from current address
        if (!eeprom_read(addr, read_buffer, 32)) {
            // Communication failed - fill buffer with error pattern
            for (u8 i = 0; i < 32; i++) {
                read_buffer[i] = 0xFF;
            }
        }
        
        // Display address and first few bytes
        uart_pr_send_byte(((addr >> 12) & 0xF) > 9 ? 'A' + ((addr >> 12) & 0xF) - 10 : '0' + ((addr >> 12) & 0xF));
        uart_pr_send_byte(((addr >> 8) & 0xF) > 9 ? 'A' + ((addr >> 8) & 0xF) - 10 : '0' + ((addr >> 8) & 0xF));
        uart_pr_send_byte(((addr >> 4) & 0xF) > 9 ? 'A' + ((addr >> 4) & 0xF) - 10 : '0' + ((addr >> 4) & 0xF));
        uart_pr_send_byte((addr & 0xF) > 9 ? 'A' + (addr & 0xF) - 10 : '0' + (addr & 0xF));
        uart_pr_send_byte(':');
        
        // Show first 8 bytes of each 32-byte block for quick overview
        for (u8 i = 0; i < 8; i++) {
            u8 val = read_buffer[i];
            uart_pr_send_byte((val >> 4) > 9 ? 'A' + (val >> 4) - 10 : '0' + (val >> 4));
            uart_pr_send_byte((val & 0xF) > 9 ? 'A' + (val & 0xF) - 10 : '0' + (val & 0xF));
        }
        
        // Check if data looks valid (not all 0xFF or all 0x00)
        u8 all_ff = 1, all_00 = 1;
        for (u8 i = 0; i < 32; i++) {
            if (read_buffer[i] != 0xFF) all_ff = 0;
            if (read_buffer[i] != 0x00) all_00 = 0;
        }
        
        if (!all_ff && !all_00) {
            uart_pr_send_byte(' '); uart_pr_send_byte('V'); // Valid data
            readable_count++;
        } else if (all_ff) {
            uart_pr_send_byte(' '); uart_pr_send_byte('F'); // All 0xFF (unprogrammed)
        } else {
            uart_pr_send_byte(' '); uart_pr_send_byte('E'); // All 0x00 (erased/empty)
        }
        
        uart_pr_send_byte('\r'); uart_pr_send_byte('\n');
        
        // Brief delay to prevent overwhelming UART
        delay_ms(1, 0);
    }
    
    // Summary
    send_uart_message("SCAN SUMMARY:");
    uart_pr_send_byte('V'); uart_pr_send_byte('A'); uart_pr_send_byte('L'); uart_pr_send_byte('I'); uart_pr_send_byte('D');
    uart_pr_send_byte('=');
    // Simple decimal output for count
    if (readable_count >= 100) {
        uart_pr_send_byte('0' + (readable_count / 100));
        readable_count %= 100;
    }
    if (readable_count >= 10) {
        uart_pr_send_byte('0' + (readable_count / 10));
        readable_count %= 10;
    }
    uart_pr_send_byte('0' + readable_count);
    uart_pr_send_byte('\r'); uart_pr_send_byte('\n');
}


    