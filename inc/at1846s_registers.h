#ifndef AT1846S_REGISTERS_H
#define AT1846S_REGISTERS_H

#include "types.h"

// AT1846S Complete Register Map and Bit Field Definitions
// Based on AT1846S Datasheet V0.3 and Programming Guide V1.4

//=============================================================================
// REGISTER ADDRESS DEFINITIONS
//=============================================================================

// Core System Registers (0x00-0x1F)
#define AT1846S_REG_CHIP_ID                0x00    // Chip ID (0x1846) - Read Only
#define AT1846S_REG_VERSION                0x01    // Version ID - Read Only
#define AT1846S_REG_ADCL_MSB               0x02    // ADC Low MSB - Read Only
#define AT1846S_REG_ADCL_LSB               0x03    // ADC Low LSB - Read Only
#define AT1846S_REG_CTRL_MODE              0x04    // Control mode register
#define AT1846S_REG_INT_MODE               0x05    // Interrupt mode register
#define AT1846S_REG_GPIO_CTRL              0x06    // GPIO control register
#define AT1846S_REG_IO_CFG                 0x07    // I/O configuration register
#define AT1846S_REG_GPIO_IO                0x08    // GPIO I/O register
#define AT1846S_REG_ADCH_MSB               0x09    // ADC High MSB - Read Only
#define AT1846S_REG_PA_CTRL                0x0A    // PA control and bias
#define AT1846S_REG_PA_BIAS                0x0B    // PA bias control
#define AT1846S_REG_FILTER_SWT             0x0C    // Filter switch register
#define AT1846S_REG_DSP_CTRL               0x0D    // DSP control register
#define AT1846S_REG_VOX_CTRL               0x0E    // VOX control register
#define AT1846S_REG_BATTERY                0x0F    // Battery voltage - Read Only
#define AT1846S_REG_CLK_MODE               0x10    // Clock mode register
#define AT1846S_REG_TUNING                 0x15    // IF filter tuning
#define AT1846S_REG_MIC_VOX                0x1A    // Mic and VOX levels - Read Only
#define AT1846S_REG_RSSI_NOISE             0x1B    // RSSI and noise - Read Only
#define AT1846S_REG_FLAG_REG               0x1C    // Flag register - Read Only
#define AT1846S_REG_GPIO_MODE              0x1F    // GPIO mode register

// PLL and Control Registers (0x20-0x2F)
#define AT1846S_REG_PLL_LOCK               0x24    // PLL lock detection
#define AT1846S_REG_INT_ENABLE             0x2D    // Interrupt enable

// Frequency Control Registers (0x29-0x2B)
#define AT1846S_REG_FREQ_HIGH              0x29    // Frequency high word
#define AT1846S_REG_FREQ_LOW               0x2A    // Frequency low word
#define AT1846S_REG_XTAL_FREQ              0x2B    // Crystal frequency
#define AT1846S_REG_ADCL_CFG               0x2C    // ADCL configuration
#define AT1846S_REG_ADCH_CFG               0x2D    // ADCH configuration

// Main Control and Power (0x30-0x3F)
#define AT1846S_REG_MAIN_CTRL              0x30    // Main control register
#define AT1846S_REG_BAND_SEL               0x31    // Band selection register
#define AT1846S_REG_AGC_TARGET             0x32    // AGC target power
#define AT1846S_REG_VOL_CTRL               0x33    // Volume control register
#define AT1846S_REG_SQ_CTRL                0x34    // Squelch control register
#define AT1846S_REG_TONE1_FREQ             0x35    // Tone1 frequency
#define AT1846S_REG_TONE2_FREQ             0x36    // Tone2 frequency
#define AT1846S_REG_SQ_AUDIO_CFG           0x3A    // Squelch and audio configuration

