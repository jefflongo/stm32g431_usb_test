#include "max17048.h"

#include "i2c.h"

// MAX17048 i2c address
#define MAX_ADDR 0x6C

// MAX17048 registers
#define VCELL 0x02     // R/-: ADC measurement of VCELL, 78.125uV/cell
#define SOC 0x04       // R/-: Battery state of charge, 1%/256
#define MODE 0x06      // -/W: Current operating mode, enable sleep
#define VERSION 0x08   // R/-: Production version
#define HIBRT 0x0A     // R/W: Hibernation thresholds
#define CONFIG 0x0C    // R/W: Compensation, toggle sleep, alert masks, config
#define VALRT 0x14     // R/W: Voltage level to generate alert
#define CRATE 0x16     // R/-: Charge/discharge rate, 0.208%/hr
#define VRESET_ID 0x18 // R/W: VCELL for chip reset
#define STATUS 0x1A    // R/W: Over/undervoltage, SOC change/low, reset alerts
#define TABLE 0x40     // -/W: Configures battery parameters
#define CMD 0xFE       // R/W: POR command

// MAX17048 masks
#define ALRT_MASK 0x0020

int max_init(void) {
    int ok = I2C_OK;

    if (ok == I2C_OK)
        ok = i2c_master_write_u16(
          MAX_ADDR, VALRT, 0xA5D7); // Set VALRT.MIN = 3.3V, VALRT.MAX = 4.3V
    if (ok == I2C_OK)
        ok = i2c_master_write_u8(
          MAX_ADDR, VRESET_ID, 0x7C); // Set VRESET to 2.5V (captive batteries)
    if (ok == I2C_OK)
        ok = i2c_master_write_u8(MAX_ADDR, STATUS, 0x00); // Clear all alerts
    if (ok == I2C_OK)
        ok = i2c_master_write_u16(
          // Disable SOC change alert, set bat low threshold, deassert alert
          MAX_ADDR,
          CONFIG,
          0x9700 | 32 - (BAT_LOW_PERCENT % 32));

    return (ok == I2C_OK) ? MAX_OK : MAX_FAILURE;
}

int max_clear_alert(void) {
    int ok = I2C_OK;

    uint16_t buf;
    if (ok == I2C_OK) ok = i2c_master_write_u8(MAX_ADDR, STATUS, 0x00);
    if (ok == I2C_OK) ok = i2c_master_read_u16(MAX_ADDR, CONFIG, &buf);
    if (ok == I2C_OK) buf &= ~ALRT_MASK;
    if (ok == I2C_OK) ok = i2c_master_write_u16(MAX_ADDR, CONFIG, buf);

    return (ok == I2C_OK) ? MAX_OK : MAX_FAILURE;
}

int max_get_vcell(uint16_t* voltage) {
    int ok = I2C_OK;

    if (ok == I2C_OK) ok = i2c_master_read_u16(MAX_ADDR, VCELL, voltage);
    if (ok == I2C_OK) *voltage *= 0.078125;

    return (ok == I2C_OK) ? MAX_OK : MAX_FAILURE;
}

int max_get_soc(uint8_t* percent) {
    int ok = I2C_OK;

    if (ok == I2C_OK) ok = i2c_master_read_u8(MAX_ADDR, SOC, percent);

    return (ok == I2C_OK) ? MAX_OK : MAX_FAILURE;
}

int max_get_status(uint8_t* status) {
    int ok = I2C_OK;

    if (ok == I2C_OK) ok = i2c_master_read_u8(MAX_ADDR, STATUS, status);

    return (ok == I2C_OK) ? MAX_OK : MAX_FAILURE;
}