#ifndef FONT_H
#define FONT_H

#include "TA3782F.h"
#include "types.h"
#include "lcd.h"

// Display and font definitions
#define DISPLAY_WIDTH 160
#define DISPLAY_HEIGHT 128
#define CHAR_WIDTH_IN_PIXELS 8
#define CHAR_HEIGHT_IN_PIXELS 16
#define MAX_CHARS_PER_STRING DISPLAY_WIDTH / CHAR_WIDTH_IN_PIXELS  
#define SPACING_BETWEEN_CHARS 12


// 16x16 font definitions
#define FONT_16X16_WIDTH 16
#define FONT_16X16_HEIGHT 16
#define FONT_16X16_CHAR_COUNT 95  // ASCII 0x20-0x7E (space to tilde)
#define FONT_16X16_BYTES_PER_CHAR 32  // 16 rows * 2 bytes per row

// Character mapping for digits 0-9  
#define GET_DIGIT_INDEX(c) ((c >= '0' && c <= '9') ? (c - '0') : 0)
// Character mapping for all printable ASCII
#define GET_CHAR_INDEX(c) ((c >= 0x20 && c <= 0x7E) ? (c - 0x20) : 0)

// 16x16 font functions
void render_16x16_char(u8 x, u8 y, char c);
void render_16x16_string(u8 x, u8 y, const char *str);
void render_16x16_number(u8 x, u8 y, u16 number);

// 16x8 font definitions and functions  
#define FONT_16X8_WIDTH 16
#define FONT_16X8_HEIGHT 8
void render_16x8_char(u8 x, u8 y, char c);
void render_16x8_string(u8 x, u8 y, const char *str);
void render_16x8_number(u8 x, u8 y, u16 number);

// 16x8 inverted text functions (for menu highlighting)
void render_16x8_char_inverted(u8 x, u8 y, char c);
void render_16x8_string_inverted(u8 x, u8 y, const char *str);

// Drawing utilities
void draw_horizontal_line(u8 x1, u8 x2, u8 y);

// 32x32 scaled font functions (2x scaling of 16x16)
#define FONT_32X32_WIDTH 32
#define FONT_32X32_HEIGHT 32
void render_32x32_char(u8 x, u8 y, char c);
void render_32x32_string(u8 x, u8 y, const char *str);
void render_32x32_number(u8 x, u8 y, u16 number);


// Utility functions

// Demo functions
void dual_font_demo(void);
void test_fonts_separate(void);

// LCD interface functions (must be implemented elsewhere)
void lcd_set_window(u8 x1, u8 y1, u8 x2, u8 y2);
void lcd_send_data(u8 data);


#endif // FONT_H