// Audio and Voice Control (0x40-0x4F)
#define AT1846S_REG_VOICE_GAIN             0x41    // Voice gain control
#define AT1846S_REG_VOICE_CTRL             0x42    // Voice control register
#define AT1846S_REG_VOICE_COMP             0x43    // Voice compression
#define AT1846S_REG_AUDIO_CTRL             0x44    // Audio control
#define AT1846S_REG_AUDIO_PROC             0x45    // Audio processing
#define AT1846S_REG_AUDIO_GAIN             0x46    // Audio gain control
#define AT1846S_REG_MIC_GAIN               0x47    // Microphone gain
#define AT1846S_REG_SQ_THRESH              0x49    // Squelch thresholds

// Sub-Audio (CTCSS/CDCSS) Registers (0x4A-0x4F)
#define AT1846S_REG_CTCSS1_FREQ            0x4A    // CTCSS1 frequency
#define AT1846S_REG_CDCSS_H                0x4B    // CDCSS high byte
#define AT1846S_REG_CDCSS_L                0x4C    // CDCSS low byte
#define AT1846S_REG_CTCSS2_FREQ            0x4D    // CTCSS2 frequency
#define AT1846S_REG_SUBAUDIO_CFG           0x4E    // Sub-audio configuration
#define AT1846S_REG_SUBAUDIO_CTL           0x4F    // Sub-audio control

// Advanced Audio and DSP (0x50-0x5F)
#define AT1846S_REG_DTMF_CTL               0x50    // DTMF control register
#define AT1846S_REG_DTMF_MODE              0x51    // DTMF mode register
#define AT1846S_REG_DTMF_TIME              0x52    // DTMF timing register
#define AT1846S_REG_FILTER_CTRL            0x53    // Filter control register
#define AT1846S_REG_AUTO_GAIN              0x54    // Auto gain control
#define AT1846S_REG_VOL_ADJ                0x55    // Volume adjustment
#define AT1846S_REG_NOISE_GATE             0x56    // Noise gate control
#define AT1846S_REG_DEVIATION              0x57    // Deviation control
#define AT1846S_REG_FILTER_CONFIG          0x58    // Filter configuration
#define AT1846S_REG_TX_DEVIATION           0x59    // TX deviation settings
#define AT1846S_REG_MIC_SENS               0x5A    // Microphone sensitivity
#define AT1846S_REG_CTCSS_THRESH           0x5B    // CTCSS thresholds

// Signal Quality and Status (0x5C-0x5F)
#define AT1846S_REG_RSSI                   0x5C    // RSSI - Read Only
#define AT1846S_REG_STATUS_1               0x5D    // Status register 1 - Read Only
#define AT1846S_REG_STATUS_2               0x5E    // Status register 2 - Read Only
#define AT1846S_REG_GPIO_STATUS            0x5F    // GPIO status - Read Only

// Test and Calibration (0x60-0x66)
#define AT1846S_REG_TEST_MODE              0x60    // Test mode register
#define AT1846S_REG_CAL_CTRL               0x63    // Calibration control
#define AT1846S_REG_CAL_DATA               0x64    // Calibration data
#define AT1846S_REG_VOX_THRESH             0x64    // VOX threshold settings

