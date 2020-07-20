#include "i2c.h"

#include "i2c_impl.h"

inline bool i2c_master_init(void) {
    return _i2c_master_init_impl();
}

inline bool
  i2c_master_write(const uint8_t device, const uint8_t reg, const void* buf, uint16_t len) {
    return _i2c_master_write_impl(device, reg, buf, len);
}

inline bool i2c_master_read(const uint8_t device, const uint8_t reg, void* buf, uint16_t len) {
    return _i2c_master_read_impl(device, reg, buf, len);
}

inline bool i2c_master_write_u8(const uint8_t device, const uint8_t reg, const uint8_t data) {
    return i2c_master_write(device, reg, &data, 1);
}

inline bool i2c_master_write_u16(const uint8_t device, const uint8_t reg, const uint16_t data) {
    return i2c_master_write(device, reg, &data, 2);
}

inline bool i2c_master_write_u32(const uint8_t device, const uint8_t reg, const uint32_t data) {
    return i2c_master_write(device, reg, &data, 4);
}

inline bool i2c_master_read_u8(const uint8_t device, const uint8_t reg, uint8_t* data) {
    return i2c_master_read(device, reg, data, 1);
}

inline bool i2c_master_read_u16(const uint8_t device, const uint8_t reg, uint16_t* data) {
    return i2c_master_read(device, reg, (uint8_t*)data, 2);
}

inline bool i2c_master_read_u32(const uint8_t device, const uint8_t reg, uint32_t* data) {
    return i2c_master_read(device, reg, (uint8_t*)data, 4);
}