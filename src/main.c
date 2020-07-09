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

// const static uint8_t config[8] = {
//     0x3F, // REG00: Vbat max = 4.44V, bat current output = 3A
//     0x1B, // REG01: Vsys min = 3.5V
//     0x40, // REG02: Charge current limit of 1.5A
//     0x00, // REG03: 128mA pre-charge/termination current limit
//     0xB2, // REG04: 3.0-4.2V battery voltage range
//     0x8A, // REG05: Disable i2c timer, enable safety timer/charge timer
//     0x03, // REG06: Thermal regulation threshold 120C
//     0x40, // REG07: No interrupt on charge/bat fault
// };

#define NUM_CELLS 1
bool pmic_init(void) {
    bool ok = true;

    if (ok) ok = bq24292i_is_present();
    if (ok) ok = bq24292i_set_iin_max(BQ_IIN_MAX_3000MA);
    if (ok) ok = bq24292i_set_vsys_min(3500);
#if NUM_CELLS == 1
    if (ok) ok = bq24292i_set_charge_current(512);
#elif NUM_CELLS == 2
    if (ok) ok = bq24292i_set_charge_current(3200);
#elif NUM_CELLS >= 3
    if (ok) ok = bq24292i_set_charge_current(4800);
#endif
    if (ok) ok = bq24292i_set_term_current(128);
    if (ok) ok = bq24292i_set_precharge_current(128);
    if (ok) ok = bq24292i_set_vchg_max(4200);
    if (ok) ok = bq24292i_set_wdt_config(BQ_WATCHDOG_DISABLE);

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

    pmic_init();

    while (1) {
        SYS_DELAY_MS(5000);
        uint16_t voltage;
        max_get_vcell(&voltage);
        uint8_t soc;
        max_get_soc(&soc);
        printf("voltage: %u soc: %u\r\n", voltage, soc);
    }
}
