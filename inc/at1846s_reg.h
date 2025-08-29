#ifndef AT1846S_REG_H
#define AT1846S_REG_H

#include "types.h"
#include "at1846s_registers.h"

//=============================================================================
// REGISTER MANAGEMENT SYSTEM
//=============================================================================

// Register cache entry structure
typedef struct {
    u16 value;          // Cached register value
    u8  dirty:1;        // 1 if cache is dirty (needs write)
    u8  valid:1;        // 1 if cache contains valid data
    u8  access_type:2;  // AT1846S_REG_READ_ONLY, etc.
    u8  reserved:4;
} at1846s_reg_cache_t;

// Register management context
typedef struct {
    at1846s_reg_cache_t cache[128];     // Register cache (0x00-0x7F)
    u8 cache_enabled;                   // 1 if caching is enabled
    u8 auto_flush;                      // 1 if auto-flush dirty registers
    u16 read_count;                     // Statistics: read operations
    u16 write_count;                    // Statistics: write operations  
    u16 cache_hits;                     // Statistics: cache hit count
    u16 cache_misses;                   // Statistics: cache miss count
} at1846s_reg_mgr_t;

//=============================================================================
// REGISTER MANAGEMENT API
//=============================================================================

/**
 * @brief Initialize the register management system
 * @param enable_cache: 1 to enable register caching, 0 to disable
 * @return AT1846S_SUCCESS on success
 */
u8 at1846s_reg_init(u8 enable_cache);

/**
 * @brief Enable/disable register caching
 * @param enable: 1 to enable, 0 to disable
 */
void at1846s_reg_set_cache_enabled(u8 enable);

/**
 * @brief Enable/disable auto-flush of dirty registers
 * @param enable: 1 to enable, 0 to disable
 */
void at1846s_reg_set_auto_flush(u8 enable);

/**
 * @brief Read register with caching support
 * @param reg_addr: Register address (0x00-0x7F)
 * @param data: Pointer to store read data
 * @return AT1846S_SUCCESS on success, error code on failure
 */
u8 at1846s_reg_read(u8 reg_addr, u16 *data);

/**
 * @brief Write register with caching support
 * @param reg_addr: Register address (0x00-0x7F) 
 * @param data: Data to write
 * @return AT1846S_SUCCESS on success, error code on failure
 */
u8 at1846s_reg_write(u8 reg_addr, u16 data);

/**
 * @brief Write register with verification
 * @param reg_addr: Register address
 * @param data: Data to write
 * @return AT1846S_SUCCESS on success, error code on failure
 */
u8 at1846s_reg_write_verify(u8 reg_addr, u16 data);

/**
 * @brief Modify specific bit field in register
 * @param reg_addr: Register address
 * @param field_mask: Bit field mask
 * @param field_pos: Bit field position
 * @param value: New value for the field
 * @return AT1846S_SUCCESS on success, error code on failure
 */
u8 at1846s_reg_modify_field(u8 reg_addr, u16 field_mask, u8 field_pos, u16 value);

/**
 * @brief Set specific bits in register
 * @param reg_addr: Register address
 * @param bits_mask: Mask of bits to set
 * @return AT1846S_SUCCESS on success, error code on failure
 */
u8 at1846s_reg_set_bits(u8 reg_addr, u16 bits_mask);

/**
 * @brief Clear specific bits in register
 * @param reg_addr: Register address
 * @param bits_mask: Mask of bits to clear
 * @return AT1846S_SUCCESS on success, error code on failure
 */
u8 at1846s_reg_clear_bits(u8 reg_addr, u16 bits_mask);

/**
 * @brief Bulk register write operation
 * @param reg_data: Array of register address/data pairs
 * @param count: Number of register writes
 * @return AT1846S_SUCCESS on success, error code on failure
 */
typedef struct {
    u8 reg_addr;
    u16 data;
} at1846s_reg_write_t;

u8 at1846s_reg_bulk_write(const at1846s_reg_write_t *reg_data, u8 count);

/**
 * @brief Bulk register read operation
 * @param reg_addrs: Array of register addresses to read
 * @param reg_data: Array to store read data
 * @param count: Number of registers to read
 * @return AT1846S_SUCCESS on success, error code on failure
 */
u8 at1846s_reg_bulk_read(const u8 *reg_addrs, u16 *reg_data, u8 count);

/**
 * @brief Flush all dirty cached registers to hardware
 * @return AT1846S_SUCCESS on success, error code on failure
 */
u8 at1846s_reg_flush_cache(void);

/**
 * @brief Invalidate specific register cache entry
 * @param reg_addr: Register address to invalidate
 */
void at1846s_reg_invalidate_cache(u8 reg_addr);

/**
 * @brief Invalidate all register cache entries
 */
void at1846s_reg_invalidate_all_cache(void);

/**
 * @brief Restore register cache from hardware
 * @return AT1846S_SUCCESS on success, error code on failure
 */
u8 at1846s_reg_restore_cache(void);

//=============================================================================
// REGISTER VALIDATION API
//=============================================================================

/**
 * @brief Validate register address
 * @param reg_addr: Register address to validate
 * @return 1 if valid, 0 if invalid
 */
