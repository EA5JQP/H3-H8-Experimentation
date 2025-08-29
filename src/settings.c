#include "settings.h"
#include "uart.h"
#include "uart_test.h"

// Global settings instance in XDATA
__xdata settings_t current_settings;

// Initialize settings system
void settings_init(void) {
    send_uart_message("Initializing settings...");
    
    if (!settings_load()) {
        send_uart_message("Loading defaults");
        settings_load_defaults();
        settings_save();
    } else {
        send_uart_message("Settings loaded from EEPROM");
    }
}

// Load settings from EEPROM
__bit settings_load(void) {
    u8 temp_data[2];
    u16 temp_value;
    
    // Read magic number
    if (!eeprom_read(SETTINGS_MAGIC_ADDR, temp_data, 2)) {
        send_uart_message("EEPROM read failed");
        return 0;
    }
    temp_value = ((u16)temp_data[0] << 8) | temp_data[1];
    if (temp_value != SETTINGS_MAGIC) {
        send_uart_message("Settings magic invalid");
        return 0;
    }
    current_settings.magic = temp_value;
    
    // Read version
    if (!eeprom_read(SETTINGS_VERSION_ADDR, temp_data, 2)) return 0;
    temp_value = ((u16)temp_data[0] << 8) | temp_data[1];
    if (temp_value != SETTINGS_VERSION) {
        send_uart_message("Settings version mismatch");
        return 0;
    }
    current_settings.version = temp_value;
    
    // Read frequency
    if (!eeprom_read(SETTING_FREQUENCY_ADDR, temp_data, 2)) return 0;
    current_settings.frequency = ((u16)temp_data[0] << 8) | temp_data[1];
    
    // Read volume
    if (!eeprom_read(SETTING_VOLUME_ADDR, temp_data, 2)) return 0;
    current_settings.volume = ((u16)temp_data[0] << 8) | temp_data[1];
    
    // Read squelch
    if (!eeprom_read(SETTING_SQUELCH_ADDR, temp_data, 2)) return 0;
    current_settings.squelch = ((u16)temp_data[0] << 8) | temp_data[1];
    
    // Read power
    if (!eeprom_read(SETTING_POWER_ADDR, temp_data, 2)) return 0;
    current_settings.power = ((u16)temp_data[0] << 8) | temp_data[1];
    
    // Read backlight
    if (!eeprom_read(SETTING_BACKLIGHT_ADDR, temp_data, 2)) return 0;
    current_settings.backlight = ((u16)temp_data[0] << 8) | temp_data[1];
    
    // Read CTCSS
    if (!eeprom_read(SETTING_CTCSS_ADDR, temp_data, 2)) return 0;
    current_settings.ctcss = ((u16)temp_data[0] << 8) | temp_data[1];
    
    // Read checksum
    if (!eeprom_read(SETTINGS_CHECKSUM_ADDR, temp_data, 2)) return 0;
    current_settings.checksum = ((u16)temp_data[0] << 8) | temp_data[1];
    
    // Validate settings
    if (!settings_validate()) {
        send_uart_message("Settings validation failed");
        return 0;
    }
    
    return 1;
}

// Save settings to EEPROM
__bit settings_save(void) {
    __xdata static u8 temp_data[2];  // Make static and in XDATA
    
    // Update checksum before saving
    current_settings.checksum = settings_calculate_checksum();
    
    send_uart_message("Saving settings to EEPROM...");
    
    // Save magic number
    temp_data[0] = (u8)(current_settings.magic >> 8);
    temp_data[1] = (u8)(current_settings.magic & 0xFF);
    if (!eeprom_write(SETTINGS_MAGIC_ADDR, temp_data, 2)) {
        send_uart_message("Failed to save magic");
        return 0;
    }
    
    // Save version
    temp_data[0] = (u8)(current_settings.version >> 8);
    temp_data[1] = (u8)(current_settings.version & 0xFF);
    if (!eeprom_write(SETTINGS_VERSION_ADDR, temp_data, 2)) return 0;
    
    // Save frequency
    temp_data[0] = (u8)(current_settings.frequency >> 8);
    temp_data[1] = (u8)(current_settings.frequency & 0xFF);
    if (!eeprom_write(SETTING_FREQUENCY_ADDR, temp_data, 2)) return 0;
    
    // Save volume
    temp_data[0] = (u8)(current_settings.volume >> 8);
    temp_data[1] = (u8)(current_settings.volume & 0xFF);
    if (!eeprom_write(SETTING_VOLUME_ADDR, temp_data, 2)) return 0;
    
    // Save squelch
    temp_data[0] = (u8)(current_settings.squelch >> 8);
    temp_data[1] = (u8)(current_settings.squelch & 0xFF);
    if (!eeprom_write(SETTING_SQUELCH_ADDR, temp_data, 2)) return 0;
    
    // Save power
    temp_data[0] = (u8)(current_settings.power >> 8);
    temp_data[1] = (u8)(current_settings.power & 0xFF);
    if (!eeprom_write(SETTING_POWER_ADDR, temp_data, 2)) return 0;
    
    // Save backlight
    temp_data[0] = (u8)(current_settings.backlight >> 8);
    temp_data[1] = (u8)(current_settings.backlight & 0xFF);
    if (!eeprom_write(SETTING_BACKLIGHT_ADDR, temp_data, 2)) return 0;
    
    // Save CTCSS
    temp_data[0] = (u8)(current_settings.ctcss >> 8);
    temp_data[1] = (u8)(current_settings.ctcss & 0xFF);
    if (!eeprom_write(SETTING_CTCSS_ADDR, temp_data, 2)) return 0;
    
    // Save checksum
    temp_data[0] = (u8)(current_settings.checksum >> 8);
    temp_data[1] = (u8)(current_settings.checksum & 0xFF);
    if (!eeprom_write(SETTINGS_CHECKSUM_ADDR, temp_data, 2)) return 0;
    
    send_uart_message("Settings saved successfully");
    return 1;
}

