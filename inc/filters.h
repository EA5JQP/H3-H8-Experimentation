#ifndef FILTERS_H
#define FILTERS_H

#include "types.h"

//=============================================================================
// FILTER CONTROL DEFINITIONS
//=============================================================================

// Filter mode definitions
#define FILTERS_MODE_STANDBY            0   // All filters off
#define FILTERS_MODE_RX                 1   // RX mode (appropriate RX filter on)
#define FILTERS_MODE_TX                 2   // TX mode (appropriate TX filter on)

// Filter path definitions
typedef enum {
    FILTERS_PATH_VRX = 0,               // VHF RX path
    FILTERS_PATH_URX = 1,               // UHF RX path
    FILTERS_PATH_VTX = 2,               // VHF TX path
    FILTERS_PATH_UTX = 3                // UHF TX path
} filters_path_t;

// Filter status structure
typedef struct {
    u8  vrx_enabled;                    // VHF RX filter enabled
    u8  urx_enabled;                    // UHF RX filter enabled
    u8  vtx_enabled;                    // VHF TX filter enabled
    u8  utx_enabled;                    // UHF TX filter enabled
    u8  current_mode;                   // Current operating mode
    u8  current_band;                   // Current band (0=UHF, 1=VHF)
    u32 frequency_hz;                   // Current frequency in Hz
} filters_status_t;

// Error codes
#define FILTERS_SUCCESS                 0   // Operation successful
#define FILTERS_ERROR_INVALID_PARAM     1   // Invalid parameter
#define FILTERS_ERROR_FREQ_READ         2   // Error reading frequency
#define FILTERS_ERROR_INVALID_FREQ      3   // Invalid frequency range

// Frequency band boundary (Hz)
#define FILTERS_VHF_UHF_BOUNDARY        300000000UL  // 300 MHz

//=============================================================================
// FUNCTION DECLARATIONS
//=============================================================================

/**
 * @brief Initialize filter control system
 */
void filters_init(void);

/**
 * @brief Get current frequency in Hz from AT1846S
 * @return Frequency in Hz, 0 on error
 */
u32 filters_get_frequency_hz(void);

/**
 * @brief Control band switching filters based on mode and frequency
 * @param mode: 0=All OFF (standby), 1=RX mode, 2=TX mode
 */
void band_switching_control(char mode);

/**
 * @brief Get current band based on frequency
 * @return 0=UHF, 1=VHF, 0xFF=Error
 */
u8 filters_get_current_band(void);

/**
 * @brief Get current filter status
 * @param status: Pointer to store filter status
 * @return FILTERS_SUCCESS on success, error code on failure
 */
u8 filters_get_status(filters_status_t *status);

/**
 * @brief Set specific filter path manually
 * @param path: Filter path to control
 * @param enable: 1 to enable, 0 to disable
 * @return FILTERS_SUCCESS on success, error code on failure
 */
u8 filters_set_path(filters_path_t path, u8 enable);

/**
 * @brief Advanced band switching with frequency validation
 * @param mode: Operating mode (FILTERS_MODE_*)
 * @param force_freq_hz: Force specific frequency (0 = use current AT1846S frequency)
 * @return FILTERS_SUCCESS on success, error code on failure
 */
u8 filters_advanced_band_control(u8 mode, u32 force_freq_hz);

/**
 * @brief Get human-readable string for current filter state
 * @param buffer: Buffer to store description
 * @param buffer_size: Size of the buffer
 * @return Length of description string
 */
u8 filters_get_state_description(char *buffer, u8 buffer_size);

#endif // FILTERS_H