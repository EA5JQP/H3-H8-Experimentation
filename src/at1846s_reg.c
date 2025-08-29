#include "at1846s_reg.h"
#include "at1846s.h"

// Global register management context - 512+ bytes in external RAM
static __xdata at1846s_reg_mgr_t g_reg_mgr;

// Configuration presets storage (8 presets max)
static __xdata u16 g_reg_presets[8][32];  // Store 32 most important registers per preset - 512 bytes in external RAM
static __xdata u8 g_preset_valid[8] = {0};

// Register access type lookup table - move to ROM to save RAM
static __code const u8 g_reg_access_types[128] = {
    // 0x00-0x0F
    AT1846S_REG_READ_ONLY,  // 0x00 CHIP_ID
    AT1846S_REG_READ_ONLY,  // 0x01 VERSION
    AT1846S_REG_READ_ONLY,  // 0x02 ADCL_MSB
    AT1846S_REG_READ_ONLY,  // 0x03 ADCL_LSB
    AT1846S_REG_READ_WRITE, // 0x04 CTRL_MODE
    AT1846S_REG_READ_WRITE, // 0x05 INT_MODE
    AT1846S_REG_READ_WRITE, // 0x06 GPIO_CTRL
    AT1846S_REG_READ_WRITE, // 0x07 IO_CFG
    AT1846S_REG_READ_WRITE, // 0x08 GPIO_IO
    AT1846S_REG_READ_ONLY,  // 0x09 ADCH_MSB
    AT1846S_REG_READ_WRITE, // 0x0A PA_CTRL
    AT1846S_REG_READ_WRITE, // 0x0B PA_BIAS
    AT1846S_REG_READ_WRITE, // 0x0C FILTER_SWT
    AT1846S_REG_READ_WRITE, // 0x0D DSP_CTRL
    AT1846S_REG_READ_WRITE, // 0x0E VOX_CTRL
    AT1846S_REG_READ_ONLY,  // 0x0F BATTERY
    
    // 0x10-0x1F
    AT1846S_REG_READ_WRITE, // 0x10 CLK_MODE
    AT1846S_REG_READ_WRITE, AT1846S_REG_READ_WRITE, AT1846S_REG_READ_WRITE, AT1846S_REG_READ_WRITE,
    AT1846S_REG_READ_WRITE, // 0x15 TUNING
    AT1846S_REG_READ_WRITE, AT1846S_REG_READ_WRITE, AT1846S_REG_READ_WRITE, AT1846S_REG_READ_WRITE,
    AT1846S_REG_READ_ONLY,  // 0x1A MIC_VOX
    AT1846S_REG_READ_ONLY,  // 0x1B RSSI_NOISE
    AT1846S_REG_READ_ONLY,  // 0x1C FLAG_REG
    AT1846S_REG_READ_WRITE, AT1846S_REG_READ_WRITE,
    AT1846S_REG_READ_WRITE, // 0x1F GPIO_MODE
    
    // 0x20-0x2F
    AT1846S_REG_READ_WRITE, AT1846S_REG_READ_WRITE, AT1846S_REG_READ_WRITE, AT1846S_REG_READ_WRITE,
    AT1846S_REG_READ_WRITE, // 0x24 PLL_LOCK
    AT1846S_REG_READ_WRITE, AT1846S_REG_READ_WRITE, AT1846S_REG_READ_WRITE, AT1846S_REG_READ_WRITE,
    AT1846S_REG_READ_WRITE, // 0x29 FREQ_HIGH
    AT1846S_REG_READ_WRITE, // 0x2A FREQ_LOW
    AT1846S_REG_READ_WRITE, // 0x2B XTAL_FREQ
    AT1846S_REG_READ_WRITE, // 0x2C ADCL_CFG
    AT1846S_REG_READ_WRITE, // 0x2D INT_ENABLE/ADCH_CFG
    AT1846S_REG_READ_WRITE, AT1846S_REG_READ_WRITE,
    
    // 0x30-0x3F
    AT1846S_REG_READ_WRITE, // 0x30 MAIN_CTRL
    AT1846S_REG_READ_WRITE, // 0x31 BAND_SEL
    AT1846S_REG_READ_WRITE, // 0x32 AGC_TARGET
    AT1846S_REG_READ_WRITE, // 0x33 VOL_CTRL
    AT1846S_REG_READ_WRITE, // 0x34 SQ_CTRL
    AT1846S_REG_READ_WRITE, // 0x35 TONE1_FREQ
    AT1846S_REG_READ_WRITE, // 0x36 TONE2_FREQ
    AT1846S_REG_READ_WRITE, AT1846S_REG_READ_WRITE, AT1846S_REG_READ_WRITE,
    AT1846S_REG_READ_WRITE, // 0x3A SQ_AUDIO_CFG
    AT1846S_REG_READ_WRITE, AT1846S_REG_READ_WRITE, AT1846S_REG_READ_WRITE, AT1846S_REG_READ_WRITE, AT1846S_REG_READ_WRITE,
    
    // 0x40-0x4F
    AT1846S_REG_READ_WRITE, // 0x41 VOICE_GAIN
    AT1846S_REG_READ_WRITE, // 0x42 VOICE_CTRL
    AT1846S_REG_READ_WRITE, // 0x43 VOICE_COMP
    AT1846S_REG_READ_WRITE, // 0x44 AUDIO_CTRL
    AT1846S_REG_READ_WRITE, // 0x45 AUDIO_PROC
    AT1846S_REG_READ_WRITE, // 0x46 AUDIO_GAIN
    AT1846S_REG_READ_WRITE, // 0x47 MIC_GAIN
    AT1846S_REG_READ_WRITE,
    AT1846S_REG_READ_WRITE, // 0x49 SQ_THRESH
    AT1846S_REG_READ_WRITE, // 0x4A CTCSS1_FREQ
    AT1846S_REG_READ_WRITE, // 0x4B CDCSS_H
    AT1846S_REG_READ_WRITE, // 0x4C CDCSS_L
    AT1846S_REG_READ_WRITE, // 0x4D CTCSS2_FREQ
    AT1846S_REG_READ_WRITE, // 0x4E SUBAUDIO_CFG
    AT1846S_REG_READ_WRITE, // 0x4F SUBAUDIO_CTL
    
    // 0x50-0x5F
    AT1846S_REG_READ_WRITE, // 0x50 DTMF_CTL
    AT1846S_REG_READ_WRITE, // 0x51 DTMF_MODE
    AT1846S_REG_READ_WRITE, // 0x52 DTMF_TIME
    AT1846S_REG_READ_WRITE, // 0x53 FILTER_CTRL
    AT1846S_REG_READ_WRITE, // 0x54 AUTO_GAIN
    AT1846S_REG_READ_WRITE, // 0x55 VOL_ADJ
    AT1846S_REG_READ_WRITE, // 0x56 NOISE_GATE
    AT1846S_REG_READ_WRITE, // 0x57 DEVIATION
    AT1846S_REG_READ_WRITE, // 0x58 FILTER_CONFIG
    AT1846S_REG_READ_WRITE, // 0x59 TX_DEVIATION
    AT1846S_REG_READ_WRITE, // 0x5A MIC_SENS
    AT1846S_REG_READ_WRITE, // 0x5B CTCSS_THRESH
    AT1846S_REG_READ_ONLY,  // 0x5C RSSI
    AT1846S_REG_READ_ONLY,  // 0x5D STATUS_1
    AT1846S_REG_READ_ONLY,  // 0x5E STATUS_2
    AT1846S_REG_READ_ONLY,  // 0x5F GPIO_STATUS
    
    // 0x60-0x6F
    AT1846S_REG_READ_WRITE, // 0x60 TEST_MODE
    AT1846S_REG_READ_WRITE, AT1846S_REG_READ_WRITE,
    AT1846S_REG_READ_WRITE, // 0x63 CAL_CTRL
    AT1846S_REG_READ_WRITE, // 0x64 VOX_THRESH
    AT1846S_REG_READ_WRITE, AT1846S_REG_READ_WRITE,
    AT1846S_REG_READ_WRITE, // 0x67 DTMF_C0
    AT1846S_REG_READ_WRITE, // 0x68 DTMF_C1
    AT1846S_REG_READ_WRITE, // 0x69 DTMF_C2
    AT1846S_REG_READ_WRITE, // 0x6A DTMF_C3
    AT1846S_REG_READ_WRITE, // 0x6B DTMF_C4
    AT1846S_REG_READ_WRITE, // 0x6C DTMF_C5
    AT1846S_REG_READ_WRITE, // 0x6D DTMF_C6
    AT1846S_REG_READ_WRITE, // 0x6E DTMF_C7
    AT1846S_REG_READ_WRITE, // 0x6F DTMF_C0_2ND
    
    // 0x70-0x7F
    AT1846S_REG_READ_WRITE, // 0x70 DTMF_C1_2ND
    AT1846S_REG_READ_WRITE, // 0x71 DTMF_C2_2ND
    AT1846S_REG_READ_WRITE, // 0x72 DTMF_C3_2ND
    AT1846S_REG_READ_WRITE, // 0x73 DTMF_C4_2ND
    AT1846S_REG_READ_WRITE, // 0x74 DTMF_C5_2ND
    AT1846S_REG_READ_WRITE, // 0x75 DTMF_C6_2ND
    AT1846S_REG_READ_WRITE, // 0x76 DTMF_C7_2ND
    AT1846S_REG_READ_WRITE, AT1846S_REG_READ_WRITE,
    AT1846S_REG_READ_WRITE, // 0x79 DTMF_DIRECT
    AT1846S_REG_READ_WRITE, // 0x7A DTMF_ENABLE
    AT1846S_REG_READ_WRITE, // 0x7B DTMF_TIME_CFG
    AT1846S_REG_READ_WRITE, AT1846S_REG_READ_WRITE,
    AT1846S_REG_READ_WRITE, // 0x7E DTMF_CODE
    AT1846S_REG_READ_WRITE  // 0x7F
};

