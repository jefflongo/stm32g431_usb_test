#pragma once

#include <stm32g4xx_hal.h>

#define SYS_DELAY_MS(ms) (HAL_Delay(ms))
#define SYS_GET_MS() (HAL_GetTick())