// DTMF Coefficient Registers (0x67-0x7E)
#define AT1846S_REG_DTMF_C0                0x67    // DTMF coefficient 0 (697Hz)
#define AT1846S_REG_DTMF_C1                0x68    // DTMF coefficient 1 (770Hz)
#define AT1846S_REG_DTMF_C2                0x69    // DTMF coefficient 2 (852Hz)
#define AT1846S_REG_DTMF_C3                0x6A    // DTMF coefficient 3 (941Hz)
#define AT1846S_REG_DTMF_C4                0x6B    // DTMF coefficient 4 (1209Hz)
#define AT1846S_REG_DTMF_C5                0x6C    // DTMF coefficient 5 (1336Hz)
#define AT1846S_REG_DTMF_C6                0x6D    // DTMF coefficient 6 (1477Hz)
#define AT1846S_REG_DTMF_C7                0x6E    // DTMF coefficient 7 (1633Hz)
#define AT1846S_REG_DTMF_C0_2ND            0x6F    // DTMF coeff 0 2nd harmonic
#define AT1846S_REG_DTMF_C1_2ND            0x70    // DTMF coeff 1 2nd harmonic
#define AT1846S_REG_DTMF_C2_2ND            0x71    // DTMF coeff 2 2nd harmonic
#define AT1846S_REG_DTMF_C3_2ND            0x72    // DTMF coeff 3 2nd harmonic
#define AT1846S_REG_DTMF_C4_2ND            0x73    // DTMF coeff 4 2nd harmonic
#define AT1846S_REG_DTMF_C5_2ND            0x74    // DTMF coeff 5 2nd harmonic
#define AT1846S_REG_DTMF_C6_2ND            0x75    // DTMF coeff 6 2nd harmonic
#define AT1846S_REG_DTMF_C7_2ND            0x76    // DTMF coeff 7 2nd harmonic
#define AT1846S_REG_DTMF_DIRECT            0x79    // DTMF direct mode
#define AT1846S_REG_DTMF_ENABLE            0x7A    // DTMF enable
#define AT1846S_REG_DTMF_TIME_CFG          0x7B    // DTMF timing configuration
#define AT1846S_REG_DTMF_CODE              0x7E    // DTMF code output/input

//=============================================================================
// REGISTER ACCESS TYPE DEFINITIONS
//=============================================================================

#define AT1846S_REG_READ_ONLY              0x01
#define AT1846S_REG_WRITE_ONLY             0x02
#define AT1846S_REG_READ_WRITE             0x03

//=============================================================================
// MAIN CONTROL REGISTER (0x30) BIT FIELD DEFINITIONS
//=============================================================================

#define AT1846S_MAIN_CTRL_SOFT_RST_POS     0
#define AT1846S_MAIN_CTRL_SOFT_RST_MASK    0x0001
#define AT1846S_MAIN_CTRL_CHIP_CAL_POS     1
#define AT1846S_MAIN_CTRL_CHIP_CAL_MASK    0x0002
#define AT1846S_MAIN_CTRL_PDN_REG_POS      2
#define AT1846S_MAIN_CTRL_PDN_REG_MASK     0x0004
#define AT1846S_MAIN_CTRL_SQ_ON_POS        3
#define AT1846S_MAIN_CTRL_SQ_ON_MASK       0x0008
#define AT1846S_MAIN_CTRL_VOX_ON_POS       4
#define AT1846S_MAIN_CTRL_VOX_ON_MASK      0x0010
#define AT1846S_MAIN_CTRL_RX_ON_POS        5
#define AT1846S_MAIN_CTRL_RX_ON_MASK       0x0020
#define AT1846S_MAIN_CTRL_TX_ON_POS        6
#define AT1846S_MAIN_CTRL_TX_ON_MASK       0x0040
#define AT1846S_MAIN_CTRL_MUTE_POS         7
#define AT1846S_MAIN_CTRL_MUTE_MASK        0x0080
#define AT1846S_MAIN_CTRL_DIRECT_REG_POS   10
#define AT1846S_MAIN_CTRL_DIRECT_REG_MASK  0x0400
#define AT1846S_MAIN_CTRL_TAIL_ELIM_POS    11
#define AT1846S_MAIN_CTRL_TAIL_ELIM_MASK   0x0800
#define AT1846S_MAIN_CTRL_BAND_MODE_POS    12
#define AT1846S_MAIN_CTRL_BAND_MODE_MASK   0x1000
#define AT1846S_MAIN_CTRL_FILTER_BAND_POS  13
#define AT1846S_MAIN_CTRL_FILTER_BAND_MASK 0x2000
#define AT1846S_MAIN_CTRL_XTAL_MODE_POS    14
#define AT1846S_MAIN_CTRL_XTAL_MODE_MASK   0x4000

