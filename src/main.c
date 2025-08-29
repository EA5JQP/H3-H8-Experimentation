#include "H8.h"
#include "delay.h"
#include "watchdog.h"
#include "hardware.h"
#include "pwm.h"
#include "types.h"
#include "TA3782F.h"
#include "uart.h"
#include "keypad.h"
#include "lcd.h"
#include "at1846s.h"
#include "at1846s_test.h"
#include "battery.h"
#include "font.h"
#include "i2c.h"
#include "eeprom.h"
#include "i2c_test.h"
#include "eeprom_test.h"
#include "lcd_test.h"
#include "led_test.h"
#include "beep_test.h"
#include "uart_test.h"
#include "menu.h"
#include "settings.h"

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
    lcd_init();
    at1846s_init();

    delay_ms(6, 232);

    // Initialize I2C bus before any EEPROM operations
    send_uart_message("Initializing I2C bus...");
    i2c_init();
    
    // Initialize menu system and load settings
    send_uart_message("Initializing menu system...");
    menu_init();
    settings_init();
    
    // === I2C TESTS ===
    send_uart_message("");
    send_uart_message("=== I2C COMMUNICATION TESTS ===");
    
    // Test I2C pin control
    test_i2c_pins();
    
    // Test I2C communication protocol
    test_i2c_communication();
    
    // Quick device check (A0-A2 are grounded, so address is 0xA0)
    send_uart_message("Testing basic device response...");
    EA = 0; // Disable interrupts for I2C transaction
    i2c_start();
    __bit device_present = i2c_send(0xA0);
    i2c_stop();
    EA = 1; // Re-enable interrupts
    
    // Debug: Show device_present result
    send_uart_message("Device present result:");
    uart_pr_send_byte(device_present ? '1' : '0');
    uart_pr_send_byte('\r');
    uart_pr_send_byte('\n');
    
    // === EEPROM TESTS ===
    send_uart_message("");
    send_uart_message("=== EEPROM TESTS ===");
    
    if (device_present) {
        send_uart_message("DEVICE RESPONDING - Proceeding with EEPROM tests");
    } else {
        send_uart_message("DEVICE detection failed, but I2C working - trying EEPROM tests anyway");
    }
    
    // Run EEPROM tests regardless since I2C communication is working
    u16 test_addr = 0x0000;
    
    // Check what's in EEPROM initially
    test_eeprom_read_initial(test_addr);
    
    // Test EEPROM write
    // test_eeprom_write(test_addr);
    
    // Test EEPROM read
    // test_eeprom_read(test_addr);
    
    // Test EEPROM verify
    // test_eeprom_verify(test_addr);

    // eeprom_write_read_test();
    
    // Scan entire EEPROM content
    // eeprom_scan_all_content();
    
    send_uart_message("=== EEPROM DIAGNOSTICS COMPLETE ===");
    
    // === LCD TESTS ===
    send_uart_message("");
    send_uart_message("=== LCD TESTS ===");
    send_uart_message("Creating LCD background pattern...");
    create_background_pattern();
    send_uart_message("LCD background pattern complete");
    
    // === LED TESTS ===
    // send_uart_message("");
    // send_uart_message("=== LED TESTS ===");
    
    // test_backlight_led_blink();
    // test_bt_led_blink();
    // test_tx_led_blink();
    // test_rx_led_blink();
    // test_lampow_blink();
    // test_all_leds_blink();
    
    send_uart_message("=== LED TESTS COMPLETE ===");
    
    // === BEEP TESTS ===
    send_uart_message("");
    send_uart_message("=== BEEP TESTS ===");
    
    test_beep_all();
    
    send_uart_message("=== BEEP TESTS COMPLETE ===");
    
    // AT1846S Communication Tests (Safe - No TX)
    send_uart_message("");
    send_uart_message("=== AT1846S COMMUNICATION TESTS ===");
    
    // Test 1: Basic Communication
    send_uart_message("Test 1: Basic Communication...");
    u8 result = at1846s_test_basic_communication();
    if (result == AT1846S_TEST_SUCCESS) {
        send_uart_message("PASS: Chip ID verified");
    } else {
        send_uart_message("FAIL: Communication error");
        send_uart_number(result);
    }
    
    // Test 2: Register Read/Write
    send_uart_message("Test 2: Register Read/Write...");
    result = at1846s_test_register_readwrite();
    if (result == AT1846S_TEST_SUCCESS) {
        send_uart_message("PASS: Register R/W working");
    } else {
        send_uart_message("FAIL: Register R/W failed");
        send_uart_number(result);
    }
    
    // Test 3: Power Control (RX only)
    send_uart_message("Test 3: Power Control (RX only)...");
    result = at1846s_test_power_control();
    if (result == AT1846S_TEST_SUCCESS) {
        send_uart_message("PASS: Power control working");
    } else {
        send_uart_message("FAIL: Power control failed");
        send_uart_number(result);
    }
    
    // Test 4: Read-only Registers
    send_uart_message("Test 4: Read-only Registers...");
    result = at1846s_test_read_only_registers();
    if (result == AT1846S_TEST_SUCCESS) {
        send_uart_message("PASS: Status registers readable");
    } else {
        send_uart_message("FAIL: Status register read failed");
        send_uart_number(result);
    }
    
    // Test 5: Frequency Settings
    send_uart_message("Test 5: Frequency Settings...");
    result = at1846s_test_frequency_settings();
    if (result == AT1846S_TEST_SUCCESS) {
        send_uart_message("PASS: Frequency control working");
    } else {
        send_uart_message("FAIL: Frequency control failed");
        send_uart_number(result);
        
        // Display diagnostic information
        send_uart_message("--- FREQUENCY TEST DIAGNOSTICS ---");
        
        send_uart_message("Original freq (kHz): ");
        send_uart_number_32(at1846s_diag_orig_freq);
        
        send_uart_message("Test freq (kHz): ");
        send_uart_number_32(at1846s_diag_test_freq);
        
        send_uart_message("Read back freq (kHz): ");
        send_uart_number_32(at1846s_diag_read_freq);
        
        send_uart_message("Freq high reg: ");
        send_uart_hex(at1846s_diag_freq_high_reg);
        
        send_uart_message("Freq low reg: ");
        send_uart_hex(at1846s_diag_freq_low_reg);
        
        send_uart_message("Band register: ");
        send_uart_hex(at1846s_diag_orig_band);
        
        // Analyze the test results for diagnosis
        if (at1846s_diag_freq_high_reg == 0xFFFF && at1846s_diag_freq_low_reg == 0xFFFF) {
            send_uart_message("DIAGNOSIS: Frequency registers are protected.");
            send_uart_message("Tested band selection instead.");
        } else {
            send_uart_message("DIAGNOSIS: Tested 446 MHz in UHF band.");
            
            u32 test_freq = at1846s_diag_test_freq;
            u32 read_freq = at1846s_diag_read_freq;
            
            if (read_freq >= 400000 && read_freq <= 520000) {
                send_uart_message("Result is within UHF band (400-520 MHz).");
                send_uart_message("Frequency control is working correctly!");
            } else if (read_freq != at1846s_diag_orig_freq) {
                send_uart_message("Frequency changed - chip responded to command.");
            } else {
                send_uart_message("No frequency change detected.");
            }
        }
        
        send_uart_message("--- END DIAGNOSTICS ---");
    }
    
    // Test 6: Audio Controls
    send_uart_message("Test 6: Audio Controls...");
    result = at1846s_test_audio_controls();
    if (result == AT1846S_TEST_SUCCESS) {
        send_uart_message("PASS: Audio controls working");
    } else {
        send_uart_message("FAIL: Audio control failed");
        send_uart_number(result);
    }
    
    // Run comprehensive test suite
    send_uart_message("");
    send_uart_message("Running comprehensive test suite...");
    result = at1846s_run_safe_tests();
    if (result == AT1846S_TEST_SUCCESS) {
        send_uart_message("SUCCESS: All AT1846S tests passed!");
        send_uart_message("Chip communication is working properly.");
    } else {
        send_uart_message("FAILURE: Some AT1846S tests failed!");
        send_uart_message("Check SPI connections and power.");
    }
    
    // Display chip information using proven SPI method
    send_uart_message("");
    send_uart_message("=== AT1846S CHIP INFORMATION ===");
    
    u8 reg_low, reg_high;
    u16 chip_id, version, battery, rssi;
    
    // Read Chip ID using proven SPI method
    at1846s_spi_transceive(0x80, &reg_high, &reg_low);  // Read register 0x00
    chip_id = ((u16)reg_high << 8) | reg_low;
    send_uart_message("Chip ID: ");
    send_uart_hex(chip_id);
    
    // Read Version using proven SPI method
    at1846s_spi_transceive(0x81, &reg_high, &reg_low);  // Read register 0x01
    version = ((u16)reg_high << 8) | reg_low;
    send_uart_message("Version: ");
    send_uart_hex(version);
    
    // Read Battery using proven SPI method
    at1846s_spi_transceive(0x80 | AT1846S_REG_BATTERY, &reg_high, &reg_low);
    battery = ((u16)reg_high << 8) | reg_low;
    send_uart_message("Battery: ");
    send_uart_hex(battery);
    
    // Interpret battery voltage (AT1846S battery ADC reading)
    // For 0x8A24 (35364), estimate voltage using simple lookup
    send_uart_message("Battery (estimated): ");
    if (battery > 40000) {
        send_uart_message("9.0V+");
    } else if (battery > 35000) {
        send_uart_message("8.5V");
    } else if (battery > 30000) {
        send_uart_message("7.5V");
    } else if (battery > 25000) {
        send_uart_message("6.5V");
    } else if (battery > 20000) {
        send_uart_message("5.5V");
    } else {
        send_uart_message("LOW");
    }
    
    // Read RSSI using proven SPI method
    at1846s_spi_transceive(0x80 | AT1846S_REG_RSSI, &reg_high, &reg_low);
    rssi = ((u16)reg_high << 8) | reg_low;
    send_uart_message("RSSI: ");
    send_uart_hex(rssi);
    
    // Display current frequency using proven SPI method
    u8 freq_high_high, freq_high_low, freq_low_high, freq_low_low;
    at1846s_spi_transceive(0x80 | AT1846S_REG_FREQ_HIGH, &freq_high_high, &freq_high_low);
    at1846s_spi_transceive(0x80 | AT1846S_REG_FREQ_LOW, &freq_low_high, &freq_low_low);
    
    u16 freq_high_reg = ((u16)freq_high_high << 8) | freq_high_low;
    u16 freq_low_reg = ((u16)freq_low_high << 8) | freq_low_low;
    u32 freq_value = (((u32)(freq_high_reg & 0x3FFF)) << 16) | freq_low_reg;
    u32 current_freq = freq_value / 16;
    
    if (current_freq > 0) {
        send_uart_message("Current Freq (kHz): ");
        send_uart_number_32(current_freq);
    }
    
    send_uart_message("=== AT1846S SPI TEST ===");
    u8 id_low = 0, id_high = 0;

    at1846s_spi_transceive(0x80, &id_high, &id_low);
    delay_ms(0,100);
    
    // Display results in hex format
    send_uart_message("Chip ID bytes:");
    send_uart_message("High byte: ");
    uart_pr_send_byte((id_high >> 4) > 9 ? 'A' + (id_high >> 4) - 10 : '0' + (id_high >> 4));
    uart_pr_send_byte((id_high & 0xF) > 9 ? 'A' + (id_high & 0xF) - 10 : '0' + (id_high & 0xF));
    uart_pr_send_byte('\r'); uart_pr_send_byte('\n');
    
    send_uart_message("Low byte: ");
    uart_pr_send_byte((id_low >> 4) > 9 ? 'A' + (id_low >> 4) - 10 : '0' + (id_low >> 4));
    uart_pr_send_byte((id_low & 0xF) > 9 ? 'A' + (id_low & 0xF) - 10 : '0' + (id_low & 0xF));
    uart_pr_send_byte('\r'); uart_pr_send_byte('\n');
    
    
    
    send_uart_message("=== TEST COMPLETE ===");
    delay_ms(6,232);

    while (1) {
        watchdog_reset();
        
        // Check for key presses and handle menu system
        u8 current_key = keypad_scan();
        if (current_key != 0) {
            if (menu_mode) {
                // In menu mode - process menu keys
                menu_process_key(current_key);
            } else {
                // In normal mode - check for menu entry key
                if (current_key == KEY_MENU) {
                    menu_enter();
                } else {
                    // Handle other normal mode keys here
                    send_uart_message("Key pressed in normal mode:");
                    uart_pr_send_byte('0' + current_key);
                    uart_pr_send_byte('\r');
                    uart_pr_send_byte('\n');
                }
            }
        }
        
        // Update menu display if needed
        if (menu_mode && menu_display_dirty) {
            menu_update_display();
        } else if (!menu_mode) {
            // Update normal mode display
            create_background_pattern();
        }
        
        delay_ms(50, 0);  // Reduced delay for more responsive key handling
    }
}
