
#include "app_button.h"

#include "board.h"
#include "dfu.h"

#include <stdbool.h>
#include <stdio.h>
#include <stm32g4xx_hal.h>
#include <stm32g4xx_ll_gpio.h>

#define BUTTON_PRESSED 0
#define BUTTON_RELEASED 1

#define LONG_PRESS_MS 10000

static uint32_t press_time;
static uint32_t release_time;

void pwr_button_on_interrupt(void) {
    bool state = LL_GPIO_IsInputPinSet(BOARD_PWR_BTN_PORT, BOARD_PWR_BTN_PIN);
    HAL_Delay(20);
    if (LL_GPIO_IsInputPinSet(BOARD_PWR_BTN_PORT, BOARD_PWR_BTN_PIN) == state) {
        if (state == BUTTON_PRESSED) {
            press_time = HAL_GetTick();
            printf("power button pressed\r\n");
        } else {
            release_time = HAL_GetTick();
            printf("power button released\r\n");
            if ((int32_t)(release_time - press_time) >= LONG_PRESS_MS) {
                printf("entering DFU\r\n");
                dfu_service_enter_bootloader();
            } else {
                LL_GPIO_TogglePin(BOARD_PWR_ON_PORT, BOARD_PWR_ON_PIN);
                LL_GPIO_TogglePin(BOARD_DEBUG_N_PORT, BOARD_DEBUG_N_PIN);
            }
        }
    }
}

void vol_up_button_on_interrupt(void) {
    printf("volume up pressed\r\n");
}

void vol_down_button_on_interrupt(void) {
    printf("volume down pressed\r\n");
}
