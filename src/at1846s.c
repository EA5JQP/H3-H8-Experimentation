#include "at1846s.h"
#include "at1846s_reg.h"
#include "at1846s_registers.h"

__code const u8 at1846s_register_addresses[40] = {
    0x30, 0x04, 0x31, 0x33, 0x34, 0x41, 0x42, 0x43, 0x44, 0x47,
    0x4F, 0x53, 0x54, 0x55, 0x56, 0x57, 0x5A, 0x60, 0x63, 0x67,
    0x68, 0x69, 0x6A, 0x6B, 0x6C, 0x6D, 0x6E, 0x6F, 0x70, 0x71,
    0x72, 0x73, 0x74, 0x75, 0x76, 0x77, 0x78, 0x79, 0x30, 0x0F
};

__code const u8 at1846s_data_high_bytes[40] = {
    0x40, 0x0F, 0x00, 0x44, 0x2B, 0x4A, 0x10, 0x01, 0x0D, 0x7F,
    0x2C, 0x00, 0x2A, 0x00, 0x0B, 0x1C, 0x2E, 0x10, 0x16, 0x06,
    0x05, 0x05, 0x04, 0x02, 0x01, 0x00, 0x0F, 0x01, 0x00, 0x0F,
    0x0D, 0x0A, 0x09, 0x08, 0x08, 0x22, 0xD9, 0x1E, 0x40, 0x8A
};

__code const u8 at1846s_data_low_bytes[40] = {
    0x04, 0xD0, 0x31, 0xA5, 0x89, 0x84, 0xFF, 0x01, 0xFF, 0x2F,
    0x62, 0x94, 0x18, 0x81, 0x22, 0x00, 0xDB, 0x1E, 0xAD, 0x28,
    0xE5, 0x55, 0xB8, 0xFE, 0xDD, 0xE1, 0x81, 0x7A, 0x4C, 0x1C,
    0x91, 0x3E, 0x0E, 0x33, 0x06, 0x64, 0x84, 0x3C, 0xA4, 0x24
};

void at1846s_init(void) {
    // This function is responsible for detecting the AT1846S chip and initializing it.
    u8 i;

    at1846s_write_register(0,0,0x30);
    delay_ms(0,100);

    // Process all 40 initialization entries
    for (i = 0; i < 40; i++) {
        at1846s_write_register(
            at1846s_data_low_bytes[i],  // data_low
            at1846s_data_high_bytes[i],  // data_high  
            at1846s_register_addresses[i]   // register_address
        );
        delay_loop();
    }

    // Value   | Likely Function
    // --------|------------------
    // 0x0000  | Complete power down/reset
    // 0x4004  | Initial power configuration
    // 0x40A4  | Power enable with features  
    // 0x40A6  | Additional power configuration
    // 0x4006  | Final power state

    // 3. Final configuration steps
    delay_ms(0, 20);  // 20ms delay

     // 4. Direct register 0x30 writes (replaces wrapper calls)
    at1846s_write_register(0xa6, 0x40, 0x30);  // // Power enable
    delay_ms(0, 100);

    at1846s_write_register(0x06, 0x40, 0x30);  // Final power config  
    delay_ms(0, 100);

    // 5. Final register configuration
    at1846s_write_register(0xa9, 0x5a, 0x1f);  // GPIO config
    delay_ms(0, 100);

    // at1846s_spi_transceive(0x80);
    
}

void at1846s_write_register(u8 data_low, u8 data_high, u8 reg_addr) __critical
{
    // This function writes a value to a specific register of the AT1846S chip.
    // Store parameters (original uses global variables)
    // data_low     =    Will be transmitted last (D[7:0])
    // data_high    =    Will be transmitted second (D[15:8])
    // reg_addr     =    Will be transmitted first (A[6:0])

    u8 bit;
    
    LE1846 = 0;  // Assert chip select
    u8 command_byte = reg_addr & 0x7F;  // Clear R/W bit (0 = write)
    
    // Send register address
    for (bit = 0; bit < 8; bit++) {
        SCK1846 = 0;
        SDA1846 = (command_byte & 0x80) ? 1 : 0;
        SCK1846 = 1;
        command_byte <<= 1;
    }

    // Send data high byte
    for (bit = 0; bit < 8; bit++) {
        SCK1846 = 0;
        SDA1846 = (data_high & 0x80) ? 1 : 0;
        SCK1846 = 1;
        data_high <<= 1;
    }

    // Send data low byte
    for (bit = 0; bit < 8; bit++) {
        SCK1846 = 0;
        SDA1846 = (data_low & 0x80) ? 1 : 0;
        SCK1846 = 1;
        data_low <<= 1;
    }
    
    LE1846 = 1;  // Deassert chip select
}
    
void at1846s_spi_transceive(u8 spi_command, u8 *reg_high, u8 *reg_low) __critical 
{
    u8 bit;

    LE1846 = 0;

    // Send 8-bit command
    for (bit = 0; bit < 8; bit++) {
        SCK1846 = 0;
        SDA1846 = (spi_command & 0x80) ? 1 : 0;
        delay_15_cycles();  // Optional delay for timing
        SCK1846 = 1;
        spi_command <<= 1;
        delay_15_cycles();  // Optional delay for timing
    }

    // Configure SDA as input (if needed)
    SDA1846 = 1;
    P2PH = 0x5e;
    delay_15_cycles();

    // Receive high byte first, then low byte
    *reg_high = at1846s_spi_read_byte();  // First byte received = high byte
    *reg_low = at1846s_spi_read_byte();   // Second byte received = low byte
    
    // Restore SDA as output (if needed)
    P2PH = 0x7e;
    LE1846 = 1;
}

u8 at1846s_spi_read_byte(void) __critical {
    
    // This function reads a byte from the AT1846S chip via SPI.
    // It assumes SDA1846 is set as input before calling this function.
    // The SCK1846 clock line is toggled to read each bit.

    // AT1846S SPI PROTOCOL:
    // - AT1846S drives data on RISING EDGE of clock
    // - MCU should sample data AFTER the rising edge
    // - Clock should start LOW, go HIGH, then back to LOW
    
    u8 result = 0;
    u8 bit;

    for (bit = 0; bit < 8; bit++) {
        SCK1846 = 0;
        delay_15_cycles();
        SCK1846 = 1;
        delay_15_cycles();
        result <<= 1;
        if (SDA1846) {
            result |= 1;
        }
    }

    return result;
}

u8 at1846s_read_register(u8 reg_addr, u16 *data)
{
    u8 reg_high, reg_low;
    u8 spi_command;
    
    if (!data) {
        return AT1846S_ERROR_INVALID_PARAM;
    }
    
    // Create read command (set R/W bit high for read)
    spi_command = 0x80 | (reg_addr & 0x7F);
    
    // Perform SPI transaction with correctly named parameters
    at1846s_spi_transceive(spi_command, &reg_high, &reg_low);
    
    // Now parameters have correct names: reg_high contains high byte, reg_low contains low byte
    *data = ((u16)reg_high << 8) | reg_low;
    
    return AT1846S_SUCCESS;
}

u8 at1846s_verify_chip_id(void)
{
    u16 chip_id;
    u8 result;
    
    // Read chip ID register
    result = at1846s_read_register(AT1846S_REG_CHIP_ID, &chip_id);
    if (result != AT1846S_SUCCESS) {
        return AT1846S_ERROR_COMM_FAIL;
    }
    
    // Check if chip ID matches expected value (0x1846 for AT1846S)
    if (chip_id == 0x1846) {
        return AT1846S_SUCCESS;
    }
    
    return AT1846S_ERROR_NOT_READY;
}

u8 at1846s_write_register_with_verify(u8 data_low, u8 data_high, u8 reg_addr)
{
    u16 written_data, read_data;
    u8 result;
    
    // Write the register
    at1846s_write_register(data_low, data_high, reg_addr);
    delay_ms(0, 5);
    
    // For read-only registers, don't verify
    if (reg_addr == AT1846S_REG_CHIP_ID || reg_addr == AT1846S_REG_VERSION ||
        reg_addr == AT1846S_REG_RSSI || reg_addr == AT1846S_REG_STATUS_1 ||
        reg_addr == AT1846S_REG_STATUS_2 || reg_addr == AT1846S_REG_BATTERY) {
        return AT1846S_SUCCESS;
    }
    
    // Read back to verify
    result = at1846s_read_register(reg_addr, &read_data);
    if (result != AT1846S_SUCCESS) {
        return result;
    }
    
    // Compare written and read values
    written_data = ((u16)data_high << 8) | data_low;
    if (written_data == read_data) {
        return AT1846S_SUCCESS;
    }
    
    return AT1846S_ERROR_COMM_FAIL;
}

