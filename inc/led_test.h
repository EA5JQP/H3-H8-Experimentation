#ifndef LED_TEST_H
#define LED_TEST_H

#include "H8.h"
#include "types.h"
#include "delay.h"
#include "uart.h"

// LED Test Functions
void test_all_leds_blink(void);
void test_backlight_led_blink(void);
void test_bt_led_blink(void);
void test_tx_led_blink(void);
void test_rx_led_blink(void);
void test_lampow_blink(void);
void test_beep_blink(void);

#endif // LED_TEST_H