//=============================================================================
// REGISTER MANAGEMENT IMPLEMENTATION
//=============================================================================

u8 at1846s_reg_init(u8 enable_cache)
{
    u8 i;
    
    // Initialize register management context
    for (i = 0; i < 128; i++) {
        g_reg_mgr.cache[i].value = 0;
        g_reg_mgr.cache[i].dirty = 0;
        g_reg_mgr.cache[i].valid = 0;
        g_reg_mgr.cache[i].access_type = g_reg_access_types[i];
    }
    
    g_reg_mgr.cache_enabled = enable_cache;
    g_reg_mgr.auto_flush = 0;  // Disabled by default
    g_reg_mgr.read_count = 0;
    g_reg_mgr.write_count = 0;
    g_reg_mgr.cache_hits = 0;
    g_reg_mgr.cache_misses = 0;
    
    // Initialize presets as invalid
    for (i = 0; i < 8; i++) {
        g_preset_valid[i] = 0;
    }
    
    return AT1846S_SUCCESS;
}

void at1846s_reg_set_cache_enabled(u8 enable)
{
    g_reg_mgr.cache_enabled = enable;
    if (!enable) {
        // Flush and invalidate cache when disabling
        at1846s_reg_flush_cache();
        at1846s_reg_invalidate_all_cache();
    }
}

