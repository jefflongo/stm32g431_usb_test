#include "dfu.h"

#include <stm32g4xx_hal.h>

#define SYSTEM_MEMORY_ADDRESS 0x1FFF0000u

typedef void (*pfunc)(void);

enum {
    DFU_SERVICE_BOOT_APP = 0,
    DFU_SERVICE_BOOT_BOOTLOADER = 0x3aeb8452e8a4a79b,
};

__attribute__((section(".noinit"))) static uint64_t dfu_magic;

static void enter_bootloader() {
    pfunc bootloader = (pfunc)(*((uint32_t*)(SYSTEM_MEMORY_ADDRESS + 4)));

    __DSB();

    __HAL_FLASH_INSTRUCTION_CACHE_DISABLE();
    __HAL_FLASH_DATA_CACHE_DISABLE();

    __HAL_SYSCFG_REMAPMEMORY_SYSTEMFLASH();
    __set_MSP(*(uint32_t*)SYSTEM_MEMORY_ADDRESS);

    bootloader();
}

void dfu_service_init(void) {
    if (dfu_magic == DFU_SERVICE_BOOT_BOOTLOADER) {
        dfu_magic = DFU_SERVICE_BOOT_APP;
        enter_bootloader();
    } else {
        dfu_magic = DFU_SERVICE_BOOT_APP;
    }
}

void dfu_service_enter_bootloader(void) {
    dfu_magic = DFU_SERVICE_BOOT_BOOTLOADER;
    __DSB();

    NVIC_SystemReset();

    // Shouldn't be possible to get here after NVIC_SystemReset, just in case..
    while (1)
        ;
}