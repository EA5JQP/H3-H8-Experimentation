#ifndef AT1846S_H
#define AT1846S_H

#include "TA3782F.h"
#include "types.h"
#include "H8.h"
#include "delay.h"

// AT1846S Register Definitions
// Core System Registers
#define AT1846S_REG_CHIP_ID       0x00  // Chip ID register (read-only)
#define AT1846S_REG_VERSION       0x01  // Version register (read-only)
#define AT1846S_REG_ADCL_MSB      0x02  // ADC Low MSB (read-only)
#define AT1846S_REG_ADCL_LSB      0x03  // ADC Low LSB (read-only)
#define AT1846S_REG_CTRL_MODE     0x04  // Control mode register
#define AT1846S_REG_INT_MODE      0x05  // Interrupt mode register
#define AT1846S_REG_GPIO_CTRL     0x06  // GPIO control register
#define AT1846S_REG_IO_CFG        0x07  // I/O configuration register
#define AT1846S_REG_GPIO_IO       0x08  // GPIO I/O register
#define AT1846S_REG_ADCH_MSB      0x09  // ADC High MSB (read-only)
#define AT1846S_REG_PA_CTRL       0x0A  // PA control and bias
#define AT1846S_REG_PA_BIAS       0x0B  // PA bias control
#define AT1846S_REG_FILTER_SWT    0x0C  // Filter switch register
#define AT1846S_REG_DSP_CTRL      0x0D  // DSP control register
#define AT1846S_REG_VOX_CTRL      0x0E  // VOX control register
#define AT1846S_REG_BATTERY       0x0F  // Battery voltage (read-only)
#define AT1846S_REG_CLK_MODE      0x10  // Clock mode register
#define AT1846S_REG_GPIO_MODE     0x1F  // GPIO mode register

// Frequency Control Registers
#define AT1846S_REG_FREQ_HIGH     0x29  // Frequency high word
#define AT1846S_REG_FREQ_LOW      0x2A  // Frequency low word
#define AT1846S_REG_XTAL_FREQ     0x2B  // Crystal frequency
#define AT1846S_REG_ADCL_CFG      0x2C  // ADCL configuration
#define AT1846S_REG_ADCH_CFG      0x2D  // ADCH configuration

// Main Control and Power
#define AT1846S_REG_MAIN_CTRL     0x30  // Main control register
#define AT1846S_REG_BAND_SEL      0x31  // Band selection register
#define AT1846S_REG_SCAN_CTRL     0x32  // Scan control register
#define AT1846S_REG_VOL_CTRL      0x33  // Volume control register
#define AT1846S_REG_SQ_CTRL       0x34  // Squelch control register

// Audio and Voice Control
#define AT1846S_REG_VOICE_GAIN    0x41  // Voice gain control
#define AT1846S_REG_VOICE_CTRL    0x42  // Voice control register
#define AT1846S_REG_VOICE_COMP    0x43  // Voice compression
#define AT1846S_REG_AUDIO_CTRL    0x44  // Audio control
#define AT1846S_REG_AUDIO_PROC    0x45  // Audio processing
#define AT1846S_REG_AUDIO_GAIN    0x46  // Audio gain control
#define AT1846S_REG_MIC_GAIN      0x47  // Microphone gain

// Sub-Audio (CTCSS/CDCSS) Registers
#define AT1846S_REG_SUBAUDIO_1    0x4A  // Sub-audio register 1
#define AT1846S_REG_SUBAUDIO_2    0x4B  // Sub-audio register 2
#define AT1846S_REG_CTCSS_FREQ    0x4E  // CTCSS frequency control

// Advanced Audio and DSP
#define AT1846S_REG_DTMF_CTL      0x50  // DTMF control register
#define AT1846S_REG_DTMF_MODE     0x51  // DTMF mode register
#define AT1846S_REG_DTMF_TIME     0x52  // DTMF timing register
#define AT1846S_REG_FILTER_CTRL   0x53  // Filter control register
#define AT1846S_REG_AUTO_GAIN     0x54  // Auto gain control
#define AT1846S_REG_VOL_ADJ       0x55  // Volume adjustment
#define AT1846S_REG_NOISE_GATE    0x56  // Noise gate control
#define AT1846S_REG_DEVIATION     0x57  // Deviation control
#define AT1846S_REG_COMPANDER     0x59  // Compander control
#define AT1846S_REG_EMPHASIS      0x5A  // Pre/de-emphasis control

// Signal Quality and Status
#define AT1846S_REG_GPIO_STATUS   0x5F  // GPIO status (read-only)

// Test and Calibration
#define AT1846S_REG_TEST_MODE     0x60  // Test mode register
#define AT1846S_REG_CAL_CTRL      0x63  // Calibration control
#define AT1846S_REG_CAL_DATA      0x64  // Calibration data

