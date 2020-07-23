#include "app_conf.h"
#include "board.h"
#include "bq24292i.h"
#include "max17048.h"
#include "stusb4500.h"
#include "timer.h"

#include <stdio.h>
#include <stm32g4xx_hal.h>

#define NUM_CELLS 1

static bq24292i_charge_state_t curr_status = BQ_STATE_NOT_CHARGING;

static void pmic_update(void* context);

static timer_task_t pmic_update_timer = {
    .callback = pmic_update,
    .context = NULL,
};

static void pmic_update(void* context) {
    (void)context;
    max17048_soc_t soc;
    max17048_voltage_t vbat;

    max17048_get_vcell(&vbat);
    max17048_get_soc(&soc);
    printf("vbat: %u soc: %u\r\n", vbat, soc);
    timer_add_new(&pmic_update_timer, 3000);
}

bool pmic_init(void) {
    bool ok = true;
    bq24292i_fault_t clear;

    if (ok) ok = bq24292i_is_present();
    if (ok) ok = bq24292i_set_iin_max(BQ_IIN_MAX_3000MA);
    if (ok) ok = bq24292i_set_vsys_min(3500);
    if (ok) ok = bq24292i_set_charge_current(1600 * NUM_CELLS);
    if (ok) ok = bq24292i_set_term_current(128);
    if (ok) ok = bq24292i_set_precharge_current(128);
    if (ok) ok = bq24292i_set_max_charge_voltage(4200);
    if (ok) ok = bq24292i_set_wdt_config(BQ_WATCHDOG_DISABLE);
    if (ok) ok = bq24292i_check_faults(&clear);
    if (ok) (void)clear;

    if (ok) ok = max17048_is_present();
    if (ok) ok = max17048_set_undervolted_voltage(3300);
    if (ok) ok = max17048_set_overvolted_voltage(4300);
    if (ok) ok = max17048_set_reset_voltage(2500);
    if (ok) ok = max17048_set_bat_low_soc(15);
    if (ok) ok = max17048_set_voltage_reset_alert(false);
    if (ok) ok = max17048_set_soc_change_alert(true);
    if (ok) ok = max17048_clear_alerts();

    stusb4500_negotiate(&stusb4500_config, false);

    timer_add_new(&pmic_update_timer, 3000);

    return ok;
}

void bq24292i_on_interrupt(void) {
    bq24292i_charge_state_t new_status;
    if (bq24292i_get_charge_state(&new_status) && new_status != curr_status) {
        switch (new_status) {
            case BQ_STATE_NOT_CHARGING:
                printf("charger removed\r\n");
                break;
            case BQ_STATE_PRECHARGE:
            case BQ_STATE_FAST_CHARGE:
                printf("charge started\r\n");
                break;
            case BQ_STATE_TERMINATED:
                printf("charge complete\r\n");
                break;
            default:
                printf("invalid charge state\r\n");
                break;
        }
        curr_status = new_status;
    }
    bq24292i_fault_t faults;
    if (bq24292i_check_faults(&faults) && (faults & ~BQ_FAULT_CHG)) {
        printf("BQ FAULT:");
        if (faults & BQ_FAULT_THERM) {
            printf(" THERMAL ");
        }
        if (faults & BQ_FAULT_BAT) {
            printf(" BATTERY ");
        }
        if (faults & BQ_FAULT_CHG && new_status != BQ_STATE_NOT_CHARGING) {
            printf(" CHARGE ");
        }
        if (faults & BQ_FAULT_BOOST) {
            printf(" BOOST ");
        }
        if (faults & BQ_FAULT_WATCHDOG) {
            printf(" WATCHDOG ");
        }
        printf("\r\n");
    }
}

void max17048_on_interrupt(void) {
    max17048_alert_t alerts;
    if (max17048_get_alerts(&alerts) && alerts) {
        printf("MAX ALERT:");
        if (alerts & MAX_ALERT_RESET) {
            printf(" RESET ");
        }
        if (alerts & MAX_ALERT_OVERVOLTED) {
            printf(" OVERVOLTED ");
        }
        if (alerts & MAX_ALERT_UNDERVOLTED) {
            printf(" UNDERVOLTED ");
        }
        if (alerts & MAX_ALERT_VOLTAGE_RESET) {
            printf(" VOLTAGE_RESET ");
        }
        if (alerts & MAX_ALERT_SOC_LOW) {
            printf(" LOW_BATTERY ");
        }
        if (alerts & MAX_ALERT_SOC_CHANGE) {
            printf(" SOC_CHANGED ");
        }
        printf("\r\n");
    }
}

void stusb4500_on_interrupt(void) {
    bool success = stusb4500_negotiate(&stusb4500_config, true);
    printf("usb cable detected: %d\r\n", success);
}