//=============================================================================
// CONTROL MODE REGISTER (0x04) BIT FIELD DEFINITIONS
//=============================================================================

#define AT1846S_CTRL_MODE_CLK_MODE_POS     0
#define AT1846S_CTRL_MODE_CLK_MODE_MASK    0x0001

//=============================================================================
// AUDIO CONTROL REGISTER (0x44) BIT FIELD DEFINITIONS
//=============================================================================

#define AT1846S_AUDIO_CTRL_VOLUME_POS      0
#define AT1846S_AUDIO_CTRL_VOLUME_MASK     0x000F
#define AT1846S_AUDIO_CTRL_DAC_GAIN_POS    4
#define AT1846S_AUDIO_CTRL_DAC_GAIN_MASK   0x00F0
#define AT1846S_AUDIO_CTRL_GAIN_TX_POS     8
#define AT1846S_AUDIO_CTRL_GAIN_TX_MASK    0x0F00

//=============================================================================
// VOICE GAIN REGISTER (0x41) BIT FIELD DEFINITIONS
//=============================================================================

#define AT1846S_VOICE_GAIN_TX_POS          0
#define AT1846S_VOICE_GAIN_TX_MASK         0x007F

//=============================================================================
// MIC GAIN REGISTER (0x47) BIT FIELD DEFINITIONS
//=============================================================================

#define AT1846S_MIC_GAIN_POS               0
#define AT1846S_MIC_GAIN_MASK              0x001F

//=============================================================================
// PA CONTROL REGISTER (0x0A) BIT FIELD DEFINITIONS
//=============================================================================

#define AT1846S_PA_BIAS_VOLTAGE_POS        0
#define AT1846S_PA_BIAS_VOLTAGE_MASK       0x003F
#define AT1846S_PA_PGA_GAIN_POS            6
#define AT1846S_PA_PGA_GAIN_MASK           0x07C0
#define AT1846S_PA_PADRV_IBIT_POS          11
#define AT1846S_PA_PADRV_IBIT_MASK         0x7800

//=============================================================================
// SQUELCH CONTROL REGISTER (0x34) BIT FIELD DEFINITIONS
//=============================================================================

#define AT1846S_SQ_CTRL_LEVEL_POS          0
#define AT1846S_SQ_CTRL_LEVEL_MASK         0x000F

//=============================================================================
// SQUELCH THRESHOLD REGISTER (0x49) BIT FIELD DEFINITIONS
//=============================================================================

#define AT1846S_SQ_THRESH_LOW_POS          0
#define AT1846S_SQ_THRESH_LOW_MASK         0x007F
#define AT1846S_SQ_THRESH_HIGH_POS         7
#define AT1846S_SQ_THRESH_HIGH_MASK        0x3F80

//=============================================================================
// SUBAUDIO CONFIGURATION REGISTER (0x4E) BIT FIELD DEFINITIONS
//=============================================================================

#define AT1846S_SUBAUDIO_CDCSS_SEL_POS     6
#define AT1846S_SUBAUDIO_CDCSS_SEL_MASK    0x01C0
#define AT1846S_SUBAUDIO_CTCSS_SEL_POS     9
#define AT1846S_SUBAUDIO_CTCSS_SEL_MASK    0x0600
#define AT1846S_SUBAUDIO_SHIFT_SEL_POS     14
#define AT1846S_SUBAUDIO_SHIFT_SEL_MASK    0xC000

//=============================================================================
// VOX CONTROL REGISTER (0x0E) BIT FIELD DEFINITIONS
//=============================================================================

#define AT1846S_VOX_ENABLE_POS             15
#define AT1846S_VOX_ENABLE_MASK            0x8000
#define AT1846S_VOX_SENSITIVITY_POS        8
#define AT1846S_VOX_SENSITIVITY_MASK       0x0F00
#define AT1846S_VOX_DELAY_POS              12
#define AT1846S_VOX_DELAY_MASK             0x7000

