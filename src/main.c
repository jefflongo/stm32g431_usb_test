#include "board.h"
#include "bq24292i.h"
#include "i2c.h"
#include "max17048.h"
#include "stusb4500.h"
#include "sys/clocks.h"
#include "usb/cdc.h"

#include <errno.h>
#include <stdbool.h>
#include <stdio.h>
#include <stm32g4xx.h>
#include <stm32g4xx_ll_gpio.h>
#include <sys/unistd.h>

// override for printf
int _write(int file, char* data, int len) {
    if ((file != STDOUT_FILENO) && (file != STDERR_FILENO)) {
        errno = EBADF;
        return -1;
    }
    return usb_cdc_write((uint8_t*)data, len) ? len : 0;
}

#define NUM_CELLS 1
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

    return ok;
}

int main(int argc, char const* argv[]) {
    (void)argc;
    (void)argv;

    HAL_Init();

    system_clock_init();
    board_init();
    usb_cdc_init();
    i2c_master_init();

    __enable_irq();

    bool init_success = pmic_init();

    while (1) {
        SYS_DELAY_MS(3000);

        max17048_soc_t soc;
        max17048_voltage_t vbat;

        max17048_get_vcell(&vbat);
        max17048_get_soc(&soc);
        printf("init: %u vbat: %u soc: %u\r\n", init_success, vbat, soc);
    }
}