void at1846s_set_frequency(u32 freq_khz)
{
    u32 freq_value;
    u16 freq_high, freq_low;
    u8 data_high, data_low;

    // Check if frequency is in valid range (50 MHz – 1000 MHz)
    if (freq_khz < 50000 || freq_khz > 1000000) {
        return;  // Invalid frequency; do nothing
    }

    // Convert frequency in kHz to internal format (f_khz * 16)
    freq_value = freq_khz * 16;

    // Split into high and low parts
    freq_high = (u16)((freq_value >> 16) & 0x3FFF);  // upper 14 bits
    freq_low  = (u16)(freq_value & 0xFFFF);          // lower 16 bits

    // Write high frequency register (0x29)
    data_high = (u8)(freq_high >> 8);
    data_low  = (u8)(freq_high & 0xFF);
    at1846s_write_register(data_low, data_high, 0x29);
    delay_ms(0, 10);

    // Write low frequency register (0x2A)
    data_high = (u8)(freq_low >> 8);
    data_low  = (u8)(freq_low & 0xFF);
    at1846s_write_register(data_low, data_high, 0x2A);
    delay_ms(0, 10);
}

u32 at1846s_get_frequency(void)
{
    u16 freq_high_reg, freq_low_reg;
    u32 freq_value;
    u8 result;
    
    // Read frequency registers (at1846s_read_register now has correct byte order)
    result = at1846s_read_register(AT1846S_REG_FREQ_HIGH, &freq_high_reg);
    if (result != AT1846S_SUCCESS) {
        return 0;  // Error reading frequency
    }
    
    result = at1846s_read_register(AT1846S_REG_FREQ_LOW, &freq_low_reg);
    if (result != AT1846S_SUCCESS) {
        return 0;  // Error reading frequency
    }
    
    // Combine high and low parts and convert back to kHz
    freq_value = (((u32)(freq_high_reg & 0x3FFF)) << 16) | freq_low_reg;
    
    return freq_value / 16;  // Convert from internal format to kHz
}

// Power and Mode Management Functions

u8 at1846s_set_power_mode(u8 enable)
{
    u16 ctrl_value;
    
    if (enable) {
        // Enable power with standard configuration
        ctrl_value = AT1846S_PWR_ENABLE;
    } else {
        // Power down mode
        ctrl_value = AT1846S_PWR_DOWN;
    }
    
    return at1846s_write_register_with_verify(
        (u8)(ctrl_value & 0xFF), 
        (u8)(ctrl_value >> 8), 
        AT1846S_REG_MAIN_CTRL
    );
}

u8 at1846s_set_tx_mode(u8 enable)
{
    u16 current_ctrl;
    u8 result;
    
    // Read current control register
    result = at1846s_read_register(AT1846S_REG_MAIN_CTRL, &current_ctrl);
    if (result != AT1846S_SUCCESS) {
        return result;
    }
    
    if (enable) {
        // Enable TX mode (set TX bit, clear RX bit)
        current_ctrl |= AT1846S_MAIN_CTRL_TX_ON;
        current_ctrl &= ~AT1846S_MAIN_CTRL_RX_ON;
    } else {
        // Disable TX mode (clear TX bit)
        current_ctrl &= ~AT1846S_MAIN_CTRL_TX_ON;
    }
    
    return at1846s_write_register_with_verify(
        (u8)(current_ctrl & 0xFF),
        (u8)(current_ctrl >> 8),
        AT1846S_REG_MAIN_CTRL
    );
}

u8 at1846s_set_rx_mode(u8 enable)
{
    u16 current_ctrl;
    u8 result;
    
    // Read current control register
    result = at1846s_read_register(AT1846S_REG_MAIN_CTRL, &current_ctrl);
    if (result != AT1846S_SUCCESS) {
        return result;
    }
    
    if (enable) {
        // Enable RX mode (set RX bit, clear TX bit)
        current_ctrl |= AT1846S_MAIN_CTRL_RX_ON;
        current_ctrl &= ~AT1846S_MAIN_CTRL_TX_ON;
    } else {
        // Disable RX mode (clear RX bit)
        current_ctrl &= ~AT1846S_MAIN_CTRL_RX_ON;
    }
    
    return at1846s_write_register_with_verify(
        (u8)(current_ctrl & 0xFF),
        (u8)(current_ctrl >> 8),
        AT1846S_REG_MAIN_CTRL
    );
}

u8 at1846s_set_sleep_mode(u8 enable)
{
    u16 ctrl_value;
    
    if (enable) {
        // Set sleep mode - power down with minimal power consumption
        ctrl_value = AT1846S_PWR_DOWN;
    } else {
        // Wake up - return to normal power mode
        ctrl_value = AT1846S_PWR_FINAL;
    }
    
    return at1846s_write_register_with_verify(
        (u8)(ctrl_value & 0xFF),
        (u8)(ctrl_value >> 8),
        AT1846S_REG_MAIN_CTRL
    );
}

u8 at1846s_reset_chip(void)
{
    u8 result;
    
    // Step 1: Issue soft reset
    result = at1846s_write_register_with_verify(
        (u8)(AT1846S_MAIN_CTRL_RESET & 0xFF),
        (u8)(AT1846S_MAIN_CTRL_RESET >> 8),
        AT1846S_REG_MAIN_CTRL
    );
    
    if (result != AT1846S_SUCCESS) {
        return result;
    }
    
    // Wait for reset to complete
    delay_ms(0, 100);
    
    // Step 2: Verify chip is responding
    result = at1846s_verify_chip_id();
    if (result != AT1846S_SUCCESS) {
        return result;
    }
    
    // Step 3: Return to normal power state
    result = at1846s_write_register_with_verify(
        (u8)(AT1846S_PWR_FINAL & 0xFF),
        (u8)(AT1846S_PWR_FINAL >> 8),
        AT1846S_REG_MAIN_CTRL
    );
    
    return result;
}

u8 at1846s_set_band(u8 band)
{
    u16 band_value;
    
    switch (band) {
        case 0: // UHF 400-520 MHz
            band_value = AT1846S_BAND_UHF_400_520;
            break;
        case 1: // VHF 134-174 MHz
            band_value = AT1846S_BAND_VHF_134_174;
            break;
        case 2: // VHF 200-260 MHz
            band_value = AT1846S_BAND_VHF_200_260;
            break;
        default:
            return AT1846S_ERROR_INVALID_PARAM;
    }
    
    return at1846s_write_register_with_verify(
        (u8)(band_value & 0xFF),
        (u8)(band_value >> 8),
        AT1846S_REG_BAND_SEL
    );
}

// Audio and Voice Control Functions

u8 at1846s_set_volume(u8 volume)
{
    u16 vol_value;
    
    // Volume range is typically 0-15 (4 bits)
    if (volume > 15) {
        return AT1846S_ERROR_INVALID_PARAM;
    }
    
    // Volume control - lower 4 bits control volume level
    vol_value = volume & 0x0F;
    
    return at1846s_write_register_with_verify(
        (u8)(vol_value & 0xFF),
        (u8)(vol_value >> 8),
        AT1846S_REG_VOL_CTRL
    );
}

u8 at1846s_set_mic_gain(u8 gain)
{
    u16 gain_value;
    
    // Microphone gain range is typically 0-31 (5 bits)
    if (gain > 31) {
        return AT1846S_ERROR_INVALID_PARAM;
    }
    
    // Set microphone gain - lower 5 bits control gain level
    gain_value = gain & 0x1F;
    
    return at1846s_write_register_with_verify(
        (u8)(gain_value & 0xFF),
        (u8)(gain_value >> 8),
        AT1846S_REG_MIC_GAIN
    );
}

