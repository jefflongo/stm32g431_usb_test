#pragma once

#include "stusb4500.h"
#include "stusb4500_nvm.h"

#include <stdbool.h>
#include <stm32g4xx_hal.h>

static stusb4500_config_t stusb4500_config = {
    .min_current_ma = 1500,
    .min_voltage_mv = 5000,
    .max_voltage_mv = 12000,
    .get_ms = HAL_GetTick,
};

__attribute__((unused)) static stusb4500_nvm_config_t stusb4500_nvm_config = {
    .pdo1_current_ma = 1500,

    .pdo2_voltage_mv = 9000,
    .pdo2_current_ma = 3000,

    .pdo3_voltage_mv = 15000,
    .pdo3_current_ma = 2000,

    .pdo_current_fallback = 2000,
    .num_valid_pdos = 3,
    .use_src_current = true,
    .only_above_5v = false,
};