void at1846s_reg_set_auto_flush(u8 enable)
{
    g_reg_mgr.auto_flush = enable;
}

u8 at1846s_reg_read(u8 reg_addr, u16 *data)
{
    at1846s_reg_cache_t *cache_entry;
    u8 result;
    
    if (!data) {
        return AT1846S_ERROR_INVALID_PARAM;
    }
    
    if (reg_addr >= 128) {
        return AT1846S_REG_ERROR_INVALID_ADDRESS;
    }
    
    cache_entry = &g_reg_mgr.cache[reg_addr];
    g_reg_mgr.read_count++;
    
    // Check if caching is enabled and cache entry is valid
    if (g_reg_mgr.cache_enabled && cache_entry->valid) {
        *data = cache_entry->value;
        g_reg_mgr.cache_hits++;
        return AT1846S_SUCCESS;
    }
    
    // Cache miss - read from hardware
    g_reg_mgr.cache_misses++;
    result = at1846s_reg_hw_read(reg_addr, data);
    
    if (result == AT1846S_SUCCESS && g_reg_mgr.cache_enabled) {
        // Update cache
        cache_entry->value = *data;
        cache_entry->valid = 1;
        cache_entry->dirty = 0;
    }
    
    return result;
}

u8 at1846s_reg_write(u8 reg_addr, u16 data)
{
    at1846s_reg_cache_t *cache_entry;
    u8 result;
    
    if (reg_addr >= 128) {
        return AT1846S_REG_ERROR_INVALID_ADDRESS;
    }
    
    if (at1846s_reg_is_read_only(reg_addr)) {
        return AT1846S_REG_ERROR_READ_ONLY;
    }
    
    cache_entry = &g_reg_mgr.cache[reg_addr];
    g_reg_mgr.write_count++;
    
    if (g_reg_mgr.cache_enabled) {
        // Update cache
        cache_entry->value = data;
        cache_entry->valid = 1;
        cache_entry->dirty = 1;
        
        // Auto-flush if enabled
        if (g_reg_mgr.auto_flush) {
            result = at1846s_reg_hw_write(reg_addr, data);
            if (result == AT1846S_SUCCESS) {
                cache_entry->dirty = 0;
            }
            return result;
        }
        
        return AT1846S_SUCCESS;  // Cached write
    } else {
        // Direct hardware write
        return at1846s_reg_hw_write(reg_addr, data);
    }
}