// Load default settings
void settings_load_defaults(void) {
    current_settings.magic = SETTINGS_MAGIC;
    current_settings.version = SETTINGS_VERSION;
    current_settings.frequency = DEFAULT_FREQUENCY;
    current_settings.volume = DEFAULT_VOLUME;
    current_settings.squelch = DEFAULT_SQUELCH;
    current_settings.power = DEFAULT_POWER;
    current_settings.backlight = DEFAULT_BACKLIGHT;
    current_settings.ctcss = DEFAULT_CTCSS;
    current_settings.checksum = settings_calculate_checksum();
}

// Calculate simple checksum for settings validation
u16 settings_calculate_checksum(void) {
    u16 sum = 0;
    sum += current_settings.magic;
    sum += current_settings.version;
    sum += current_settings.frequency;
    sum += current_settings.volume;
    sum += current_settings.squelch;
    sum += current_settings.power;
    sum += current_settings.backlight;
    sum += current_settings.ctcss;
    return sum;
}

// Validate settings structure and ranges
__bit settings_validate(void) {
    // Check checksum
    u16 calculated_checksum = settings_calculate_checksum();
    if (current_settings.checksum != calculated_checksum) {
        send_uart_message("Checksum mismatch");
        return 0;
    }
    
    // Validate individual settings
    if (!settings_is_valid_frequency(current_settings.frequency)) {
        send_uart_message("Invalid frequency");
        return 0;
    }
    
    if (!settings_is_valid_volume((u8)current_settings.volume)) {
        send_uart_message("Invalid volume");
        return 0;
    }
    
    if (!settings_is_valid_squelch((u8)current_settings.squelch)) {
        send_uart_message("Invalid squelch");
        return 0;
    }
    
    if (!settings_is_valid_power((u8)current_settings.power)) {
        send_uart_message("Invalid power");
        return 0;
    }
    
    if (!settings_is_valid_backlight((u8)current_settings.backlight)) {
        send_uart_message("Invalid backlight");
        return 0;
    }
    
    if (!settings_is_valid_ctcss((u8)current_settings.ctcss)) {
        send_uart_message("Invalid CTCSS");
        return 0;
    }
    
    return 1;
}

// Individual setting accessors
u16 settings_get_frequency(void) {
    return current_settings.frequency;
}

void settings_set_frequency(u16 freq_khz) {
    if (settings_is_valid_frequency(freq_khz)) {
        current_settings.frequency = freq_khz;
    }
}

u8 settings_get_volume(void) {
    return (u8)current_settings.volume;
}

void settings_set_volume(u8 volume) {
    if (settings_is_valid_volume(volume)) {
        current_settings.volume = (u16)volume;
    }
}

u8 settings_get_squelch(void) {
    return (u8)current_settings.squelch;
}

void settings_set_squelch(u8 squelch) {
    if (settings_is_valid_squelch(squelch)) {
        current_settings.squelch = (u16)squelch;
    }
}

u8 settings_get_power(void) {
    return (u8)current_settings.power;
}

void settings_set_power(u8 power) {
    if (settings_is_valid_power(power)) {
        current_settings.power = (u16)power;
    }
}

u8 settings_get_backlight(void) {
    return (u8)current_settings.backlight;
}

void settings_set_backlight(u8 seconds) {
    if (settings_is_valid_backlight(seconds)) {
        current_settings.backlight = (u16)seconds;
    }
}

u8 settings_get_ctcss(void) {
    return (u8)current_settings.ctcss;
}

void settings_set_ctcss(u8 tone_index) {
    if (settings_is_valid_ctcss(tone_index)) {
        current_settings.ctcss = (u16)tone_index;
    }
}

// Settings validation helpers
__bit settings_is_valid_frequency(u16 freq_khz) {
    return (freq_khz >= FREQ_MIN && freq_khz <= FREQ_MAX);
}

__bit settings_is_valid_volume(u8 volume) {
    return (volume >= VOLUME_MIN && volume <= VOLUME_MAX);
}

__bit settings_is_valid_squelch(u8 squelch) {
    return (squelch >= SQUELCH_MIN && squelch <= SQUELCH_MAX);
}

__bit settings_is_valid_power(u8 power) {
    return (power >= POWER_MIN && power <= POWER_MAX);
}

__bit settings_is_valid_backlight(u8 seconds) {
    return (seconds >= BACKLIGHT_MIN && seconds <= BACKLIGHT_MAX);
}

__bit settings_is_valid_ctcss(u8 tone_index) {
    return (tone_index >= CTCSS_MIN && tone_index <= CTCSS_MAX);
}