u8 at1846s_reg_is_valid_address(u8 reg_addr);

/**
 * @brief Check if register is read-only
 * @param reg_addr: Register address
 * @return 1 if read-only, 0 if writable
 */
u8 at1846s_reg_is_read_only(u8 reg_addr);

/**
 * @brief Validate register data value
 * @param reg_addr: Register address
 * @param data: Data value to validate
 * @return 1 if valid, 0 if invalid
 */
u8 at1846s_reg_validate_data(u8 reg_addr, u16 data);

/**
 * @brief Validate field value for specific register field
 * @param reg_addr: Register address
 * @param field_mask: Field mask
 * @param field_pos: Field position
 * @param value: Value to validate
 * @return 1 if valid, 0 if invalid
 */
u8 at1846s_reg_validate_field(u8 reg_addr, u16 field_mask, u8 field_pos, u16 value);

//=============================================================================
// DIAGNOSTIC AND MONITORING API
//=============================================================================

/**
 * @brief Get register management statistics
 * @param mgr: Pointer to store statistics
 */
void at1846s_reg_get_stats(at1846s_reg_mgr_t *mgr);

/**
 * @brief Reset register management statistics
 */
void at1846s_reg_reset_stats(void);

/**
 * @brief Dump register cache contents (for debugging)
 * @param start_addr: Starting register address
 * @param count: Number of registers to dump
 */
void at1846s_reg_dump_cache(u8 start_addr, u8 count);

/**
 * @brief Dump all registers from hardware (for debugging)
 * @param start_addr: Starting register address
 * @param count: Number of registers to dump
 * @return AT1846S_SUCCESS on success, error code on failure
 */
u8 at1846s_reg_dump_hardware(u8 start_addr, u8 count);

/**
 * @brief Compare cache with hardware registers
 * @param start_addr: Starting register address
 * @param count: Number of registers to compare
 * @return Number of mismatches found
 */
u8 at1846s_reg_compare_cache_hardware(u8 start_addr, u8 count);

//=============================================================================
// CONFIGURATION PRESET API
//=============================================================================

/**
 * @brief Save current register configuration to preset
 * @param preset_id: Preset identifier (0-7)
 * @return AT1846S_SUCCESS on success, error code on failure
 */
u8 at1846s_reg_save_preset(u8 preset_id);

/**
 * @brief Load register configuration from preset
 * @param preset_id: Preset identifier (0-7)
 * @return AT1846S_SUCCESS on success, error code on failure
 */
u8 at1846s_reg_load_preset(u8 preset_id);

/**
 * @brief Get register access type
 * @param reg_addr: Register address
 * @return Access type (AT1846S_REG_READ_ONLY, etc.)
 */
u8 at1846s_reg_get_access_type(u8 reg_addr);

//=============================================================================
// LOW-LEVEL HARDWARE INTERFACE
//=============================================================================

/**
 * @brief Raw register read (bypasses cache)
 * @param reg_addr: Register address
 * @param data: Pointer to store read data
 * @return AT1846S_SUCCESS on success, error code on failure
 */
u8 at1846s_reg_hw_read(u8 reg_addr, u16 *data);

/**
 * @brief Raw register write (bypasses cache)
 * @param reg_addr: Register address
 * @param data: Data to write
 * @return AT1846S_SUCCESS on success, error code on failure
 */
u8 at1846s_reg_hw_write(u8 reg_addr, u16 data);

//=============================================================================
// MACRO HELPERS FOR COMMON OPERATIONS
//=============================================================================

#define AT1846S_REG_SET_FIELD(reg, field, value) \
    at1846s_reg_modify_field(reg, AT1846S_##field##_MASK, AT1846S_##field##_POS, value)

#define AT1846S_REG_GET_FIELD(reg, field, result) \
    do { \
        u16 reg_val; \
        if (at1846s_reg_read(reg, &reg_val) == AT1846S_SUCCESS) { \
            result = AT1846S_GET_FIELD(reg_val, field); \
        } \
    } while(0)

#define AT1846S_REG_SET_BIT(reg, bit) \
    at1846s_reg_set_bits(reg, (1 << bit))

#define AT1846S_REG_CLEAR_BIT(reg, bit) \
    at1846s_reg_clear_bits(reg, (1 << bit))

#define AT1846S_REG_IS_BIT_SET(reg, bit, result) \
    do { \
        u16 reg_val; \
        if (at1846s_reg_read(reg, &reg_val) == AT1846S_SUCCESS) { \
            result = (reg_val & (1 << bit)) ? 1 : 0; \
        } \
    } while(0)

//=============================================================================
// ERROR CODES (extending existing AT1846S error codes)
//=============================================================================

#define AT1846S_REG_ERROR_INVALID_ADDRESS  0x10
#define AT1846S_REG_ERROR_READ_ONLY         0x11
#define AT1846S_REG_ERROR_INVALID_DATA      0x12
#define AT1846S_REG_ERROR_CACHE_FULL        0x13
#define AT1846S_REG_ERROR_PRESET_INVALID    0x14

#endif // AT1846S_REG_H