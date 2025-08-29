#include "filters.h"
#include "at1846s.h"
#include "H8.h"

//=============================================================================
// BAND SWITCHING AND FILTER CONTROL
//=============================================================================

/**
 * @brief Get current frequency in Hz from AT1846S
 * @return Frequency in Hz, 0 on error
 * @note Uses the existing at1846s_get_frequency() which returns kHz
 */
u32 filters_get_frequency_hz(void)
{
    u32 freq_khz = at1846s_get_frequency();
    if (freq_khz == 0) {
        return 0;  // Error reading frequency
    }
    
    return freq_khz * 1000;  // Convert kHz to Hz
}

/**
 * @brief Control band switching filters based on mode and frequency
 * @param mode: 0=All OFF (standby), 1=RX mode, 2=TX mode
 */
void band_switching_control(char mode)
{
    u32 freq_hz = filters_get_frequency_hz();
    
    // If frequency read failed, default to safe state (all off)
    if (freq_hz == 0) {
        mode = 0;
    }

    if (mode == 0) {
        // All OFF (standby mode)
        VRX_P = 1;  // Disable VHF RX
        URX_P = 1;  // Disable UHF RX
        UTX_P = 1;  // Disable UHF TX
        VTX_P = 1;  // Disable VHF TX
    }
    else if (mode == 1) {
        // RX mode - enable appropriate RX path, disable all TX
        if (freq_hz < 300000000UL) {   // Below 300 MHz = VHF
            VRX_P = 0;    // Enable VHF RX
            URX_P = 1;    // Disable UHF RX
        } else {
            URX_P = 0;    // Enable UHF RX
            VRX_P = 1;    // Disable VHF RX
        }
        UTX_P = 1;  // Disable UHF TX
        VTX_P = 1;  // Disable VHF TX
    }
    else if (mode == 2) {
        // TX mode - enable appropriate TX path, disable all RX
        if (freq_hz < 300000000UL) {   // Below 300 MHz = VHF
            VTX_P = 0;    // Enable VHF TX
            UTX_P = 1;    // Disable UHF TX
        } else {
            UTX_P = 0;    // Enable UHF TX
            VTX_P = 1;    // Disable VHF TX
        }
        VRX_P = 1;  // Disable VHF RX
        URX_P = 1;  // Disable UHF RX
    }
    else {
        // Invalid mode - default to standby
        VRX_P = 1;
        URX_P = 1;
        UTX_P = 1;
        VTX_P = 1;
    }
}

/**
 * @brief Get current band based on frequency
 * @return 0=UHF, 1=VHF, 0xFF=Error
 */
u8 filters_get_current_band(void)
{
    u32 freq_hz = filters_get_frequency_hz();
    
    if (freq_hz == 0) {
        return 0xFF;  // Error reading frequency
    }
    
    if (freq_hz < 300000000UL) {
        return 1;  // VHF band
    } else {
        return 0;  // UHF band
    }
}

/**
 * @brief Get current filter status
 * @param status: Pointer to store filter status
 * @return 0 on success, error code on failure
 */
u8 filters_get_status(filters_status_t *status)
{
    if (!status) {
        return FILTERS_ERROR_INVALID_PARAM;
    }
    
    // Read current pin states
    status->vrx_enabled = (VRX_P == 0) ? 1 : 0;
    status->urx_enabled = (URX_P == 0) ? 1 : 0;
    status->vtx_enabled = (VTX_P == 0) ? 1 : 0;
    status->utx_enabled = (UTX_P == 0) ? 1 : 0;
    
    // Determine current mode
    if (status->vrx_enabled || status->urx_enabled) {
        status->current_mode = FILTERS_MODE_RX;
    } else if (status->vtx_enabled || status->utx_enabled) {
        status->current_mode = FILTERS_MODE_TX;
    } else {
        status->current_mode = FILTERS_MODE_STANDBY;
    }
    
    // Get current frequency and band
    status->frequency_hz = filters_get_frequency_hz();
    status->current_band = filters_get_current_band();
    
    return FILTERS_SUCCESS;
}

/**
 * @brief Set specific filter path manually
 * @param path: Filter path to control
 * @param enable: 1 to enable, 0 to disable
 * @return 0 on success, error code on failure
 */
