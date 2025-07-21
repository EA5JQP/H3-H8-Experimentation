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

#define led1 TXLED // Define led1 as RX_VLED for LED control
#define led2 RXLED // Define P31 as TX_VLED for LED control


void main(void)
{
        hardware_init(); // Initialize LED control
        timer_init(); // Initialize the timer
        pwm_init(0,0xc); // Initialize PWM with duty cycle 0 and 0xC
        watchdog_init(); // Initialize the watchdog timer
        watchdog_reset(); // Reset the watchdog timer
        watchdog_config(); // Configure the watchdog timer
        pwm_pin_setup();
        uart_pr_init(); // Initialize the primary UART
        uart_bt_init(); // Initialize the Bluetooth UART
        lcd_init(); // Initialize the LCD display
        delay_ms(1,0x2c);
        at1846s_init(); // Initialize the AT1846S transceiver

        u8 id_low, id_high;

        // Read chip ID (register 0x00) 
        at1846s_spi_transceive(0x80, &id_low, &id_high);


        u8 square_size = 16;  // 16x16 pixel squares

        lcd_set_window(0, 0, 159, 127);  // Full screen
    
        for (u8 row = 0; row < 128; row++) {
            for (u8 col = 0; col < 160; col++) {
                // Determine if we're in a "black" or "white" square
                u8 square_row = row / square_size;
                u8 square_col = col / square_size;
                
                if ((square_row + square_col) % 2 == 0) {
                    // White square
                    lcd_send_data(0xFF);  // High byte
                    lcd_send_data(0xFF);  // Low byte
                } else {
                    // Black square  
                    lcd_send_data(0x00);  // High byte
                    lcd_send_data(0x00);  // Low byte
                }
            }
        }
    

        while (1) 
	{
                RXLED = 0; 
                TXLED = 1; // Turn on TX_VLED
                LAMPOW = 0; // Turn off LAMPOW

                watchdog_reset(); // Reset the watchdog timer
                // TXLED = 0; // Turn off TX_VLED
                // RXLED = 1;
                delay_ms(1,232);

                at1846s_spi_transceive(0x80, &id_low, &id_high);
                delay_ms(0,100);
                uart_pr_send_byte(id_low);
                uart_pr_send_byte(id_high);
                delay_ms(6,232);

                at1846s_spi_transceive(0x82, &id_low, &id_high);
                delay_ms(0,100);
                uart_pr_send_byte(id_low);
                uart_pr_send_byte(id_high);
                delay_ms(6,232);

                at1846s_spi_transceive(0x9c, &id_low, &id_high);
                delay_ms(0,100);
                uart_pr_send_byte(id_low);
                uart_pr_send_byte(id_high);
                delay_ms(6,232);
                
                TXLED = 0; // Turn on TX_VLED
                
        }

}