// Bit Definitions for Main Control Register (0x30)
#define AT1846S_MAIN_CTRL_TX_ON         0x0020  // TX enable
#define AT1846S_MAIN_CTRL_RX_ON         0x0040  // RX enable
#define AT1846S_MAIN_CTRL_CHIP_CAL      0x2000  // Chip calibration
#define AT1846S_MAIN_CTRL_PWR_DWN       0x0000  // Power down mode
#define AT1846S_MAIN_CTRL_RESET         0x0001  // Soft reset

// Bit Definitions for Control Mode Register (0x04)
#define AT1846S_CTRL_MODE_TX            0x0020  // Transmit mode
#define AT1846S_CTRL_MODE_RX            0x0040  // Receive mode
#define AT1846S_CTRL_MODE_MUTE          0x0080  // Mute audio

// Bit Definitions for VOX Control Register (0x0E)
#define AT1846S_VOX_ENABLE              0x8000  // VOX enable
#define AT1846S_VOX_DELAY_MASK          0x7000  // VOX delay mask
#define AT1846S_VOX_SENSITIVITY_MASK    0x0F00  // VOX sensitivity mask

// Bit Definitions for Status Registers
#define AT1846S_STATUS_TX_ON            0x0020  // TX active flag
#define AT1846S_STATUS_RX_ON            0x0040  // RX active flag
#define AT1846S_STATUS_SQ_OPEN          0x0001  // Squelch open flag
#define AT1846S_STATUS_CTCSS_PHASE      0x0008  // CTCSS phase detect
#define AT1846S_STATUS_CDCSS_PHASE      0x0010  // CDCSS phase detect

// CTCSS Tone Frequencies (Hz * 10 for integer math)
#define AT1846S_CTCSS_670               670    // 67.0 Hz
#define AT1846S_CTCSS_719               719    // 71.9 Hz
#define AT1846S_CTCSS_744               744    // 74.4 Hz
#define AT1846S_CTCSS_770               770    // 77.0 Hz
#define AT1846S_CTCSS_797               797    // 79.7 Hz
#define AT1846S_CTCSS_825               825    // 82.5 Hz
#define AT1846S_CTCSS_854               854    // 85.4 Hz
#define AT1846S_CTCSS_885               885    // 88.5 Hz
#define AT1846S_CTCSS_915               915    // 91.5 Hz
#define AT1846S_CTCSS_948               948    // 94.8 Hz
#define AT1846S_CTCSS_974               974    // 97.4 Hz
#define AT1846S_CTCSS_1000              1000   // 100.0 Hz
#define AT1846S_CTCSS_1035              1035   // 103.5 Hz
#define AT1846S_CTCSS_1072              1072   // 107.2 Hz
#define AT1846S_CTCSS_1109              1109   // 110.9 Hz
#define AT1846S_CTCSS_1148              1148   // 114.8 Hz
#define AT1846S_CTCSS_1188              1188   // 118.8 Hz
#define AT1846S_CTCSS_1230              1230   // 123.0 Hz
#define AT1846S_CTCSS_1273              1273   // 127.3 Hz
#define AT1846S_CTCSS_1318              1318   // 131.8 Hz
#define AT1846S_CTCSS_1365              1365   // 136.5 Hz
#define AT1846S_CTCSS_1413              1413   // 141.3 Hz
#define AT1846S_CTCSS_1462              1462   // 146.2 Hz
#define AT1846S_CTCSS_1514              1514   // 151.4 Hz
#define AT1846S_CTCSS_1567              1567   // 156.7 Hz
#define AT1846S_CTCSS_1622              1622   // 162.2 Hz
#define AT1846S_CTCSS_1679              1679   // 167.9 Hz
#define AT1846S_CTCSS_1738              1738   // 173.8 Hz
#define AT1846S_CTCSS_1799              1799   // 179.9 Hz
#define AT1846S_CTCSS_1862              1862   // 186.2 Hz
#define AT1846S_CTCSS_1928              1928   // 192.8 Hz
#define AT1846S_CTCSS_2035              2035   // 203.5 Hz
#define AT1846S_CTCSS_2107              2107   // 210.7 Hz
#define AT1846S_CTCSS_2181              2181   // 218.1 Hz
#define AT1846S_CTCSS_2257              2257   // 225.7 Hz
#define AT1846S_CTCSS_2336              2336   // 233.6 Hz
#define AT1846S_CTCSS_2418              2418   // 241.8 Hz
#define AT1846S_CTCSS_2503              2503   // 250.3 Hz