u8 at1846s_set_voice_gain(u8 gain)
{
    u16 gain_value;
    
    // Voice gain range is typically 0-63 (6 bits)
    if (gain > 63) {
        return AT1846S_ERROR_INVALID_PARAM;
    }
    
    // Set voice gain - lower 6 bits control gain level
    gain_value = gain & 0x3F;
    
    return at1846s_write_register_with_verify(
        (u8)(gain_value & 0xFF),
        (u8)(gain_value >> 8),
        AT1846S_REG_VOICE_GAIN
    );
}

u8 at1846s_mute_audio(u8 mute)
{
    u16 current_ctrl;
    u8 result;
    
    // Read current control mode register
    result = at1846s_read_register(AT1846S_REG_CTRL_MODE, &current_ctrl);
    if (result != AT1846S_SUCCESS) {
        return result;
    }
    
    if (mute) {
        // Enable mute (set mute bit)
        current_ctrl |= AT1846S_CTRL_MODE_MUTE;
    } else {
        // Disable mute (clear mute bit)
        current_ctrl &= ~AT1846S_CTRL_MODE_MUTE;
    }
    
    return at1846s_write_register_with_verify(
        (u8)(current_ctrl & 0xFF),
        (u8)(current_ctrl >> 8),
        AT1846S_REG_CTRL_MODE
    );
}

u8 at1846s_set_audio_processing(u8 enable)
{
    u16 audio_ctrl_value;
    
    if (enable) {
        // Enable audio processing features
        audio_ctrl_value = 0x0001;  // Basic audio processing enable
    } else {
        // Disable audio processing
        audio_ctrl_value = 0x0000;
    }
    
    return at1846s_write_register_with_verify(
        (u8)(audio_ctrl_value & 0xFF),
        (u8)(audio_ctrl_value >> 8),
        AT1846S_REG_AUDIO_PROC
    );
}

u8 at1846s_set_emphasis(u8 enable)
{
    u16 emphasis_value;
    
    if (enable) {
        // Enable pre/de-emphasis (typical for voice communication)
        emphasis_value = 0x0001;
    } else {
        // Disable emphasis
        emphasis_value = 0x0000;
    }
    
    return at1846s_write_register_with_verify(
        (u8)(emphasis_value & 0xFF),
        (u8)(emphasis_value >> 8),
        AT1846S_REG_EMPHASIS
    );
}

u8 at1846s_set_compander(u8 enable)
{
    u16 compander_value;
    
    if (enable) {
        // Enable compander for dynamic range compression
        compander_value = 0x0001;
    } else {
        // Disable compander
        compander_value = 0x0000;
    }
    
    return at1846s_write_register_with_verify(
        (u8)(compander_value & 0xFF),
        (u8)(compander_value >> 8),
        AT1846S_REG_COMPANDER
    );
}

u8 at1846s_set_deviation(u8 deviation)
{
    u16 dev_value;
    
    // Deviation range is typically 0-15 (4 bits)
    if (deviation > 15) {
        return AT1846S_ERROR_INVALID_PARAM;
    }
    
    // Set frequency deviation for FM modulation
    dev_value = deviation & 0x0F;
    
    return at1846s_write_register_with_verify(
        (u8)(dev_value & 0xFF),
        (u8)(dev_value >> 8),
        AT1846S_REG_DEVIATION
    );
}

// Squelch and Signal Quality Functions

u8 at1846s_set_squelch(u8 level)
{
    u16 sq_value;
    
    // Squelch level range is typically 0-15 (4 bits)
    if (level > 15) {
        return AT1846S_ERROR_INVALID_PARAM;
    }
    
    // Set squelch level - lower 4 bits control squelch threshold
    sq_value = level & 0x0F;
    
    return at1846s_write_register_with_verify(
        (u8)(sq_value & 0xFF),
        (u8)(sq_value >> 8),
        AT1846S_REG_SQ_CTRL
    );
}

u8 at1846s_get_squelch_status(void)
{
    u16 status_value;
    u8 result;
    
    // Read status register to check squelch state
    result = at1846s_read_register(AT1846S_REG_STATUS_1, &status_value);
    if (result != AT1846S_SUCCESS) {
        return 0;  // Error reading status
    }
    
    // Return 1 if squelch is open (signal present), 0 if closed
    return (status_value & AT1846S_STATUS_SQ_OPEN) ? 1 : 0;
}

u8 at1846s_set_noise_gate(u8 level)
{
    u16 ng_value;
    
    // Noise gate level range is typically 0-7 (3 bits)
    if (level > 7) {
        return AT1846S_ERROR_INVALID_PARAM;
    }
    
    // Set noise gate level
    ng_value = level & 0x07;
    
    return at1846s_write_register_with_verify(
        (u8)(ng_value & 0xFF),
        (u8)(ng_value >> 8),
        AT1846S_REG_NOISE_GATE
    );
}

u8 at1846s_get_rssi(void)
{
    u16 rssi_value;
    u8 result;
    
    // Read RSSI register
    result = at1846s_read_register(AT1846S_REG_RSSI, &rssi_value);
    if (result != AT1846S_SUCCESS) {
        return 0;  // Error reading RSSI
    }
    
    // Return RSSI as 8-bit value (lower byte)
    return (u8)(rssi_value & 0xFF);
}

u16 at1846s_get_status(void)
{
    u16 status_value;
    u8 result;
    
    // Read main status register
    result = at1846s_read_register(AT1846S_REG_STATUS_1, &status_value);
    if (result != AT1846S_SUCCESS) {
        return 0;  // Error reading status
    }
    
    return status_value;
}

u8 at1846s_get_battery_voltage(void)
{
    u16 battery_value;
    u8 result;
    
    // Read battery voltage register
    result = at1846s_read_register(AT1846S_REG_BATTERY, &battery_value);
    if (result != AT1846S_SUCCESS) {
        return 0;  // Error reading battery voltage
    }
    
    // Return battery voltage as 8-bit value
    return (u8)(battery_value & 0xFF);
}

u8 at1846s_get_tx_status(void)
{
    u16 status_value;
    u8 result;
    
    // Read status register to check TX state
    result = at1846s_read_register(AT1846S_REG_STATUS_1, &status_value);
    if (result != AT1846S_SUCCESS) {
        return 0;  // Error reading status
    }
    
    // Return 1 if TX is active, 0 if not
    return (status_value & AT1846S_STATUS_TX_ON) ? 1 : 0;
}

u8 at1846s_get_rx_status(void)
{
    u16 status_value;
    u8 result;
    
    // Read status register to check RX state
    result = at1846s_read_register(AT1846S_REG_STATUS_1, &status_value);
    if (result != AT1846S_SUCCESS) {
        return 0;  // Error reading status
    }
    
    // Return 1 if RX is active, 0 if not
    return (status_value & AT1846S_STATUS_RX_ON) ? 1 : 0;
}

// CTCSS/CDCSS Sub-Audio Functions

// CTCSS frequency conversion is implemented later in the file

u8 at1846s_set_ctcss_tx(u16 tone_freq)
{
    u16 reg_value;
    u8 result;
    
    // Convert frequency to register value
    reg_value = at1846s_ctcss_freq_to_reg(tone_freq);
    if (reg_value == 0) {
        return AT1846S_ERROR_INVALID_PARAM;
    }
    
    // Set CTCSS frequency register
    result = at1846s_write_register_with_verify(
        (u8)(reg_value & 0xFF),
        (u8)(reg_value >> 8),
        AT1846S_REG_CTCSS_FREQ
    );
    
    if (result != AT1846S_SUCCESS) {
        return result;
    }
    
    // Enable CTCSS TX in sub-audio configuration
    result = at1846s_write_register_with_verify(
        0x01,  // Enable CTCSS TX
        0x00,
        AT1846S_REG_SUBAUDIO_CFG
    );
    
    return result;
}

u8 at1846s_set_ctcss_rx(u16 tone_freq)
{
    u16 reg_value;
    u8 result;
    
    // Convert frequency to register value
    reg_value = at1846s_ctcss_freq_to_reg(tone_freq);
    if (reg_value == 0) {
        return AT1846S_ERROR_INVALID_PARAM;
    }
    
    // Set CTCSS frequency register
    result = at1846s_write_register_with_verify(
        (u8)(reg_value & 0xFF),
        (u8)(reg_value >> 8),
        AT1846S_REG_CTCSS_FREQ
    );
    
    if (result != AT1846S_SUCCESS) {
        return result;
    }
    
    // Enable CTCSS RX in sub-audio configuration
    result = at1846s_write_register_with_verify(
        0x02,  // Enable CTCSS RX
        0x00,
        AT1846S_REG_SUBAUDIO_CFG
    );
    
    return result;
}

