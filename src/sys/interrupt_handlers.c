#include "board.h"
#include "stusb4500.h"

#include <stdio.h>
#include <stm32g4xx_hal.h>
#include <stm32g4xx_ll_exti.h>
#include <stm32g4xx_ll_gpio.h>

#define HANDLER(thing, action)                                                                     \
    void thing##_Handler(void);                                                                    \
    void thing##_Handler(void) {                                                                   \
        action;                                                                                    \
    }

HANDLER(HardFault, while (1));
HANDLER(MemManage, while (1));
HANDLER(BusFault, while (1));
HANDLER(UsageFault, while (1));

HANDLER(DebugMon, );

HANDLER(SysTick, HAL_IncTick());

#include "i2c.h"
void EXTI2_IRQHandler(void) {
    if (LL_EXTI_IsActiveFlag_0_31(LL_EXTI_LINE_2)) {
        SYS_DELAY_MS(20);
        if (!LL_GPIO_IsInputPinSet(BOARD_PWR_BTN_PORT, BOARD_PWR_BTN_PIN)) {
            printf("button pressed\r\n");
            // stusb_negotiate(false);
            LL_GPIO_TogglePin(BOARD_PWR_ON_PORT, BOARD_PWR_ON_PIN);
            LL_GPIO_TogglePin(BOARD_DEBUG_N_PORT, BOARD_DEBUG_N_PIN);
        }
        LL_EXTI_ClearFlag_0_31(LL_EXTI_LINE_2);
    }
}

void EXTI15_10_IRQHandler(void) {
    if (LL_EXTI_IsActiveFlag_0_31(LL_EXTI_LINE_10)) {
        // NVIC_DisableIRQ(BOARD_CABLE_DET_IRQ);
        LL_EXTI_ClearFlag_0_31(LL_EXTI_LINE_10);
        // stusb_negotiate(true);
        // // TODO: A result of the stusb resetting in the interrupt,
        // // this is obviously temporary and bad
        // HAL_Delay(5000);
        // HAL_NVIC_EnableIRQ(BOARD_CABLE_DET_IRQ);
    }
}
