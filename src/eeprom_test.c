#include "eeprom_test.h"

// EEPROM test data
__xdata u8 eeprom_test_data[32] = {
    0x01,0x02,0x03,0x04,0x05,0x06,0x07,0x08,
    0x09,0x0A,0x0B,0x0C,0x0D,0x0E,0x0F,0x10,
    0x11,0x12,0x13,0x14,0x15,0x16,0x17,0x18,
    0x19,0x1A,0x1B,0x1C,0x1D,0x1E,0x1F,0x20
};

// Helper function declaration - defined in main.c
extern void send_uart_message(char* message);

void test_eeprom_read_initial(u16 addr) {
    static __xdata u8 read_buffer_initial[32];
    
    send_uart_message("=== EEPROM INITIAL READ TEST ===");
    send_uart_message("Checking what's in EEPROM before writing...");
    
    __bit read_result = eeprom_read(addr, read_buffer_initial, 32);
    
    if (read_result) {
        send_uart_message("Initial EEPROM read: SUCCESS");
        
        // Show first 8 bytes
        send_uart_message("Current first 8 bytes: ");
        for (u8 i = 0; i < 8; i++) {
            u8 val = read_buffer_initial[i];
            uart_pr_send_byte((val >> 4) > 9 ? 'A' + (val >> 4) - 10 : '0' + (val >> 4));
            uart_pr_send_byte((val & 0xF) > 9 ? 'A' + (val & 0xF) - 10 : '0' + (val & 0xF));
            uart_pr_send_byte(' ');
        }
        uart_pr_send_byte('\r'); uart_pr_send_byte('\n');
        
        // Analyze what we found
        if (read_buffer_initial[0] == 0x00 && read_buffer_initial[1] == 0x00) {
            send_uart_message("INITIAL STATE: All zeros (may be cleared EEPROM)");
        } else if (read_buffer_initial[0] == 0xFF && read_buffer_initial[1] == 0xFF) {
            send_uart_message("INITIAL STATE: All 0xFF (fresh/erased EEPROM)");
        } else if (read_buffer_initial[0] == 0x01 && read_buffer_initial[1] == 0x02) {
            send_uart_message("INITIAL STATE: Contains our test pattern from previous run");
        } else {
            send_uart_message("INITIAL STATE: Contains other data");
        }
    } else {
        send_uart_message("Initial EEPROM read: FAILED");
    }
}

void test_eeprom_write(u16 addr) {
    send_uart_message("=== EEPROM WRITE TEST ===");
    
    // Show what we're going to write
    send_uart_message("Writing test pattern to EEPROM:");
    send_uart_message("Address: 0x0300");
    send_uart_message("Data (first 8 bytes): 01 02 03 04 05 06 07 08");
    
    send_uart_message("Starting EEPROM write...");
    __bit write_result = eeprom_write(addr, eeprom_test_data, 32);
    delay_ms(50, 0);  // Give EEPROM time to complete write cycle
    
    if (write_result) {
        send_uart_message("EEPROM WRITE: SUCCESS");
    } else {
        send_uart_message("EEPROM WRITE: FAILED - Check I2C communication");
    }
}

void test_eeprom_read(u16 addr) {
    static __xdata u8 read_buffer_read[32];
    
    send_uart_message("=== EEPROM READ TEST ===");
    send_uart_message("Reading from address 0x0300...");
    
    __bit read_result = eeprom_read(addr, read_buffer_read, 32);
    
    if (read_result) {
        send_uart_message("EEPROM READ: SUCCESS");
        send_uart_message("Data read from EEPROM:");
        
        // Print first 8 bytes with explanation
        send_uart_message("First 8 bytes: ");
        for (u8 i = 0; i < 8; i++) {
            u8 val = read_buffer_read[i];
            uart_pr_send_byte((val >> 4) > 9 ? 'A' + (val >> 4) - 10 : '0' + (val >> 4));
            uart_pr_send_byte((val & 0xF) > 9 ? 'A' + (val & 0xF) - 10 : '0' + (val & 0xF));
            uart_pr_send_byte(' ');
        }
        uart_pr_send_byte('\r'); uart_pr_send_byte('\n');
        
        send_uart_message("All 32 bytes:");
        // Print all hex values
        for (u8 i = 0; i < 32; i++) {
            u8 val = read_buffer_read[i];
            uart_pr_send_byte((val >> 4) > 9 ? 'A' + (val >> 4) - 10 : '0' + (val >> 4));
            uart_pr_send_byte((val & 0xF) > 9 ? 'A' + (val & 0xF) - 10 : '0' + (val & 0xF));
        }
        uart_pr_send_byte('\r'); uart_pr_send_byte('\n');
        
        // Analyze the data
        if (read_buffer_read[0] == 0x01 && read_buffer_read[1] == 0x02 && read_buffer_read[2] == 0x03) {
            send_uart_message("ANALYSIS: Data matches expected test pattern!");
        } else if (read_buffer_read[0] == 0x00 && read_buffer_read[1] == 0x00) {
            send_uart_message("ANALYSIS: All zeros - write may have failed or fresh EEPROM");
        } else if (read_buffer_read[0] == 0xFF && read_buffer_read[1] == 0xFF) {
            send_uart_message("ANALYSIS: All 0xFF - typical fresh/erased EEPROM state");
        } else {
            send_uart_message("ANALYSIS: Contains other data - may be previously used");
        }
    } else {
        send_uart_message("EEPROM READ: FAILED - Check I2C communication");
    }
}

