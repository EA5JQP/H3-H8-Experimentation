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
#define MENU_TX_POWER       3
#define MENU_BANDWIDTH      4
#define MENU_CTCSS_TX       5
#define MENU_CTCSS_RX       6
#define MENU_DCS_TX         7
#define MENU_DCS_RX         8
#define MENU_VOX            9
#define MENU_MIC_GAIN       10
#define MENU_COMPANDER      11
#define MENU_PRE_EMPHASIS   12
#define MENU_TAIL_ELIM      13
#define MENU_SCRAMBLER      14
#define MENU_BATTERY_INFO   15
#define MENU_RSSI_INFO      16
#define MENU_VERSION        17
#define MENU_COUNT          18

// Menu navigation limits
#define MENU_MIN_CURSOR     0
#define MENU_MAX_CURSOR     (MENU_COUNT - 1)

// Menu edit modes
#define MENU_EDIT_NONE      0
#define MENU_EDIT_CONFIRM   1
#define MENU_EDIT_VALUE     2

// Menu display constants
#define MENU_TITLE_Y        0
#define MENU_ITEM_Y         32
#define MENU_VALUE_Y        64
#define MENU_NAV_Y          96
#define MENU_TEXT_X         8

/**
 * Menu item structure (stored in ROM to save RAM)
 * Defines the properties and behavior of each menu item
 * All string data stored in ROM to preserve precious RAM
 */
typedef struct {
    u8 id;                    // Menu item unique identifier (MENU_* constants)
    const char* name;         // Menu item display name (stored in ROM)
    u8 type;                  // Menu item behavior type (numeric/choice/action/info)
    u16 min_value;           // Minimum allowable value (for numeric types)
    u16 max_value;           // Maximum allowable value (for numeric types)
    u16 step;                // Value increment/decrement step size
    u8 eeprom_addr;          // EEPROM storage address for persistent settings
} menu_item_t;

/**
 * Menu value operations structure for function pointer-based access
 * Eliminates switch statements by providing direct function access
 * More efficient and maintainable than large switch/case blocks
 */
typedef struct {
    u16 (*get_func)(void);    // Function pointer to get current setting value
    void (*set_func)(u16);    // Function pointer to set new setting value
} menu_value_ops_t;

// Global menu state variables
extern __xdata volatile u8 menu_mode;
extern __xdata volatile u8 menu_cursor;
extern __xdata volatile u8 menu_edit_state;
extern __xdata volatile u8 menu_display_dirty;
extern __xdata volatile u16 menu_edit_value;
extern __xdata volatile u8 menu_last_cursor;
extern __xdata volatile u8 menu_current_start_item;

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

// New menu system functions
void menu_render_new_style(void);
void menu_render_single_item(u8 item_index, u8 is_selected);
void menu_render_partial_update(u8 old_cursor, u8 new_cursor);
void menu_format_value_string(const menu_item_t* item, u16 value, char* buffer);
u16 menu_get_current_value_for_item(u8 item_index);

#endif // MENU_H