#include "menu.h"
#include "uart.h"
#include "uart_test.h"
#include "at1846s.h"

// Global menu state variables
__xdata volatile u8 menu_mode = MENU_MODE_NORMAL;
__xdata volatile u8 menu_cursor = 0;
__xdata volatile u8 menu_in_edit = MENU_EDIT_NONE;
__xdata volatile u8 menu_display_dirty = 0;
__xdata volatile u16 menu_edit_value = 0;

// Menu item definitions (stored in ROM to save RAM)
const __code menu_item_t menu_items[MENU_COUNT] = {
    {MENU_FREQUENCY,   "Frequency",   MENU_TYPE_NUMERIC, FREQ_MIN,      FREQ_MAX,      FREQ_STEP,      SETTING_FREQUENCY_ADDR},
    {MENU_VOLUME,      "Volume",      MENU_TYPE_NUMERIC, VOLUME_MIN,    VOLUME_MAX,    VOLUME_STEP,    SETTING_VOLUME_ADDR},
    {MENU_SQUELCH,     "Squelch",     MENU_TYPE_NUMERIC, SQUELCH_MIN,   SQUELCH_MAX,   SQUELCH_STEP,   SETTING_SQUELCH_ADDR},
    {MENU_POWER,       "Power",       MENU_TYPE_NUMERIC, POWER_MIN,     POWER_MAX,     POWER_STEP,     SETTING_POWER_ADDR},
    {MENU_BACKLIGHT,   "Backlight",   MENU_TYPE_NUMERIC, BACKLIGHT_MIN, BACKLIGHT_MAX, BACKLIGHT_STEP, SETTING_BACKLIGHT_ADDR},
    {MENU_CTCSS,       "CTCSS",       MENU_TYPE_NUMERIC, CTCSS_MIN,     CTCSS_MAX,     CTCSS_STEP,     SETTING_CTCSS_ADDR},
    {MENU_BATTERY_INFO,"Battery",     MENU_TYPE_INFO,    0,             0,             0,              0},
    {MENU_VERSION,     "Version",     MENU_TYPE_INFO,    0,             0,             0,              0},
    {MENU_EXIT,        "Exit Menu",   MENU_TYPE_ACTION,  0,             0,             0,              0}
};

// Menu initialization
void menu_init(void) {
    menu_mode = MENU_MODE_NORMAL;
    menu_cursor = 0;
    menu_in_edit = MENU_EDIT_NONE;
    menu_display_dirty = 0;
    menu_edit_value = 0;
}

// Enter menu system
void menu_enter(void) {
    menu_mode = MENU_MODE_MENU;
    menu_cursor = 0;
    menu_in_edit = MENU_EDIT_NONE;
    menu_display_dirty = 1;
    menu_clear_screen();
    send_uart_message("Menu entered");
}

// Exit menu system
void menu_exit(void) {
    menu_mode = MENU_MODE_NORMAL;
    menu_in_edit = MENU_EDIT_NONE;
    menu_display_dirty = 1;
    menu_clear_screen();
    send_uart_message("Menu exited");
}

// Process key input when in menu mode
void menu_process_key(u8 key) {
    if (key == 0) return;

    switch (key) {
        case KEY_MENU:
            if (menu_in_edit == MENU_EDIT_NONE) {
                menu_exit();
            } else {
                menu_end_edit(0); // Cancel edit without saving
            }
            break;

        case KEY_EXIT:
            if (menu_in_edit == MENU_EDIT_NONE) {
                menu_exit();
            } else {
                menu_end_edit(0); // Cancel edit without saving
            }
            break;

        case KEY_UP:
            if (menu_in_edit == MENU_EDIT_NONE) {
                menu_navigate_up();
            } else {
                menu_edit_increment();
            }
            break;

        case KEY_DOWN:
            if (menu_in_edit == MENU_EDIT_NONE) {
                menu_navigate_down();
            } else {
                menu_edit_decrement();
            }
            break;

        case KEY_1:
        case KEY_2:
        case KEY_3:
        case KEY_4:
        case KEY_5:
        case KEY_6:
        case KEY_7:
        case KEY_8:
        case KEY_9:
        case KEY_0:
            if (menu_in_edit == MENU_EDIT_NONE) {
                // Start editing for numeric menu items
                const menu_item_t* item = menu_get_current_item();
                if (item->type == MENU_TYPE_NUMERIC) {
                    menu_start_edit();
                }
            }
            if (menu_in_edit == MENU_EDIT_VALUE) {
                menu_edit_digit_input(key);
            }
            break;

        case KEY_STAR:
            if (menu_in_edit == MENU_EDIT_NONE) {
                // Enter edit mode or execute action
                const menu_item_t* item = menu_get_current_item();
                if (item->type == MENU_TYPE_NUMERIC) {
                    menu_start_edit();
                } else if (item->type == MENU_TYPE_ACTION && item->id == MENU_EXIT) {
                    menu_exit();
                }
            } else {
                menu_end_edit(1); // Save changes
            }
            break;

        case KEY_HASH:
            if (menu_in_edit == MENU_EDIT_VALUE) {
                menu_end_edit(1); // Save changes
            }
            break;

        default:
            // Ignore other keys
            break;
    }

    // Update display if needed
    if (menu_display_dirty) {
        menu_update_display();
    }
}

