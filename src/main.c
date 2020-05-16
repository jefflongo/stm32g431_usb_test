#include "board.h"
#include "i2c.h"
#include "stusb4500.h"
#include "sys/clocks.h"
#include "usb/cdc.h"

#include <errno.h>
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

int main(int argc, char const* argv[]) {
    (void)argc;
    (void)argv;

    HAL_Init();

    system_clock_init();
    board_init();
    usb_cdc_init();
    i2c_master_init();

    __enable_irq();

    while (1) {
    }
}