u8 at1846s_reg_write_verify(u8 reg_addr, u16 data)
{
    u8 result;
    u16 readback;
    
    result = at1846s_reg_write(reg_addr, data);
    if (result != AT1846S_SUCCESS) {
        return result;
    }
    
    // Flush to hardware if cached
    if (g_reg_mgr.cache_enabled && !g_reg_mgr.auto_flush) {
        result = at1846s_reg_flush_cache();
        if (result != AT1846S_SUCCESS) {
            return result;
        }
    }
    
    // Verify by reading back (skip read-only registers)
    if (!at1846s_reg_is_read_only(reg_addr)) {
        // Invalidate cache to force hardware read
        at1846s_reg_invalidate_cache(reg_addr);
        
        result = at1846s_reg_read(reg_addr, &readback);
        if (result != AT1846S_SUCCESS) {
            return result;
        }
        
        if (readback != data) {
            return AT1846S_ERROR_COMM_FAIL;
        }
    }
    
    return AT1846S_SUCCESS;
}

u8 at1846s_reg_modify_field(u8 reg_addr, u16 field_mask, u8 field_pos, u16 value)
{
    u16 reg_value;
    u8 result;
    
    // Validate field value
    if (!at1846s_reg_validate_field(reg_addr, field_mask, field_pos, value)) {
        return AT1846S_REG_ERROR_INVALID_DATA;
    }
    
    // Read current register value
    result = at1846s_reg_read(reg_addr, &reg_value);
    if (result != AT1846S_SUCCESS) {
        return result;
    }
    
    // Modify the field
    reg_value = (reg_value & ~field_mask) | ((value << field_pos) & field_mask);
    
    // Write back modified value
    return at1846s_reg_write(reg_addr, reg_value);
}

