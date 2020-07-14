#pragma once

#include <stdbool.h>
#include <stdint.h>

#define BAT_LOW_PERCENT 15 // 1 to 32

typedef uint16_t max17048_voltage_t;

typedef uint8_t max17048_soc_t;

enum {
    MAX_ALERT_RESET = 0x01,
    MAX_ALERT_OVERVOLTED = 0x02,
    MAX_ALERT_UNDERVOLTED = 0x04,
    MAX_ALERT_VOLTAGE_RESET = 0x80,
    MAX_ALERT_SOC_LOW = 0x10,
    MAX_ALERT_SOC_CHANGE = 0x20,
};
typedef uint8_t max17048_alert_t;

bool max17048_is_present(void);

__attribute__((nonnull)) bool max17048_get_vcell(max17048_voltage_t* mv);

__attribute__((nonnull)) bool max17048_get_soc(max17048_soc_t* percent);

bool max17048_set_bat_low_soc(max17048_soc_t percent);

bool max17048_set_undervolted_voltage(max17048_voltage_t mv);

bool max17048_set_overvolted_voltage(max17048_voltage_t mv);

bool max17048_set_reset_voltage(max17048_voltage_t mv);

bool max17048_set_soc_change_alert(bool enable);

bool max17048_set_voltage_reset_alert(bool enable);

bool max17048_clear_alerts(void);

__attribute__((nonnull)) bool max17048_get_alerts(max17048_alert_t* status);
