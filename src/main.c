#include "H8.h"
#include "delay.h"
#include "watchdog.h"
#include "hardware.h"
#include "pwm.h"
#include "types.h"
#include "TA3782F.h"
#include "uart.h"
#include "keypad.h"

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
        delay_ms(1,0x2c);



        while (1) 
	{
                RXLED = 0; 
                TXLED = 1; // Turn on TX_VLED
                BEEP = 0; // Turn off LAMPOW
                LAMPOW = 0; // Turn off LAMPOW
                watchdog_reset(); // Reset the watchdog timer
                delay_ms(6,232);
                TXLED = 0; // Turn off TX_VLED
                RXLED = 1;
                delay_ms(3,232);
                uart_pr_send_byte(0x48);    // 'H'
                uart_pr_send_byte(0x45);    // 'E' 
                uart_pr_send_byte(0x4C);    // 'L'
                uart_pr_send_byte(0x4C);    // 'L'
                uart_pr_send_byte(0x4F);    // 'O'               
                delay_ms(6,232);




        }
}