// Navigate to previous menu item
void menu_navigate_up(void) {
    if (menu_cursor > MENU_MIN_CURSOR) {
        menu_cursor--;
    } else {
        menu_cursor = MENU_MAX_CURSOR; // Wrap around
    }
    menu_display_dirty = 1;
}

// Navigate to next menu item
void menu_navigate_down(void) {
    if (menu_cursor < MENU_MAX_CURSOR) {
        menu_cursor++;
    } else {
        menu_cursor = MENU_MIN_CURSOR; // Wrap around
    }
    menu_display_dirty = 1;
}

// Start editing current menu item value
void menu_start_edit(void) {
    const menu_item_t* item = menu_get_current_item();
    if (item->type == MENU_TYPE_NUMERIC) {
        menu_edit_value = menu_get_current_value();
        menu_in_edit = MENU_EDIT_VALUE;
        menu_display_dirty = 1;
        send_uart_message("Started editing");
    }
}

// End editing current menu item value
void menu_end_edit(__bit save) {
    if (menu_in_edit == MENU_EDIT_VALUE) {
        if (save) {
            menu_set_current_value(menu_edit_value);
            send_uart_message("Changes saved");
        } else {
            send_uart_message("Changes cancelled");
        }
        menu_in_edit = MENU_EDIT_NONE;
        menu_display_dirty = 1;
    }
}

// Increment current edit value
void menu_edit_increment(void) {
    const menu_item_t* item = menu_get_current_item();
    if (menu_edit_value + item->step <= item->max_value) {
        menu_edit_value += item->step;
        menu_display_dirty = 1;
    }
}

// Decrement current edit value
void menu_edit_decrement(void) {
    const menu_item_t* item = menu_get_current_item();
    if (menu_edit_value >= item->min_value + item->step) {
        menu_edit_value -= item->step;
        menu_display_dirty = 1;
    }
}

// Handle digit input during value editing
void menu_edit_digit_input(u8 digit) {
    const menu_item_t* item = menu_get_current_item();
    u32 new_value;
    
    // Convert key code to actual digit
    u8 actual_digit;
    if (digit >= KEY_1 && digit <= KEY_9) {
        actual_digit = digit; // KEY_1 = 1, KEY_2 = 2, etc.
    } else if (digit == KEY_0) {
        actual_digit = 0;
    } else {
        return; // Invalid digit
    }
    
    // Shift current value and add new digit
    new_value = (u32)menu_edit_value * 10 + actual_digit;
    
    // Validate range
    if (new_value >= item->min_value && new_value <= item->max_value) {
        menu_edit_value = (u16)new_value;
        menu_display_dirty = 1;
    }
}

// Get current menu item value from settings
u16 menu_get_current_value(void) {
    const menu_item_t* item = menu_get_current_item();
    
    switch (item->id) {
        case MENU_FREQUENCY:
            return settings_get_frequency();
        case MENU_VOLUME:
            return (u16)settings_get_volume();
        case MENU_SQUELCH:
            return (u16)settings_get_squelch();
        case MENU_POWER:
            return (u16)settings_get_power();
        case MENU_BACKLIGHT:
            return (u16)settings_get_backlight();
        case MENU_CTCSS:
            return (u16)settings_get_ctcss();
        default:
            return 0;
    }
}

// Set current menu item value in settings
void menu_set_current_value(u16 value) {
    const menu_item_t* item = menu_get_current_item();
    
    switch (item->id) {
        case MENU_FREQUENCY:
            settings_set_frequency(value);
            menu_apply_setting(item->id, value);
            break;
        case MENU_VOLUME:
            settings_set_volume((u8)value);
            menu_apply_setting(item->id, value);
            break;
        case MENU_SQUELCH:
            settings_set_squelch((u8)value);
            menu_apply_setting(item->id, value);
            break;
        case MENU_POWER:
            settings_set_power((u8)value);
            menu_apply_setting(item->id, value);
            break;
        case MENU_BACKLIGHT:
            settings_set_backlight((u8)value);
            menu_apply_setting(item->id, value);
            break;
        case MENU_CTCSS:
            settings_set_ctcss((u8)value);
            menu_apply_setting(item->id, value);
            break;
    }
    
    // Save settings to EEPROM immediately
    settings_save();
}