u8 at1846s_reg_set_bits(u8 reg_addr, u16 bits_mask)
{
    u16 reg_value;
    u8 result;
    
    result = at1846s_reg_read(reg_addr, &reg_value);
    if (result != AT1846S_SUCCESS) {
        return result;
    }
    
    reg_value |= bits_mask;
    return at1846s_reg_write(reg_addr, reg_value);
}

u8 at1846s_reg_clear_bits(u8 reg_addr, u16 bits_mask)
{
    u16 reg_value;
    u8 result;
    
    result = at1846s_reg_read(reg_addr, &reg_value);
    if (result != AT1846S_SUCCESS) {
        return result;
    }
    
    reg_value &= ~bits_mask;
    return at1846s_reg_write(reg_addr, reg_value);
}

u8 at1846s_reg_bulk_write(const at1846s_reg_write_t *reg_data, u8 count)
{
    u8 i, result;
    
    if (!reg_data || count == 0) {
        return AT1846S_ERROR_INVALID_PARAM;
    }
    
    for (i = 0; i < count; i++) {
        result = at1846s_reg_write(reg_data[i].reg_addr, reg_data[i].data);
        if (result != AT1846S_SUCCESS) {
            return result;
        }
    }
    
    return AT1846S_SUCCESS;
}

u8 at1846s_reg_bulk_read(const u8 *reg_addrs, u16 *reg_data, u8 count)
{
    u8 i, result;
    
    if (!reg_addrs || !reg_data || count == 0) {
        return AT1846S_ERROR_INVALID_PARAM;
    }
    
    for (i = 0; i < count; i++) {
        result = at1846s_reg_read(reg_addrs[i], &reg_data[i]);
        if (result != AT1846S_SUCCESS) {
            return result;
        }
    }
    
    return AT1846S_SUCCESS;
}

u8 at1846s_reg_flush_cache(void)
{
    u8 i, result;
    at1846s_reg_cache_t *cache_entry;
    
    if (!g_reg_mgr.cache_enabled) {
        return AT1846S_SUCCESS;
    }
    
    for (i = 0; i < 128; i++) {
        cache_entry = &g_reg_mgr.cache[i];
        
        if (cache_entry->dirty && cache_entry->valid) {
            result = at1846s_reg_hw_write(i, cache_entry->value);
            if (result != AT1846S_SUCCESS) {
                return result;
            }
            cache_entry->dirty = 0;
        }
    }
    
    return AT1846S_SUCCESS;
}

void at1846s_reg_invalidate_cache(u8 reg_addr)
{
    if (reg_addr < 128) {
        g_reg_mgr.cache[reg_addr].valid = 0;
        g_reg_mgr.cache[reg_addr].dirty = 0;
    }
}

void at1846s_reg_invalidate_all_cache(void)
{
    u8 i;
    
    for (i = 0; i < 128; i++) {
        g_reg_mgr.cache[i].valid = 0;
        g_reg_mgr.cache[i].dirty = 0;
    }
}

u8 at1846s_reg_restore_cache(void)
{
    u8 i, result;
    u16 data;
    at1846s_reg_cache_t *cache_entry;
    
    if (!g_reg_mgr.cache_enabled) {
        return AT1846S_SUCCESS;
    }
    
    for (i = 0; i < 128; i++) {
        cache_entry = &g_reg_mgr.cache[i];
        
        // Only restore readable registers
        if (cache_entry->access_type != AT1846S_REG_WRITE_ONLY) {
            result = at1846s_reg_hw_read(i, &data);
            if (result == AT1846S_SUCCESS) {
                cache_entry->value = data;
                cache_entry->valid = 1;
                cache_entry->dirty = 0;
            }
        }
    }
    
    return AT1846S_SUCCESS;
}

