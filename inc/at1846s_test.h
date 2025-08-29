#ifndef AT1846S_TEST_H
#define AT1846S_TEST_H

#include "at1846s.h"
#include "at1846s_registers.h"
#include "types.h"
#include "delay.h"

// Test Function Return Codes (same as main AT1846S codes)
#define AT1846S_TEST_SUCCESS                 0       // Operation successful
#define AT1846S_TEST_ERROR_TIMEOUT           1       // Communication timeout
#define AT1846S_TEST_ERROR_INVALID_PARAM     2       // Invalid parameter
#define AT1846S_TEST_ERROR_NOT_READY         3       // Chip not ready
#define AT1846S_TEST_ERROR_COMM_FAIL         4       // Communication failure

// Safe Communication Test Functions (NO TX - Safe without antenna)
u8 at1846s_test_basic_communication(void);
u8 at1846s_test_register_readwrite(void);
u8 at1846s_test_power_control(void);
u8 at1846s_test_read_only_registers(void);
u8 at1846s_test_frequency_settings(void);
u8 at1846s_test_audio_controls(void);
u8 at1846s_run_safe_tests(void);

// Frequency Test Diagnostic Variables (for debugging)
extern u32 at1846s_diag_orig_freq;
extern u32 at1846s_diag_test_freq;
extern u32 at1846s_diag_read_freq;
extern u16 at1846s_diag_orig_band;
extern u16 at1846s_diag_freq_high_reg;
extern u16 at1846s_diag_freq_low_reg;

#endif // AT1846S_TEST_H