void test_eeprom_verify(u16 addr) {
    static __xdata u8 read_buffer_verify[32];
    u8 ok = 1;
    u8 first_mismatch = 0;

    send_uart_message("=== EEPROM VERIFY TEST ===");
    send_uart_message("Comparing written data with expected pattern...");
    
    __bit read_result = eeprom_read(addr, read_buffer_verify, 32);
    
    // Debug: Show what we actually read in verify
    send_uart_message("DEBUG - First 8 bytes from verify read:");
    for (u8 i = 0; i < 8; i++) {
        u8 val = read_buffer_verify[i];
        uart_pr_send_byte((val >> 4) > 9 ? 'A' + (val >> 4) - 10 : '0' + (val >> 4));
        uart_pr_send_byte((val & 0xF) > 9 ? 'A' + (val & 0xF) - 10 : '0' + (val & 0xF));
        uart_pr_send_byte(' ');
    }
    uart_pr_send_byte('\r'); uart_pr_send_byte('\n');
    
    if (!read_result) {
        send_uart_message("EEPROM VERIFY: FAILED - Could not read data");
        return;
    }

    // Compare data byte by byte
    for (u8 i = 0; i < 32; i++) {
        if (read_buffer_verify[i] != eeprom_test_data[i]) {
            if (ok) {  // First mismatch
                first_mismatch = i;
            }
            ok = 0;
        }
    }

    if (ok) {
        send_uart_message("EEPROM VERIFY: SUCCESS - All data matches!");
    } else {
        send_uart_message("EEPROM VERIFY: FAILED - Data mismatch detected");
        
        // Show details of first mismatch
        send_uart_message("First mismatch at byte:");
        uart_pr_send_byte('0' + (first_mismatch / 10));
        uart_pr_send_byte('0' + (first_mismatch % 10));
        uart_pr_send_byte('\r'); uart_pr_send_byte('\n');
        
        send_uart_message("Expected vs Read:");
        uart_pr_send_byte('0'); uart_pr_send_byte('x');
        u8 expected = eeprom_test_data[first_mismatch];
        uart_pr_send_byte((expected >> 4) > 9 ? 'A' + (expected >> 4) - 10 : '0' + (expected >> 4));
        uart_pr_send_byte((expected & 0xF) > 9 ? 'A' + (expected & 0xF) - 10 : '0' + (expected & 0xF));
        
        send_uart_message(" vs 0x");
        u8 actual = read_buffer_verify[first_mismatch];
        uart_pr_send_byte((actual >> 4) > 9 ? 'A' + (actual >> 4) - 10 : '0' + (actual >> 4));
        uart_pr_send_byte((actual & 0xF) > 9 ? 'A' + (actual & 0xF) - 10 : '0' + (actual & 0xF));
        uart_pr_send_byte('\r'); uart_pr_send_byte('\n');
    }
}

