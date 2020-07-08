#include "bq24292i.h"

#include "i2c.h"

#include <stdbool.h>

// BQ24292i i2c address
#define BQ_ADDR 0x6B

// BQ24292i registers
#define INPUT_SRC_CTRL 0x00            // R/W: Input source control
#define PWR_ON_CONF 0x01               // R/W: Power-on configuration
#define CHRG_CURRENT_CTRL 0x02         // R/W: Charge current control
#define PRECHRG_TERM_CURRENT_CTRL 0x03 // R/W: Pre-charge/termination current control
#define CHRG_VOLTAGE_CTRL 0x04         // R/W: Charge voltage control
#define CHRG_TERM_TIMER_CTRL 0x05      // R/W: Charge termination/timer control
#define IR_COMP_THERMAL_REG_CTRL 0x06  // R/W: IR compensation / thermal regulation control
#define MISC_OPERATION_CTRL 0x07       // R/W: Misc operation control
#define SYSTEM_STATUS 0x08             // R/-: System status
#define FAULT 0x09                     // R/-: Fault
#define VENDOR_PART_REV_STATUS 0x0A    // R/-: Vendor / part / revision status

// BQ24292i masks/constants
#define IIN_MAX_POS 0
#define IIN_MAX_MSK (0x07 << IIN_MAX_POS)

#define VIN_MAX_POS 3
#define VIN_MAX_MSK (0x0F << VIN_MAX_POS)
#define VIN_MAX_OFFSET 3880
#define VIN_MAX_INCR 80

#define VSYS_MIN_POS 1
#define VSYS_MIN_MSK (0x07 << VSYS_MIN_POS)
#define VSYS_MIN_OFFSET 3000
#define VSYS_MIN_INCR 100

#define CHG_CONFIG_POS 4
#define CHG_CONFIG_MSK (0x03 << CHG_CONFIG_POS)

#define WDT_POS 6
#define WDT_MSK (0x01 << WDT_POS)

#define ICHG_POS 2
#define ICHG_MSK (0x3F << ICHG_POS)
#define ICHG_OFFSET 512
#define ICHG_INCR 64

#define ITERM_POS 0
#define ITERM_MSK (0x0F << ITERM_POS)
#define ITERM_OFFSET 128
#define ITERM_INCR 128

#define IPRECHG_POS 4
#define IPRECHG_MSK (0x0F << IPRECHG_POS)
#define IPRECHG_OFFSET 128
#define IPRECHG_INCR 128

#define VRECHG_POS 0
#define VRECHG_MSK (0x01 << VRECHG_POS)

#define VBATLOW_POS 1
#define VBATLOW_MSK (0x01 << VBATLOW_POS)

#define VCHG_MAX_POS 2
#define VCHG_MAX_MSK (0x3F << VCHG_MAX_POS)
#define VCHG_MAX_OFFSET 3504
#define VCHG_MAX_INCR 16

#define CHG_TIMER_POS 1
#define CHG_TIMER_MSK (0x03 << CHG_TIMER_POS)
#define CHG_TIMER_EN_POS 3
#define CHG_TIMER_EN_MSK (0x01 << CHG_TIMER_EN_POS)

#define WDT_CONF_POS 4
#define WDT_CONF_MSK (0x03 << WDT_CONF_POS)

#define TERM_EN_POS 7
#define TERM_EN_MSK (0x01 << TERM_EN_POS)

#define THERMAL_REG_POS 0
#define THERMAL_REG_MSK (0x03 << THERMAL_REG_POS)

#define VCLAMP_POS 2
#define VCLAMP_MSK (0x07 << VCLAMP_POS)
#define VCLAMP_OFFSET 0
#define VCLAMP_INCR 16

#define BAT_COMP_POS 5
#define BAT_COMP_MSK (0x07 << BAT_COMP_POS)
#define BAT_COMP_OFFSET 0
#define BAT_COMP_INCR 10

#define INT_MASK_POS 0
#define INT_MASK_MSK (0x03 << INT_MASK_POS)

#define BATFET_POS 5
#define BATFET_MSK (0x01 << BATFET_POS)

#define VSYS_STAT_POS 0
#define VSYS_STAT_MSK (0x01 << VSYS_STAT_POS)

#define THERM_STAT_POS 1
#define THERM_STAT_MSK (0x01 << THERM_STAT_POS)

#define PG_STAT_POS 2
#define PG_STAT_MSK (0x01 << PG_STAT_POS)

