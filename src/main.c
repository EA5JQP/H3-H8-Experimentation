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
#include "battery.h"
#include "font.h"
#include "i2c.h"
#include "eeprom.h"

#define led1 TXLED
#define led2 RXLED

// --- UART helpers ---
void send_uart_message(char* message) {
    u8 count = 0;
    while (*message && count < 100) {
        uart_pr_send_byte(*message);
        message++;
        count++;
        delay_ms(1, 0);
    }
    uart_pr_send_byte('\r');
    uart_pr_send_byte('\n');
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
        delay_ms(1, 0);
    }
}

// --- EEPROM test data ---
__xdata u8 eeprom_test_data[32] = {
    0x01,0x02,0x03,0x04,0x05,0x06,0x07,0x08,
    0x09,0x0A,0x0B,0x0C,0x0D,0x0E,0x0F,0x10,
    0x11,0x12,0x13,0x14,0x15,0x16,0x17,0x18,
    0x19,0x1A,0x1B,0x1C,0x1D,0x1E,0x1F,0x20
};

// --- I2C diagnostic functions ---
void test_i2c_pins(void) {
    send_uart_message("I2C Pin Test:");
    
    // Test SDA control
    send_uart_message("Setting SDA HIGH...");
    i2c_set_sda_high();
    uart_pr_send_byte('S'); uart_pr_send_byte('D'); uart_pr_send_byte('A'); uart_pr_send_byte('=');
    uart_pr_send_byte(SDA24 ? '1' : '0');
    uart_pr_send_byte('\r'); uart_pr_send_byte('\n');
    
    send_uart_message("Setting SDA LOW...");
    i2c_set_sda_low();
    uart_pr_send_byte('S'); uart_pr_send_byte('D'); uart_pr_send_byte('A'); uart_pr_send_byte('=');
    uart_pr_send_byte(SDA24 ? '1' : '0');
    uart_pr_send_byte('\r'); uart_pr_send_byte('\n');
    
    // Test SCK control
    send_uart_message("Setting SCK HIGH...");
    i2c_set_sck_high();
    uart_pr_send_byte('S'); uart_pr_send_byte('C'); uart_pr_send_byte('K'); uart_pr_send_byte('=');
    uart_pr_send_byte(SCK24 ? '1' : '0');
    uart_pr_send_byte('\r'); uart_pr_send_byte('\n');
    
    send_uart_message("Setting SCK LOW...");
    i2c_set_sck_low();
    uart_pr_send_byte('S'); uart_pr_send_byte('C'); uart_pr_send_byte('K'); uart_pr_send_byte('=');
    uart_pr_send_byte(SCK24 ? '1' : '0');
    uart_pr_send_byte('\r'); uart_pr_send_byte('\n');
    
    // Test pin direction switching
    send_uart_message("Testing pin direction...");
    i2c_set_sda_output();
    uart_pr_send_byte('O'); uart_pr_send_byte('U'); uart_pr_send_byte('T'); uart_pr_send_byte('\r'); uart_pr_send_byte('\n');
    
    i2c_set_sda_input();
    uart_pr_send_byte('I'); uart_pr_send_byte('N'); uart_pr_send_byte('\r'); uart_pr_send_byte('\n');
    
    // Show P4CON register value
    send_uart_message("P4CON=");
    u8 p4con_val = P4CON;
    uart_pr_send_byte((p4con_val >> 4) > 9 ? 'A' + (p4con_val >> 4) - 10 : '0' + (p4con_val >> 4));
    uart_pr_send_byte((p4con_val & 0xF) > 9 ? 'A' + (p4con_val & 0xF) - 10 : '0' + (p4con_val & 0xF));
    uart_pr_send_byte('\r'); uart_pr_send_byte('\n');
}