//=============================================================================
// STATUS REGISTER 1 (0x5D) BIT FIELD DEFINITIONS
//=============================================================================

#define AT1846S_STATUS_1_SQ_OPEN_POS       0
#define AT1846S_STATUS_1_SQ_OPEN_MASK      0x0001
#define AT1846S_STATUS_1_VOX_CMP_POS       1
#define AT1846S_STATUS_1_VOX_CMP_MASK      0x0002
#define AT1846S_STATUS_1_CTCSS_CMP_POS     2
#define AT1846S_STATUS_1_CTCSS_CMP_MASK    0x0004
#define AT1846S_STATUS_1_CTCSS_PHASE_POS   3
#define AT1846S_STATUS_1_CTCSS_PHASE_MASK  0x0018
#define AT1846S_STATUS_1_INVERT_DET_POS    5
#define AT1846S_STATUS_1_INVERT_DET_MASK   0x0020
#define AT1846S_STATUS_1_CDCSS_NEG_POS     6
#define AT1846S_STATUS_1_CDCSS_NEG_MASK    0x0040
#define AT1846S_STATUS_1_CDCSS_POS_POS     7
#define AT1846S_STATUS_1_CDCSS_POS_MASK    0x0080
#define AT1846S_STATUS_1_CTCSS2_CMP_POS    8
#define AT1846S_STATUS_1_CTCSS2_CMP_MASK   0x0100
#define AT1846S_STATUS_1_CTCSS1_CMP_POS    9
#define AT1846S_STATUS_1_CTCSS1_CMP_MASK   0x0200

//=============================================================================
// GPIO MODE REGISTER (0x1F) BIT FIELD DEFINITIONS
//=============================================================================

#define AT1846S_GPIO0_MODE_POS             0
#define AT1846S_GPIO0_MODE_MASK            0x0003
#define AT1846S_GPIO1_MODE_POS             2
#define AT1846S_GPIO1_MODE_MASK            0x000C
#define AT1846S_GPIO2_MODE_POS             4
#define AT1846S_GPIO2_MODE_MASK            0x0030
#define AT1846S_GPIO3_MODE_POS             6
#define AT1846S_GPIO3_MODE_MASK            0x00C0
#define AT1846S_GPIO4_MODE_POS             8
#define AT1846S_GPIO4_MODE_MASK            0x0300
#define AT1846S_GPIO5_MODE_POS             10
#define AT1846S_GPIO5_MODE_MASK            0x0C00
#define AT1846S_GPIO6_MODE_POS             12
#define AT1846S_GPIO6_MODE_MASK            0x3000
#define AT1846S_GPIO7_MODE_POS             14
#define AT1846S_GPIO7_MODE_MASK            0xC000

//=============================================================================
// FREQUENCY CALCULATION CONSTANTS
//=============================================================================

#define AT1846S_FREQ_MULTIPLIER            16      // Frequency scaling factor
#define AT1846S_FREQ_MIN_KHZ               50000   // 50 MHz minimum
#define AT1846S_FREQ_MAX_KHZ               1000000 // 1 GHz maximum

// AT1846S frequency ranges
#define AT1846S_FREQ_VHF_LOW_MIN           134000  // 134 MHz
#define AT1846S_FREQ_VHF_LOW_MAX           174000  // 174 MHz
#define AT1846S_FREQ_VHF_HIGH_MIN          200000  // 200 MHz  
#define AT1846S_FREQ_VHF_HIGH_MAX          260000  // 260 MHz
#define AT1846S_FREQ_UHF_MIN               400000  // 400 MHz
#define AT1846S_FREQ_UHF_MAX               520000  // 520 MHz

// AT1846SD frequency range
#define AT1846SD_FREQ_MIN                  320000  // 320 MHz
#define AT1846SD_FREQ_MAX                  400000  // 400 MHz

//=============================================================================
// POWER MODE DEFINITIONS
//=============================================================================

