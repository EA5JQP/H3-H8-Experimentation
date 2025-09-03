#include "lcd.h"

const u8 __code lcd_init_data[] = {
    CLR_LCD_RST,                     
    DELAY_LCD, 100,
    CLR_LCD_RST,
    DELAY_LCD, 100,
    SET_LCD_RST,                    
    CLR_LCD_CD,                     
    DELAY_LCD, 150,                 // Delay 150ms 
    CMD_LCD, 0x11,                  // Sleep out
    DELAY_LCD, 120,                 // Delay 120ms 
    CMD_LCD, 0xB1,  DATA_LCD, 0x02, DATA_LCD, 0x35, DATA_LCD, 0x36,
    CMD_LCD, 0xB2,  DATA_LCD, 0x03, DATA_LCD, 0x35, DATA_LCD, 0x36,
    CMD_LCD, 0xB3,  DATA_LCD, 0x02, DATA_LCD, 0x35, DATA_LCD, 0x36, DATA_LCD, 0x02, DATA_LCD, 0x35, DATA_LCD, 0x36,
    CMD_LCD, 0xB4,  DATA_LCD, 0x03,                 
    CMD_LCD, 0xC0,  DATA_LCD, 0xA2, DATA_LCD, 0x02, DATA_LCD, 0x84, 
    CMD_LCD, 0xC1,  DATA_LCD, 0xC5,                 
    CMD_LCD, 0xC2,  DATA_LCD, 0x0D, DATA_LCD, 0x00,
    CMD_LCD, 0xC3,  DATA_LCD, 0x8D, DATA_LCD, 0xEA,
    CMD_LCD, 0xC4,  DATA_LCD, 0x8D, DATA_LCD, 0xEE,
    CMD_LCD, 0xC5,  DATA_LCD, 0x1A, 
    CMD_LCD, 0x36,  DATA_LCD, 0x60,                                                                                                                                             
    CMD_LCD, 0xE0,  DATA_LCD, 0x03, DATA_LCD, 0x1b, DATA_LCD, 0x09, DATA_LCD, 0x0E, DATA_LCD, 0x32, DATA_LCD, 0x2D,DATA_LCD, 0x28, DATA_LCD, 0x2C, DATA_LCD, 0x2B, DATA_LCD, 0x29, DATA_LCD, 0x30, DATA_LCD, 0x3B, DATA_LCD, 0x00, DATA_LCD, 0x01, DATA_LCD, 0x02, DATA_LCD, 0x10,
    CMD_LCD, 0xE1,  DATA_LCD, 0x03, DATA_LCD, 0x1B, DATA_LCD, 0x09, DATA_LCD, 0x0E, DATA_LCD, 0x32, DATA_LCD, 0x2E,DATA_LCD, 0x28, DATA_LCD, 0x2C, DATA_LCD, 0x2B, DATA_LCD, 0x28, DATA_LCD, 0x31, DATA_LCD, 0x3C, DATA_LCD, 0x00, DATA_LCD, 0x00, DATA_LCD, 0x02, DATA_LCD, 0x10,
    CMD_LCD, 0x3A,  DATA_LCD, 0x05,
    CMD_LCD, 0x2A,  DATA_LCD, 0x00, DATA_LCD, 0x00, DATA_LCD, 0x00, DATA_LCD, 0xA0,
    CMD_LCD, 0x2B,  DATA_LCD, 0x00, DATA_LCD, 0x00, DATA_LCD, 0x00, DATA_LCD, 0x80,
    CMD_LCD, 0x2C,                  // Memory write command
    // lcd_fill_pattern(),          // Fill the LCD with a pattern
    CMD_LCD, 0x29,                  // Display on command
    DELAY_LCD, 0x32,                // Delay 50ms
    CLR_LCD_CD,                     // Clear the command/data select
    SET_BACKLED,                    // Turn on the backlight
};

void lcd_init(void)
{
    const __code u8 *p = lcd_init_data;
    u8 i = 0;
    while(1)
    {
        u8 cmd = *p++;
        switch(cmd)
        {
            case SET_BACKLED:
                BACKLED = 1;        
                return;
            case DELAY_LCD:
                delay_ms(0, *p++);
                break;
            case SET_LCD_RST:
                LCD_RST = 1;
                break;
            case CLR_LCD_RST:
                LCD_RST = 0;
                break;
            case CLR_LCD_CD:
                LCD_CD = 0;
                break;
            case DATA_LCD:
                lcd_send_data(*p++);        
                break;
            case CMD_LCD:
                lcd_send_cmd(*p++);
                break;
        }
    }
}

void lcd_send_data(u8 data) {
    LCD_CD = 0;      // Keep in data mode (D/C = 1 after cmd)
    EXBL = 0;
    EXBH = data;     // Send data byte
    // Wait for transmission complete
    while (!EXBL);
}

void lcd_send_cmd(u8 command) {
    LCD_DAO = 0;     // Set data direction
    LCD_CD = 0;      // Set to command mode (D/C = 0)
    EXBL = 0;
    EXBH = command;  // Send command byte
    // Wait for transmission complete
    while (!EXBL);
    LCD_DAO = 1;
}

void spi_write_pixel_data(u8 byte_high,u8 byte_low)
{
    // Function to write pixel data to the LCD
    lcd_send_data(byte_high); // Send high byte
    lcd_send_data(byte_low);  // Send low byte
}

void lcd_set_window(u8 x0, u8 y0, u8 x1, u8 y1)
{
    // CASET (0x2A) - Column Address Set
    lcd_send_cmd(0x2A);
    lcd_send_data(0x00);    // XS[15:8] - high byte (always 0 for this display)
    lcd_send_data(x0);      // XS[7:0] - start column
    lcd_send_data(0x00);    // XE[15:8] - high byte (always 0)
    lcd_send_data(x1);      // XE[7:0] - end column
    
    // RASET (0x2B) - Row Address Set
    lcd_send_cmd(0x2B);  
    lcd_send_data(0x00);    // YS[15:8] - high byte (always 0)
    lcd_send_data(y0);      // YS[7:0] - start row
    lcd_send_data(0x00);    // YE[15:8] - high byte (always 0)
    lcd_send_data(y1);      // YE[7:0] - end row

    lcd_send_cmd(0x2C); // Memory write command
}

void clear_area(u8 x1, u8 y1, u8 x2, u8 y2) {
    lcd_set_window(x1, y1, x2, y2);
    
    u16 pixel_count = (u16)(x2 - x1 + 1) * (y2 - y1 + 1);
    for (u16 i = 0; i < pixel_count; i++) {
        lcd_send_data(0x00);  // Black background
        lcd_send_data(0x00);
    }
}



