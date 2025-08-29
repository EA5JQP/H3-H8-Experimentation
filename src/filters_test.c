#include "filters_test.h"
#include "filters.h"
#include "at1846s.h"
#include "H8.h"
#include "delay.h"

//=============================================================================
// FILTER TESTING FUNCTIONS
//=============================================================================

/**
 * @brief Test basic filter initialization
 * @return FILTERS_SUCCESS on success, error code on failure
 */
u8 filters_test_init(void)
{
    filters_status_t status;
    
    // Initialize filters
    filters_init();
    
    // Verify all filters are disabled
    if (filters_get_status(&status) != FILTERS_SUCCESS) {
        return FILTERS_ERROR_FREQ_READ;
    }
    
    // Check that all filters are disabled (standby mode)
    if (status.vrx_enabled || status.urx_enabled || 
        status.vtx_enabled || status.utx_enabled) {
        return FILTERS_ERROR_INVALID_PARAM;  // Some filter still enabled
    }
    
    if (status.current_mode != FILTERS_MODE_STANDBY) {
        return FILTERS_ERROR_INVALID_PARAM;  // Not in standby mode
    }
    
    return FILTERS_SUCCESS;
}

/**
 * @brief Test individual filter path control
 * @return FILTERS_SUCCESS on success, error code on failure
 */
u8 filters_test_individual_paths(void)
{
    filters_status_t status;
    u8 result;
    
    // Start with all filters off
    filters_init();
    
    // Test VRX path
    result = filters_set_path(FILTERS_PATH_VRX, 1);
    if (result != FILTERS_SUCCESS) {
        return result;
    }
    
    if (filters_get_status(&status) != FILTERS_SUCCESS) {
        return FILTERS_ERROR_FREQ_READ;
    }
    
    if (!status.vrx_enabled || status.urx_enabled || 
        status.vtx_enabled || status.utx_enabled) {
        return FILTERS_ERROR_INVALID_PARAM;  // Wrong filter state
    }
    
    // Test URX path
    filters_init();  // Reset
    result = filters_set_path(FILTERS_PATH_URX, 1);
    if (result != FILTERS_SUCCESS) {
        return result;
    }
    
    if (filters_get_status(&status) != FILTERS_SUCCESS) {
        return FILTERS_ERROR_FREQ_READ;
    }
    
    if (status.vrx_enabled || !status.urx_enabled || 
        status.vtx_enabled || status.utx_enabled) {
        return FILTERS_ERROR_INVALID_PARAM;  // Wrong filter state
    }
    
    // Test VTX path
    filters_init();  // Reset
    result = filters_set_path(FILTERS_PATH_VTX, 1);
    if (result != FILTERS_SUCCESS) {
        return result;
    }
    
    if (filters_get_status(&status) != FILTERS_SUCCESS) {
        return FILTERS_ERROR_FREQ_READ;
    }
    
    if (status.vrx_enabled || status.urx_enabled || 
        !status.vtx_enabled || status.utx_enabled) {
        return FILTERS_ERROR_INVALID_PARAM;  // Wrong filter state
    }
    
    // Test UTX path
    filters_init();  // Reset
    result = filters_set_path(FILTERS_PATH_UTX, 1);
    if (result != FILTERS_SUCCESS) {
        return result;
    }
    
    if (filters_get_status(&status) != FILTERS_SUCCESS) {
        return FILTERS_ERROR_FREQ_READ;
    }
    
    if (status.vrx_enabled || status.urx_enabled || 
        status.vtx_enabled || !status.utx_enabled) {
        return FILTERS_ERROR_INVALID_PARAM;  // Wrong filter state
    }
    
    // Clean up
    filters_init();
    
    return FILTERS_SUCCESS;
}

/**
 * @brief Test band switching control with simulated frequencies
 * @return FILTERS_SUCCESS on success, error code on failure
 */
