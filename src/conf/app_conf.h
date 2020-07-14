#pragma once

#include "stusb4500.h"

#include <stm32g4xx_hal.h>

static stusb4500_config_t stusb4500_config = {
    .min_current_ma = 1500,
    .min_voltage_mv = 5000,
    .max_voltage_mv = 15000,
    .get_ms = HAL_GetTick,
    .delay_ms = HAL_Delay,
};
