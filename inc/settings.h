#ifndef SETTINGS_H
#define SETTINGS_H

#include "TA3782F.h"
#include "types.h"
#include "eeprom.h"

// EEPROM Layout - Settings storage addresses
#define SETTINGS_BASE_ADDR      0x0100  // Start settings at offset 256
#define SETTINGS_MAGIC_ADDR     0x0100  // Magic number to detect valid settings
#define SETTINGS_VERSION_ADDR   0x0102  // Settings version
#define SETTINGS_CHECKSUM_ADDR  0x0104  // Simple checksum

// Individual setting addresses (2 bytes each for 16-bit values)
#define SETTING_FREQUENCY_ADDR  0x0110  // Operating frequency (kHz)
#define SETTING_VOLUME_ADDR     0x0112  // Volume level (0-15)
#define SETTING_SQUELCH_ADDR    0x0114  // Squelch level (0-8)
#define SETTING_POWER_ADDR      0x0116  // Power level (0-7)
#define SETTING_BACKLIGHT_ADDR  0x0118  // Backlight timeout (seconds)
#define SETTING_CTCSS_ADDR      0x011A  // CTCSS tone index

// Settings validation constants
#define SETTINGS_MAGIC          0x4839  // "H8" in ASCII + 1
#define SETTINGS_VERSION        0x0001  // Current settings version
#define SETTINGS_SIZE           32      // Total settings block size

// Default values
#define DEFAULT_FREQUENCY       446000  // 446 MHz in kHz
#define DEFAULT_VOLUME          8       // Mid-range volume
#define DEFAULT_SQUELCH         3       // Moderate squelch
#define DEFAULT_POWER           4       // Mid power
#define DEFAULT_BACKLIGHT       30      // 30 second timeout
#define DEFAULT_CTCSS           0       // No CTCSS

// Value ranges (for menu system validation)
#define FREQ_MIN                136000  // 136 MHz minimum
#define FREQ_MAX                520000  // 520 MHz maximum
#define FREQ_STEP               5       // 5 kHz steps

#define VOLUME_MIN              0       // Mute
#define VOLUME_MAX              15      // Maximum volume
#define VOLUME_STEP             1       // Single steps

#define SQUELCH_MIN             0       // Open squelch
#define SQUELCH_MAX             8       // Tight squelch
#define SQUELCH_STEP            1       // Single steps

#define POWER_MIN               0       // Minimum power
#define POWER_MAX               7       // Maximum power
#define POWER_STEP              1       // Single steps

#define BACKLIGHT_MIN           5       // 5 seconds minimum
#define BACKLIGHT_MAX           120     // 2 minutes maximum
#define BACKLIGHT_STEP          5       // 5 second steps

#define CTCSS_MIN               0       // No tone
#define CTCSS_MAX               38      // Standard CTCSS tone count
#define CTCSS_STEP              1       // Single steps

// Settings structure (for RAM operations)
typedef struct {
    u16 magic;
    u16 version;
    u16 frequency;
    u16 volume;
    u16 squelch;
    u16 power;
    u16 backlight;
    u16 ctcss;
    u16 checksum;
} settings_t;

// Global settings instance
extern __xdata settings_t current_settings;

// Settings function declarations
void settings_init(void);
__bit settings_load(void);
__bit settings_save(void);
void settings_load_defaults(void);
u16 settings_calculate_checksum(void);
__bit settings_validate(void);

// Individual setting accessors
u16 settings_get_frequency(void);
void settings_set_frequency(u16 freq_khz);
u8 settings_get_volume(void);
void settings_set_volume(u8 volume);
u8 settings_get_squelch(void);
void settings_set_squelch(u8 squelch);
u8 settings_get_power(void);
void settings_set_power(u8 power);
u8 settings_get_backlight(void);
void settings_set_backlight(u8 seconds);
u8 settings_get_ctcss(void);
void settings_set_ctcss(u8 tone_index);

// Settings validation helpers
__bit settings_is_valid_frequency(u16 freq_khz);
__bit settings_is_valid_volume(u8 volume);
__bit settings_is_valid_squelch(u8 squelch);
__bit settings_is_valid_power(u8 power);
__bit settings_is_valid_backlight(u8 seconds);
__bit settings_is_valid_ctcss(u8 tone_index);

#endif // SETTINGS_H