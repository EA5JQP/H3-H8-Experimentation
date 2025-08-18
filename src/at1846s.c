#include "at1846s.h"

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
    
void at1846s_spi_transceive(u8 spi_command, u8 *reg_low, u8 *reg_high) __critical 
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

    // Receive low and high bytes
    *reg_low = at1846s_spi_read_byte();
    *reg_high = at1846s_spi_read_byte();
    
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
