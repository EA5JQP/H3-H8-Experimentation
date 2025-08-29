#ifndef __FONT_TEST_H__
#define __FONT_TEST_H__

#include "types.h"
#include "font.h"
#include "lcd.h"
#include "delay.h"

// Font test functions
void test_font_all_characters(void);
void test_font_numbers(void);
void test_font_letters(void);
void test_font_symbols(void);
void test_font_positioning(void);
void test_font_scaling(void);

// Main font test function
void test_fonts_all(void);

// Screen management
void font_test_next_screen(void);
void font_test_clear_screen(void);

// Display constants for font testing
#define FONT_TEST_CHARS_PER_LINE 8  // 160 / (16 + 2) = 8.8, use 8 for safety
#define FONT_TEST_LINES_PER_SCREEN 6  // 128 / 16 = 8, use 6 for spacing
#define FONT_TEST_LINE_HEIGHT 20   // 16 pixel height + 4 pixel spacing
#define FONT_TEST_CHAR_WIDTH 18    // 16 pixel width + 2 pixel spacing

#endif // __FONT_TEST_H__