u8 at1846s_disable_ctcss(void)
{
    // Disable CTCSS by clearing sub-audio configuration
    return at1846s_write_register_with_verify(
        0x00,  // Disable CTCSS
        0x00,
        AT1846S_REG_SUBAUDIO_CFG
    );
}

u8 at1846s_set_cdcss_tx(u16 code)
{
    u8 result;
    
    // CDCSS codes are typically 9-bit values (1-511)
    if (code < 1 || code > 511) {
        return AT1846S_ERROR_INVALID_PARAM;
    }
    
    // Set CDCSS code in high and low registers
    result = at1846s_write_register_with_verify(
        (u8)(code & 0xFF),    // Low byte
        (u8)(code >> 8),      // High byte (upper bit)
        AT1846S_REG_CDCSS_L
    );
    
    if (result != AT1846S_SUCCESS) {
        return result;
    }
    
    // Enable CDCSS TX in sub-audio configuration
    result = at1846s_write_register_with_verify(
        0x04,  // Enable CDCSS TX
        0x00,
        AT1846S_REG_SUBAUDIO_CFG
    );
    
    return result;
}

u8 at1846s_set_cdcss_rx(u16 code)
{
    u8 result;
    
    // CDCSS codes are typically 9-bit values (1-511)
    if (code < 1 || code > 511) {
        return AT1846S_ERROR_INVALID_PARAM;
    }
    
    // Set CDCSS code in high and low registers
    result = at1846s_write_register_with_verify(
        (u8)(code & 0xFF),    // Low byte
        (u8)(code >> 8),      // High byte (upper bit)
        AT1846S_REG_CDCSS_L
    );
    
    if (result != AT1846S_SUCCESS) {
        return result;
    }
    
    // Enable CDCSS RX in sub-audio configuration
    result = at1846s_write_register_with_verify(
        0x08,  // Enable CDCSS RX
        0x00,
        AT1846S_REG_SUBAUDIO_CFG
    );
    
    return result;
}

u8 at1846s_disable_cdcss(void)
{
    // Disable CDCSS by clearing sub-audio configuration
    return at1846s_write_register_with_verify(
        0x00,  // Disable CDCSS
        0x00,
        AT1846S_REG_SUBAUDIO_CFG
    );
}

u8 at1846s_get_ctcss_detect(void)
{
    u16 status_value;
    u8 result;
    
    // Read status register to check CTCSS detection
    result = at1846s_read_register(AT1846S_REG_STATUS_1, &status_value);
    if (result != AT1846S_SUCCESS) {
        return 0;  // Error reading status
    }
    
    // Return 1 if CTCSS tone detected, 0 if not
    return (status_value & AT1846S_STATUS_CTCSS_PHASE) ? 1 : 0;
}

u8 at1846s_get_cdcss_detect(void)
{
    u16 status_value;
    u8 result;
    
    // Read status register to check CDCSS detection
    result = at1846s_read_register(AT1846S_REG_STATUS_1, &status_value);
    if (result != AT1846S_SUCCESS) {
        return 0;  // Error reading status
    }
    
    // Return 1 if CDCSS code detected, 0 if not
    return (status_value & AT1846S_STATUS_CDCSS_PHASE) ? 1 : 0;
}

// VOX (Voice Operated Exchange) Functions

u8 at1846s_enable_vox(u8 sensitivity, u8 delay)
{
    u16 vox_ctrl_value;
    u8 result;
    
    // Check parameter ranges
    if (sensitivity > 15 || delay > 7) {
        return AT1846S_ERROR_INVALID_PARAM;
    }
    
    // Build VOX control register value
    vox_ctrl_value = AT1846S_VOX_ENABLE |                    // Enable VOX
                     ((u16)(delay & 0x07) << 12) |          // VOX delay (3 bits)
                     ((u16)(sensitivity & 0x0F) << 8);      // VOX sensitivity (4 bits)
    
    // Write VOX control register
    result = at1846s_write_register_with_verify(
        (u8)(vox_ctrl_value & 0xFF),
        (u8)(vox_ctrl_value >> 8),
        AT1846S_REG_VOX_CTRL
    );
    
    return result;
}

u8 at1846s_disable_vox(void)
{
    // Disable VOX by clearing the enable bit
    return at1846s_write_register_with_verify(
        0x00,  // Clear all VOX settings
        0x00,
        AT1846S_REG_VOX_CTRL
    );
}

u8 at1846s_get_vox_status(void)
{
    u16 vox_value;
    u8 result;
    
    // Read VOX control register
    result = at1846s_read_register(AT1846S_REG_VOX_CTRL, &vox_value);
    if (result != AT1846S_SUCCESS) {
        return 0;  // Error reading VOX status
    }
    
    // Return 1 if VOX is enabled, 0 if disabled
    return (vox_value & AT1846S_VOX_ENABLE) ? 1 : 0;
}

// DTMF (Dual-Tone Multi-Frequency) Functions

u8 at1846s_enable_dtmf(void)
{
    u8 result;
    
    // Enable DTMF mode
    result = at1846s_write_register_with_verify(
        0x01,  // Enable DTMF
        0x00,
        AT1846S_REG_DTMF_CTL
    );
    
    if (result != AT1846S_SUCCESS) {
        return result;
    }
    
    // Set DTMF timing parameters (default values)
    result = at1846s_write_register_with_verify(
        0x64,  // DTMF timing - 100ms tone duration
        0x00,
        AT1846S_REG_DTMF_TIME
    );
    
    return result;
}

u8 at1846s_disable_dtmf(void)
{
    // Disable DTMF mode
    return at1846s_write_register_with_verify(
        0x00,  // Disable DTMF
        0x00,
        AT1846S_REG_DTMF_CTL
    );
}

u8 at1846s_dtmf_digit_to_code(u8 digit)
{
    // Convert digit character to DTMF code
    switch (digit) {
        case '0': return 0x0A;
        case '1': return 0x01;
        case '2': return 0x02;
        case '3': return 0x03;
        case '4': return 0x04;
        case '5': return 0x05;
        case '6': return 0x06;
        case '7': return 0x07;
        case '8': return 0x08;
        case '9': return 0x09;
        case '*': return 0x0B;
        case '#': return 0x0C;
        case 'A': return 0x0D;
        case 'B': return 0x0E;
        case 'C': return 0x0F;
        case 'D': return 0x00;
        default:  return 0xFF; // Invalid digit
    }
}

u8 at1846s_send_dtmf_tone(u8 digit, u16 duration_ms)
{
    u8 dtmf_code;
    u8 result;
    u16 timing_value;
    
    // Convert digit to DTMF code
    dtmf_code = at1846s_dtmf_digit_to_code(digit);
    if (dtmf_code == 0xFF) {
        return AT1846S_ERROR_INVALID_PARAM;
    }
    
    // Calculate timing value (duration in units that the chip understands)
    // Typically duration_ms / 10 for 10ms units
    timing_value = duration_ms / 10;
    if (timing_value > 255) {
        timing_value = 255;  // Maximum duration
    }
    
    // Set DTMF timing
    result = at1846s_write_register_with_verify(
        (u8)(timing_value & 0xFF),
        0x00,
        AT1846S_REG_DTMF_TIME
    );
    
    if (result != AT1846S_SUCCESS) {
        return result;
    }
    
    // Send DTMF digit by writing to DTMF mode register
    result = at1846s_write_register_with_verify(
        dtmf_code,  // DTMF digit code
        0x00,
        AT1846S_REG_DTMF_MODE
    );
    
    if (result != AT1846S_SUCCESS) {
        return result;
    }
    
    // Wait for tone duration
    delay_ms(0, duration_ms);
    
    return AT1846S_SUCCESS;
}

u8 at1846s_get_dtmf_detect(void)
{
    u16 dtmf_status;
    u8 result;
    
    // Read DTMF mode register to check for detected digits
    result = at1846s_read_register(AT1846S_REG_DTMF_MODE, &dtmf_status);
    if (result != AT1846S_SUCCESS) {
        return 0;  // Error reading DTMF status
    }
    
    // Return detected DTMF code (0 if none detected)
    return (u8)(dtmf_status & 0x0F);
}

