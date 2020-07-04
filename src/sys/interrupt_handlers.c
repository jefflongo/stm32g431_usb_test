#include "board.h"
#include "stusb4500.h"

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

void EXTI15_10_IRQHandler(void) {
    if (LL_EXTI_IsActiveFlag_0_31(LL_EXTI_LINE_10)) {
        NVIC_DisableIRQ(CABLE_DET_IRQ);
        LL_EXTI_ClearFlag_0_31(LL_EXTI_LINE_10);
        stusb_negotiate(true);
        // TODO: A result of the stusb resetting in the interrupt,
        // this is obviously temporary and bad
        HAL_Delay(5000);
        HAL_NVIC_EnableIRQ(CABLE_DET_IRQ);
    }
}
