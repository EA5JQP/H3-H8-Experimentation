#include "i2c_test.h"

// Helper function declaration - defined in main.c
extern void send_uart_message(char* message);

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