// GPIO and Interrupt Management Functions

u8 at1846s_config_gpio(u8 pin, u8 mode)
{
    u16 gpio_ctrl;
    u8 result;
    
    // Check valid pin number (0-7 typically)
    if (pin > 7) {
        return AT1846S_ERROR_INVALID_PARAM;
    }
    
    // Check valid mode (0 = input, 1 = output)
    if (mode > 1) {
        return AT1846S_ERROR_INVALID_PARAM;
    }
    
    // Read current GPIO control register
    result = at1846s_read_register(AT1846S_REG_GPIO_CTRL, &gpio_ctrl);
    if (result != AT1846S_SUCCESS) {
        return result;
    }
    
    // Modify the specific pin's mode bit
    if (mode) {
        gpio_ctrl |= (1 << pin);   // Set bit for output
    } else {
        gpio_ctrl &= ~(1 << pin);  // Clear bit for input
    }
    
    // Write back the modified control register
    return at1846s_write_register_with_verify(
        (u8)(gpio_ctrl & 0xFF),
        (u8)(gpio_ctrl >> 8),
        AT1846S_REG_GPIO_CTRL
    );
}

u8 at1846s_set_gpio(u8 pin, u8 value)
{
    u16 gpio_io;
    u8 result;
    
    // Check valid pin number (0-7 typically)
    if (pin > 7) {
        return AT1846S_ERROR_INVALID_PARAM;
    }
    
    // Check valid value (0 or 1)
    if (value > 1) {
        return AT1846S_ERROR_INVALID_PARAM;
    }
    
    // Read current GPIO I/O register
    result = at1846s_read_register(AT1846S_REG_GPIO_IO, &gpio_io);
    if (result != AT1846S_SUCCESS) {
        return result;
    }
    
    // Modify the specific pin's value bit
    if (value) {
        gpio_io |= (1 << pin);   // Set bit for high
    } else {
        gpio_io &= ~(1 << pin);  // Clear bit for low
    }
    
    // Write back the modified I/O register
    return at1846s_write_register_with_verify(
        (u8)(gpio_io & 0xFF),
        (u8)(gpio_io >> 8),
        AT1846S_REG_GPIO_IO
    );
}

u8 at1846s_get_gpio(u8 pin)
{
    u16 gpio_io;
    u8 result;
    
    // Check valid pin number (0-7 typically)
    if (pin > 7) {
        return 0;  // Invalid pin
    }
    
    // Read GPIO I/O register
    result = at1846s_read_register(AT1846S_REG_GPIO_IO, &gpio_io);
    if (result != AT1846S_SUCCESS) {
        return 0;  // Error reading GPIO
    }
    
    // Return the specific pin's value (0 or 1)
    return (gpio_io & (1 << pin)) ? 1 : 0;
}

u8 at1846s_enable_interrupts(u16 int_mask)
{
    // Enable specific interrupts based on mask
    return at1846s_write_register_with_verify(
        (u8)(int_mask & 0xFF),
        (u8)(int_mask >> 8),
        AT1846S_REG_INT_MODE
    );
}

u8 at1846s_disable_interrupts(void)
{
    // Disable all interrupts
    return at1846s_write_register_with_verify(
        0x00,  // Disable all interrupts
        0x00,
        AT1846S_REG_INT_MODE
    );
}

u16 at1846s_get_interrupt_status(void)
{
    u16 int_status;
    u8 result;
    
    // Read interrupt status/flag register
    result = at1846s_read_register(AT1846S_REG_FLAG_REG, &int_status);
    if (result != AT1846S_SUCCESS) {
        return 0;  // Error reading interrupt status
    }
    
    return int_status;
}

u8 at1846s_set_pa_bias(u8 bias)
{
    u16 pa_bias_value;
    
    // PA bias range is typically 0-31 (5 bits)
    if (bias > 31) {
        return AT1846S_ERROR_INVALID_PARAM;
    }
    
    // Set PA bias level
    pa_bias_value = bias & 0x1F;
    
    return at1846s_write_register_with_verify(
        (u8)(pa_bias_value & 0xFF),
        (u8)(pa_bias_value >> 8),
        AT1846S_REG_PA_BIAS
    );
}

u8 at1846s_calibrate(void)
{
    u8 result;
    u16 main_ctrl;
    
    // Read current main control register
    result = at1846s_read_register(AT1846S_REG_MAIN_CTRL, &main_ctrl);
    if (result != AT1846S_SUCCESS) {
        return result;
    }
    
    // Set calibration bit
    main_ctrl |= AT1846S_MAIN_CTRL_CHIP_CAL;
    
    // Start calibration
    result = at1846s_write_register_with_verify(
        (u8)(main_ctrl & 0xFF),
        (u8)(main_ctrl >> 8),
        AT1846S_REG_MAIN_CTRL
    );
    
    if (result != AT1846S_SUCCESS) {
        return result;
    }
    
    // Wait for calibration to complete (typical 100ms)
    delay_ms(0, 100);
    
    // Clear calibration bit
    main_ctrl &= ~AT1846S_MAIN_CTRL_CHIP_CAL;
    
    return at1846s_write_register_with_verify(
        (u8)(main_ctrl & 0xFF),
        (u8)(main_ctrl >> 8),
        AT1846S_REG_MAIN_CTRL
    );
}

// Diagnostic and Monitoring Functions

u8 at1846s_self_test(void)
{
    u8 result;
    u16 original_ctrl, test_value;
    
    // Step 1: Verify chip communication by checking ID
    result = at1846s_verify_chip_id();
    if (result != AT1846S_SUCCESS) {
        return AT1846S_ERROR_COMM_FAIL;
    }
    
    // Step 2: Test register read/write functionality
    // Read original main control register
    result = at1846s_read_register(AT1846S_REG_MAIN_CTRL, &original_ctrl);
    if (result != AT1846S_SUCCESS) {
        return AT1846S_ERROR_COMM_FAIL;
    }
    
    // Write test pattern
    test_value = 0x1234;
    result = at1846s_write_register_with_verify(
        (u8)(test_value & 0xFF),
        (u8)(test_value >> 8),
        AT1846S_REG_MAIN_CTRL
    );
    
    if (result != AT1846S_SUCCESS) {
        // Restore original value before failing
        at1846s_write_register(
            (u8)(original_ctrl & 0xFF),
            (u8)(original_ctrl >> 8),
            AT1846S_REG_MAIN_CTRL
        );
        return AT1846S_ERROR_COMM_FAIL;
    }
    
    // Step 3: Restore original control register
    result = at1846s_write_register_with_verify(
        (u8)(original_ctrl & 0xFF),
        (u8)(original_ctrl >> 8),
        AT1846S_REG_MAIN_CTRL
    );
    
    if (result != AT1846S_SUCCESS) {
        return AT1846S_ERROR_COMM_FAIL;
    }
    
    // Step 4: Test basic functionality
    // Try to read a read-only register (battery voltage)
    u16 battery_reading;
    result = at1846s_read_register(AT1846S_REG_BATTERY, &battery_reading);
    if (result != AT1846S_SUCCESS) {
        return AT1846S_ERROR_COMM_FAIL;
    }
    
    return AT1846S_SUCCESS;  // All tests passed
}

u8 at1846s_read_all_registers(u16 *reg_values)
{
    u8 reg_addr;
    u8 result;
    u8 reg_count = 0;
    
    if (!reg_values) {
        return AT1846S_ERROR_INVALID_PARAM;
    }
    
    // Read all important registers (0x00 to 0x7F)
    for (reg_addr = 0x00; reg_addr <= 0x7F; reg_addr++) {
        result = at1846s_read_register(reg_addr, &reg_values[reg_count]);
        if (result != AT1846S_SUCCESS) {
            // Continue reading other registers even if one fails
            reg_values[reg_count] = 0xFFFF;  // Mark as error
        }
        reg_count++;
    }
    
    return AT1846S_SUCCESS;
}