// Power Control Values
#define AT1846S_PWR_DOWN                0x0000  // Complete power down
#define AT1846S_PWR_INIT                0x4004  // Initial power configuration
#define AT1846S_PWR_ENABLE              0x40A4  // Power enable with features
#define AT1846S_PWR_CONFIG              0x40A6  // Additional power config
#define AT1846S_PWR_FINAL               0x4006  // Final power state

// Band Selection
#define AT1846S_BAND_UHF_400_520        0x0000  // UHF 400-520 MHz
#define AT1846S_BAND_VHF_134_174        0x0020  // VHF 134-174 MHz
#define AT1846S_BAND_VHF_200_260        0x0040  // VHF 200-260 MHz

// Function Return Codes
#define AT1846S_SUCCESS                 0       // Operation successful
#define AT1846S_ERROR_TIMEOUT           1       // Communication timeout
#define AT1846S_ERROR_INVALID_PARAM     2       // Invalid parameter
#define AT1846S_ERROR_NOT_READY         3       // Chip not ready
#define AT1846S_ERROR_COMM_FAIL         4       // Communication failure

#define spi_setup_delay()   __builtin_delay_cycles(5)
#define spi_hold_delay()    __builtin_delay_cycles(8)

// Core Functions
void at1846s_init(void);
u8 at1846s_read_register(u8 reg_addr, u16 *data);
void at1846s_write_register(u8 data_low, u8 data_high, u8 reg_addr);
u8 at1846s_write_register_with_verify(u8 data_low, u8 data_high, u8 reg_addr);
u8 at1846s_spi_read_byte(void);
void at1846s_spi_transceive(u8 spi_command, u8 *reg_high, u8 *reg_low);
u8 at1846s_verify_chip_id(void);

// Power and Mode Control
u8 at1846s_set_power_mode(u8 enable);
u8 at1846s_set_tx_mode(u8 enable);
u8 at1846s_set_rx_mode(u8 enable);
u8 at1846s_set_sleep_mode(u8 enable);
u8 at1846s_reset_chip(void);

// Frequency Control
void at1846s_set_frequency(u32 freq_khz);
u8 at1846s_set_band(u8 band);
u32 at1846s_get_frequency(void);

// Audio and Voice Control
u8 at1846s_set_volume(u8 volume);
u8 at1846s_set_mic_gain(u8 gain);
u8 at1846s_set_voice_gain(u8 gain);
u8 at1846s_mute_audio(u8 mute);
u8 at1846s_set_audio_processing(u8 enable);

// Squelch Control
u8 at1846s_set_squelch(u8 level);
u8 at1846s_get_squelch_status(void);
u8 at1846s_set_noise_gate(u8 level);

// Sub-Audio (CTCSS/CDCSS) Functions
u8 at1846s_set_ctcss_tx(u16 tone_freq);
u8 at1846s_set_ctcss_rx(u16 tone_freq);
u8 at1846s_disable_ctcss(void);
u8 at1846s_set_cdcss_tx(u16 code);
u8 at1846s_set_cdcss_rx(u16 code);
u8 at1846s_disable_cdcss(void);
u8 at1846s_get_ctcss_detect(void);
u8 at1846s_get_cdcss_detect(void);

// VOX Functions
u8 at1846s_enable_vox(u8 sensitivity, u8 delay);
u8 at1846s_disable_vox(void);
u8 at1846s_get_vox_status(void);

// DTMF Functions
u8 at1846s_enable_dtmf(void);
u8 at1846s_disable_dtmf(void);
u8 at1846s_send_dtmf_tone(u8 digit, u16 duration_ms);
u8 at1846s_get_dtmf_detect(void);

// Signal Quality and Status
u8 at1846s_get_rssi(void);
u16 at1846s_get_status(void);
u8 at1846s_get_battery_voltage(void);
u8 at1846s_get_tx_status(void);
u8 at1846s_get_rx_status(void);

// GPIO and Interrupt Management
u8 at1846s_config_gpio(u8 pin, u8 mode);
u8 at1846s_set_gpio(u8 pin, u8 value);
u8 at1846s_get_gpio(u8 pin);
u8 at1846s_enable_interrupts(u16 int_mask);
u8 at1846s_disable_interrupts(void);
u16 at1846s_get_interrupt_status(void);

// Advanced Configuration
u8 at1846s_set_pa_bias(u8 bias);
u8 at1846s_set_deviation(u8 deviation);
u8 at1846s_set_emphasis(u8 enable);
u8 at1846s_set_compander(u8 enable);
u8 at1846s_calibrate(void);

// Diagnostic Functions
u8 at1846s_self_test(void);
u8 at1846s_read_all_registers(u16 *reg_values);
void at1846s_dump_registers(void);

//=============================================================================
// ENHANCED HIGH-LEVEL API DECLARATIONS
//=============================================================================

