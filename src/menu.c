#include "menu.h"
#include "uart.h"
#include "uart_test.h"
#include "at1846s.h"

/**
 * Global menu state variables
 * Stored in external RAM (__xdata) to conserve internal 8051 memory
 * Volatile to ensure proper access in interrupt contexts
 */
__xdata volatile u8 menu_mode = MENU_MODE_NORMAL;      // Current menu system mode (normal/menu/item)
__xdata volatile u8 menu_cursor = 0;                   // Current menu item cursor position
__xdata volatile u8 menu_edit_state = MENU_EDIT_NONE;  // Current edit state (none/value editing)
__xdata volatile u8 menu_display_dirty = 0;           // Flag indicating display needs refresh
__xdata volatile u16 menu_edit_value = 0;              // Current value being edited
__xdata volatile u8 menu_last_cursor = 0xFF;           // Last cursor position for partial updates
__xdata volatile u8 menu_current_start_item = 0;       // Current scroll window start position

// Item detail screen state
__xdata volatile u8 menu_item_cursor = 0;              // Cursor position within item detail screen
__xdata volatile u8 menu_item_selection = 0;           // Current selection for choice items
__xdata volatile u16 menu_item_temp_value = 0;         // Temporary value during editing

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
    // Core radio settings
    {0,  "FREQUENCY",      MENU_TYPE_NUMERIC, 136000,  520000,  25,    0x0110},  // 136-520 MHz, 25kHz step
    {1,  "STEP",           MENU_TYPE_NUMERIC, 1,       50000,   2,     0x0200},  // 1-50000 step 2
    {2,  "SQUELCH",        MENU_TYPE_NUMERIC, 0,       9,       1,     0x0114},  // 0-9
    {3,  "BANDWIDTH",      MENU_TYPE_CHOICE,  0,       1,       1,     0x0118},  // Wide/Narrow
    {4,  "TX POWER",       MENU_TYPE_NUMERIC, 0,       255,     1,     0x0116},  // 0-255
    {5,  "TX CTCSS",       MENU_TYPE_NUMERIC, 0,       3000,    1,     0x011A},  // 0-3000
    {6,  "TX DCS",         MENU_TYPE_NUMERIC, 0,       0x1FF,   1,     0x011E},  // 0-0x1FF
    {7,  "RX CTCSS",       MENU_TYPE_NUMERIC, 0,       3000,    1,     0x011C},  // 0-3000
    {8,  "RX DCS",         MENU_TYPE_NUMERIC, 0,       0x1FF,   1,     0x0120},  // 0-0x1FF
    
    // Scan settings
    {9,  "VFO SCAN RANGE", MENU_TYPE_NUMERIC, 1,       60000,   2,     0x0202},  // 1-60000 step 2
    {10, "SCAN PERSIST",   MENU_TYPE_NUMERIC, 0,       200,     1,     0x0204},  // 0-200
    {11, "SCAN RESUME",    MENU_TYPE_NUMERIC, 0,       250,     1,     0x0206},  // 0-250
    {12, "SCAN ULTRA",     MENU_TYPE_NUMERIC, 0,       20,      1,     0x0208},  // 0-20
    {13, "SCAN UPDATE",    MENU_TYPE_NUMERIC, 0,       50,      1,     0x020A},  // 0-50
    
    // Audio and interface settings
    {14, "TX TIMEOUT",     MENU_TYPE_NUMERIC, 0,       250,     1,     0x020C},  // 0-250
    {15, "MIC GAIN",       MENU_TYPE_NUMERIC, 0,       31,      1,     0x0124},  // 0-31
    {16, "DTMF Decode",    MENU_TYPE_CHOICE,  0,       2,       1,     0x020E},  // Off/Always/Squelched
    {17, "REPEATER TONE",  MENU_TYPE_NUMERIC, 100,     4000,    1,     0x0210},  // 100-4000
    {18, "TONE MONITOR",   MENU_TYPE_CHOICE,  0,       2,       1,     0x0212},  // Off/On/Clone
    
    // Display settings
    {19, "LCD BRIGHTNESS", MENU_TYPE_NUMERIC, 0,       35,      1,     0x0214},  // 0-35
    {20, "DIM BRIGHTNESS", MENU_TYPE_NUMERIC, 0,       14,      1,     0x0216},  // 0-14
    {21, "LCD TIMEOUT",    MENU_TYPE_NUMERIC, 0,       250,     1,     0x0218},  // 0-250
    {22, "HEARTBEAT",      MENU_TYPE_NUMERIC, 0,       30,      1,     0x021A},  // 0-30
    {23, "SQ TAIL ELIM",   MENU_TYPE_CHOICE,  0,       3,       1,     0x021C},  // Off/RX/TX/Both
    {24, "KEY TONES",      MENU_TYPE_CHOICE,  0,       3,       1,     0x021E},  // Off/On/Differential/Voice
    {25, "BLUETOOTH",      MENU_TYPE_CHOICE,  0,       1,       1,     0x0220},  // Off/On
    
    // Info displays
    {26, "BATTERY",        MENU_TYPE_INFO,    0,       0,       0,     0},
    {27, "RSSI",           MENU_TYPE_INFO,    0,       0,       0,     0},
    {28, "VERSION",        MENU_TYPE_INFO,    0,       0,       0,     0}
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
    menu_last_cursor = 0xFF;  // Initialize to invalid value
    menu_current_start_item = 0;  // Initialize scroll window
}

/**
 * Enter menu system from normal operation mode
 * Switches to menu mode, resets cursor, and triggers display update
 * Only clears screen once during initialization
 */
