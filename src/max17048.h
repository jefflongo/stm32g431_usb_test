#pragma once

#include <stdbool.h>
#include <stdint.h>

#define BAT_LOW_PERCENT 15 // 1 to 32

// MAX17048 status masks
#define MAX_STATUS_RESET_MASK 0x01
#define MAX_STATUS_OVERVOLTED_MASK 0x02
#define MAX_STATUS_UNDERVOLTED_MASK 0x04
#define MAX_STATUS_VOLTAGE_RESET_MASK 0x80
#define MAX_STATUS_SOC_LOW_MASK 0x10
#define MAX_STATUS_SOC_CHANGE_MASK 0x20

bool max_init(void);
bool max_clear_alert(void);
bool max_get_vcell(uint16_t* voltage);
bool max_get_soc(uint8_t* percent);
bool max_get_status(uint8_t* status);