u8 filters_test_band_switching(void)
{
    filters_status_t status;
    u8 result;
    
    // Test VHF frequency (below 300 MHz) - RX mode
    result = filters_advanced_band_control(FILTERS_MODE_RX, 146000000UL);  // 146 MHz
    if (result != FILTERS_SUCCESS) {
        return result;
    }
    
    if (filters_get_status(&status) != FILTERS_SUCCESS) {
        return FILTERS_ERROR_FREQ_READ;
    }
    
    // Should enable VRX only
    if (!status.vrx_enabled || status.urx_enabled || 
        status.vtx_enabled || status.utx_enabled) {
        return FILTERS_ERROR_INVALID_PARAM;  // Wrong VHF RX configuration
    }
    
    // Test UHF frequency (above 300 MHz) - RX mode
    result = filters_advanced_band_control(FILTERS_MODE_RX, 446000000UL);  // 446 MHz
    if (result != FILTERS_SUCCESS) {
        return result;
    }
    
    if (filters_get_status(&status) != FILTERS_SUCCESS) {
        return FILTERS_ERROR_FREQ_READ;
    }
    
    // Should enable URX only
    if (status.vrx_enabled || !status.urx_enabled || 
        status.vtx_enabled || status.utx_enabled) {
        return FILTERS_ERROR_INVALID_PARAM;  // Wrong UHF RX configuration
    }
    
    // Test VHF frequency - TX mode
    result = filters_advanced_band_control(FILTERS_MODE_TX, 146000000UL);  // 146 MHz
    if (result != FILTERS_SUCCESS) {
        return result;
    }
    
    if (filters_get_status(&status) != FILTERS_SUCCESS) {
        return FILTERS_ERROR_FREQ_READ;
    }
    
    // Should enable VTX only
    if (status.vrx_enabled || status.urx_enabled || 
        !status.vtx_enabled || status.utx_enabled) {
        return FILTERS_ERROR_INVALID_PARAM;  // Wrong VHF TX configuration
    }
    
    // Test UHF frequency - TX mode
    result = filters_advanced_band_control(FILTERS_MODE_TX, 446000000UL);  // 446 MHz
    if (result != FILTERS_SUCCESS) {
        return result;
    }
    
    if (filters_get_status(&status) != FILTERS_SUCCESS) {
        return FILTERS_ERROR_FREQ_READ;
    }
    
    // Should enable UTX only
    if (status.vrx_enabled || status.urx_enabled || 
        status.vtx_enabled || !status.utx_enabled) {
        return FILTERS_ERROR_INVALID_PARAM;  // Wrong UHF TX configuration
    }
    
    // Test standby mode
    result = filters_advanced_band_control(FILTERS_MODE_STANDBY, 0);
    if (result != FILTERS_SUCCESS) {
        return result;
    }
    
    if (filters_get_status(&status) != FILTERS_SUCCESS) {
        return FILTERS_ERROR_FREQ_READ;
    }
    
    // Should disable all filters
    if (status.vrx_enabled || status.urx_enabled || 
        status.vtx_enabled || status.utx_enabled) {
        return FILTERS_ERROR_INVALID_PARAM;  // Filters not disabled in standby
    }
    
    return FILTERS_SUCCESS;
}

/**
 * @brief Test frequency reading from AT1846S
 * @return FILTERS_SUCCESS on success, error code on failure
 */
u8 filters_test_frequency_reading(void)
{
    u32 freq_hz;
    u8 band;
    
    // Try to read frequency from AT1846S
    freq_hz = filters_get_frequency_hz();
    
    // If AT1846S is not initialized or not responding, this might return 0
    // This is not necessarily an error in test environment
    if (freq_hz == 0) {
        // AT1846S not available - skip frequency-dependent tests
        return FILTERS_SUCCESS;
    }
    
    // Validate frequency is in reasonable range
    if (freq_hz < 30000000UL || freq_hz > 1000000000UL) {
        return FILTERS_ERROR_INVALID_FREQ;
    }
    
    // Test band detection
    band = filters_get_current_band();
    if (band == 0xFF) {
        return FILTERS_ERROR_FREQ_READ;
    }
    
    // Validate band logic
    if (freq_hz < FILTERS_VHF_UHF_BOUNDARY) {
        if (band != 1) {  // Should be VHF
            return FILTERS_ERROR_INVALID_PARAM;
        }
    } else {
        if (band != 0) {  // Should be UHF
            return FILTERS_ERROR_INVALID_PARAM;
        }
    }
    
    return FILTERS_SUCCESS;
}