#define AT1846S_PWR_DOWN                   0x0000
#define AT1846S_PWR_INIT                   0x4004
#define AT1846S_PWR_ENABLE                 0x40A4
#define AT1846S_PWR_CONFIG                 0x40A6
#define AT1846S_PWR_FINAL                  0x4006

//=============================================================================
// BAND SELECTION DEFINITIONS
//=============================================================================

#define AT1846S_BAND_UHF_400_520           0x0000
#define AT1846S_BAND_VHF_134_174           0x0020
#define AT1846S_BAND_VHF_200_260           0x0040

//=============================================================================
// AUDIO CONTROL DEFINITIONS
//=============================================================================

#define AT1846S_VOLUME_MIN                 0
#define AT1846S_VOLUME_MAX                 15
#define AT1846S_MIC_GAIN_MIN               0
#define AT1846S_MIC_GAIN_MAX               31
#define AT1846S_VOICE_GAIN_MIN             0
#define AT1846S_VOICE_GAIN_MAX             127
#define AT1846S_DAC_GAIN_MIN               0
#define AT1846S_DAC_GAIN_MAX               15

//=============================================================================
// SQUELCH DEFINITIONS
//=============================================================================

#define AT1846S_SQUELCH_MIN                0
#define AT1846S_SQUELCH_MAX                15
#define AT1846S_SQUELCH_THRESH_OFFSET      137     // dBm offset for threshold calculation

//=============================================================================
// VOX DEFINITIONS
//=============================================================================

#define AT1846S_VOX_SENSITIVITY_MIN        0
#define AT1846S_VOX_SENSITIVITY_MAX        15
#define AT1846S_VOX_DELAY_MIN              0
#define AT1846S_VOX_DELAY_MAX              7

//=============================================================================
// PA BIAS DEFINITIONS
//=============================================================================

#define AT1846S_PA_BIAS_MIN                0
#define AT1846S_PA_BIAS_MAX                63
#define AT1846S_PA_BIAS_VOLTAGE_MIN        1040    // mV
#define AT1846S_PA_BIAS_VOLTAGE_MAX        3280    // mV

//=============================================================================
// CTCSS STANDARD FREQUENCIES (Hz * 10 for integer math)
//=============================================================================

#define AT1846S_CTCSS_67_0                 670
#define AT1846S_CTCSS_71_9                 719
#define AT1846S_CTCSS_74_4                 744
#define AT1846S_CTCSS_77_0                 770
#define AT1846S_CTCSS_79_7                 797
#define AT1846S_CTCSS_82_5                 825
#define AT1846S_CTCSS_85_4                 854
#define AT1846S_CTCSS_88_5                 885
#define AT1846S_CTCSS_91_5                 915
#define AT1846S_CTCSS_94_8                 948
#define AT1846S_CTCSS_97_4                 974
#define AT1846S_CTCSS_100_0                1000
#define AT1846S_CTCSS_103_5                1035
#define AT1846S_CTCSS_107_2                1072
#define AT1846S_CTCSS_110_9                1109
#define AT1846S_CTCSS_114_8                1148
#define AT1846S_CTCSS_118_8                1188
#define AT1846S_CTCSS_123_0                1230
#define AT1846S_CTCSS_127_3                1273
#define AT1846S_CTCSS_131_8                1318
#define AT1846S_CTCSS_136_5                1365
#define AT1846S_CTCSS_141_3                1413
#define AT1846S_CTCSS_146_2                1462
#define AT1846S_CTCSS_151_4                1514
#define AT1846S_CTCSS_156_7                1567
#define AT1846S_CTCSS_162_2                1622
#define AT1846S_CTCSS_167_9                1679
#define AT1846S_CTCSS_173_8                1738
#define AT1846S_CTCSS_179_9                1799
#define AT1846S_CTCSS_186_2                1862
#define AT1846S_CTCSS_192_8                1928
#define AT1846S_CTCSS_203_5                2035
#define AT1846S_CTCSS_210_7                2107
#define AT1846S_CTCSS_218_1                2181
#define AT1846S_CTCSS_225_7                2257
#define AT1846S_CTCSS_233_6                2336
#define AT1846S_CTCSS_241_8                2418
#define AT1846S_CTCSS_250_3                2503