void at1846s_dump_registers(void)
{
    u16 reg_value;
    u8 reg_addr;
    u8 result;
    
    // List of important registers to dump
    u8 important_regs[] = {
        AT1846S_REG_CHIP_ID,      // 0x00
        AT1846S_REG_VERSION,      // 0x01  
        AT1846S_REG_CTRL_MODE,    // 0x04
        AT1846S_REG_VOX_CTRL,     // 0x0E
        AT1846S_REG_BATTERY,      // 0x0F
        AT1846S_REG_FREQ_HIGH,    // 0x29
        AT1846S_REG_FREQ_LOW,     // 0x2A
        AT1846S_REG_MAIN_CTRL,    // 0x30
        AT1846S_REG_BAND_SEL,     // 0x31
        AT1846S_REG_VOL_CTRL,     // 0x33
        AT1846S_REG_SQ_CTRL,      // 0x34
        AT1846S_REG_VOICE_GAIN,   // 0x41
        AT1846S_REG_AUDIO_CTRL,   // 0x44
        AT1846S_REG_CTCSS_FREQ,   // 0x4E
        AT1846S_REG_DTMF_CTL,     // 0x50
        AT1846S_REG_RSSI,         // 0x5B
        AT1846S_REG_STATUS_1,     // 0x5C
        AT1846S_REG_STATUS_2      // 0x5D
    };
    
    u8 num_regs = sizeof(important_regs) / sizeof(important_regs[0]);
    
    // Note: In a real implementation, this would send output via UART
    // For now, we just read the registers (values could be logged elsewhere)
    
    for (u8 i = 0; i < num_regs; i++) {
        reg_addr = important_regs[i];
        result = at1846s_read_register(reg_addr, &reg_value);
        
        if (result == AT1846S_SUCCESS) {
            // In a real implementation, format and send via UART:
            // printf("Reg 0x%02X: 0x%04X\n", reg_addr, reg_value);
            
            // For now, just ensure the read succeeded
            // The calling code can capture these values if needed
            (void)reg_value;  // Suppress unused variable warning
        }
    }
}

//=============================================================================
// ENHANCED HIGH-LEVEL API USING REGISTER MANAGEMENT LAYER
//=============================================================================

// Register management layer is now included at the top

//=============================================================================
// STRUCTURED CONFIGURATION SYSTEM
//=============================================================================

// Struct definitions are now in at1846s.h

//=============================================================================
// ENHANCED CONFIGURATION FUNCTIONS
//=============================================================================

/**
 * @brief Initialize AT1846S with enhanced register management
 * @param enable_cache: 1 to enable register caching for performance
 * @return AT1846S_SUCCESS on success, error code on failure
 */
u8 at1846s_init_enhanced(u8 enable_cache)
{
    u8 result;
    
    // Initialize the register management system
    result = at1846s_reg_init(enable_cache);
    if (result != AT1846S_SUCCESS) {
        return result;
    }
    
    // Perform basic chip initialization
    at1846s_init();
    
    // Verify chip is responding
    result = at1846s_verify_chip_id();
    if (result != AT1846S_SUCCESS) {
        return result;
    }
    
    return AT1846S_SUCCESS;
}

/**
 * @brief Apply complete radio configuration
 * @param config: Pointer to configuration structure
 * @return AT1846S_SUCCESS on success, error code on failure
 */
u8 at1846s_apply_config(const at1846s_config_t *config)
{
    u8 result;
    
    if (!config) {
        return AT1846S_ERROR_INVALID_PARAM;
    }
    
    // Set frequency and band
    result = at1846s_set_band(config->band);
    if (result != AT1846S_SUCCESS) {
        return result;
    }
    
    at1846s_set_frequency(config->frequency_khz);
    
    // Configure audio settings
    result = at1846s_set_volume(config->volume);
    if (result != AT1846S_SUCCESS) {
        return result;
    }
    
    result = at1846s_set_mic_gain(config->mic_gain);
    if (result != AT1846S_SUCCESS) {
        return result;
    }
    
    result = at1846s_set_voice_gain(config->voice_gain);
    if (result != AT1846S_SUCCESS) {
        return result;
    }
    
    result = at1846s_set_squelch(config->squelch_level);
    if (result != AT1846S_SUCCESS) {
        return result;
    }
    
    // Configure sub-audio based on mode
    switch (config->subaudio_mode) {
        case AT1846S_SUBAUDIO_CTCSS:
            result = at1846s_set_ctcss_tx(config->ctcss_tx_freq);
            if (result != AT1846S_SUCCESS) return result;
            result = at1846s_set_ctcss_rx(config->ctcss_rx_freq);
            if (result != AT1846S_SUCCESS) return result;
            break;
            
        case AT1846S_SUBAUDIO_CDCSS:
            result = at1846s_set_cdcss_tx(config->cdcss_tx_code);
            if (result != AT1846S_SUCCESS) return result;
            result = at1846s_set_cdcss_rx(config->cdcss_rx_code);
            if (result != AT1846S_SUCCESS) return result;
            break;
            
        case AT1846S_SUBAUDIO_BOTH:
            // Enable both CTCSS and CDCSS if supported
            result = at1846s_set_ctcss_tx(config->ctcss_tx_freq);
            if (result != AT1846S_SUCCESS) return result;
            result = at1846s_set_cdcss_rx(config->cdcss_rx_code);
            if (result != AT1846S_SUCCESS) return result;
            break;
            
        case AT1846S_SUBAUDIO_NONE:
        default:
            result = at1846s_disable_ctcss();
            if (result != AT1846S_SUCCESS) return result;
            result = at1846s_disable_cdcss();
            if (result != AT1846S_SUCCESS) return result;
            break;
    }
    
    // Configure VOX if enabled
    if (config->vox_enabled) {
        result = at1846s_enable_vox(config->vox_sensitivity, config->vox_delay);
        if (result != AT1846S_SUCCESS) {
            return result;
        }
    } else {
        result = at1846s_disable_vox();
        if (result != AT1846S_SUCCESS) {
            return result;
        }
    }
    
    // Configure power and advanced settings
    result = at1846s_set_pa_bias(config->pa_bias);
    if (result != AT1846S_SUCCESS) {
        return result;
    }
    
    result = at1846s_set_emphasis(config->emphasis);
    if (result != AT1846S_SUCCESS) {
        return result;
    }
    
    result = at1846s_set_compander(config->compander);
    if (result != AT1846S_SUCCESS) {
        return result;
    }
    
    result = at1846s_set_noise_gate(config->noise_gate);
    if (result != AT1846S_SUCCESS) {
        return result;
    }
    
    result = at1846s_set_deviation(config->deviation);
    if (result != AT1846S_SUCCESS) {
        return result;
    }
    
    // Flush all register changes to hardware
    return at1846s_reg_flush_cache();
}

/**
 * @brief Load predefined configuration preset
 * @param preset_id: Preset identifier
 * @param config: Pointer to store loaded configuration
 * @return AT1846S_SUCCESS on success, error code on failure
 */