void test_i2c_communication(void) {
    __bit ack_result;
    
    send_uart_message("I2C Communication Test:");
    
    // Test full EEPROM sequence with detailed ACK checking
    send_uart_message("Full EEPROM sequence test:");
    
    // Step 1: START
    i2c_start();
    send_uart_message("START sent");
    
    // Step 2: Device address (write)
    ack_result = i2c_send(0xA0);
    uart_pr_send_byte('0'); uart_pr_send_byte('x'); uart_pr_send_byte('A'); uart_pr_send_byte('0');
    uart_pr_send_byte(' '); uart_pr_send_byte('A'); uart_pr_send_byte('C'); uart_pr_send_byte('K'); uart_pr_send_byte('=');
    uart_pr_send_byte(ack_result ? '1' : '0');
    uart_pr_send_byte('\r'); uart_pr_send_byte('\n');
    
    // Step 3: High address byte (0x00)
    ack_result = i2c_send(0x00);
    uart_pr_send_byte('H'); uart_pr_send_byte('I'); uart_pr_send_byte(' '); 
    uart_pr_send_byte('A'); uart_pr_send_byte('C'); uart_pr_send_byte('K'); uart_pr_send_byte('=');
    uart_pr_send_byte(ack_result ? '1' : '0');
    uart_pr_send_byte('\r'); uart_pr_send_byte('\n');
    
    // Step 4: Low address byte (0x00)  
    ack_result = i2c_send(0x00);
    uart_pr_send_byte('L'); uart_pr_send_byte('O'); uart_pr_send_byte(' ');
    uart_pr_send_byte('A'); uart_pr_send_byte('C'); uart_pr_send_byte('K'); uart_pr_send_byte('=');
    uart_pr_send_byte(ack_result ? '1' : '0');
    uart_pr_send_byte('\r'); uart_pr_send_byte('\n');
    
    // Step 5: Repeated START
    i2c_start();
    send_uart_message("Repeated START sent");
    
    // Step 6: Device address (read)
    ack_result = i2c_send(0xA1);
    uart_pr_send_byte('0'); uart_pr_send_byte('x'); uart_pr_send_byte('A'); uart_pr_send_byte('1');
    uart_pr_send_byte(' '); uart_pr_send_byte('A'); uart_pr_send_byte('C'); uart_pr_send_byte('K'); uart_pr_send_byte('=');
    uart_pr_send_byte(ack_result ? '1' : '0');
    uart_pr_send_byte('\r'); uart_pr_send_byte('\n');
    
    // Step 7: Read one byte
    u8 test_byte = i2c_receive(0);  // Send NACK (last byte)
    uart_pr_send_byte('R'); uart_pr_send_byte('E'); uart_pr_send_byte('A'); uart_pr_send_byte('D'); uart_pr_send_byte('=');
    uart_pr_send_byte((test_byte >> 4) > 9 ? 'A' + (test_byte >> 4) - 10 : '0' + (test_byte >> 4));
    uart_pr_send_byte((test_byte & 0xF) > 9 ? 'A' + (test_byte & 0xF) - 10 : '0' + (test_byte & 0xF));
    uart_pr_send_byte('\r'); uart_pr_send_byte('\n');
    
    // Step 8: STOP
    i2c_stop();
    send_uart_message("STOP sent");
}

// --- EEPROM test functions ---
void test_eeprom_write(u16 addr) {
    send_uart_message("EEPROM WRITE start...");
    eeprom_write(addr, eeprom_test_data, 32);
    delay_ms(50, 0);
    send_uart_message("EEPROM WRITE done");
}

void test_eeprom_read(u16 addr) {
    static __xdata u8 read_buffer[32];
    eeprom_read(addr, read_buffer, 32);

    // Print hex values without spaces for faster output
    for (u8 i = 0; i < 32; i++) {
        u8 val = read_buffer[i];
        uart_pr_send_byte((val >> 4) > 9 ? 'A' + (val >> 4) - 10 : '0' + (val >> 4));
        uart_pr_send_byte((val & 0xF) > 9 ? 'A' + (val & 0xF) - 10 : '0' + (val & 0xF));
    }
    uart_pr_send_byte('\r');
    uart_pr_send_byte('\n');
}

void test_eeprom_verify(u16 addr) {
    static __xdata u8 read_buffer[32];
    u8 ok = 1;

    eeprom_read(addr, read_buffer, 32);

    for (u8 i = 0; i < 32; i++) {
        if (read_buffer[i] != eeprom_test_data[i]) {
            ok = 0;
            break;
        }
    }

    if (ok) {
        send_uart_message("EEPROM VERIFY: OK");
    } else {
        send_uart_message("EEPROM VERIFY: ERROR");
    }
}

void create_background_pattern(void) {
    u8 row, col;

    lcd_set_window(0, 0, 159, 127);  // Full screen

    for (row = 0; row < 128; row++) {
        for (col = 0; col < 160; col++) {
            lcd_send_data(0x00);  // High byte
            lcd_send_data(0x00);  // Low byte
        }
    }
}

// --- main ---
void main(void) {
    u16 test_addr = 0x0300;

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

    // Priority: Test EEPROM reading first (most important)
    create_background_pattern();
    
    // Comprehensive EEPROM scan of all addresses
    eeprom_check_all_addresses();
    
    // Quick I2C communication test
    send_uart_message("I2C TEST:");
    i2c_start();
    __bit ack = i2c_send(0xA0);
    i2c_stop();
    uart_pr_send_byte('A'); uart_pr_send_byte('C'); uart_pr_send_byte('K'); uart_pr_send_byte('=');
    uart_pr_send_byte(ack ? '1' : '0');
    uart_pr_send_byte('\r'); uart_pr_send_byte('\n');
    
    send_uart_message("=== DIAGNOSTICS COMPLETE ===");

    while (1) {
        watchdog_reset();
        delay_ms(100, 0);
    }
}