#define DPM_STAT_POS 3
#define DPM_STAT_MSK (0x01 << DPM_STAT_POS)

#define CHRG_STAT_POS 4
#define CHRG_STAT_MSK (0x03 << CHRG_STAT_POS)

#define VBUS_STAT_POS 6
#define VBUS_STAT_MSK (0x03 << VBUS_STAT_POS)

#define PART_NUMBER_POS 3
#define PART_NUMBER_MSK (0x07 << PART_NUMBER_POS)
#define PART_NUMBER 0b011

__attribute__((nonnull)) static inline bool read_reg(uint8_t const reg, uint8_t* out) {
    return i2c_master_read_u8(BQ_ADDR, reg, out);
}

static inline bool write_reg(uint8_t const reg, uint8_t const data) {
    return i2c_master_write_u8(BQ_ADDR, reg, data);
}

static inline bool modify_reg(uint8_t const reg, uint8_t const data, uint8_t const mask) {
    uint8_t buf;
    if (!read_reg(reg, &buf)) {
        return false;
    }

    buf = (buf & ~mask) | (data & mask);
    return write_reg(reg, buf)
}

static uint8_t perform_adc(uint16_t in, uint16_t const offset, uint16_t const incr) {
    if (in < offset) {
        return 0;
    }

    in -= offset;
    uint8_t out = 0;

    for (uint8_t i = 0; i < 8; i++) {
        uint16_t val = incr * (1u << (7 - i));
        if (in >= val) {
            in -= val;
            out |= (1u << (7 - i));
        }
    }

    return out;
}

static uint16_t perform_dac(uint8_t in, uint16_t const offset, uint16_t const incr) {
    uint16_t out = offset;
    for (uint8_t i = 0; i < 8; i++) {
        if (in & (1u << i)) {
            out += incr * (1u << i);
        }
    }

    return out;
}

bool bq24292i_is_present(void) {
    uint8_t data;
    if (!read_reg(VENDOR_PART_REV_STATUS, &data)) {
        return false;
    }

    return (((data & PART_NUMBER_MSK) >> PART_NUMBER_POS) == PART_NUMBER)
}

bool bq24292i_set_iin_max(bq24292i_iin_max_t const ma) {
    uint8_t const data = (uint8_t)((ma << IIN_MAX_POS) & IIN_MAX_MSK);

    return modify_reg(INPUT_SRC_CTRL, data, IIN_MAX_MSK);
}

bool bq24292i_get_iin_max(bq24292i_iin_max_t* ma) {
    uint8_t data;
    if (!read_reg(INPUT_SRC_CTRL, &data)) {
        return false;
    }

    *ma = (bq24292i_iin_max_t)((data & IIN_MAX_MSK) >> IIN_MAX_POS);
    return true;
}

bool bq24292i_set_vin_max(bq24292i_vin_max_t const mv) {
    uint8_t const data =
      (perform_adc(mv, VIN_MAX_OFFSET, VIN_MAX_INCR) << VIN_MAX_POS) & VIN_MAX_MSK;

    return modify_reg(INPUT_SRC_CTRL, data, VIN_MAX_MSK);
}

bool bq24292i_get_vin_max(bq24292i_vin_max_t* mv) {
    uint8_t data;
    if (!read_reg(INPUT_SRC_CTRL, &data)) {
        return false;
    }

    *mv = (bq24292i_vin_max_t)(
      perform_dac((data & VIN_MAX_MSK) >> VIN_MAX_POS), VIN_MAX_OFFSET, VIN_MAX_INCR);
    return true;
}

bool bq24292i_set_vsys_min(bq24292i_vsys_min_t const mv) {
    uint8_t const data =
      (perform_adc(mv, VSYS_MIN_OFFSET, VSYS_MIN_INCR) << VSYS_MIN_POS) & VSYS_MIN_MSK;

    return modify_reg(PWR_ON_CONF, data, VSYS_MIN_MSK);
}

bool bq24292i_get_vin_max(bq24292i_vsys_min_t* mv) {
    uint8_t data;
    if (!read_reg(PWR_ON_CONF, &data)) {
        return false;
    }

    *mv = (bq24292i_vsys_min_t)(
      perform_dac((data & VSYS_MIN_MSK) >> VSYS_MIN_POS), VSYS_MIN_OFFSET, VSYS_MIN_INCR);
    return true;
}