void eeprom_write_read_test(void) {
    static __xdata u8 write_data[32];
    static __xdata u8 read_data[32];
    u16 test_addr = 0x0200; // Test at address 0

    uart_pr_send_string("=== EEPROM WRITE/READ TEST ===\r\n");

    // Create test pattern
    for (u8 i = 0; i < 32; i++) {
        write_data[i] = 0x55 + i; // Pattern: 0x55, 0x56, 0x57...
    }

    // Show what we're writing
    uart_pr_send_string("Writing test data: ");
    for (u8 i = 0; i < 8; i++) { // Show first 8 bytes
        u8 val = write_data[i];
        uart_pr_send_byte((val >> 4) > 9 ? 'A' + (val >> 4) - 10 : '0' + (val >> 4));
        uart_pr_send_byte((val & 0xF) > 9 ? 'A' + (val & 0xF) - 10 : '0' + (val & 0xF));
    }
    uart_pr_send_string("\r\n");

    // Write test data
    uart_pr_send_string("Writing to EEPROM...\r\n");
    __bit write_success = eeprom_write(test_addr, write_data, 32);

    if (!write_success) {
        uart_pr_send_string("WRITE FAILED!\r\n");
        return;
    }
    uart_pr_send_string("Write completed successfully.\r\n");

    // Brief delay to ensure write is complete
    delay_ms(10, 0);

    // Clear read buffer
    for (u8 i = 0; i < 32; i++) {
        read_data[i] = 0x00;
    }

    // Read back the data
    uart_pr_send_string("Reading back from EEPROM...\r\n");
    __bit read_success = eeprom_read(test_addr, read_data, 32);

    if (!read_success) {
        uart_pr_send_string("READ FAILED!\r\n");
        return;
    }

    // Show what we read
    uart_pr_send_string("Read back data:   ");
    for (u8 i = 0; i < 8; i++) { // Show first 8 bytes
        u8 val = read_data[i];
        uart_pr_send_byte((val >> 4) > 9 ? 'A' + (val >> 4) - 10 : '0' + (val >> 4));
        uart_pr_send_byte((val & 0xF) > 9 ? 'A' + (val & 0xF) - 10 : '0' + (val & 0xF));
    }
    uart_pr_send_string("\r\n");

    // Compare data
    u8 errors = 0;
    for (u8 i = 0; i < 32; i++) {
        if (write_data[i] != read_data[i]) {
            errors++;
        }
    }

    if (errors == 0) {
        uart_pr_send_string("SUCCESS: All data matches!\r\n");
        uart_pr_send_string("EEPROM is working correctly.\r\n");
    } else {
        uart_pr_send_string("FAILURE: Data mismatch detected!\r\n");
        uart_pr_send_string("ERRORS=");
        uart_pr_send_byte('0' + (errors / 10));
        uart_pr_send_byte('0' + (errors % 10));
        uart_pr_send_string("\r\n");

        // Show detailed comparison for first few bytes
        uart_pr_send_string("First few bytes comparison:\r\n");
        for (u8 i = 0; i < 8 && i < 32; i++) {
            uart_pr_send_byte('0' + i);
            uart_pr_send_byte(':');
            uart_pr_send_byte(' ');
            uart_pr_send_byte('W'); uart_pr_send_byte('=');
            u8 wval = write_data[i];
            uart_pr_send_byte((wval >> 4) > 9 ? 'A' + (wval >> 4) - 10 : '0' + (wval >> 4));
            uart_pr_send_byte((wval & 0xF) > 9 ? 'A' + (wval & 0xF) - 10 : '0' + (wval & 0xF));
            uart_pr_send_byte(' ');
            uart_pr_send_byte('R'); uart_pr_send_byte('=');
            u8 rval = read_data[i];
            uart_pr_send_byte((rval >> 4) > 9 ? 'A' + (rval >> 4) - 10 : '0' + (rval >> 4));
            uart_pr_send_byte((rval & 0xF) > 9 ? 'A' + (rval & 0xF) - 10 : '0' + (rval & 0xF));
            uart_pr_send_string("\r\n");
        }
    }

    uart_pr_send_string("=== WRITE/READ TEST COMPLETE ===\r\n");
}