u8 filters_set_path(filters_path_t path, u8 enable)
{
    u8 pin_value = enable ? 0 : 1;  // Active low logic
    
    switch (path) {
        case FILTERS_PATH_VRX:
            VRX_P = pin_value;
            break;
            
        case FILTERS_PATH_URX:
            URX_P = pin_value;
            break;
            
        case FILTERS_PATH_VTX:
            VTX_P = pin_value;
            break;
            
        case FILTERS_PATH_UTX:
            UTX_P = pin_value;
            break;
            
        default:
            return FILTERS_ERROR_INVALID_PARAM;
    }
    
    return FILTERS_SUCCESS;
}

/**
 * @brief Initialize all filter paths to safe state
 */
void filters_init(void)
{
    // Initialize all filter paths to disabled (standby mode)
    VRX_P = 1;  // Disable VHF RX
    URX_P = 1;  // Disable UHF RX
    VTX_P = 1;  // Disable VHF TX
    UTX_P = 1;  // Disable UHF TX
}

/**
 * @brief Advanced band switching with frequency validation
 * @param mode: Operating mode
 * @param force_freq_hz: Force specific frequency (0 = use current AT1846S frequency)
 * @return 0 on success, error code on failure
 */
u8 filters_advanced_band_control(u8 mode, u32 force_freq_hz)
{
    u32 freq_hz;
    
    if (force_freq_hz > 0) {
        // Use forced frequency
        freq_hz = force_freq_hz;
    } else {
        // Use current AT1846S frequency
        freq_hz = filters_get_frequency_hz();
        if (freq_hz == 0) {
            return FILTERS_ERROR_FREQ_READ;
        }
    }
    
    // Validate frequency ranges
    if (freq_hz < 30000000UL || freq_hz > 1000000000UL) {
        return FILTERS_ERROR_INVALID_FREQ;
    }
    
    // Apply band switching logic
    switch (mode) {
        case FILTERS_MODE_STANDBY:
            VRX_P = 1;
            URX_P = 1;
            UTX_P = 1;
            VTX_P = 1;
            break;
            
        case FILTERS_MODE_RX:
            if (freq_hz < 300000000UL) {   // VHF
                VRX_P = 0;
                URX_P = 1;
            } else {                       // UHF
                URX_P = 0;
                VRX_P = 1;
            }
            UTX_P = 1;
            VTX_P = 1;
            break;
            
        case FILTERS_MODE_TX:
            if (freq_hz < 300000000UL) {   // VHF
                VTX_P = 0;
                UTX_P = 1;
            } else {                       // UHF
                UTX_P = 0;
                VTX_P = 1;
            }
            VRX_P = 1;
            URX_P = 1;
            break;
            
        default:
            return FILTERS_ERROR_INVALID_PARAM;
    }
    
    return FILTERS_SUCCESS;
}

/**
 * @brief Get human-readable string for current filter state
 * @param buffer: Buffer to store description
 * @param buffer_size: Size of the buffer
 * @return Length of description string
 */
u8 filters_get_state_description(char *buffer, u8 buffer_size)
{
    filters_status_t status;
    const char *mode_str;
    const char *band_str;
    
    if (!buffer || buffer_size < 20) {
        return 0;
    }
    
    // Get current status
    if (filters_get_status(&status) != FILTERS_SUCCESS) {
        // Simple string copy for error case
        const char *err_msg = "ERROR";
        u8 i = 0;
        while (err_msg[i] != '\0' && i < (buffer_size - 1)) {
            buffer[i] = err_msg[i];
            i++;
        }
        buffer[i] = '\0';
        return i;
    }
    
    // Determine mode string
    switch (status.current_mode) {
        case FILTERS_MODE_STANDBY:
            mode_str = "STANDBY";
            break;
        case FILTERS_MODE_RX:
            mode_str = "RX";
            break;
        case FILTERS_MODE_TX:
            mode_str = "TX";
            break;
        default:
            mode_str = "UNKNOWN";
            break;
    }
    
    // Determine band string
    switch (status.current_band) {
        case 0:
            band_str = "UHF";
            break;
        case 1:
            band_str = "VHF";
            break;
        default:
            band_str = "ERR";
            break;
    }
    
    // Simple string formatting (avoiding sprintf for embedded systems)
    u8 pos = 0;
    const char *src;
    
    // Copy mode string
    src = mode_str;
    while (*src != '\0' && pos < (buffer_size - 5)) {
        buffer[pos++] = *src++;
    }
    
    // Add separator
    buffer[pos++] = ' ';
    
    // Copy band string
    src = band_str;
    while (*src != '\0' && pos < (buffer_size - 1)) {
        buffer[pos++] = *src++;
    }
    
    buffer[pos] = '\0';
    return pos;
}