//=============================================================================
// REGISTER VALIDATION IMPLEMENTATION
//=============================================================================

u8 at1846s_reg_is_valid_address(u8 reg_addr)
{
    return (reg_addr < 128) ? 1 : 0;
}

u8 at1846s_reg_is_read_only(u8 reg_addr)
{
    if (reg_addr >= 128) {
        return 0;
    }
    return (g_reg_access_types[reg_addr] == AT1846S_REG_READ_ONLY) ? 1 : 0;
}

u8 at1846s_reg_validate_data(u8 reg_addr, u16 data)
{
    // Basic validation - could be extended with register-specific limits
    switch (reg_addr) {
        case AT1846S_REG_VOL_CTRL:
            return (data <= AT1846S_VOLUME_MAX) ? 1 : 0;
            
        case AT1846S_REG_MIC_GAIN:
            return (data <= AT1846S_MIC_GAIN_MAX) ? 1 : 0;
            
        case AT1846S_REG_SQ_CTRL:
            return (data <= AT1846S_SQUELCH_MAX) ? 1 : 0;
            
        default:
            return 1;  // Accept all values for unknown registers
    }
}

u8 at1846s_reg_validate_field(u8 reg_addr, u16 field_mask, u8 field_pos, u16 value)
{
    u16 max_value;
    
    // Calculate maximum value for this field
    max_value = field_mask >> field_pos;
    
    return (value <= max_value) ? 1 : 0;
}

u8 at1846s_reg_get_access_type(u8 reg_addr)
{
    if (reg_addr >= 128) {
        return 0;
    }
    return g_reg_access_types[reg_addr];
}

//=============================================================================
// LOW-LEVEL HARDWARE INTERFACE IMPLEMENTATION
//=============================================================================

u8 at1846s_reg_hw_read(u8 reg_addr, u16 *data)
{
    // Use existing AT1846S hardware functions
    return at1846s_read_register(reg_addr, data);
}

u8 at1846s_reg_hw_write(u8 reg_addr, u16 data)
{
    u8 data_low = (u8)(data & 0xFF);
    u8 data_high = (u8)(data >> 8);
    
    // Use existing AT1846S hardware functions
    at1846s_write_register(data_low, data_high, reg_addr);
    return AT1846S_SUCCESS;
}

//=============================================================================
// DIAGNOSTIC AND MONITORING IMPLEMENTATION
//=============================================================================

void at1846s_reg_get_stats(at1846s_reg_mgr_t *mgr)
{
    if (mgr) {
        *mgr = g_reg_mgr;
    }
}

void at1846s_reg_reset_stats(void)
{
    g_reg_mgr.read_count = 0;
    g_reg_mgr.write_count = 0;
    g_reg_mgr.cache_hits = 0;
    g_reg_mgr.cache_misses = 0;
}

void at1846s_reg_dump_cache(u8 start_addr, u8 count)
{
    u8 i;
    at1846s_reg_cache_t *cache_entry;
    
    for (i = start_addr; i < (start_addr + count) && i < 128; i++) {
        cache_entry = &g_reg_mgr.cache[i];
        
        // In a real implementation, this would output via UART or debug interface
        // For now, just ensure the function exists
        (void)cache_entry; // Suppress unused variable warning
    }
}

u8 at1846s_reg_dump_hardware(u8 start_addr, u8 count)
{
    u8 i, result;
    u16 data;
    
    for (i = start_addr; i < (start_addr + count) && i < 128; i++) {
        if (g_reg_access_types[i] != AT1846S_REG_WRITE_ONLY) {
            result = at1846s_reg_hw_read(i, &data);
            if (result != AT1846S_SUCCESS) {
                return result;
            }
            
            // In a real implementation, this would output via UART or debug interface
            (void)data; // Suppress unused variable warning
        }
    }
    
    return AT1846S_SUCCESS;
}