// Complete radio configuration structure
typedef struct {
    // Frequency settings
    u32 frequency_khz;          // Operating frequency in kHz
    u8  band;                   // Band selection (UHF/VHF)
    
    // Audio settings  
    u8  volume;                 // Volume level (0-15)
    u8  mic_gain;               // Microphone gain (0-31) 
    u8  voice_gain;             // Voice gain TX (0-127)
    u8  squelch_level;          // Squelch threshold (0-15)
    
    // Sub-audio settings
    u16 ctcss_tx_freq;          // CTCSS TX frequency (Hz*10)
    u16 ctcss_rx_freq;          // CTCSS RX frequency (Hz*10)
    u16 cdcss_tx_code;          // CDCSS TX code
    u16 cdcss_rx_code;          // CDCSS RX code
    u8  subaudio_mode;          // Sub-audio mode (none/CTCSS/CDCSS)
    
    // VOX settings
    u8  vox_enabled;            // VOX enable flag
    u8  vox_sensitivity;        // VOX sensitivity (0-15)
    u8  vox_delay;              // VOX delay (0-7)
    
    // Power settings
    u8  pa_bias;                // PA bias voltage (0-63)
    u8  tx_power;               // TX power level (0-15)
    
    // Advanced settings
    u8  emphasis;               // Pre/de-emphasis enable
    u8  compander;              // Compander enable
    u8  noise_gate;             // Noise gate level
    u8  deviation;              // Deviation setting
} at1846s_config_t;

// Configuration preset definitions
typedef enum {
    AT1846S_PRESET_SIMPLEX_VHF = 0,    // Basic VHF simplex
    AT1846S_PRESET_SIMPLEX_UHF = 1,    // Basic UHF simplex
    AT1846S_PRESET_REPEATER_VHF = 2,   // VHF repeater with CTCSS
    AT1846S_PRESET_REPEATER_UHF = 3,   // UHF repeater with CTCSS
    AT1846S_PRESET_EMERGENCY = 4,      // Emergency/high power
    AT1846S_PRESET_LOW_POWER = 5,      // Low power/battery saver
    AT1846S_PRESET_DIGITAL = 6,        // Digital mode optimized
    AT1846S_PRESET_CUSTOM = 7          // User-defined custom
} at1846s_preset_id_t;

// Sub-audio mode definitions
typedef enum {
    AT1846S_SUBAUDIO_NONE = 0,
    AT1846S_SUBAUDIO_CTCSS = 1,
    AT1846S_SUBAUDIO_CDCSS = 2,
    AT1846S_SUBAUDIO_BOTH = 3
} at1846s_subaudio_mode_t;
typedef struct at1846s_status at1846s_status_t;
typedef struct at1846s_reg_stats at1846s_reg_stats_t;

// Enhanced initialization and configuration
u8 at1846s_init_enhanced(u8 enable_cache);
u8 at1846s_apply_config(const at1846s_config_t *config);
u8 at1846s_load_preset(at1846s_preset_id_t preset_id, at1846s_config_t *config);
u8 at1846s_save_preset(at1846s_preset_id_t preset_id);
u8 at1846s_get_config(at1846s_config_t *config);

// Quick operations
u8 at1846s_quick_channel_change(u32 freq_khz, u16 ctcss_freq);

// Advanced status and diagnostics
u8 at1846s_get_comprehensive_status(at1846s_status_t *status);
u8 at1846s_get_register_stats(at1846s_reg_stats_t *stats);
u8 at1846s_comprehensive_self_test(void);

// Value mapping and conversion functions
u16 at1846s_ctcss_freq_to_reg(u16 freq_hz_x10);
u16 at1846s_reg_to_ctcss_freq(u16 reg_value);
u16 at1846s_pa_bias_to_mv(u8 bias_value);
u8 at1846s_mv_to_pa_bias(u16 voltage_mv);
u8 at1846s_freq_to_registers(u32 freq_khz, u16 *freq_high, u16 *freq_low);
u32 at1846s_registers_to_freq(u16 freq_high, u16 freq_low);
i16 at1846s_rssi_to_dbm(u16 rssi_reg);
u16 at1846s_battery_reg_to_mv(u16 battery_reg);
u16 at1846s_find_closest_ctcss(u16 target_freq);
u8 at1846s_validate_frequency_for_band(u32 freq_khz, u8 band);
u8 at1846s_power_percent_to_reg(u8 power_percent);
u8 at1846s_reg_to_power_percent(u8 reg_value);
i16 at1846s_squelch_level_to_dbm(u8 squelch_level);
u8 at1846s_dbm_to_squelch_level(i16 threshold_dbm);
u8 at1846s_create_simple_config(u32 freq_khz, u8 volume, u16 ctcss_freq, at1846s_config_t *config);

#endif // AT1846S_H