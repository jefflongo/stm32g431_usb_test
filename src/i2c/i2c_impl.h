#pragma once

#include <stdbool.h>
#include <stdint.h>

#define I2C_TIMEOUT 5

bool _i2c_master_init_impl(void);
bool _i2c_master_write_impl(const uint8_t device, const uint8_t reg, const void* buf, uint16_t len);
bool _i2c_master_read_impl(const uint8_t device, const uint8_t reg, void* buf, uint16_t len);