u8 at1846s_reg_compare_cache_hardware(u8 start_addr, u8 count)
{
    u8 i, mismatches = 0;
    u16 hw_data;
    at1846s_reg_cache_t *cache_entry;
    
    if (!g_reg_mgr.cache_enabled) {
        return 0;
    }
    
    for (i = start_addr; i < (start_addr + count) && i < 128; i++) {
        if (g_reg_access_types[i] != AT1846S_REG_WRITE_ONLY) {
            cache_entry = &g_reg_mgr.cache[i];
            
            if (cache_entry->valid) {
                if (at1846s_reg_hw_read(i, &hw_data) == AT1846S_SUCCESS) {
                    if (cache_entry->value != hw_data) {
                        mismatches++;
                    }
                }
            }
        }
    }
    
    return mismatches;
}

//=============================================================================
// CONFIGURATION PRESET IMPLEMENTATION
//=============================================================================

// Important registers to save in presets
static __code const u8 g_preset_registers[] = {
    AT1846S_REG_MAIN_CTRL,      AT1846S_REG_BAND_SEL,       AT1846S_REG_VOL_CTRL,
    AT1846S_REG_SQ_CTRL,        AT1846S_REG_FREQ_HIGH,      AT1846S_REG_FREQ_LOW,
    AT1846S_REG_VOICE_GAIN,     AT1846S_REG_AUDIO_CTRL,     AT1846S_REG_MIC_GAIN,
    AT1846S_REG_CTCSS1_FREQ,    AT1846S_REG_CDCSS_H,        AT1846S_REG_CDCSS_L,
    AT1846S_REG_CTCSS2_FREQ,    AT1846S_REG_SUBAUDIO_CFG,   AT1846S_REG_VOX_CTRL,
    AT1846S_REG_PA_CTRL,        AT1846S_REG_SQ_THRESH,      AT1846S_REG_TX_DEVIATION,
    AT1846S_REG_FILTER_CONFIG,  AT1846S_REG_GPIO_MODE,      AT1846S_REG_TONE1_FREQ,
    AT1846S_REG_TONE2_FREQ,     AT1846S_REG_DTMF_CTL,       AT1846S_REG_DTMF_MODE,
    AT1846S_REG_CTRL_MODE,      AT1846S_REG_AGC_TARGET,     AT1846S_REG_DEVIATION,
    AT1846S_REG_NOISE_GATE,     AT1846S_REG_AUTO_GAIN,      AT1846S_REG_VOL_ADJ,
    AT1846S_REG_MIC_SENS,       AT1846S_REG_FILTER_CTRL
};

#define PRESET_REG_COUNT (sizeof(g_preset_registers) / sizeof(g_preset_registers[0]))

u8 at1846s_reg_save_preset(u8 preset_id)
{
    u8 i, result;
    
    if (preset_id >= 8) {
        return AT1846S_REG_ERROR_PRESET_INVALID;
    }
    
    for (i = 0; i < PRESET_REG_COUNT; i++) {
        result = at1846s_reg_read(g_preset_registers[i], &g_reg_presets[preset_id][i]);
        if (result != AT1846S_SUCCESS) {
            return result;
        }
    }
    
    g_preset_valid[preset_id] = 1;
    return AT1846S_SUCCESS;
}

u8 at1846s_reg_load_preset(u8 preset_id)
{
    u8 i, result;
    
    if (preset_id >= 8 || !g_preset_valid[preset_id]) {
        return AT1846S_REG_ERROR_PRESET_INVALID;
    }
    
    for (i = 0; i < PRESET_REG_COUNT; i++) {
        result = at1846s_reg_write(g_preset_registers[i], g_reg_presets[preset_id][i]);
        if (result != AT1846S_SUCCESS) {
            return result;
        }
    }
    
    // Flush to hardware if caching is enabled
    if (g_reg_mgr.cache_enabled) {
        return at1846s_reg_flush_cache();
    }
    
    return AT1846S_SUCCESS;
}