bool bq24292i_reset_wdt(void) {
    return modify_reg(PWR_ON_CONF, (1 << WDT_POS), WDT_MSK);
}

bool bq24292i_set_charge_config(bq24292i_chg_config_t const conf) {
    uint8_t const data = (uint8_t)((conf << CHG_CONFIG_POS) & CHG_CONFIG_MSK);

    return modify_reg(PWR_ON_CONF, data, CHG_CONFIG_MSK);
}

bool bq24292i_get_charge_config(bq24292i_chg_config_t* conf) {
    uint8_t data;
    if (!read_reg(PWR_ON_CONF, &data)) {
        return false;
    }

    *conf = (bq24292i_chg_config_t)((data & CHG_CONFIG_MSK) >> CHG_CONFIG_POS);
    return true;
}

bool bq24292i_set_charge_current(bq24292i_chg_current_t const ma) {
    uint8_t const data = (perform_adc(ma, ICHG_OFFSET, ICHG_INCR) << ICHG_POS) & ICHG_MSK;

    return modify_reg(CHRG_CURRENT_CTRL, data, ICHG_MSK);
}

bool bq24292i_get_charge_current(bq24292i_chg_current_t* ma) {
    uint8_t data;
    if (!read_reg(CHRG_CURRENT_CTRL, &data)) {
        return false;
    }

    *ma =
      (bq24292i_chg_current_t)(perform_dac((data & ICHG_MSK) >> ICHG_POS), ICHG_OFFSET, ICHG_INCR);
    return true;
}

bool bq24292i_set_precharge_current(bq24292i_prechg_current_t const ma) {
    uint8_t const data =
      (perform_adc(ma, IPRECHG_OFFSET, IPRECHG_INCR) << IPRECHG_POS) & IPRECHG_MSK;

    return modify_reg(PRECHRG_TERM_CURRENT_CTRL, data, IPRECHG_MSK);
}

bool bq24292i_get_charge_current(bq24292i_prechg_current_t* ma) {
    uint8_t data;
    if (!read_reg(PRECHRG_TERM_CURRENT_CTRL, &data)) {
        return false;
    }

    *ma = (bq24292i_prechg_current_t)(
      perform_dac((data & IPRECHG_MSK) >> IPRECHG_POS), IPRECHG_OFFSET, IPRECHG_INCR);
    return true;
}

bool bq24292i_set_precharge_current(bq24292i_prechg_current_t const ma) {
    uint8_t const data =
      (perform_adc(ma, IPRECHG_OFFSET, IPRECHG_INCR) << IPRECHG_POS) & IPRECHG_MSK;

    return modify_reg(PRECHRG_TERM_CURRENT_CTRL, data, IPRECHG_MSK);
}

bool bq24292i_get_term_current(bq24292i_term_current_t* ma) {
    uint8_t data;
    if (!read_reg(PRECHRG_TERM_CURRENT_CTRL, &data)) {
        return false;
    }

    *ma = (bq24292i_term_current_t)(
      perform_dac((data & ITERM_MSK) >> ITERM_POS), ITERM_OFFSET, ITERM_INCR);
    return true;
}

bool bq24292i_set_recharge_offset(bq24292i_vrechg_offset_t const offset) {
    uint8_t const data = (uint8_t)((offset << VRECHG_POS) & VRECHG_MSK);

    return modify_reg(CHRG_VOLTAGE_CTRL, data, VRECHG_MSK);
}

bool bq24292i_set_recharge_offset(bq24292i_vrechg_offset_t* offset) {
    uint8_t data;
    if (!read_reg(CHRG_VOLTAGE_CTRL, &data)) {
        return false;
    }

    *offset = (bq24292i_vrechg_offset_t)((data & VRECHG_MSK) >> VRECHG_POS);
    return true;
}

bool bq24292i_set_batlow_voltage(bq24292i_vbatlow_t const mv) {
    uint8_t const data = (uint8_t)((mv << VBATLOW_POS) & VBATLOW_MSK);

    return modify_reg(CHRG_VOLTAGE_CTRL, data, VBATLOW_MSK);
}

bool bq24292i_get_batlow_voltage(bq24292i_vbatlow_t* mv) {
    uint8_t data;
    if (!read_reg(CHRG_VOLTAGE_CTRL, &data)) {
        return false;
    }

    *mv = (bq24292i_vbatlow_t)((data & VBATLOW_MSK) >> VBATLOW_POS);
    return true;
}

