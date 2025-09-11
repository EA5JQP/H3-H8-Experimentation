#include "menu.h"
#include "uart.h"
#include "uart_test.h"
#include "at1846s.h"

/**
 * Global menu state variables
 * Stored in external RAM (__xdata) to conserve internal 8051 memory
 * Volatile to ensure proper access in interrupt contexts
 */
__xdata volatile u8 menu_mode = MENU_MODE_NORMAL;      // Current menu system mode (normal/menu)
__xdata volatile u8 menu_cursor = 0;                   // Current menu item cursor position
__xdata volatile u8 menu_edit_state = MENU_EDIT_NONE;  // Current edit state (none/value editing)
__xdata volatile u8 menu_display_dirty = 0;           // Flag indicating display needs refresh
__xdata volatile u16 menu_edit_value = 0;              // Current value being edited

/**
 * Menu item definitions (stored in ROM to save RAM)
 * Each menu item contains:
 * - id: Unique menu identifier (MENU_* constants)
 * - name: Display string (stored in ROM)
 * - type: Item behavior (numeric/choice/action/info)
 * - min_value/max_value/step: Value constraints for numeric items
 * - eeprom_addr: Storage location for persistent settings
 */
const __code menu_item_t menu_items[MENU_COUNT] = {
    {MENU_FREQUENCY,    "Frequency",   MENU_TYPE_NUMERIC, FREQ_MIN,         FREQ_MAX,         FREQ_STEP,         SETTING_FREQUENCY_ADDR},
    {MENU_VOLUME,       "Volume",      MENU_TYPE_NUMERIC, VOLUME_MIN,       VOLUME_MAX,       VOLUME_STEP,       SETTING_VOLUME_ADDR},
    {MENU_SQUELCH,      "Squelch",     MENU_TYPE_NUMERIC, SQUELCH_MIN,      SQUELCH_MAX,      SQUELCH_STEP,      SETTING_SQUELCH_ADDR},
    {MENU_TX_POWER,     "TX Power",    MENU_TYPE_CHOICE,  TX_POWER_MIN,     TX_POWER_MAX,     TX_POWER_STEP,     SETTING_TX_POWER_ADDR},
    {MENU_BANDWIDTH,    "Bandwidth",   MENU_TYPE_CHOICE,  BANDWIDTH_MIN,    BANDWIDTH_MAX,    BANDWIDTH_STEP,    SETTING_BANDWIDTH_ADDR},
    {MENU_CTCSS_TX,     "CTCSS TX",    MENU_TYPE_NUMERIC, CTCSS_MIN,        CTCSS_MAX,        CTCSS_STEP,        SETTING_CTCSS_TX_ADDR},
    {MENU_CTCSS_RX,     "CTCSS RX",    MENU_TYPE_NUMERIC, CTCSS_MIN,        CTCSS_MAX,        CTCSS_STEP,        SETTING_CTCSS_RX_ADDR},
    {MENU_DCS_TX,       "DCS TX",      MENU_TYPE_NUMERIC, DCS_MIN,          DCS_MAX,          DCS_STEP,          SETTING_DCS_TX_ADDR},
    {MENU_DCS_RX,       "DCS RX",      MENU_TYPE_NUMERIC, DCS_MIN,          DCS_MAX,          DCS_STEP,          SETTING_DCS_RX_ADDR},
    {MENU_VOX,          "VOX Level",   MENU_TYPE_NUMERIC, VOX_MIN,          VOX_MAX,          VOX_STEP,          SETTING_VOX_ADDR},
    {MENU_MIC_GAIN,     "Mic Gain",    MENU_TYPE_NUMERIC, MIC_GAIN_MIN,     MIC_GAIN_MAX,     MIC_GAIN_STEP,     SETTING_MIC_GAIN_ADDR},
    {MENU_COMPANDER,    "Compander",   MENU_TYPE_CHOICE,  COMPANDER_MIN,    COMPANDER_MAX,    COMPANDER_STEP,    SETTING_COMPANDER_ADDR},
    {MENU_PRE_EMPHASIS, "Pre-emph",    MENU_TYPE_CHOICE,  PRE_EMPHASIS_MIN, PRE_EMPHASIS_MAX, PRE_EMPHASIS_STEP, SETTING_PRE_EMPHASIS_ADDR},
    {MENU_TAIL_ELIM,    "Tail Elim",   MENU_TYPE_CHOICE,  TAIL_ELIM_MIN,    TAIL_ELIM_MAX,    TAIL_ELIM_STEP,    SETTING_TAIL_ELIM_ADDR},
    {MENU_SCRAMBLER,    "Scrambler",   MENU_TYPE_CHOICE,  SCRAMBLER_MIN,    SCRAMBLER_MAX,    SCRAMBLER_STEP,    SETTING_SCRAMBLER_ADDR},
    {MENU_BATTERY_INFO, "Battery",     MENU_TYPE_INFO,    0,                0,                0,                 0},
    {MENU_RSSI_INFO,    "RSSI",        MENU_TYPE_INFO,    0,                0,                0,                 0},
    {MENU_VERSION,      "Version",     MENU_TYPE_INFO,    0,                0,                0,                 0}
};

