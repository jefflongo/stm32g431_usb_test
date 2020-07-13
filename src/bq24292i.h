#pragma once

#include "bq24292i_defs.h"

#include <stdbool.h>

bool bq24292i_is_present(void);

bool bq24292i_set_iin_max(bq24292i_iin_max_t ma);
__attribute__((nonnull)) bool bq24292i_get_iin_max(bq24292i_iin_max_t* ma);

bool bq24292i_set_vin_max(bq24292i_vin_max_t mv);
__attribute__((nonnull)) bool bq24292i_get_vin_max(bq24292i_vin_max_t* mv);

bool bq24292i_set_vsys_min(bq24292i_vsys_min_t mv);
__attribute__((nonnull)) bool bq24292i_get_vin_max(bq24292i_vsys_min_t* mv);

bool bq24292i_set_charge_config(bq24292i_chg_config_t conf);
__attribute__((nonnull)) bool bq24292i_get_charge_config(bq24292i_chg_config_t* conf);

bool bq24292i_reset_wdt(void);

bool bq24292i_set_charge_current(bq24292i_chg_current_t ma);
__attribute__((nonnull)) bool bq24292i_get_charge_current(bq24292i_chg_current_t* ma);

bool bq24292i_set_term_current(bq24292i_term_current_t ma);
__attribute__((nonnull)) bool bq24292i_get_term_current(bq24292i_term_current_t* ma);

bool bq24292i_set_precharge_current(bq24292i_prechg_current_t ma);
__attribute__((nonnull)) bool bq24292i_get_precharge_current(bq24292i_prechg_current_t* ma);

bool bq24292i_set_recharge_offset(bq24292i_vrechg_offset_t offset);
__attribute__((nonnull)) bool bq24292i_get_recharge_offset(bq24292i_vrechg_offset_t* offset);

bool bq24292i_set_batlow_voltage(bq24292i_vbatlow_t mv);
__attribute__((nonnull)) bool bq24292i_get_batlow_voltage(bq24292i_vbatlow_t* mv);

bool bq24292i_set_max_charge_voltage(bq24292i_vchg_max_t mv);
__attribute__((nonnull)) bool bq24292i_get_max_charge_voltage(bq24292i_vchg_max_t* mv);

bool bq24292i_set_charge_timer(bq24292i_chg_timer_t conf);
__attribute__((nonnull)) bool bq24292i_get_charge_timer(bq24292i_chg_timer_t* conf);

bool bq24292i_set_wdt_config(bq24292i_watchdog_conf_t conf);
__attribute__((nonnull)) bool bq24292i_get_wdt_config(bq24292i_watchdog_conf_t* conf);

bool bq24292i_set_charge_termination(bool enable);
__attribute__((nonnull)) bool bq24292i_get_charge_termination(bool* enable);

bool bq24292i_set_max_temp(bq_24292i_max_temp_t temp);
__attribute__((nonnull)) bool bq24292i_get_max_temp(bq_24292i_max_temp_t* temp);

bool bq24292i_set_voltage_clamp(bq24292i_clamp_voltage_t mv);
__attribute__((nonnull)) bool bq24292i_get_voltage_clamp(bq24292i_clamp_voltage_t* mv);

bool bq24292i_set_comp_resistor(bq24292i_comp_resistor_t mohms);
__attribute__((nonnull)) bool bq24292i_get_comp_resistor(bq24292i_comp_resistor_t* mohms);

bool bq24292i_set_interrupt_mask(bq24292i_interrupt_mask_t mask);
__attribute__((nonnull)) bool bq24292i_get_interrupt_mask(bq24292i_interrupt_mask_t* mask);

bool bq24292i_set_batfet_enabled(bool enable);
__attribute__((nonnull)) bool bq24292i_get_batfet_enabled(bool* enable);

bool bq24292i_is_vsys_boosted(bool* result);

bool bq24292i_is_overtemp(bool* result);

bool bq24292i_is_power_good(bool* result);

bool bq24292i_is_in_dpm(bool* result);

__attribute__((nonnull)) bool bq24292i_get_charge_state(bq24292i_charge_state_t* state);

__attribute__((nonnull)) bool bq24292i_get_source_type(bq24292i_source_type_t* source);

__attribute__((nonnull)) bool bq24292i_check_faults(bq24292i_fault_t* faults);