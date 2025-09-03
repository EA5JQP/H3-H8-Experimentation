#ifndef LCD_H
#define LCD_H

#include "TA3782F.h"
#include "types.h"
#include "delay.h"
#include "H8.h"

#define CLR_LCD_RST 0xF8
#define SET_LCD_RST 0xF9  
#define CLR_LCD_CD  0xFA
#define SET_LCD_CD  0xFB
#define DATA_LCD    0xFC
#define CMD_LCD     0xFD
#define DELAY_LCD   0xFE
#define SET_BACKLED 0xFF

void lcd_init(void);
void lcd_send_data(u8 data);
void lcd_send_cmd(u8 command);
void lcd_set_window(u8 x0, u8 y0, u8 x1, u8 y1);
void spi_write_pixel_data(u8 byte_high, u8 byte_low);
void clear_area(u8 x1, u8 y1, u8 x2, u8 y2);

#endif