//=============================================================================
// DTMF DEFINITIONS
//=============================================================================

#define AT1846S_DTMF_CODE_0                0x0A
#define AT1846S_DTMF_CODE_1                0x01
#define AT1846S_DTMF_CODE_2                0x02
#define AT1846S_DTMF_CODE_3                0x03
#define AT1846S_DTMF_CODE_4                0x04
#define AT1846S_DTMF_CODE_5                0x05
#define AT1846S_DTMF_CODE_6                0x06
#define AT1846S_DTMF_CODE_7                0x07
#define AT1846S_DTMF_CODE_8                0x08
#define AT1846S_DTMF_CODE_9                0x09
#define AT1846S_DTMF_CODE_STAR             0x0B
#define AT1846S_DTMF_CODE_HASH             0x0C
#define AT1846S_DTMF_CODE_A                0x0D
#define AT1846S_DTMF_CODE_B                0x0E
#define AT1846S_DTMF_CODE_C                0x0F
#define AT1846S_DTMF_CODE_D                0x00

//=============================================================================
// GPIO MODE DEFINITIONS
//=============================================================================

#define AT1846S_GPIO_MODE_HIZ              0x00
#define AT1846S_GPIO_MODE_FUNC             0x01
#define AT1846S_GPIO_MODE_LOW              0x02
#define AT1846S_GPIO_MODE_HIGH             0x03

//=============================================================================
// REGISTER DEFAULT VALUES (from datasheet)
//=============================================================================

#define AT1846S_REG_DEFAULT_CHIP_ID        0x1846
#define AT1846S_REG_DEFAULT_VERSION        0x003F
#define AT1846S_REG_DEFAULT_CTRL_MODE      0x0FD1
#define AT1846S_REG_DEFAULT_MAIN_CTRL      0x0000
#define AT1846S_REG_DEFAULT_PA_CTRL        0x7C20
#define AT1846S_REG_DEFAULT_FREQ_HIGH      0x2486
#define AT1846S_REG_DEFAULT_FREQ_LOW       0x3A84
#define AT1846S_REG_DEFAULT_VOL_CTRL       0x0000
#define AT1846S_REG_DEFAULT_SQ_CTRL        0x0000
#define AT1846S_REG_DEFAULT_VOICE_GAIN     0x4006
#define AT1846S_REG_DEFAULT_AUDIO_CTRL     0x00FF
#define AT1846S_REG_DEFAULT_SUBAUDIO_CFG   0x20C2

//=============================================================================
// UTILITY MACROS
//=============================================================================

#define AT1846S_SET_FIELD(reg, field, value) \
    ((reg & ~AT1846S_##field##_MASK) | ((value << AT1846S_##field##_POS) & AT1846S_##field##_MASK))

#define AT1846S_GET_FIELD(reg, field) \
    ((reg & AT1846S_##field##_MASK) >> AT1846S_##field##_POS)

#define AT1846S_FREQ_TO_REG(freq_khz) \
    ((u32)(freq_khz) * AT1846S_FREQ_MULTIPLIER)

#define AT1846S_REG_TO_FREQ(reg_val) \
    ((u32)(reg_val) / AT1846S_FREQ_MULTIPLIER)

#define AT1846S_CTCSS_TO_REG(freq_hz_x10) \
    ((u16)((freq_hz_x10) * 100))

#define AT1846S_PA_BIAS_TO_MV(bias_val) \
    (1040 + (bias_val * 35))

#define AT1846S_MV_TO_PA_BIAS(mv) \
    (((mv) - 1040) / 35)

#endif // AT1846S_REGISTERS_H