/**
 * Initialize menu system to default state
 * Resets all state variables and prepares for normal operation
 */
void menu_init(void) {
    menu_mode = MENU_MODE_NORMAL;
    menu_cursor = 0;
    menu_edit_state = MENU_EDIT_NONE;
    menu_display_dirty = 0;
    menu_edit_value = 0;
}

/**
 * Enter menu system from normal operation mode
 * Switches to menu mode, resets cursor, and triggers display update
 */
void menu_enter(void) {
    menu_mode = MENU_MODE_MENU;
    menu_cursor = 0;
    menu_edit_state = MENU_EDIT_NONE;
    menu_display_dirty = 1;
    menu_clear_screen();
    send_uart_message("Menu entered");
}

/**
 * Exit menu system and return to normal operation mode
 * Cancels any active editing and triggers display update
 */
void menu_exit(void) {
    menu_mode = MENU_MODE_NORMAL;
    menu_edit_state = MENU_EDIT_NONE;
    menu_display_dirty = 1;
    menu_clear_screen();
    send_uart_message("Menu exited");
}

/**
 * Process key input when in menu mode
 * Handles navigation, editing, and menu actions based on current state
 * Optimized to call menu_get_current_item() only once per invocation
 * @param key: Key code from keypad scanner
 */
void menu_process_key(u8 key) {
    if (key == 0) return;

    // Get current item once and reuse throughout function
    const menu_item_t* current_item = menu_get_current_item();

    switch (key) {
        case KEY_MENU:
            if (menu_edit_state == MENU_EDIT_NONE) {
                menu_exit();
            } else {
                menu_end_edit(0); // Cancel edit without saving
            }
            break;

        case KEY_EXIT:
            if (menu_edit_state == MENU_EDIT_NONE) {
                menu_exit();
            } else {
                menu_end_edit(0); // Cancel edit without saving
            }
            break;

        case KEY_UP:
            if (menu_edit_state == MENU_EDIT_NONE) {
                menu_navigate_up();
            } else {
                menu_edit_increment();
            }
            break;

        case KEY_DOWN:
            if (menu_edit_state == MENU_EDIT_NONE) {
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
            if (menu_edit_state == MENU_EDIT_NONE) {
                // Start editing for numeric menu items
                if (current_item->type == MENU_TYPE_NUMERIC) {
                    menu_start_edit();
                }
            }
            if (menu_edit_state == MENU_EDIT_VALUE) {
                menu_edit_digit_input(key);
            }
            break;

        case KEY_STAR:
            if (menu_edit_state == MENU_EDIT_NONE) {
                // Enter edit mode for numeric items
                if (current_item->type == MENU_TYPE_NUMERIC) {
                    menu_start_edit();
                }
            } else {
                menu_end_edit(1); // Save changes
            }
            break;

        case KEY_HASH:
            if (menu_edit_state == MENU_EDIT_VALUE) {
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

/**
 * Navigate to previous menu item
 * Implements circular navigation (wraps from first to last)
 * Sets display dirty flag to trigger UI refresh
 */
void menu_navigate_up(void) {
    if (menu_cursor > MENU_MIN_CURSOR) {
        menu_cursor--;
    } else {
        menu_cursor = MENU_MAX_CURSOR; // Wrap around
    }
    menu_display_dirty = 1;
}

/**
 * Navigate to next menu item  
 * Implements circular navigation (wraps from last to first)
 * Sets display dirty flag to trigger UI refresh
 */
void menu_navigate_down(void) {
    if (menu_cursor < MENU_MAX_CURSOR) {
        menu_cursor++;
    } else {
        menu_cursor = MENU_MIN_CURSOR; // Wrap around
    }
    menu_display_dirty = 1;
}

/**
 * Start editing current menu item value
 * Only works for numeric menu items
 * Loads current setting value into edit buffer
 */
void menu_start_edit(void) {
    const menu_item_t* item = menu_get_current_item();
    if (item->type == MENU_TYPE_NUMERIC) {
        menu_edit_value = menu_get_current_value();
        menu_edit_state = MENU_EDIT_VALUE;
        menu_display_dirty = 1;
        send_uart_message("Started editing");
    }
}

/**
 * End editing current menu item value
 * @param save: If true, saves changes; if false, discards changes
 * Returns to navigation mode and triggers display update
 */
void menu_end_edit(__bit save) {
    if (menu_edit_state == MENU_EDIT_VALUE) {
        if (save) {
            menu_set_current_value(menu_edit_value);
            send_uart_message("Changes saved");
        } else {
            send_uart_message("Changes cancelled");
        }
        menu_edit_state = MENU_EDIT_NONE;
        menu_display_dirty = 1;
    }
}

/**
 * Increment current edit value by configured step amount
 * Respects maximum value constraint from menu item definition
 * Triggers display update to show new value
 */
void menu_edit_increment(void) {
    const menu_item_t* item = menu_get_current_item();
    if (menu_edit_value + item->step <= item->max_value) {
        menu_edit_value += item->step;
        menu_display_dirty = 1;
    }
}

/**
 * Decrement current edit value by configured step amount
 * Respects minimum value constraint from menu item definition
 * Triggers display update to show new value
 */
void menu_edit_decrement(void) {
    const menu_item_t* item = menu_get_current_item();
    if (menu_edit_value >= item->min_value + item->step) {
        menu_edit_value -= item->step;
        menu_display_dirty = 1;
    }
}

/**
 * Handle digit input during value editing
 * Converts keypad input to numeric digits and builds new value
 * Validates against menu item min/max constraints
 * @param digit: Key code representing digit (KEY_0 through KEY_9)
 */
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

/**
 * Individual getter functions for each setting
 * These functions provide a consistent interface for reading setting values
 * Used by the function pointer system to eliminate switch statements
 */
u16 menu_get_frequency(void) { 
    return settings_get_frequency(); 
}

u16 menu_get_volume(void) { 
    return (u16)settings_get_volume(); 
}

u16 menu_get_squelch(void) { 
    return (u16)settings_get_squelch(); 
}

u16 menu_get_tx_power(void) { 
    return (u16)settings_get_tx_power(); 
}

u16 menu_get_bandwidth(void) { 
    return (u16)settings_get_bandwidth(); 
}

u16 menu_get_ctcss_tx(void) { 
    return (u16)settings_get_ctcss_tx(); 
}

u16 menu_get_ctcss_rx(void) { 
    return (u16)settings_get_ctcss_rx(); 
}

u16 menu_get_dcs_tx(void) { 
    return (u16)settings_get_dcs_tx(); 
}

u16 menu_get_dcs_rx(void) { 
    return (u16)settings_get_dcs_rx(); 
}

u16 menu_get_vox(void) { 
    return (u16)settings_get_vox(); 
}

u16 menu_get_mic_gain(void) { 
    return (u16)settings_get_mic_gain(); 
}

u16 menu_get_compander(void) { 
    return (u16)settings_get_compander(); 
}

u16 menu_get_pre_emphasis(void) { 
    return (u16)settings_get_pre_emphasis(); 
}

u16 menu_get_tail_elim(void) { 
    return (u16)settings_get_tail_elim(); 
}

u16 menu_get_scrambler(void) { 
    return (u16)settings_get_scrambler(); 
}

/**
 * Individual setter functions for each setting
 * These functions provide a consistent interface for writing setting values
 * Each function calls the appropriate hardware application function
 * Used by the function pointer system to eliminate switch statements
 */
void menu_set_frequency(u16 value) { 
    settings_set_frequency(value); 
    menu_apply_setting(MENU_FREQUENCY, value); 
}

void menu_set_volume(u16 value) { 
    settings_set_volume((u8)value); 
    menu_apply_setting(MENU_VOLUME, value); 
}

void menu_set_squelch(u16 value) { 
    settings_set_squelch((u8)value); 
    menu_apply_setting(MENU_SQUELCH, value); 
}

void menu_set_tx_power(u16 value) { 
    settings_set_tx_power((u8)value); 
    menu_apply_setting(MENU_TX_POWER, value); 
}

void menu_set_bandwidth(u16 value) { 
    settings_set_bandwidth((u8)value); 
    menu_apply_setting(MENU_BANDWIDTH, value); 
}

void menu_set_ctcss_tx(u16 value) { 
    settings_set_ctcss_tx((u8)value); 
    menu_apply_setting(MENU_CTCSS_TX, value); 
}

void menu_set_ctcss_rx(u16 value) { 
    settings_set_ctcss_rx((u8)value); 
    menu_apply_setting(MENU_CTCSS_RX, value); 
}

void menu_set_dcs_tx(u16 value) { 
    settings_set_dcs_tx((u8)value); 
    menu_apply_setting(MENU_DCS_TX, value); 
}

void menu_set_dcs_rx(u16 value) { 
    settings_set_dcs_rx((u8)value); 
    menu_apply_setting(MENU_DCS_RX, value); 
}

void menu_set_vox(u16 value) { 
    settings_set_vox((u8)value); 
    menu_apply_setting(MENU_VOX, value); 
}

void menu_set_mic_gain(u16 value) { 
    settings_set_mic_gain((u8)value); 
    menu_apply_setting(MENU_MIC_GAIN, value); 
}

void menu_set_compander(u16 value) { 
    settings_set_compander((u8)value); 
    menu_apply_setting(MENU_COMPANDER, value); 
}

void menu_set_pre_emphasis(u16 value) { 
    settings_set_pre_emphasis((u8)value); 
    menu_apply_setting(MENU_PRE_EMPHASIS, value); 
}

void menu_set_tail_elim(u16 value) { 
    settings_set_tail_elim((u8)value); 
    menu_apply_setting(MENU_TAIL_ELIM, value); 
}

void menu_set_scrambler(u16 value) { 
    settings_set_scrambler((u8)value); 
    menu_apply_setting(MENU_SCRAMBLER, value); 
}

/**
 * Function pointer array for menu value operations (stored in ROM)
 * This array eliminates switch statements and provides O(1) access to get/set functions
 * Indexed by menu item ID for efficient lookup
 * Non-settable items (info, action) have NULL pointers
 */
const __code menu_value_ops_t menu_value_ops[MENU_COUNT] = {
    {menu_get_frequency,    menu_set_frequency},    // MENU_FREQUENCY
    {menu_get_volume,       menu_set_volume},       // MENU_VOLUME
    {menu_get_squelch,      menu_set_squelch},      // MENU_SQUELCH
    {menu_get_tx_power,     menu_set_tx_power},     // MENU_TX_POWER
    {menu_get_bandwidth,    menu_set_bandwidth},    // MENU_BANDWIDTH
    {menu_get_ctcss_tx,     menu_set_ctcss_tx},     // MENU_CTCSS_TX
    {menu_get_ctcss_rx,     menu_set_ctcss_rx},     // MENU_CTCSS_RX
    {menu_get_dcs_tx,       menu_set_dcs_tx},       // MENU_DCS_TX
    {menu_get_dcs_rx,       menu_set_dcs_rx},       // MENU_DCS_RX
    {menu_get_vox,          menu_set_vox},          // MENU_VOX
    {menu_get_mic_gain,     menu_set_mic_gain},     // MENU_MIC_GAIN
    {menu_get_compander,    menu_set_compander},    // MENU_COMPANDER
    {menu_get_pre_emphasis, menu_set_pre_emphasis}, // MENU_PRE_EMPHASIS
    {menu_get_tail_elim,    menu_set_tail_elim},    // MENU_TAIL_ELIM
    {menu_get_scrambler,    menu_set_scrambler},    // MENU_SCRAMBLER
    {0, 0},                                         // MENU_BATTERY_INFO (info type)
    {0, 0},                                         // MENU_RSSI_INFO (info type)
    {0, 0}                                          // MENU_VERSION (info type)
};

/**
 * Get current menu item value from settings using function pointers
 * Optimized approach using function pointer array instead of switch statement
 * @return Current value of the selected menu item, or 0 if not applicable
 */
u16 menu_get_current_value(void) {
    const menu_value_ops_t* ops = &menu_value_ops[menu_cursor];
    if (ops->get_func) {
        return ops->get_func();
    }
    return 0;
}

/**
 * Set current menu item value in settings using function pointers
 * Optimized approach using function pointer array instead of switch statement
 * Automatically saves to EEPROM after successful update
 * @param value: New value to set for current menu item
 */
void menu_set_current_value(u16 value) {
    const menu_value_ops_t* ops = &menu_value_ops[menu_cursor];
    if (ops->set_func) {
        ops->set_func(value);
        // Save settings to EEPROM immediately after successful set
        settings_save();
    }
}

/**
 * Apply setting to hardware immediately after value change
 * Translates menu setting changes to actual hardware configuration
 * @param menu_id: Menu item identifier
 * @param value: New value to apply to hardware
 */
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
        case MENU_TX_POWER:
            // Power setting - map to AT1846S power control
            // TODO: Implement at1846s_set_power_level() when available
            break;
        case MENU_BANDWIDTH:
            // Bandwidth setting - TODO: Implement when available
            break;
        case MENU_CTCSS_TX:
            // CTCSS TX setting - use existing function
            at1846s_set_ctcss_tx((u16)value);
            break;
        case MENU_CTCSS_RX:
            // CTCSS RX setting - TODO: Implement separate RX function
            break;
        case MENU_DCS_TX:
            // DCS TX setting - TODO: Implement when available
            break;
        case MENU_DCS_RX:
            // DCS RX setting - TODO: Implement when available
            break;
        case MENU_VOX:
            // VOX setting - TODO: Implement when available
            break;
        case MENU_MIC_GAIN:
            // Mic gain setting - TODO: Implement when available
            break;
        case MENU_COMPANDER:
            // Compander setting - TODO: Implement when available
            break;
        case MENU_PRE_EMPHASIS:
            // Pre-emphasis setting - TODO: Implement when available
            break;
        case MENU_TAIL_ELIM:
            // Tail eliminator setting - TODO: Implement when available
            break;
        case MENU_SCRAMBLER:
            // Scrambler setting - TODO: Implement when available
            break;
    }
}

/**
 * Get pointer to current menu item based on cursor position
 * Provides access to menu item metadata (name, type, constraints)
 * @return Pointer to current menu item structure in ROM
 */
const menu_item_t* menu_get_current_item(void) {
    return &menu_items[menu_cursor];
}

/**
 * Update display when dirty flag is set
 * Centralized display update function that refreshes all menu components
 * Clears screen first to prevent display artifacts, then clears dirty flag
 */
void menu_update_display(void) {
    if (menu_display_dirty) {
        menu_clear_screen();  // Clear screen before rendering new content
        menu_render_title();
        menu_render_item_name();
        menu_render_value();
        menu_render_navigation();
        menu_display_dirty = 0;
    }
}

/**
 * Render menu title at top of display
 * Shows "MENU" text to indicate menu mode is active
 */
void menu_render_title(void) {
    render_16x16_string(MENU_TEXT_X, MENU_TITLE_Y, "MENU");
}

/**
 * Render current menu item name
 * Displays name of currently selected item
 */
void menu_render_item_name(void) {
    const menu_item_t* item = menu_get_current_item();
    render_16x16_string(MENU_TEXT_X, MENU_ITEM_Y, item->name);
}

/**
 * Render current menu item value with appropriate formatting
 * Handles different item types (numeric, choice, info)
 * Shows edit indicator (">") when in edit mode
 * Special formatting for frequency, choice options, and info displays
 */
void menu_render_value(void) {
    const menu_item_t* item = menu_get_current_item();
    u16 display_value;
    
    if (item->type == MENU_TYPE_NUMERIC || item->type == MENU_TYPE_CHOICE) {
        if (menu_edit_state == MENU_EDIT_VALUE) {
            display_value = menu_edit_value;
            render_16x16_string(0, MENU_VALUE_Y, ">");  // Edit indicator
        } else {
            display_value = menu_get_current_value();
        }
        
        // Handle different display formats
        if (item->type == MENU_TYPE_CHOICE) {
            // Display choice options as text
            if (item->id == MENU_TX_POWER) {
                switch (display_value) {
                    case 0: render_16x16_string(MENU_TEXT_X + 16, MENU_VALUE_Y, "Low"); break;
                    case 1: render_16x16_string(MENU_TEXT_X + 16, MENU_VALUE_Y, "Mid1"); break;
                    case 2: render_16x16_string(MENU_TEXT_X + 16, MENU_VALUE_Y, "Mid2"); break;
                    case 3: render_16x16_string(MENU_TEXT_X + 16, MENU_VALUE_Y, "High"); break;
                }
            } else if (item->id == MENU_BANDWIDTH) {
                render_16x16_string(MENU_TEXT_X + 16, MENU_VALUE_Y, display_value ? "Wide" : "Narrow");
            } else {
                // Binary choices (On/Off)
                render_16x16_string(MENU_TEXT_X + 16, MENU_VALUE_Y, display_value ? "On" : "Off");
            }
        } else if (item->id == MENU_FREQUENCY) {
            // Display frequency as XXX.XXX MHz
            u16 mhz_part = display_value / 1000;
            u16 khz_part = display_value % 1000;
            render_16x16_number(MENU_TEXT_X + 16, MENU_VALUE_Y, mhz_part);
            render_16x16_string(MENU_TEXT_X + 64, MENU_VALUE_Y, ".");
            render_16x16_number(MENU_TEXT_X + 80, MENU_VALUE_Y, khz_part);
        } else if (item->id == MENU_CTCSS_TX || item->id == MENU_CTCSS_RX) {
            // Display CTCSS tone (0 = Off, 1-38 = tone number)
            if (display_value == 0) {
                render_16x16_string(MENU_TEXT_X + 16, MENU_VALUE_Y, "Off");
            } else {
                render_16x16_number(MENU_TEXT_X + 16, MENU_VALUE_Y, display_value);
            }
        } else if (item->id == MENU_DCS_TX || item->id == MENU_DCS_RX) {
            // Display DCS code (0 = Off, 1-83 = code number)
            if (display_value == 0) {
                render_16x16_string(MENU_TEXT_X + 16, MENU_VALUE_Y, "Off");
            } else {
                render_16x16_number(MENU_TEXT_X + 16, MENU_VALUE_Y, display_value);
            }
        } else if (item->id == MENU_VOX) {
            // Display VOX level (0 = Off, 1-8 = sensitivity)
            if (display_value == 0) {
                render_16x16_string(MENU_TEXT_X + 16, MENU_VALUE_Y, "Off");
            } else {
                render_16x16_number(MENU_TEXT_X + 16, MENU_VALUE_Y, display_value);
            }
        } else {
            // Default numeric display
            render_16x16_number(MENU_TEXT_X + 16, MENU_VALUE_Y, display_value);
        }
    } else if (item->type == MENU_TYPE_INFO) {
        // Handle info displays
        if (item->id == MENU_BATTERY_INFO) {
            u16 battery = at1846s_get_battery_voltage();
            render_16x16_number(MENU_TEXT_X + 16, MENU_VALUE_Y, battery);
            render_16x16_string(MENU_TEXT_X + 64, MENU_VALUE_Y, "mV");
        } else if (item->id == MENU_RSSI_INFO) {
            u16 rssi = at1846s_get_rssi();
            render_16x16_number(MENU_TEXT_X + 16, MENU_VALUE_Y, rssi);
            render_16x16_string(MENU_TEXT_X + 64, MENU_VALUE_Y, "dBm");
        } else if (item->id == MENU_VERSION) {
            render_16x16_string(MENU_TEXT_X + 16, MENU_VALUE_Y, "v1.0");
        }
    }
}

/**
 * Render navigation indicators at bottom of display
 * Shows available key actions based on current menu state
 * Different indicators for navigation mode vs. edit mode
 */
void menu_render_navigation(void) {
    if (menu_edit_state == MENU_EDIT_NONE) {
        render_16x16_string(MENU_TEXT_X, MENU_NAV_Y, "UP/DN * EXIT");
    } else {
        render_16x16_string(MENU_TEXT_X, MENU_NAV_Y, "UP/DN # EXIT");
    }
}

/**
 * Clear entire screen for menu display
 * Uses working LCD pattern structure for reliable clearing
 */
void menu_clear_screen(void) {
    u8 row, col;
    
    // Use same structure as working background pattern
    lcd_set_window(0, 0, 159, 127);  // Full screen
    
    for (row = 0; row < 128; row++) {
        for (col = 0; col < 160; col++) {
            lcd_send_data(0x00);  // Black background
            lcd_send_data(0x00);
        }
    }
}