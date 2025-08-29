#ifndef MENU_H
#define MENU_H

#include "TA3782F.h"
#include "types.h"
#include "keypad.h"
#include "lcd.h"
#include "font.h"
#include "settings.h"

// Menu system states
#define MENU_MODE_NORMAL    0
#define MENU_MODE_MENU      1

// Menu item types
#define MENU_TYPE_NUMERIC   0
#define MENU_TYPE_CHOICE    1
#define MENU_TYPE_ACTION    2
#define MENU_TYPE_INFO      3

// Menu item IDs (enum-like defines for 8051 compatibility)
#define MENU_FREQUENCY      0
#define MENU_VOLUME         1
#define MENU_SQUELCH        2
#define MENU_POWER          3
#define MENU_BACKLIGHT      4
#define MENU_CTCSS          5
#define MENU_BATTERY_INFO   6
#define MENU_VERSION        7
#define MENU_EXIT           8
#define MENU_COUNT          9

// Menu navigation limits
#define MENU_MIN_CURSOR     0
#define MENU_MAX_CURSOR     (MENU_COUNT - 1)

// Menu edit modes
#define MENU_EDIT_NONE      0
#define MENU_EDIT_VALUE     1

// Menu display constants
#define MENU_TITLE_Y        0
#define MENU_ITEM_Y         32
#define MENU_VALUE_Y        64
#define MENU_NAV_Y          96
#define MENU_TEXT_X         8

// Menu item structure (stored in ROM to save RAM)
typedef struct {
    u8 id;                    // Menu item ID
    const char* name;         // Menu item display name (ROM)
    u8 type;                  // Menu item type
    u16 min_value;           // Minimum value (for numeric types)
    u16 max_value;           // Maximum value (for numeric types)
    u16 step;                // Value step increment
    u8 eeprom_addr;          // EEPROM storage address
} menu_item_t;

// Global menu state variables
extern __xdata volatile u8 menu_mode;
extern __xdata volatile u8 menu_cursor;
extern __xdata volatile u8 menu_in_edit;
extern __xdata volatile u8 menu_display_dirty;
extern __xdata volatile u16 menu_edit_value;

// Menu function declarations
void menu_init(void);
void menu_enter(void);
void menu_exit(void);
void menu_process_key(u8 key);
void menu_update_display(void);
void menu_navigate_up(void);
void menu_navigate_down(void);
void menu_start_edit(void);
void menu_end_edit(__bit save);
void menu_edit_increment(void);
void menu_edit_decrement(void);
void menu_edit_digit_input(u8 digit);
u16 menu_get_current_value(void);
void menu_set_current_value(u16 value);
void menu_apply_setting(u8 menu_id, u16 value);
const menu_item_t* menu_get_current_item(void);

// Menu display helpers
void menu_render_title(void);
void menu_render_item_name(void);
void menu_render_value(void);
void menu_render_navigation(void);
void menu_clear_screen(void);

#endif // MENU_H