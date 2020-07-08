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

const static uint8_t config[8] = {
    0x3F, // REG00: Vbat max = 4.44V, bat current output = 3A
    0x1B, // REG01: Vsys min = 3.5V
#if NUM_CELLS == 1
    0x40, // REG02: Charge current limit of 1.5A
#elif NUM_CELLS == 2
    0xA0, // REG02: Charge current limit of 3A
#else // >= 3 cells
    0xFC, // REG02: Charge current limit of 4.5A
#endif
    0x00, // REG03: 128mA pre-charge/termination current limit
    0xB2, // REG04: 3.0-4.2V battery voltage range
    0x8A, // REG05: Disable i2c timer, enable safety timer/charge timer
    0x03, // REG06: Thermal regulation threshold 120C
    0x40, // REG07: No interrupt on charge/bat fault
};

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
        // printf("Hello Cody\r\n");
        // SYS_DELAY_MS(1000);
    }
}