bool bq24292i_set_vchg_max(bq24292i_vchg_max_t const mv) {
    uint8_t const data =
      (perform_adc(mv, VCHG_MAX_OFFSET, VCHG_MAX_INCR) << VCHG_MAX_POS) & VCHG_MAX_MSK;

    return modify_reg(CHRG_VOLTAGE_CTRL, data, VCHG_MAX_MSK);
}

bool bq24292i_get_vchg_max(bq24292i_vchg_max_t* mv) {
    uint8_t data;
    if (!read_reg(CHRG_VOLTAGE_CTRL, &data)) {
        return false;
    }

    *mv = (bq24292i_vchg_max_t)(
      perform_dac((data & VCHG_MAX_MSK) >> VCHG_MAX_POS), VCHG_MAX_OFFSET, VCHG_MAX_INCR);
    return true;
}

bool bq24292i_set_charge_timer(bq24292i_chg_timer_t const conf) {
    if (conf != BQ_CHG_TIMER_DISABLE) {
        uint8_t const data = (uint8_t)(
          (1u << CHG_TIMER_EN_POS) & CHG_TIMER_EN_MSK | (conf << CHG_TIMER_POS) & CHG_TIMER_MSK);
        return modify_reg(CHRG_TERM_TIMER_CTRL, data, CHG_TIMER_EN_MSK | CHG_TIMER_MSK);
    } else {
        return modify_reg(CHRG_TERM_TIMER_CTRL, 0, CHG_TIMER_EN_MSK);
    }
}

bool bq24292i_get_charge_timer(bq24292i_chg_timer_t* conf) {
    uint8_t data;
    if (!read_reg(CHRG_TERM_TIMER_CTRL, &data)) {
        return false;
    }

    if (!(data & CHG_TIMER_EN_MSK)) {
        *conf = BQ_CHG_TIMER_DISABLE;
    } else {
        *conf = (bq24292i_chg_timer_t)((data & CHG_TIMER_MSK) >> CHG_TIMER_POS);
    }
    return true;
}

bool bq24292i_set_wdt_config(bq24292i_watchdog_conf_t const conf) {
    uint8_t const data = (uint8_t)((conf << WDT_CONF_POS) & WDT_CONF_MSK);

    return modify_reg(CHRG_TERM_TIMER_CTRL, data, WDT_CONF_MSK);
}

bool bq24292i_get_wdt_config(bq24292i_watchdog_conf_t* conf) {
    uint8_t data;
    if (!read_reg(CHRG_TERM_TIMER_CTRL, &data)) {
        return false;
    }

    *conf = (bq24292i_watchdog_conf_t)((data & WDT_CONF_MSK) >> WDT_CONF_POS);
    return true;
}

bool bq24292i_set_charge_termination(bool const enable) {
    uint8_t const data = (uint8_t)((enable << TERM_EN_POS) & TERM_EN_MSK);

    return modify_reg(CHRG_TERM_TIMER_CTRL, data, TERM_EN_MSK);
}

bool bq24292i_get_charge_termination(bool* enable) {
    uint8_t data;
    if (!read_reg(CHRG_TERM_TIMER_CTRL, &data)) {
        return false;
    }

    *enable = ((data & TERM_EN_MSK) >> TERM_EN_POS);
    return true;
}

bool bq24292i_set_max_temp(bq_24292i_max_temp_t const temp) {
    uint8_t const data = (uint8_t)((temp << THERMAL_REG_POS) & THERMAL_REG_MSK);

    return modify_reg(IR_COMP_THERMAL_REG_CTRL, data, THERMAL_REG_MSK);
}

bool bq24292i_get_max_temp(bq_24292i_max_temp_t* temp) {
    uint8_t data;
    if (!read_reg(IR_COMP_THERMAL_REG_CTRL, &data)) {
        return false;
    }

    *temp = (bq_24292i_max_temp_t)((data & THERMAL_REG_MSK) >> THERMAL_REG_POS);
    return true;
}

bool bq24292i_set_voltage_clamp(bq24292i_clamp_voltage_t const mv) {
    uint8_t const data = (perform_adc(mv, VCLAMP_OFFSET, VCLAMP_INCR) << VCLAMP_POS) & VCLAMP_MSK;

    return modify_reg(IR_COMP_THERMAL_REG_CTRL, data, VCLAMP_MSK);
}