// Apply setting to hardware
void menu_apply_setting(u8 menu_id, u16 value) {
    switch (menu_id) {
        case MENU_FREQUENCY:
            at1846s_set_frequency((u32)value);
            break;
        case MENU_VOLUME:
            at1846s_set_volume((u8)value);
            break;
        case MENU_SQUELCH:
            at1846s_set_squelch((u8)value);
            break;
        case MENU_POWER:
            // Power setting would be applied to PA control
            break;
        case MENU_BACKLIGHT:
            // Backlight timeout would be applied to timer
            break;
        case MENU_CTCSS:
            at1846s_set_ctcss_tx((u16)value);
            break;
    }
}

// Get pointer to current menu item
const menu_item_t* menu_get_current_item(void) {
    return &menu_items[menu_cursor];
}

// Update display when needed
void menu_update_display(void) {
    if (menu_display_dirty) {
        menu_render_title();
        menu_render_item_name();
        menu_render_value();
        menu_render_navigation();
        menu_display_dirty = 0;
    }
}

// Render menu title
void menu_render_title(void) {
    render_16x16_string(MENU_TEXT_X, MENU_TITLE_Y, "MENU");
}

// Render current menu item name
void menu_render_item_name(void) {
    const menu_item_t* item = menu_get_current_item();
    clear_area(0, MENU_ITEM_Y, DISPLAY_WIDTH - 1, MENU_ITEM_Y + 15);
    render_16x16_string(MENU_TEXT_X, MENU_ITEM_Y, item->name);
}

// Render current menu item value
void menu_render_value(void) {
    const menu_item_t* item = menu_get_current_item();
    u16 display_value;
    
    clear_area(0, MENU_VALUE_Y, DISPLAY_WIDTH - 1, MENU_VALUE_Y + 15);
    
    if (item->type == MENU_TYPE_NUMERIC) {
        if (menu_in_edit == MENU_EDIT_VALUE) {
            display_value = menu_edit_value;
            render_16x16_string(0, MENU_VALUE_Y, ">");  // Edit indicator
        } else {
            display_value = menu_get_current_value();
        }
        
        // Special formatting for frequency
        if (item->id == MENU_FREQUENCY) {
            // Display frequency as XXX.XXX MHz
            u16 mhz_part = display_value / 1000;
            u16 khz_part = display_value % 1000;
            render_16x16_number(MENU_TEXT_X + 16, MENU_VALUE_Y, mhz_part);
            render_16x16_string(MENU_TEXT_X + 64, MENU_VALUE_Y, ".");
            render_16x16_number(MENU_TEXT_X + 80, MENU_VALUE_Y, khz_part);
        } else {
            render_16x16_number(MENU_TEXT_X + 16, MENU_VALUE_Y, display_value);
        }
    } else if (item->type == MENU_TYPE_INFO) {
        if (item->id == MENU_BATTERY_INFO) {
            u16 battery = at1846s_get_battery_voltage();
            render_16x16_number(MENU_TEXT_X + 16, MENU_VALUE_Y, battery);
            render_16x16_string(MENU_TEXT_X + 64, MENU_VALUE_Y, "mV");
        } else if (item->id == MENU_VERSION) {
            render_16x16_string(MENU_TEXT_X + 16, MENU_VALUE_Y, "v1.0");
        }
    } else if (item->type == MENU_TYPE_ACTION) {
        render_16x16_string(MENU_TEXT_X + 16, MENU_VALUE_Y, "Press *");
    }
}

// Render navigation indicators
void menu_render_navigation(void) {
    clear_area(0, MENU_NAV_Y, DISPLAY_WIDTH - 1, MENU_NAV_Y + 15);
    
    if (menu_in_edit == MENU_EDIT_NONE) {
        render_16x16_string(MENU_TEXT_X, MENU_NAV_Y, "UP/DN * EXIT");
    } else {
        render_16x16_string(MENU_TEXT_X, MENU_NAV_Y, "UP/DN # EXIT");
    }
}

// Clear entire screen
void menu_clear_screen(void) {
    clear_area(0, 0, DISPLAY_WIDTH - 1, DISPLAY_HEIGHT - 1);
}