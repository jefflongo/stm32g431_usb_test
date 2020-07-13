#pragma once

#include <stdint.h>

typedef enum {
    BQ_IIN_MAX_100MA = 0b000,
    BQ_IIN_MAX_150MA = 0b001,
    BQ_IIN_MAX_500MA = 0b010,
    BQ_IIN_MAX_900MA = 0b011,
    BQ_IIN_MAX_1200MA = 0b100,
    BQ_IIN_MAX_1500MA = 0b101,
    BQ_IIN_MAX_2000MA = 0b110,
    BQ_IIN_MAX_3000MA = 0b111,
} bq24292i_iin_max_t;

typedef uint16_t bq24292i_vin_max_t;

typedef uint16_t bq24292i_vsys_min_t;

typedef enum {
    BQ_CHG_CONFIG_DISABLE = 0b00,
    BQ_CHG_CONFIG_ENABLE = 0b01,
    BQ_CHG_CONFIG_OTG = 0b10,
} bq24292i_chg_config_t;

typedef uint16_t bq24292i_chg_current_t;

typedef uint16_t bq24292i_term_current_t;

typedef uint16_t bq24292i_prechg_current_t;

typedef enum {
    BQ_VRECHG_100MV = 0,
    BQ_VRECHG_300MV = 1,
} bq24292i_vrechg_offset_t;

typedef enum {
    BQ_VBATLOW_2800MV = 0,
    BQ_VBATLOW_3000MV = 1,
} bq24292i_vbatlow_t;

typedef uint16_t bq24292i_vchg_max_t;

typedef enum {
    BQ_CHG_TIMER_5H = 0b00,
    BQ_CHG_TIMER_8H = 0b01,
    BQ_CHG_TIMER_12H = 0b10,
    BQ_CHG_TIMER_20H = 0b11,
    BQ_CHG_TIMER_DISABLE,
} bq24292i_chg_timer_t;

typedef enum {
    BQ_WATCHDOG_DISABLE = 0b00,
    BQ_WATCHDOG_40S = 0b01,
    BQ_WATCHDOG_80S = 0b10,
    BQ_WATCHDOG_160S = 0b11,
} bq24292i_watchdog_conf_t;

typedef enum {
    BQ_MAX_TEMP_60C = 0b00,
    BQ_MAX_TEMP_80C = 0b01,
    BQ_MAX_TEMP_100C = 0b10,
    BQ_MAX_TEMP_120C = 0b11,
} bq_24292i_max_temp_t;

typedef uint8_t bq24292i_clamp_voltage_t;

typedef uint8_t bq24292i_comp_resistor_t;

typedef enum {
    BQ_INT_NONE = 0b00,
    BQ_INT_BAT_FAULT = 0b01,
    BQ_INT_CHRG_FAULT = 0b10,
    BQ_INT_BAT_AND_CHRG_FAULT = 0b11,
} bq24292i_interrupt_mask_t;

typedef enum {
    BQ_STATE_NOT_CHARGING = 0b00,
    BQ_STATE_PRECHARGE = 0b01,
    BQ_STATE_FAST_CHARGE = 0b10,
    BQ_STATE_TERMINATED = 0b11,
} bq24292i_charge_state_t;

typedef enum {
    BQ_SOURCE_UNKNOWN = 0b00,
    BQ_SOURCE_USB = 0b01,
    BQ_SOURCE_ADAPTER = 0b10,
    BQ_SOURCE_OTG = 0b11,
} bq24292i_source_type_t;

enum {
    BQ_FAULT_THERM = 0x07,
    BQ_FAULT_BAT = 0x08,
    BQ_FAULT_CHG_ = 0x30,
    BQ_FAULT_BOOST = 0x40,
    BQ_FAULT_WATCHDOG = 0x80,
};
typedef uint8_t bq24292i_fault_t;