void eeprom_scan_all_content(void) {
    static __xdata u8 read_buffer[32];
    u16 addr;
    u16 non_zero_blocks = 0;
    u16 total_blocks = 0;
    
    uart_pr_send_string("=== EEPROM FULL CONTENT SCAN ===\r\n");
    uart_pr_send_string("Scanning EEPROM from 0x0000 to 0x1FFF (8KB)...\r\n");
    
    // Scan EEPROM in 32-byte blocks (EEPROM page size)
    for (addr = 0x0000; addr < 0x2000; addr += 32) {
        total_blocks++;
        
        // Try to read 32 bytes from current address
        __bit read_success = eeprom_read(addr, read_buffer, 32);
        
        // Display address in hex format
        uart_pr_send_string("0x");
        uart_pr_send_byte(((addr >> 12) & 0xF) > 9 ? 'A' + ((addr >> 12) & 0xF) - 10 : '0' + ((addr >> 12) & 0xF));
        uart_pr_send_byte(((addr >> 8) & 0xF) > 9 ? 'A' + ((addr >> 8) & 0xF) - 10 : '0' + ((addr >> 8) & 0xF));
        uart_pr_send_byte(((addr >> 4) & 0xF) > 9 ? 'A' + ((addr >> 4) & 0xF) - 10 : '0' + ((addr >> 4) & 0xF));
        uart_pr_send_byte((addr & 0xF) > 9 ? 'A' + (addr & 0xF) - 10 : '0' + (addr & 0xF));
        uart_pr_send_string(": ");
        
        if (!read_success) {
            uart_pr_send_string("READ FAILED\r\n");
            continue;
        }
        
        // Check if block contains non-zero data
        u8 has_data = 0;
        for (u8 i = 0; i < 32; i++) {
            if (read_buffer[i] != 0x00) {
                has_data = 1;
                break;
            }
        }
        
        if (has_data) {
            non_zero_blocks++;
            
            // Display first 16 bytes of blocks with data
            for (u8 i = 0; i < 16; i++) {
                u8 val = read_buffer[i];
                uart_pr_send_byte((val >> 4) > 9 ? 'A' + (val >> 4) - 10 : '0' + (val >> 4));
                uart_pr_send_byte((val & 0xF) > 9 ? 'A' + (val & 0xF) - 10 : '0' + (val & 0xF));
                uart_pr_send_byte(' ');
            }
            uart_pr_send_string("... [DATA]\r\n");
            
            // Show full 32 bytes for blocks with interesting data patterns
            u8 all_same = 1;
            u8 first_val = read_buffer[0];
            for (u8 i = 1; i < 32; i++) {
                if (read_buffer[i] != first_val) {
                    all_same = 0;
                    break;
                }
            }
            
            if (!all_same) {
                uart_pr_send_string("     Full: ");
                for (u8 i = 0; i < 32; i++) {
                    u8 val = read_buffer[i];
                    uart_pr_send_byte((val >> 4) > 9 ? 'A' + (val >> 4) - 10 : '0' + (val >> 4));
                    uart_pr_send_byte((val & 0xF) > 9 ? 'A' + (val & 0xF) - 10 : '0' + (val & 0xF));
                    if ((i + 1) % 16 == 0) {
                        uart_pr_send_string("\r\n     ");
                    } else {
                        uart_pr_send_byte(' ');
                    }
                }
                uart_pr_send_string("\r\n");
            }
        } else {
            uart_pr_send_string("00 00 00 00 00 00 00 00 ... [EMPTY]\r\n");
        }
        
        // Brief delay to prevent UART overflow
        delay_ms(2, 0);
        
        // Progress indicator every 64 blocks
        if ((total_blocks % 64) == 0) {
            uart_pr_send_string("--- Scanned ");
            uart_pr_send_byte('0' + (total_blocks / 100));
            uart_pr_send_byte('0' + ((total_blocks / 10) % 10));
            uart_pr_send_byte('0' + (total_blocks % 10));
            uart_pr_send_string(" blocks ---\r\n");
        }
    }
    
    // Summary
    uart_pr_send_string("\r\n=== SCAN SUMMARY ===\r\n");
    uart_pr_send_string("Total blocks scanned: ");
    uart_pr_send_byte('0' + (total_blocks / 100));
    uart_pr_send_byte('0' + ((total_blocks / 10) % 10));
    uart_pr_send_byte('0' + (total_blocks % 10));
    uart_pr_send_string("\r\n");
    
    uart_pr_send_string("Blocks with data: ");
    uart_pr_send_byte('0' + (non_zero_blocks / 100));
    uart_pr_send_byte('0' + ((non_zero_blocks / 10) % 10));
    uart_pr_send_byte('0' + (non_zero_blocks % 10));
    uart_pr_send_string("\r\n");
    
    uart_pr_send_string("Empty blocks: ");
    u16 empty_blocks = total_blocks - non_zero_blocks;
    uart_pr_send_byte('0' + (empty_blocks / 100));
    uart_pr_send_byte('0' + ((empty_blocks / 10) % 10));
    uart_pr_send_byte('0' + (empty_blocks % 10));
    uart_pr_send_string("\r\n");
    
    uart_pr_send_string("=== EEPROM SCAN COMPLETE ===\r\n");
}