u8 at1846s_load_preset(at1846s_preset_id_t preset_id, at1846s_config_t *config)
{
    if (!config) {
        return AT1846S_ERROR_INVALID_PARAM;
    }
    
    // Initialize common defaults
    config->volume = 8;
    config->squelch_level = 5;
    config->mic_gain = 16;
    config->voice_gain = 40;
    config->pa_bias = 32;
    config->emphasis = 1;
    config->compander = 0;
    config->noise_gate = 3;
    config->deviation = 8;
    config->vox_enabled = 0;
    config->vox_sensitivity = 8;
    config->vox_delay = 2;
    
    switch (preset_id) {
        case AT1846S_PRESET_SIMPLEX_VHF:
            config->frequency_khz = 146520;    // 146.52 MHz (VHF calling freq)
            config->band = 1;                  // VHF 134-174
            config->ctcss_tx_freq = 0;
            config->ctcss_rx_freq = 0;
            config->subaudio_mode = AT1846S_SUBAUDIO_NONE;
            config->tx_power = 12;             // Medium power
            break;
            
        case AT1846S_PRESET_SIMPLEX_UHF:
            config->frequency_khz = 446000;    // 446.0 MHz (UHF PMR)
            config->band = 0;                  // UHF 400-520
            config->ctcss_tx_freq = 0;
            config->ctcss_rx_freq = 0;
            config->subaudio_mode = AT1846S_SUBAUDIO_NONE;
            config->tx_power = 10;             // Lower power for UHF
            break;
            
        case AT1846S_PRESET_REPEATER_VHF:
            config->frequency_khz = 145500;    // Common VHF repeater
            config->band = 1;                  // VHF 134-174
            config->ctcss_tx_freq = AT1846S_CTCSS_1000;  // 100.0 Hz
            config->ctcss_rx_freq = AT1846S_CTCSS_1000;
            config->subaudio_mode = AT1846S_SUBAUDIO_CTCSS;
            config->tx_power = 15;             // High power
            break;
            
        case AT1846S_PRESET_REPEATER_UHF:
            config->frequency_khz = 442000;    // Common UHF repeater
            config->band = 0;                  // UHF 400-520
            config->ctcss_tx_freq = AT1846S_CTCSS_1230;  // 123.0 Hz
            config->ctcss_rx_freq = AT1846S_CTCSS_1230;
            config->subaudio_mode = AT1846S_SUBAUDIO_CTCSS;
            config->tx_power = 12;             // Medium-high power
            break;
            
        case AT1846S_PRESET_EMERGENCY:
            config->frequency_khz = 146520;    // VHF calling frequency
            config->band = 1;                  // VHF 134-174
            config->volume = 15;               // Maximum volume
            config->mic_gain = 24;             // Higher mic gain
            config->voice_gain = 60;           // Higher voice gain
            config->squelch_level = 2;         // More sensitive squelch
            config->tx_power = 15;             // Maximum power
            config->subaudio_mode = AT1846S_SUBAUDIO_NONE;
            config->vox_enabled = 1;           // Enable VOX for hands-free
            config->vox_sensitivity = 12;      // High VOX sensitivity
            break;
            
        case AT1846S_PRESET_LOW_POWER:
            config->frequency_khz = 146520;
            config->band = 1;                  // VHF 134-174
            config->volume = 6;                // Lower volume
            config->tx_power = 5;              // Low power
            config->pa_bias = 20;              // Lower PA bias
            config->subaudio_mode = AT1846S_SUBAUDIO_NONE;
            break;
            
        case AT1846S_PRESET_DIGITAL:
            config->frequency_khz = 446000;
            config->band = 0;                  // UHF typically better for digital
            config->emphasis = 0;              // Disable emphasis for digital
            config->compander = 0;             // Disable compander
            config->deviation = 4;             // Lower deviation for digital
            config->noise_gate = 1;            // Minimal noise gate
            config->subaudio_mode = AT1846S_SUBAUDIO_NONE;
            config->tx_power = 10;
            break;
            
        case AT1846S_PRESET_CUSTOM:
        default:
            // Load from saved preset using register management
            return at1846s_reg_load_preset(7);  // Use preset slot 7 for custom
    }
    
    return AT1846S_SUCCESS;
}

/**
 * @brief Quick channel change with validation
 * @param freq_khz: New frequency in kHz
 * @param ctcss_freq: CTCSS frequency (0 = disable)
 * @return AT1846S_SUCCESS on success, error code on failure
 */
u8 at1846s_quick_channel_change(u32 freq_khz, u16 ctcss_freq)
{
    u8 result;
    u8 band;
    
    // Auto-detect band based on frequency
    if (freq_khz >= 134000 && freq_khz <= 174000) {
        band = 1;  // VHF 134-174
    } else if (freq_khz >= 200000 && freq_khz <= 260000) {
        band = 2;  // VHF 200-260  
    } else if (freq_khz >= 400000 && freq_khz <= 520000) {
        band = 0;  // UHF 400-520
    } else {
        return AT1846S_ERROR_INVALID_PARAM;
    }
    
    // Set band first
    result = at1846s_set_band(band);
    if (result != AT1846S_SUCCESS) {
        return result;
    }
    
    // Set frequency
    at1846s_set_frequency(freq_khz);
    
    // Set CTCSS if specified
    if (ctcss_freq > 0) {
        result = at1846s_set_ctcss_tx(ctcss_freq);
        if (result != AT1846S_SUCCESS) {
            return result;
        }
        result = at1846s_set_ctcss_rx(ctcss_freq);
        if (result != AT1846S_SUCCESS) {
            return result;
        }
    } else {
        result = at1846s_disable_ctcss();
        if (result != AT1846S_SUCCESS) {
            return result;
        }
    }
    
    return at1846s_reg_flush_cache();
}

//=============================================================================
// VALUE MAPPING AND CONVERSION SYSTEM
//=============================================================================

/**
 * @brief Convert CTCSS frequency in Hz*10 to register value
 * @param freq_hz_x10: CTCSS frequency (Hz * 10, e.g., 1000 for 100.0 Hz)
 * @return Register value for CTCSS frequency
 */
u16 at1846s_ctcss_freq_to_reg(u16 freq_hz_x10)
{
    // AT1846S uses frequency * 100 for CTCSS register encoding
    return AT1846S_CTCSS_TO_REG(freq_hz_x10);
}

/**
 * @brief Convert register value to CTCSS frequency in Hz*10
 * @param reg_value: Register value
 * @return CTCSS frequency (Hz * 10)
 */
u16 at1846s_reg_to_ctcss_freq(u16 reg_value)
{
    // Reverse the conversion: reg_value / 100
    return (u16)(reg_value / 100);
}

/**
 * @brief Convert PA bias register value to voltage in millivolts
 * @param bias_value: PA bias register value (0-63)
 * @return Voltage in millivolts
 */
u16 at1846s_pa_bias_to_mv(u8 bias_value)
{
    return AT1846S_PA_BIAS_TO_MV(bias_value);
}

/**
 * @brief Convert voltage in millivolts to PA bias register value
 * @param voltage_mv: Voltage in millivolts
 * @return PA bias register value (0-63)
 */
u8 at1846s_mv_to_pa_bias(u16 voltage_mv)
{
    u8 bias_value = AT1846S_MV_TO_PA_BIAS(voltage_mv);
    
    // Clamp to valid range
    if (bias_value > AT1846S_PA_BIAS_MAX) {
        bias_value = AT1846S_PA_BIAS_MAX;
    }
    
    return bias_value;
}

/**
 * @brief Convert frequency in kHz to register values
 * @param freq_khz: Frequency in kHz
 * @param freq_high: Pointer to store high word register value
 * @param freq_low: Pointer to store low word register value
 * @return AT1846S_SUCCESS on success, error code on failure
 */
u8 at1846s_freq_to_registers(u32 freq_khz, u16 *freq_high, u16 *freq_low)
{
    u32 freq_reg_value;
    
    if (!freq_high || !freq_low) {
        return AT1846S_ERROR_INVALID_PARAM;
    }
    
    // Validate frequency range
    if (freq_khz < AT1846S_FREQ_MIN_KHZ || freq_khz > AT1846S_FREQ_MAX_KHZ) {
        return AT1846S_ERROR_INVALID_PARAM;
    }
    
    // Convert using the macro
    freq_reg_value = AT1846S_FREQ_TO_REG(freq_khz);
    
    // Split into high and low words
    *freq_high = (u16)((freq_reg_value >> 16) & 0x3FFF);  // Upper 14 bits
    *freq_low = (u16)(freq_reg_value & 0xFFFF);           // Lower 16 bits
    
    return AT1846S_SUCCESS;
}

/**
 * @brief Convert register values to frequency in kHz
 * @param freq_high: High word register value
 * @param freq_low: Low word register value
 * @return Frequency in kHz
 */
u32 at1846s_registers_to_freq(u16 freq_high, u16 freq_low)
{
    u32 freq_reg_value;
    
    // Combine register values
    freq_reg_value = (((u32)(freq_high & 0x3FFF)) << 16) | freq_low;
    
    // Convert using the macro
    return AT1846S_REG_TO_FREQ(freq_reg_value);
}

/**
 * @brief Convert RSSI register value to dBm
 * @param rssi_reg: RSSI register value
 * @return Signal strength in dBm
 */
i16 at1846s_rssi_to_dbm(u16 rssi_reg)
{
    // AT1846S RSSI conversion formula (typical for AT1846S)
    // RSSI dBm = -137 + (register_value / 2)
    return (i16)(-137 + (rssi_reg / 2));
}

