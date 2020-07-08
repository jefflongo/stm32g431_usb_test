/*
 * File:     max17048.h
 * Author:   jefflongo
 * Comments: MAX17048 driver
 */

#ifndef MAX17048_H
#define MAX17048_H

#include <stdint.h>

// User adjustable parameters
#define BAT_LOW_PERCENT 15 // 1 to 32
// --------------------------

// MAX17048 status masks
#define MAX_STATUS_RESET_MASK 0x01
#define MAX_STATUS_OVERVOLTED_MASK 0x02
#define MAX_STATUS_UNDERVOLTED_MASK 0x04
#define MAX_STATUS_VOLTAGE_RESET_MASK 0x80
#define MAX_STATUS_SOC_LOW_MASK 0x10
#define MAX_STATUS_SOC_CHANGE_MASK 0x20

#define MAX_OK 0
#define MAX_FAILURE -1

int max_init(void);
int max_clear_alert(void);
int max_get_vcell(uint16_t* voltage);
int max_get_soc(uint8_t* percent);
int max_get_status(uint8_t* status);

#endif /* MAX17048_H */
