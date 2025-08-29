#ifndef FILTERS_TEST_H
#define FILTERS_TEST_H

#include "types.h"
#include "filters.h"

//=============================================================================
// FILTER TEST FUNCTION DECLARATIONS
//=============================================================================

/**
 * @brief Test basic filter initialization
 * @return FILTERS_SUCCESS on success, error code on failure
 */
u8 filters_test_init(void);

/**
 * @brief Test individual filter path control
 * @return FILTERS_SUCCESS on success, error code on failure
 */
u8 filters_test_individual_paths(void);

/**
 * @brief Test band switching control with simulated frequencies
 * @return FILTERS_SUCCESS on success, error code on failure
 */
u8 filters_test_band_switching(void);

/**
 * @brief Test frequency reading from AT1846S
 * @return FILTERS_SUCCESS on success, error code on failure
 */
u8 filters_test_frequency_reading(void);

/**
 * @brief Test error handling and edge cases
 * @return FILTERS_SUCCESS on success, error code on failure
 */
u8 filters_test_error_handling(void);

/**
 * @brief Test filter switching timing and delays
 * @return FILTERS_SUCCESS on success, error code on failure
 */
u8 filters_test_switching_timing(void);

/**
 * @brief Run all filter tests
 * @return FILTERS_SUCCESS if all tests pass, error code of first failed test
 */
u8 filters_run_all_tests(void);

/**
 * @brief Simple test function for basic filter operation
 * This function tests basic filter control without AT1846S dependency
 */
void filters_simple_test(void);

/**
 * @brief Test filter control with specific frequency scenarios
 * @param test_scenario: Test scenario number (0-2)
 * @return FILTERS_SUCCESS on success, error code on failure
 */
u8 filters_test_scenario(u8 test_scenario);

//=============================================================================
// TEST SCENARIO DEFINITIONS
//=============================================================================

#define FILTERS_TEST_VHF_REPEATER       0   // VHF repeater test scenario
#define FILTERS_TEST_UHF_REPEATER       1   // UHF repeater test scenario
#define FILTERS_TEST_BAND_SWITCHING     2   // Band switching test scenario

#endif // FILTERS_TEST_H