/**
 * @brief Test error handling and edge cases
 * @return FILTERS_SUCCESS on success, error code on failure
 */
u8 filters_test_error_handling(void)
{
    u8 result;
    
    // Test invalid parameters
    result = filters_set_path(255, 1);  // Invalid path
    if (result == FILTERS_SUCCESS) {
        return FILTERS_ERROR_INVALID_PARAM;  // Should have failed
    }
    
    // Test invalid mode
    result = filters_advanced_band_control(255, 146000000UL);  // Invalid mode
    if (result == FILTERS_SUCCESS) {
        return FILTERS_ERROR_INVALID_PARAM;  // Should have failed
    }
    
    // Test invalid frequency
    result = filters_advanced_band_control(FILTERS_MODE_RX, 10000000UL);  // Too low
    if (result == FILTERS_SUCCESS) {
        return FILTERS_ERROR_INVALID_PARAM;  // Should have failed
    }
    
    result = filters_advanced_band_control(FILTERS_MODE_RX, 2000000000UL);  // Too high
    if (result == FILTERS_SUCCESS) {
        return FILTERS_ERROR_INVALID_PARAM;  // Should have failed
    }
    
    // Test NULL pointer handling
    result = filters_get_status(0);  // NULL pointer
    if (result == FILTERS_SUCCESS) {
        return FILTERS_ERROR_INVALID_PARAM;  // Should have failed
    }
    
    return FILTERS_SUCCESS;
}

/**
 * @brief Test filter switching timing and delays
 * @return FILTERS_SUCCESS on success, error code on failure
 */
u8 filters_test_switching_timing(void)
{
    filters_status_t status;
    u8 result;
    
    // Test rapid switching between modes
    filters_init();
    
    // RX -> TX -> Standby -> RX sequence
    result = filters_advanced_band_control(FILTERS_MODE_RX, 146000000UL);
    if (result != FILTERS_SUCCESS) {
        return result;
    }
    
    delay_ms(0, 10);  // Small delay
    
    result = filters_advanced_band_control(FILTERS_MODE_TX, 146000000UL);
    if (result != FILTERS_SUCCESS) {
        return result;
    }
    
    delay_ms(0, 10);  // Small delay
    
    result = filters_advanced_band_control(FILTERS_MODE_STANDBY, 0);
    if (result != FILTERS_SUCCESS) {
        return result;
    }
    
    delay_ms(0, 10);  // Small delay
    
    result = filters_advanced_band_control(FILTERS_MODE_RX, 446000000UL);  // Switch to UHF
    if (result != FILTERS_SUCCESS) {
        return result;
    }
    
    // Verify final state
    if (filters_get_status(&status) != FILTERS_SUCCESS) {
        return FILTERS_ERROR_FREQ_READ;
    }
    
    // Should be in UHF RX mode
    if (status.vrx_enabled || !status.urx_enabled || 
        status.vtx_enabled || status.utx_enabled) {
        return FILTERS_ERROR_INVALID_PARAM;
    }
    
    // Clean up
    filters_init();
    
    return FILTERS_SUCCESS;
}

/**
 * @brief Run all filter tests
 * @return FILTERS_SUCCESS if all tests pass, error code of first failed test
 */