void menu_enter(void) {
    menu_mode = MENU_MODE_MENU;
    menu_cursor = 0;
    menu_edit_state = MENU_EDIT_NONE;
    menu_display_dirty = 1;
    menu_last_cursor = 0xFF;  // Force full redraw on first update
    menu_current_start_item = 0;  // Reset scroll window
    
    // One-time full screen clear for menu initialization
    menu_clear_screen();
    send_uart_message("Menu entered");
}

/**
 * Exit menu system and return to normal operation mode
 * Cancels any active editing and triggers display update
 * Clears screen once when exiting to normal mode
 */
void menu_exit(void) {
    menu_mode = MENU_MODE_NORMAL;
    menu_edit_state = MENU_EDIT_NONE;
    menu_display_dirty = 1;
    
    // One-time full screen clear when exiting to normal mode
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
            if (menu_mode == MENU_MODE_MENU) {
                // Enter item detail screen for numeric, choice, and info items
                if (current_item->type == MENU_TYPE_NUMERIC || current_item->type == MENU_TYPE_CHOICE || current_item->type == MENU_TYPE_INFO) {
                    menu_enter_item_detail();
                } else {
                    menu_exit(); // Exit menu if not supported (action items)
                }
            } else if (menu_mode == MENU_MODE_ITEM) {
                // Save current edit and return to menu list
                menu_save_and_return();
            }
            break;

        case KEY_EXIT:
            if (menu_mode == MENU_MODE_MENU) {
                menu_exit(); // Exit to normal mode
            } else if (menu_mode == MENU_MODE_ITEM) {
                menu_return_to_list(); // Return to menu list without saving
            }
            break;

        case KEY_UP:
            if (menu_mode == MENU_MODE_MENU) {
                menu_navigate_up();
            } else if (menu_mode == MENU_MODE_ITEM) {
                menu_item_navigate_up();
            }
            break;

        case KEY_DOWN:
            if (menu_mode == MENU_MODE_MENU) {
                menu_navigate_down();
            } else if (menu_mode == MENU_MODE_ITEM) {
                menu_item_navigate_down();
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
            if (menu_mode == MENU_MODE_ITEM) {
                menu_item_digit_input(key);
            }
            // Ignore digits in menu list mode
            break;

        case KEY_STAR:
            // STAR key is now unused - MENU handles edit entry and save
            break;

        case KEY_HASH:
            // HASH key is now unused - MENU handles save
            break;

        default:
            // Ignore other keys
            break;
    }

    // Update display if needed
    if (menu_display_dirty) {
        if (menu_mode == MENU_MODE_MENU) {
            menu_update_display();
        } else if (menu_mode == MENU_MODE_ITEM) {
            menu_update_item_display();
        }
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
    if (item->type == MENU_TYPE_NUMERIC || item->type == MENU_TYPE_CHOICE) {
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
    return (u16)settings_get_power(); 
}

u16 menu_get_bandwidth(void) { 
    return 0; // TODO: Implement bandwidth setting
}

u16 menu_get_ctcss_tx(void) { 
    return (u16)settings_get_ctcss(); 
}

u16 menu_get_ctcss_rx(void) { 
    return (u16)settings_get_ctcss(); 
}

u16 menu_get_dcs_tx(void) { 
    return 0; // TODO: Implement DCS TX setting
}

u16 menu_get_dcs_rx(void) { 
    return 0; // TODO: Implement DCS RX setting
}

u16 menu_get_vox(void) { 
    return 0; // TODO: Implement VOX setting
}

u16 menu_get_mic_gain(void) { 
    return 0; // TODO: Implement mic gain setting
}

u16 menu_get_compander(void) { 
    return 0; // TODO: Implement compander setting
}

u16 menu_get_pre_emphasis(void) { 
    return 0; // TODO: Implement pre-emphasis setting
}

u16 menu_get_tail_elim(void) { 
    return 0; // TODO: Implement tail eliminator setting
}

u16 menu_get_scrambler(void) { 
    return 0; // TODO: Implement scrambler setting
}

// New getter functions for additional menu items
u16 menu_get_step(void) {
    // TODO: Implement step setting getter
    return 25; // Default 25kHz step
}

u16 menu_get_scan_range(void) {
    // TODO: Implement scan range getter
    return 100; // Default range
}

u16 menu_get_scan_persist(void) {
    // TODO: Implement scan persist getter
    return 10; // Default persist time
}

u16 menu_get_scan_resume(void) {
    // TODO: Implement scan resume getter
    return 5; // Default resume time
}

u16 menu_get_ultra_scan(void) {
    // TODO: Implement ultra scan getter
    return 0; // Default off
}

u16 menu_get_scan_update(void) {
    // TODO: Implement scan update getter
    return 10; // Default update rate
}

u16 menu_get_tx_timeout(void) {
    // TODO: Implement TX timeout getter
    return 60; // Default 60 seconds
}

u16 menu_get_dtmf_decode(void) {
    // TODO: Implement DTMF decode getter
    return 0; // Default off
}

u16 menu_get_repeater_tone(void) {
    // TODO: Implement repeater tone getter
    return 1750; // Default 1750Hz
}

u16 menu_get_tone_monitor(void) {
    // TODO: Implement tone monitor getter
    return 0; // Default off
}

u16 menu_get_lcd_brightness(void) {
    // TODO: Implement LCD brightness getter
    return 20; // Default brightness
}

u16 menu_get_dimmer(void) {
    // TODO: Implement dimmer getter
    return 5; // Default dim level
}

u16 menu_get_lcd_timeout(void) {
    // TODO: Implement LCD timeout getter
    return 30; // Default 30 seconds
}

u16 menu_get_breathe(void) {
    // TODO: Implement breathe/heartbeat getter
    return 0; // Default off
}

u16 menu_get_ste(void) {
    // TODO: Implement squelch tail eliminator getter
    return 0; // Default off
}

u16 menu_get_key_tones(void) {
    // TODO: Implement key tones getter
    return 1; // Default on
}

u16 menu_get_bluetooth(void) {
    // TODO: Implement bluetooth getter
    return 0; // Default off
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
    settings_set_power((u8)value); 
    menu_apply_setting(4, value); // TX Power
}

void menu_set_bandwidth(u16 value) { 
    // TODO: Implement bandwidth setting
    menu_apply_setting(3, value); // Bandwidth
}

void menu_set_ctcss_tx(u16 value) { 
    settings_set_ctcss((u8)value); 
    menu_apply_setting(5, value); // TX CTCSS
}

void menu_set_ctcss_rx(u16 value) { 
    settings_set_ctcss((u8)value); 
    menu_apply_setting(7, value); // RX CTCSS
}

void menu_set_dcs_tx(u16 value) { 
    // TODO: Implement DCS TX setting
    menu_apply_setting(6, value); // TX DCS
}

void menu_set_dcs_rx(u16 value) { 
    // TODO: Implement DCS RX setting
    menu_apply_setting(8, value); // RX DCS
}

void menu_set_vox(u16 value) { 
    // TODO: Implement VOX setting
    menu_apply_setting(9, value); // VOX
}

void menu_set_mic_gain(u16 value) { 
    // TODO: Implement mic gain setting
    menu_apply_setting(15, value); // Mic Gain
}

void menu_set_compander(u16 value) { 
    // TODO: Implement compander setting
    menu_apply_setting(10, value); // Compander
}

void menu_set_pre_emphasis(u16 value) { 
    // TODO: Implement pre-emphasis setting
    menu_apply_setting(11, value); // Pre-emphasis
}

void menu_set_tail_elim(u16 value) { 
    // TODO: Implement tail eliminator setting
    menu_apply_setting(23, value); // Sq Tail Elim
}

void menu_set_scrambler(u16 value) { 
    // TODO: Implement scrambler setting
    menu_apply_setting(12, value); // Scrambler
}

// New setter functions for additional menu items
void menu_set_step(u16 value) {
    // TODO: Implement step setting setter
    menu_apply_setting(1, value); // MENU_STEP
}

void menu_set_scan_range(u16 value) {
    // TODO: Implement scan range setter
    menu_apply_setting(9, value); // MENU_SCAN_RANGE
}

void menu_set_scan_persist(u16 value) {
    // TODO: Implement scan persist setter
    menu_apply_setting(10, value); // MENU_SCAN_PERSIST
}

void menu_set_scan_resume(u16 value) {
    // TODO: Implement scan resume setter
    menu_apply_setting(11, value); // MENU_SCAN_RESUME
}

void menu_set_ultra_scan(u16 value) {
    // TODO: Implement ultra scan setter
    menu_apply_setting(12, value); // MENU_ULTRA_SCAN
}

void menu_set_scan_update(u16 value) {
    // TODO: Implement scan update setter
    menu_apply_setting(13, value); // MENU_SCAN_UPDATE
}

void menu_set_tx_timeout(u16 value) {
    // TODO: Implement TX timeout setter
    menu_apply_setting(14, value); // MENU_TX_TIMEOUT
}

void menu_set_dtmf_decode(u16 value) {
    // TODO: Implement DTMF decode setter
    menu_apply_setting(16, value); // MENU_DTMF_DECODE
}

void menu_set_repeater_tone(u16 value) {
    // TODO: Implement repeater tone setter
    menu_apply_setting(17, value); // MENU_REPEATER_TONE
}

void menu_set_tone_monitor(u16 value) {
    // TODO: Implement tone monitor setter
    menu_apply_setting(18, value); // MENU_TONE_MONITOR
}

void menu_set_lcd_brightness(u16 value) {
    // TODO: Implement LCD brightness setter
    menu_apply_setting(19, value); // MENU_LCD_BL
}

void menu_set_dimmer(u16 value) {
    // TODO: Implement dimmer setter
    menu_apply_setting(20, value); // MENU_DIMMER
}

void menu_set_lcd_timeout(u16 value) {
    // TODO: Implement LCD timeout setter
    menu_apply_setting(21, value); // MENU_LCD_TIMEOUT
}

void menu_set_breathe(u16 value) {
    // TODO: Implement breathe/heartbeat setter
    menu_apply_setting(22, value); // MENU_BREATHE
}

void menu_set_ste(u16 value) {
    // TODO: Implement squelch tail eliminator setter
    menu_apply_setting(23, value); // MENU_STE
}

void menu_set_key_tones(u16 value) {
    // TODO: Implement key tones setter
    menu_apply_setting(24, value); // MENU_KEY_TONES
}

void menu_set_bluetooth(u16 value) {
    // TODO: Implement bluetooth setter
    menu_apply_setting(25, value); // MENU_BLUETOOTH
}

/**
 * Function pointer array for menu value operations (stored in ROM)
 * This array eliminates switch statements and provides O(1) access to get/set functions
 * Indexed by menu item ID for efficient lookup
 * Non-settable items (info, action) have NULL pointers
 */
const __code menu_value_ops_t menu_value_ops[MENU_COUNT] = {
    {menu_get_frequency,      menu_set_frequency},      // 0: Frequency
    {menu_get_step,           menu_set_step},           // 1: Step
    {menu_get_squelch,        menu_set_squelch},        // 2: Squelch
    {menu_get_bandwidth,      menu_set_bandwidth},      // 3: Bandwidth
    {menu_get_tx_power,       menu_set_tx_power},       // 4: TX Power
    {menu_get_ctcss_tx,       menu_set_ctcss_tx},       // 5: TX CTCSS
    {menu_get_dcs_tx,         menu_set_dcs_tx},         // 6: TX DCS
    {menu_get_ctcss_rx,       menu_set_ctcss_rx},       // 7: RX CTCSS
    {menu_get_dcs_rx,         menu_set_dcs_rx},         // 8: RX DCS
    {menu_get_scan_range,     menu_set_scan_range},     // 9: VFO Scan Range
    {menu_get_scan_persist,   menu_set_scan_persist},   // 10: Scan Persist
    {menu_get_scan_resume,    menu_set_scan_resume},    // 11: Scan Resume
    {menu_get_ultra_scan,     menu_set_ultra_scan},     // 12: Scan Ultra
    {menu_get_scan_update,    menu_set_scan_update},    // 13: Scan Update
    {menu_get_tx_timeout,     menu_set_tx_timeout},     // 14: TX Timeout
    {menu_get_mic_gain,       menu_set_mic_gain},       // 15: Mic Gain
    {menu_get_dtmf_decode,    menu_set_dtmf_decode},    // 16: DTMF Decode
    {menu_get_repeater_tone,  menu_set_repeater_tone},  // 17: Repeater Tone
    {menu_get_tone_monitor,   menu_set_tone_monitor},   // 18: Tone Monitor
    {menu_get_lcd_brightness, menu_set_lcd_brightness}, // 19: LCD Brightness
    {menu_get_dimmer,         menu_set_dimmer},         // 20: Dim Brightness
    {menu_get_lcd_timeout,    menu_set_lcd_timeout},    // 21: LCD Timeout
    {menu_get_breathe,        menu_set_breathe},        // 22: Heartbeat
    {menu_get_ste,            menu_set_ste},            // 23: Sq Tail Elim
    {menu_get_key_tones,      menu_set_key_tones},      // 24: Key Tones
    {menu_get_bluetooth,      menu_set_bluetooth},      // 25: Bluetooth
    {0, 0},                                             // 26: Battery (info type)
    {0, 0},                                             // 27: RSSI (info type)
    {0, 0}                                              // 28: Version (info type)
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
        case 0: // Frequency
            at1846s_set_frequency((u32)value);
            break;
        case 1: // Step
            // TODO: Implement frequency step setting
            break;
        case 2: // Squelch
            at1846s_set_squelch((u8)value);
            break;
        case 3: // Bandwidth
            // TODO: Implement bandwidth setting
            break;
        case 4: // TX Power
            // TODO: Implement at1846s_set_power_level() when available
            break;
        case 5: // TX CTCSS
            at1846s_set_ctcss_tx((u16)value);
            break;
        case 6: // TX DCS
            // TODO: Implement DCS TX when available
            break;
        case 7: // RX CTCSS
            // TODO: Implement CTCSS RX when available
            break;
        case 8: // RX DCS
            // TODO: Implement DCS RX when available
            break;
        case 9: // VFO Scan Range
            // TODO: Implement scan range setting
            break;
        case 10: // Scan Persist
            // TODO: Implement scan persist setting
            break;
        case 11: // Scan Resume
            // TODO: Implement scan resume setting
            break;
        case 12: // Scan Ultra
            // TODO: Implement ultra scan setting
            break;
        case 13: // Scan Update
            // TODO: Implement scan update setting
            break;
        case 14: // TX Timeout
            // TODO: Implement TX timeout setting
            break;
        case 15: // Mic Gain
            // TODO: Implement microphone gain setting
            break;
        case 16: // DTMF Decode
            // TODO: Implement DTMF decode setting
            break;
        case 17: // Repeater Tone
            // TODO: Implement repeater tone setting
            break;
        case 18: // Tone Monitor
            // TODO: Implement tone monitor setting
            break;
        case 19: // LCD Brightness
            // TODO: Implement LCD brightness control
            break;
        case 20: // Dimmer
            // TODO: Implement dimmer control
            break;
        case 21: // LCD Timeout
            // TODO: Implement LCD timeout setting
            break;
        case 22: // Heartbeat
            // TODO: Implement heartbeat/breathe setting
            break;
        case 23: // Sq Tail Elim
            // TODO: Implement squelch tail eliminator
            break;
        case 24: // Key Tones
            // TODO: Implement key tone setting
            break;
        case 25: // Bluetooth
            // TODO: Implement bluetooth setting
            break;
        default:
            // Unknown menu item
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
        u8 visible_items = 6;
        u8 new_start_item = menu_current_start_item;
        
        // Smart scrolling logic with hysteresis
        // Only change scroll window when absolutely necessary
        
        // Check if cursor is outside current window
        if (menu_cursor < menu_current_start_item) {
            // Cursor moved above current window - need to scroll up
            new_start_item = menu_cursor;
        } else if (menu_cursor >= menu_current_start_item + visible_items) {
            // Cursor moved below current window - need to scroll down
            new_start_item = menu_cursor - visible_items + 1;
        }
        // If cursor is within current window, keep the same window
        
        // Check if we need full redraw or partial update
        if (menu_last_cursor == 0xFF || new_start_item != menu_current_start_item) {
            // First time rendering or scroll window changed - clear menu area only
            menu_current_start_item = new_start_item;
            
            // Clear only the menu content area (below title and line)
            menu_clear_area(0, 18, 160, 110, 0x00, 0x00);  // Clear menu area, preserve title
            
            menu_render_new_style();
            menu_last_cursor = menu_cursor;
        } else if (menu_last_cursor != menu_cursor) {
            // Cursor moved within same window - partial update
            menu_render_partial_update(menu_last_cursor, menu_cursor);
            menu_last_cursor = menu_cursor;
        }
        
        menu_display_dirty = 0;
    }
}

/**
 * Render new menu system with numbered list and inverted selection
 * Layout: Centered "MENU", horizontal line, numbered list items with values
 */
void menu_render_new_style(void) {
    u8 y_pos = 20;  // Increased spacing from line to first item
    u8 visible_items = 6;  // Reduce items to save memory
    u8 start_item = menu_current_start_item;  // Use current scroll window
    
    // Add watchdog reset
    watchdog_reset();
    
    // Centered "MENU" title - better centering calculation
    // MENU = 4 chars, each char = 12 pixels spacing, total = 4*12 = 48 pixels
    // Screen width = 160, so center = (160-48)/2 = 56
    render_16x8_string(56, 5, "MENU");
    
    // Horizontal line below title (moved further from title)
    draw_horizontal_line(10, 150, 16);
    
    watchdog_reset(); // Reset before heavy rendering
    
    // Render visible menu items
    for (u8 i = 0; i < visible_items && (start_item + i) < MENU_COUNT; i++) {
        u8 item_index = start_item + i;
        const menu_item_t* item = &menu_items[item_index];
        u8 current_y = y_pos + (i * 14);  // 14 pixel spacing between items
        
        // Reset watchdog every few items
        if (i % 3 == 0) {
            watchdog_reset();
        }
        
        // Render decimal number (1-18)
        u8 display_number = item_index + 1;  // Convert to 1-based decimal
        if (display_number < 10) {
            // Single digit
            if (item_index == menu_cursor) {
                render_16x8_char_inverted(5, current_y, '0' + display_number);
            } else {
                render_16x8_char(5, current_y, '0' + display_number);
            }
        } else {
            // Two digits (10-18)
            u8 tens = display_number / 10;
            u8 ones = display_number % 10;
            if (item_index == menu_cursor) {
                render_16x8_char_inverted(5, current_y, '0' + tens);
                render_16x8_char_inverted(17, current_y, '0' + ones);
            } else {
                render_16x8_char(5, current_y, '0' + tens);
                render_16x8_char(17, current_y, '0' + ones);
            }
        }
        
        // Create uppercase name string (closer spacing to number)
        static char name_buffer[12];  // Static to save stack space
        u8 j = 0;
        while (item->name[j] && j < 11) {  // Limit name length
            char c = item->name[j];
            if (c >= 'a' && c <= 'z') {
                c = c - 'a' + 'A';  // Convert to uppercase
            }
            name_buffer[j] = c;
            j++;
        }
        name_buffer[j] = '\0';
        
        // Render name with appropriate spacing for single/double digit numbers
        u8 name_x_pos = (display_number < 10) ? 18 : 30;  // Closer for single digits
        
        if (item_index == menu_cursor) {
            // Selected item - inverted text for name
            render_16x8_string_inverted(name_x_pos, current_y, name_buffer);
        } else {
            // Normal item
            render_16x8_string(name_x_pos, current_y, name_buffer);
        }
    }
    
    watchdog_reset(); // Final reset
}

/**
 * Render single menu item at specified position
 */
void menu_render_single_item(u8 item_index, u8 is_selected) {
    const menu_item_t* item = &menu_items[item_index];
    u8 visible_items = 6;
    
    // Use the global scroll window position instead of calculating independently
    // Check if this item is currently visible in the current scroll window
    if (item_index < menu_current_start_item || item_index >= menu_current_start_item + visible_items) {
        return; // Item not visible, don't render
    }
    
    u8 screen_position = item_index - menu_current_start_item;
    u8 current_y = 20 + (screen_position * 14);
    
    // Clear the line first using consistent area clear function
    menu_clear_area(5, current_y, 146, 8, 0x00, 0x00);
    
    // Render decimal number (1-18)
    u8 display_number = item_index + 1;  // Convert to 1-based decimal
    if (display_number < 10) {
        // Single digit
        if (is_selected) {
            render_16x8_char_inverted(5, current_y, '0' + display_number);
        } else {
            render_16x8_char(5, current_y, '0' + display_number);
        }
    } else {
        // Two digits (10-18)
        u8 tens = display_number / 10;
        u8 ones = display_number % 10;
        if (is_selected) {
            render_16x8_char_inverted(5, current_y, '0' + tens);
            render_16x8_char_inverted(17, current_y, '0' + ones);
        } else {
            render_16x8_char(5, current_y, '0' + tens);
            render_16x8_char(17, current_y, '0' + ones);
        }
    }
    
    // Create uppercase name
    static char name_buffer[12];
    u8 j = 0;
    while (item->name[j] && j < 11) {
        char c = item->name[j];
        if (c >= 'a' && c <= 'z') {
            c = c - 'a' + 'A';
        }
        name_buffer[j] = c;
        j++;
    }
    name_buffer[j] = '\0';
    
    // Render name with appropriate spacing for single/double digit numbers
    u8 name_x_pos = (display_number < 10) ? 18 : 30;  // Closer for single digits
    
    if (is_selected) {
        render_16x8_string_inverted(name_x_pos, current_y, name_buffer);
    } else {
        render_16x8_string(name_x_pos, current_y, name_buffer);
    }
}

/**
 * Partial screen update - only redraw changed items
 */
void menu_render_partial_update(u8 old_cursor, u8 new_cursor) {
    watchdog_reset();
    
    // Redraw old cursor position as normal
    menu_render_single_item(old_cursor, 0);
    
    // Redraw new cursor position as selected
    menu_render_single_item(new_cursor, 1);
    
    watchdog_reset();
}

/**
 * Format value as string for display
 */
void menu_format_value_string(const menu_item_t* item, u16 value, char* buffer) {
    // Simplified formatting to reduce memory usage
    if (item->type == MENU_TYPE_CHOICE) {
        if (value == 0) {
            buffer[0] = 'O'; buffer[1] = 'F'; buffer[2] = 'F'; buffer[3] = '\0';
        } else {
            buffer[0] = 'O'; buffer[1] = 'N'; buffer[2] = '\0';
        }
    } else {
        // Simple number formatting (max 999)
        if (value == 0) {
            buffer[0] = '0'; buffer[1] = '\0';
        } else if (value < 10) {
            buffer[0] = '0' + value; buffer[1] = '\0';
        } else if (value < 100) {
            buffer[0] = '0' + (value / 10);
            buffer[1] = '0' + (value % 10);
            buffer[2] = '\0';
        } else {
            buffer[0] = '0' + (value / 100);
            buffer[1] = '0' + ((value / 10) % 10);
            buffer[2] = '0' + (value % 10);
            buffer[3] = '\0';
        }
    }
}

/**
 * Format value as compact string for menu list display
 * Handles different menu item types with appropriate formatting
 * @param item: Menu item definition
 * @param value: Current value to format
 * @param buffer: Output buffer (should be at least 8 chars)
 */
void menu_format_value_compact(const menu_item_t* item, u32 value, char* buffer) {
    if (item->type == MENU_TYPE_CHOICE) {
        // Handle specific choice items with meaningful labels
        switch (item->id) {
            case 3: // Bandwidth
                if (value == 0) {
                    buffer[0] = 'N'; buffer[1] = 'A'; buffer[2] = 'R'; buffer[3] = '\0'; // "NAR"
                } else {
                    buffer[0] = 'W'; buffer[1] = 'I'; buffer[2] = 'D'; buffer[3] = '\0'; // "WID"
                }
                break;
            case 16: // DTMF Decode
                switch (value) {
                    case 0: buffer[0] = 'O'; buffer[1] = 'F'; buffer[2] = 'F'; buffer[3] = '\0'; break; // "OFF"
                    case 1: buffer[0] = 'A'; buffer[1] = 'L'; buffer[2] = 'W'; buffer[3] = '\0'; break; // "ALW"
                    case 2: buffer[0] = 'S'; buffer[1] = 'Q'; buffer[2] = 'L'; buffer[3] = '\0'; break; // "SQL"
                    default: buffer[0] = '?'; buffer[1] = '\0'; break;
                }
                break;
            case 18: // Tone Monitor
                switch (value) {
                    case 0: buffer[0] = 'O'; buffer[1] = 'F'; buffer[2] = 'F'; buffer[3] = '\0'; break; // "OFF"
                    case 1: buffer[0] = 'O'; buffer[1] = 'N'; buffer[2] = '\0'; break; // "ON"
                    case 2: buffer[0] = 'C'; buffer[1] = 'L'; buffer[2] = 'N'; buffer[3] = '\0'; break; // "CLN"
                    default: buffer[0] = '?'; buffer[1] = '\0'; break;
                }
                break;
            case 23: // Squelch Tail Eliminator
                switch (value) {
                    case 0: buffer[0] = 'O'; buffer[1] = 'F'; buffer[2] = 'F'; buffer[3] = '\0'; break; // "OFF"
                    case 1: buffer[0] = 'R'; buffer[1] = 'X'; buffer[2] = '\0'; break; // "RX"
                    case 2: buffer[0] = 'T'; buffer[1] = 'X'; buffer[2] = '\0'; break; // "TX"
                    case 3: buffer[0] = 'B'; buffer[1] = 'T'; buffer[2] = 'H'; buffer[3] = '\0'; break; // "BTH"
                    default: buffer[0] = '?'; buffer[1] = '\0'; break;
                }
                break;
            case 24: // Key Tones
                switch (value) {
                    case 0: buffer[0] = 'O'; buffer[1] = 'F'; buffer[2] = 'F'; buffer[3] = '\0'; break; // "OFF"
                    case 1: buffer[0] = 'O'; buffer[1] = 'N'; buffer[2] = '\0'; break; // "ON"
                    case 2: buffer[0] = 'D'; buffer[1] = 'I'; buffer[2] = 'F'; buffer[3] = '\0'; break; // "DIF"
                    case 3: buffer[0] = 'V'; buffer[1] = 'O'; buffer[2] = 'X'; buffer[3] = '\0'; break; // "VOX"
                    default: buffer[0] = '?'; buffer[1] = '\0'; break;
                }
                break;
            case 25: // Bluetooth
                if (value == 0) {
                    buffer[0] = 'O'; buffer[1] = 'F'; buffer[2] = 'F'; buffer[3] = '\0'; // "OFF"
                } else {
                    buffer[0] = 'O'; buffer[1] = 'N'; buffer[2] = '\0'; // "ON"
                }
                break;
            default:
                // Generic choice formatting
                if (value == 0) {
                    buffer[0] = 'O'; buffer[1] = 'F'; buffer[2] = 'F'; buffer[3] = '\0';
                } else {
                    buffer[0] = 'O'; buffer[1] = 'N'; buffer[2] = '\0';
                }
                break;
        }
    } else {
        // Numeric formatting - compact for small screen
        if (value == 0) {
            buffer[0] = '0'; buffer[1] = '\0';
        } else if (value < 10) {
            buffer[0] = '0' + value; buffer[1] = '\0';
        } else if (value < 100) {
            buffer[0] = '0' + (value / 10);
            buffer[1] = '0' + (value % 10);
            buffer[2] = '\0';
        } else if (value < 1000) {
            buffer[0] = '0' + (value / 100);
            buffer[1] = '0' + ((value / 10) % 10);
            buffer[2] = '0' + (value % 10);
            buffer[3] = '\0';
        } else {
            // For larger values (like frequency), show in compact form
            if (value >= 100000) {
                // Frequency in MHz (e.g., 446000 -> "446M")
                u16 mhz = value / 1000;
                if (mhz < 10) {
                    buffer[0] = '0' + mhz; buffer[1] = 'M'; buffer[2] = '\0';
                } else if (mhz < 100) {
                    buffer[0] = '0' + (mhz / 10);
                    buffer[1] = '0' + (mhz % 10);
                    buffer[2] = 'M'; buffer[3] = '\0';
                } else {
                    buffer[0] = '0' + (mhz / 100);
                    buffer[1] = '0' + ((mhz / 10) % 10);
                    buffer[2] = '0' + (mhz % 10);
                    buffer[3] = 'M'; buffer[4] = '\0';
                }
            } else {
                // 4-digit numbers, show first 3 digits + "K" if needed
                buffer[0] = '0' + (value / 1000);
                buffer[1] = '0' + ((value / 100) % 10);
                buffer[2] = '0' + ((value / 10) % 10);
                buffer[3] = 'K'; buffer[4] = '\0';
            }
        }
    }
}

/**
 * Get current value for specific menu item
 */
u16 menu_get_current_value_for_item(u8 item_index) {
    const menu_value_ops_t* ops = &menu_value_ops[item_index];
    if (ops->get_func) {
        return ops->get_func();
    }
    return 0;
}

/**
 * Render menu title at top of display (legacy function)
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
        render_16x16_string(MENU_TEXT_X, MENU_NAV_Y, "MENU=EDIT EXIT");
    } else {
        render_16x16_string(MENU_TEXT_X, MENU_NAV_Y, "UP/DN MENU EXIT");
    }
}

/**
 * Clear rectangular area of screen with specified color
 * More efficient than full screen clear for partial updates
 * @param x: Starting X coordinate
 * @param y: Starting Y coordinate  
 * @param w: Width in pixels
 * @param h: Height in pixels
 * @param color_hi: High byte of 16-bit color
 * @param color_lo: Low byte of 16-bit color
 */
void menu_clear_area(u8 x, u8 y, u8 w, u8 h, u8 color_hi, u8 color_lo) {
    u8 row, col;
    
    lcd_set_window(x, y, x + w - 1, y + h - 1);
    
    for (row = 0; row < h; row++) {
        for (col = 0; col < w; col++) {
            lcd_send_data(color_hi);
            lcd_send_data(color_lo);
        }
    }
}

/**
 * Clear entire screen for menu display (one-time use)
 * Uses working LCD pattern structure for reliable clearing
 * Only call this during menu initialization or major transitions
 */
void menu_clear_screen(void) {
    // Use working LCD pattern structure (clear_area has bugs)
    u8 row, col;
    
    lcd_set_window(0, 0, 159, 127);  // Set full screen window
    for (row = 0; row < 128; row++) {
        for (col = 0; col < 160; col++) {
            lcd_send_data(0x00);  // Black high byte
            lcd_send_data(0x00);  // Black low byte
        }
    }
}

/**
 * Enter item detail screen for editing specific item
 */
void menu_enter_item_detail(void) {
    const menu_item_t* item = menu_get_current_item();
    
    menu_mode = MENU_MODE_ITEM;
    menu_item_cursor = 0;
    menu_item_selection = 0;
    
    // Load current value into temporary buffer
    menu_item_temp_value = menu_get_current_value();
    
    // For choice items, find current selection index
    if (item->type == MENU_TYPE_CHOICE) {
        menu_item_selection = (menu_item_temp_value == 0) ? 0 : 1;
    }
    
    menu_display_dirty = 1;
    send_uart_message("Entered item detail");
}

/**
 * Return to menu list without saving changes
 */
void menu_return_to_list(void) {
    menu_mode = MENU_MODE_MENU;
    menu_display_dirty = 1;
    menu_last_cursor = 0xFF;  // Force full redraw but keep cursor position
    menu_clear_screen();
    send_uart_message("Returned to menu list");
}

/**
 * Save current edit and return to menu list
 */
void menu_save_and_return(void) {
    menu_set_current_value(menu_item_temp_value);
    menu_mode = MENU_MODE_MENU;
    menu_display_dirty = 1;
    menu_last_cursor = 0xFF;  // Force full redraw but keep cursor position
    menu_clear_screen();
    send_uart_message("Saved and returned to menu list");
}

/**
 * Navigate up in item detail screen
 */
void menu_item_navigate_up(void) {
    const menu_item_t* item = menu_get_current_item();
    
    if (item->type == MENU_TYPE_CHOICE) {
        // For choice items, navigate between options
        if (menu_item_selection > 0) {
            menu_item_selection--;
            menu_item_temp_value = (menu_item_selection == 0) ? 0 : 1;
            menu_display_dirty = 1;
        }
    } else if (item->type == MENU_TYPE_NUMERIC) {
        // For numeric items, increment value
        if (menu_item_temp_value + item->step <= item->max_value) {
            menu_item_temp_value += item->step;
            menu_display_dirty = 1;
        }
    }
}

/**
 * Navigate down in item detail screen
 */
void menu_item_navigate_down(void) {
    const menu_item_t* item = menu_get_current_item();
    
    if (item->type == MENU_TYPE_CHOICE) {
        // For choice items, navigate between options
        u8 max_choices = 2; // Default binary choice
        
        // Handle special multi-choice items
        switch (item->id) {
            case 16: max_choices = 3; break; // DTMF Decode: Off/Always/Squelched
            case 18: max_choices = 3; break; // Tone Monitor: Off/On/Clone
            case 23: max_choices = 4; break; // STE: Off/RX/TX/Both
            case 24: max_choices = 4; break; // Key Tones: Off/On/Differential/Voice
        }
        
        if (menu_item_selection < max_choices - 1) {
            menu_item_selection++;
            menu_item_temp_value = menu_item_selection;
            menu_display_dirty = 1;
        }
    } else if (item->type == MENU_TYPE_NUMERIC) {
        // For numeric items, decrement value
        if (menu_item_temp_value >= item->min_value + item->step) {
            menu_item_temp_value -= item->step;
            menu_display_dirty = 1;
        }
    }
}

/**
 * Handle digit input in item detail screen
 */
void menu_item_digit_input(u8 digit) {
    const menu_item_t* item = menu_get_current_item();
    
    if (item->type == MENU_TYPE_NUMERIC) {
        u32 new_value;
        u8 actual_digit;
        
        // Convert key code to actual digit
        if (digit >= KEY_1 && digit <= KEY_9) {
            actual_digit = digit; // KEY_1 = 1, KEY_2 = 2, etc.
        } else if (digit == KEY_0) {
            actual_digit = 0;
        } else {
            return; // Invalid digit
        }
        
        // Shift current value and add new digit
        new_value = (u32)menu_item_temp_value * 10 + actual_digit;
        
        // Validate range
        if (new_value >= item->min_value && new_value <= item->max_value) {
            menu_item_temp_value = (u16)new_value;
            menu_display_dirty = 1;
        }
    }
}

/**
 * Update item detail screen display
 */
void menu_update_item_display(void) {
    if (menu_display_dirty) {
        menu_render_item_detail();
        menu_display_dirty = 0;
    }
}

/**
 * Render item detail screen
 */
void menu_render_item_detail(void) {
    const menu_item_t* item = menu_get_current_item();
    u8 y_pos = 25;
    
    // Clear screen first
    menu_clear_screen();
    
    watchdog_reset();
    
    // Calculate center position for item name  
    // Each character = 8 pixels width for proper centering
    const char* str = item->name;
    u8 name_len = 0;
    while (*str && name_len < MAX_CHARS_PER_STRING) {
        name_len++;
        str++;
    }
    u8 name_width = (name_len-1) * (CHAR_WIDTH_IN_PIXELS + 5);
    u8 center_x;
    if (name_width >= DISPLAY_WIDTH) {
        center_x = 0; // Text too wide, left-align
    } else {
        center_x = (DISPLAY_WIDTH - name_width) / 2;
    }
    
    // Render centered item name at top (same Y position as main MENU)
    render_16x8_string(center_x, 5, item->name);
    
    // Horizontal line below title
    draw_horizontal_line(10, 150, 16);
    
    if (item->type == MENU_TYPE_INFO) {
        // Show info display
        render_16x8_string(10, y_pos, "INFO DISPLAY");
        // TODO: Add specific info rendering
    } else if (item->type == MENU_TYPE_CHOICE) {
        // Show choice options
        menu_render_choice_options(item, y_pos);
    } else if (item->type == MENU_TYPE_NUMERIC) {
        // Show current value and allow editing
        menu_render_numeric_value(item, y_pos);
    }
    
    watchdog_reset();
}

/**
 * Render choice options for choice items
 */
void menu_render_choice_options(const menu_item_t* item, u8 start_y) {
    u8 current_y = start_y;
    u8 max_choices = 2;
    const char* choices[4];
    
    // Set up choices based on item type (all uppercase)
    switch (item->id) {
        case 3: // Bandwidth
            choices[0] = "NARROW";
            choices[1] = "WIDE";
            max_choices = 2;
            break;
        case 16: // DTMF Decode
            choices[0] = "OFF";
            choices[1] = "ALWAYS";
            choices[2] = "SQUELCHED";
            max_choices = 3;
            break;
        case 18: // Tone Monitor
            choices[0] = "OFF";
            choices[1] = "ON";
            choices[2] = "CLONE";
            max_choices = 3;
            break;
        case 23: // STE
            choices[0] = "OFF";
            choices[1] = "RX";
            choices[2] = "TX";
            choices[3] = "BOTH";
            max_choices = 4;
            break;
        case 24: // Key Tones
            choices[0] = "OFF";
            choices[1] = "ON";
            choices[2] = "DIFFERENTIAL";
            choices[3] = "VOICE";
            max_choices = 4;
            break;
        default: // Generic binary choice
            choices[0] = "OFF";
            choices[1] = "ON";
            max_choices = 2;
            break;
    }
    
    // Render choices with selection indicator (no ">" symbol)
    for (u8 i = 0; i < max_choices; i++) {
        if (i == menu_item_selection) {
            render_16x8_string_inverted(25, current_y, choices[i]);
        } else {
            render_16x8_string(25, current_y, choices[i]);
        }
        current_y += 14;
    }
}

/**
 * Render numeric value for numeric items
 */
void menu_render_numeric_value(const menu_item_t* item, u8 start_y) {
    static char value_str[16];
    
    // Format the current temporary value
    if (menu_item_temp_value < 10) {
        value_str[0] = '0' + menu_item_temp_value;
        value_str[1] = '\0';
    } else if (menu_item_temp_value < 100) {
        value_str[0] = '0' + (menu_item_temp_value / 10);
        value_str[1] = '0' + (menu_item_temp_value % 10);
        value_str[2] = '\0';
    } else if (menu_item_temp_value < 1000) {
        value_str[0] = '0' + (menu_item_temp_value / 100);
        value_str[1] = '0' + ((menu_item_temp_value / 10) % 10);
        value_str[2] = '0' + (menu_item_temp_value % 10);
        value_str[3] = '\0';
    } else {
        // Handle larger numbers (like frequency)
        value_str[0] = '0' + (menu_item_temp_value / 1000);
        value_str[1] = '0' + ((menu_item_temp_value / 100) % 10);
        value_str[2] = '0' + ((menu_item_temp_value / 10) % 10);
        value_str[3] = '0' + (menu_item_temp_value % 10);
        value_str[4] = '\0';
    }
    
    render_16x8_string(10, start_y, "VALUE:");
    render_16x8_string_inverted(10, start_y + 14, value_str);
}