bool bq24292i_get_voltage_clamp(bq24292i_clamp_voltage_t* mv) {
    uint8_t data;
    if (!read_reg(IR_COMP_THERMAL_REG_CTRL, &data)) {
        return false;
    }

    *mv = (bq24292i_vchg_max_t)(
      perform_dac((data & VCLAMP_MSK) >> VCLAMP_POS), VCLAMP_OFFSET, VCLAMP_INCR);
    return true;
}

bool bq24292i_set_comp_resistor(bq24292i_comp_resistor_t const mohms) {
    uint8_t const data =
      (perform_adc(mohms, BAT_COMP_OFFSET, BAT_COMP_INCR) << BAT_COMP_POS) & BAT_COMP_MSK;

    return modify_reg(IR_COMP_THERMAL_REG_CTRL, data, BAT_COMP_MSK);
}

bool bq24292i_get_comp_resistor(bq24292i_comp_resistor_t* mohms) {
    uint8_t data;
    if (!read_reg(IR_COMP_THERMAL_REG_CTRL, &data)) {
        return false;
    }

    *mohms = (bq24292i_vchg_max_t)(
      perform_dac((data & BAT_COMP_MSK) >> BAT_COMP_POS), BAT_COMP_OFFSET, BAT_COMP_INCR);
    return true;
}

bool bq24292i_set_interrupt_mask(bq24292i_interrupt_mask_t const mask) {
    uint8_t const data = (uint8_t)((mask << INT_MASK_POS) & INT_MASK_MSK);

    return modify_reg(MISC_OPERATION_CTRL, data, INT_MASK_MSK);
}

bool bq24292i_get_interrupt_mask(bq24292i_interrupt_mask_t* mask) {
    uint8_t data;
    if (!read_reg(MISC_OPERATION_CTRL, &data)) {
        return false;
    }

    *mask = (bq24292i_interrupt_mask_t)((data & INT_MASK_MSK) >> INT_MASK_POS);
    return true;
}

bool bq24292i_set_batfet_enabled(bool const enable) {
    uint8_t const data = (uint8_t)((enable << BATFET_POS) & BATFET_MSK);

    return modify_reg(MISC_OPERATION_CTRL, data, BATFET_MSK);
}

bool bq24292i_get_batfet_enabled(bool* enable) {
    uint8_t data;
    if (!read_reg(MISC_OPERATION_CTRL, &data)) {
        return false;
    }

    *enable = ((data & BATFET_MSK) >> BATFET_POS);
    return true;
}

bool bq24292i_is_vsys_boosted(bool result) {
    uint8_t data;
    if (!read_reg(SYSTEM_STATUS, &data)) {
        return false;
    }

    *result = ((data & VSYS_STAT_MSK) >> VSYS_STAT_POS);
    return true;
}

bool bq24292i_is_overtemp(bool result) {
    uint8_t data;
    if (!read_reg(SYSTEM_STATUS, &data)) {
        return false;
    }

    *result = ((data & THERM_STAT_MSK) >> THERM_STAT_POS);
    return true;
}

bool bq24292i_is_power_good(bool result) {
    uint8_t data;
    if (!read_reg(SYSTEM_STATUS, &data)) {
        return false;
    }

    *result = ((data & PG_STAT_MSK) >> PG_STAT_POS);
    return true;
}

bool bq24292i_is_in_dpm(bool result) {
    uint8_t data;
    if (!read_reg(SYSTEM_STATUS, &data)) {
        return false;
    }

    *result = ((data & DPM_STAT_MSK) >> DPM_STAT_POS);
    return true;
}

bool bq24292i_get_charge_state(bq24292i_charge_state_t* state) {
    uint8_t data;
    if (!read_reg(SYSTEM_STATUS, &data)) {
        return false;
    }

    *state = (bq24292i_charge_state_t)((data & CHRG_STAT_MSK) >> CHG_TIMER_POS);
    return true;
}

bool bq24292i_get_source_type(bq24292i_source_type_t* source) {
    uint8_t data;
    if (!read_reg(SYSTEM_STATUS, &data)) {
        return false;
    }

    *source = (bq24292i_source_type_t)((data & VBUS_STAT_MSK) >> VBUS_STAT_POS);
    return true;
}

bool bq24292i_check_faults(bq24292i_fault_mask_t mask, bool* fault) {
    uint8_t data;
    if (!read_reg(FAULT, &data)) {
        return false;
    }

    *fault = data & mask;
    return true;
}