u8 filters_run_all_tests(void)
{
    u8 result;
    
    // Test 1: Initialization
    result = filters_test_init();
    if (result != FILTERS_SUCCESS) {
        return result;
    }
    
    // Test 2: Individual path control
    result = filters_test_individual_paths();
    if (result != FILTERS_SUCCESS) {
        return result;
    }
    
    // Test 3: Band switching logic
    result = filters_test_band_switching();
    if (result != FILTERS_SUCCESS) {
        return result;
    }
    
    // Test 4: Frequency reading (may pass even if AT1846S not available)
    result = filters_test_frequency_reading();
    if (result != FILTERS_SUCCESS) {
        return result;
    }
    
    // Test 5: Error handling
    result = filters_test_error_handling();
    if (result != FILTERS_SUCCESS) {
        return result;
    }
    
    // Test 6: Switching timing
    result = filters_test_switching_timing();
    if (result != FILTERS_SUCCESS) {
        return result;
    }
    
    return FILTERS_SUCCESS;
}

/**
 * @brief Simple test function for basic filter operation
 * This function tests basic filter control without AT1846S dependency
 */
void filters_simple_test(void)
{
    // Initialize filters
    filters_init();
    
    // Test VHF RX
    band_switching_control(1);  // RX mode with default frequency handling
    delay_ms(0, 100);
    
    // Test standby
    band_switching_control(0);  // Standby mode
    delay_ms(0, 100);
    
    // Test individual paths
    filters_set_path(FILTERS_PATH_VRX, 1);
    delay_ms(0, 100);
    filters_set_path(FILTERS_PATH_VRX, 0);
    
    filters_set_path(FILTERS_PATH_URX, 1);
    delay_ms(0, 100);
    filters_set_path(FILTERS_PATH_URX, 0);
    
    filters_set_path(FILTERS_PATH_VTX, 1);
    delay_ms(0, 100);
    filters_set_path(FILTERS_PATH_VTX, 0);
    
    filters_set_path(FILTERS_PATH_UTX, 1);
    delay_ms(0, 100);
    filters_set_path(FILTERS_PATH_UTX, 0);
    
    // Return to safe state
    filters_init();
}

/**
 * @brief Test filter control with specific frequency scenarios
 * @param test_scenario: Test scenario number
 * @return FILTERS_SUCCESS on success, error code on failure
 */
u8 filters_test_scenario(u8 test_scenario)
{
    u8 result;
    
    switch (test_scenario) {
        case 0:  // VHF Repeater scenario
            result = filters_advanced_band_control(FILTERS_MODE_RX, 145500000UL);  // 145.5 MHz
            if (result != FILTERS_SUCCESS) return result;
            delay_ms(0, 50);
            
            result = filters_advanced_band_control(FILTERS_MODE_TX, 145500000UL);
            if (result != FILTERS_SUCCESS) return result;
            delay_ms(0, 50);
            
            result = filters_advanced_band_control(FILTERS_MODE_STANDBY, 0);
            return result;
            
        case 1:  // UHF Repeater scenario
            result = filters_advanced_band_control(FILTERS_MODE_RX, 446000000UL);  // 446.0 MHz
            if (result != FILTERS_SUCCESS) return result;
            delay_ms(0, 50);
            
            result = filters_advanced_band_control(FILTERS_MODE_TX, 446000000UL);
            if (result != FILTERS_SUCCESS) return result;
            delay_ms(0, 50);
            
            result = filters_advanced_band_control(FILTERS_MODE_STANDBY, 0);
            return result;
            
        case 2:  // Band switching scenario
            result = filters_advanced_band_control(FILTERS_MODE_RX, 146000000UL);  // VHF
            if (result != FILTERS_SUCCESS) return result;
            delay_ms(0, 50);
            
            result = filters_advanced_band_control(FILTERS_MODE_RX, 446000000UL);  // UHF
            if (result != FILTERS_SUCCESS) return result;
            delay_ms(0, 50);
            
            result = filters_advanced_band_control(FILTERS_MODE_STANDBY, 0);
            return result;
            
        default:
            return FILTERS_ERROR_INVALID_PARAM;
    }
}