#include "app_pmic.h"
#include "board.h"
#include "dfu.h"
#include "i2c.h"
#include "sys/clocks.h"
#include "timer.h"
#include "usb/cdc.h"

#include <errno.h>
#include <stdbool.h>
#include <stdio.h>
#include <sys/unistd.h>

// override for printf
int _write(int file, char* data, int len) {
    if ((file != STDOUT_FILENO) && (file != STDERR_FILENO)) {
        errno = EBADF;
        return -1;
    }
    return usb_cdc_write((uint8_t*)data, len) ? len : 0;
}

// timer timebase
timer_time_t _timer_get_time(void) {
    return HAL_GetTick();
}

int main(int argc, char const* argv[]) {
    (void)argc;
    (void)argv;

    dfu_service_init();

    HAL_Init();

    system_clock_init();
    board_gpio_init();

    usb_cdc_init();
    i2c_master_init();

    __enable_irq();

    pmic_init();
    board_interrupts_init();

    while (1) {
        timer_update();
    }
}