/**
 * @brief Convert battery voltage register to actual voltage in millivolts
 * @param battery_reg: Battery register value
 * @return Battery voltage in millivolts
 */
u16 at1846s_battery_reg_to_mv(u16 battery_reg)
{
    // AT1846S battery voltage conversion (typical scaling)
    // Assuming 10-bit ADC with 3.3V reference and voltage divider
    // V_bat = (reg_value / 1024) * 3300 * 2 (for 2:1 voltage divider)
    return (u16)((battery_reg * 6600) / 1024);
}

/**
 * @brief Find closest standard CTCSS frequency
 * @param target_freq: Target frequency (Hz * 10)
 * @return Closest standard CTCSS frequency (Hz * 10)
 */
u16 at1846s_find_closest_ctcss(u16 target_freq)
{
    // Standard CTCSS frequencies table
    static __code const u16 ctcss_table[] = {
        AT1846S_CTCSS_67_0, AT1846S_CTCSS_71_9, AT1846S_CTCSS_74_4, AT1846S_CTCSS_77_0,
        AT1846S_CTCSS_79_7, AT1846S_CTCSS_82_5, AT1846S_CTCSS_85_4, AT1846S_CTCSS_88_5,
        AT1846S_CTCSS_91_5, AT1846S_CTCSS_94_8, AT1846S_CTCSS_97_4, AT1846S_CTCSS_100_0,
        AT1846S_CTCSS_103_5, AT1846S_CTCSS_107_2, AT1846S_CTCSS_110_9, AT1846S_CTCSS_114_8,
        AT1846S_CTCSS_118_8, AT1846S_CTCSS_123_0, AT1846S_CTCSS_127_3, AT1846S_CTCSS_131_8,
        AT1846S_CTCSS_136_5, AT1846S_CTCSS_141_3, AT1846S_CTCSS_146_2, AT1846S_CTCSS_151_4,
        AT1846S_CTCSS_156_7, AT1846S_CTCSS_162_2, AT1846S_CTCSS_167_9, AT1846S_CTCSS_173_8,
        AT1846S_CTCSS_179_9, AT1846S_CTCSS_186_2, AT1846S_CTCSS_192_8, AT1846S_CTCSS_203_5,
        AT1846S_CTCSS_210_7, AT1846S_CTCSS_218_1, AT1846S_CTCSS_225_7, AT1846S_CTCSS_233_6,
        AT1846S_CTCSS_241_8, AT1846S_CTCSS_250_3
    };
    
    const u8 table_size = sizeof(ctcss_table) / sizeof(ctcss_table[0]);
    u8 i;
    u16 closest_freq = ctcss_table[0];
    u16 min_difference = (target_freq > ctcss_table[0]) ? 
                        (target_freq - ctcss_table[0]) : 
                        (ctcss_table[0] - target_freq);
    
    // Find the frequency with minimum difference
    for (i = 1; i < table_size; i++) {
        u16 difference = (target_freq > ctcss_table[i]) ? 
                        (target_freq - ctcss_table[i]) : 
                        (ctcss_table[i] - target_freq);
        
        if (difference < min_difference) {
            min_difference = difference;
            closest_freq = ctcss_table[i];
        }
    }
    
    return closest_freq;
}

/**
 * @brief Validate frequency for specific band
 * @param freq_khz: Frequency in kHz
 * @param band: Band identifier
 * @return 1 if frequency is valid for band, 0 otherwise
 */
u8 at1846s_validate_frequency_for_band(u32 freq_khz, u8 band)
{
    switch (band) {
        case 0: // UHF 400-520 MHz
            return (freq_khz >= AT1846S_FREQ_UHF_MIN && freq_khz <= AT1846S_FREQ_UHF_MAX) ? 1 : 0;
            
        case 1: // VHF 134-174 MHz
            return (freq_khz >= AT1846S_FREQ_VHF_LOW_MIN && freq_khz <= AT1846S_FREQ_VHF_LOW_MAX) ? 1 : 0;
            
        case 2: // VHF 200-260 MHz
            return (freq_khz >= AT1846S_FREQ_VHF_HIGH_MIN && freq_khz <= AT1846S_FREQ_VHF_HIGH_MAX) ? 1 : 0;
            
        default:
            return 0;
    }
}

/**
 * @brief Convert power level percentage to register value
 * @param power_percent: Power level as percentage (0-100)
 * @return Register value for PA gain control
 */
u8 at1846s_power_percent_to_reg(u8 power_percent)
{
    if (power_percent > 100) {
        power_percent = 100;
    }
    
    // Convert percentage to register range (0-15 typical for PA gain)
    return (u8)((power_percent * 15) / 100);
}

/**
 * @brief Convert register value to power level percentage
 * @param reg_value: Register value
 * @return Power level as percentage (0-100)
 */
u8 at1846s_reg_to_power_percent(u8 reg_value)
{
    if (reg_value > 15) {
        reg_value = 15;
    }
    
    // Convert register value to percentage
    return (u8)((reg_value * 100) / 15);
}

/**
 * @brief Calculate squelch threshold in dBm
 * @param squelch_level: Squelch level (0-15)
 * @return Threshold in dBm
 */
i16 at1846s_squelch_level_to_dbm(u8 squelch_level)
{
    if (squelch_level > 15) {
        squelch_level = 15;
    }
    
    // AT1846S squelch calculation (typical formula)
    // Threshold dBm = -137 + (level * 3)
    return (i16)(-137 + (squelch_level * 3));
}

/**
 * @brief Convert dBm threshold to squelch level
 * @param threshold_dbm: Threshold in dBm
 * @return Squelch level (0-15)
 */
u8 at1846s_dbm_to_squelch_level(i16 threshold_dbm)
{
    i16 level;
    
    // Reverse the calculation
    level = (threshold_dbm + 137) / 3;
    
    // Clamp to valid range
    if (level < 0) level = 0;
    if (level > 15) level = 15;
    
    return (u8)level;
}

/**
 * @brief Create a complete configuration from basic parameters
 * @param freq_khz: Operating frequency
 * @param volume: Volume level (0-15)
 * @param ctcss_freq: CTCSS frequency (0 = none)
 * @param config: Pointer to store created configuration
 * @return AT1846S_SUCCESS on success, error code on failure
 */
u8 at1846s_create_simple_config(u32 freq_khz, u8 volume, u16 ctcss_freq, 
                                at1846s_config_t *config)
{
    if (!config) {
        return AT1846S_ERROR_INVALID_PARAM;
    }
    
    // Auto-detect band
    if (freq_khz >= 134000 && freq_khz <= 174000) {
        config->band = 1;  // VHF 134-174
    } else if (freq_khz >= 200000 && freq_khz <= 260000) {
        config->band = 2;  // VHF 200-260
    } else if (freq_khz >= 400000 && freq_khz <= 520000) {
        config->band = 0;  // UHF 400-520
    } else {
        return AT1846S_ERROR_INVALID_PARAM;
    }
    
    // Validate frequency for the detected band
    if (!at1846s_validate_frequency_for_band(freq_khz, config->band)) {
        return AT1846S_ERROR_INVALID_PARAM;
    }
    
    // Set basic parameters
    config->frequency_khz = freq_khz;
    config->volume = (volume > 15) ? 15 : volume;
    
    // Set reasonable defaults
    config->mic_gain = 16;
    config->voice_gain = 40;
    config->squelch_level = 5;
    config->pa_bias = 32;
    config->tx_power = 12;
    config->emphasis = 1;
    config->compander = 0;
    config->noise_gate = 3;
    config->deviation = 8;
    config->vox_enabled = 0;
    config->vox_sensitivity = 8;
    config->vox_delay = 2;
    
    // Configure CTCSS
    if (ctcss_freq > 0) {
        config->ctcss_tx_freq = at1846s_find_closest_ctcss(ctcss_freq);
        config->ctcss_rx_freq = config->ctcss_tx_freq;
        config->subaudio_mode = AT1846S_SUBAUDIO_CTCSS;
    } else {
        config->ctcss_tx_freq = 0;
        config->ctcss_rx_freq = 0;
        config->subaudio_mode = AT1846S_SUBAUDIO_NONE;
    }
    
    config->cdcss_tx_code = 0;
    config->cdcss_rx_code = 0;
    
    return AT1846S_SUCCESS;
}

