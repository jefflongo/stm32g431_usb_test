#include "app_button.h"
#include "app_pmic.h"

#include <stm32g4xx_hal.h>
#include <stm32g4xx_ll_exti.h>

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

void EXTI0_IRQHandler(void) {
    if (LL_EXTI_IsActiveFlag_0_31(LL_EXTI_LINE_0)) {
        bq24292i_on_interrupt();
        LL_EXTI_ClearFlag_0_31(LL_EXTI_LINE_0);
    }
}

void EXTI1_IRQHandler(void) {
    if (LL_EXTI_IsActiveFlag_0_31(LL_EXTI_LINE_1)) {
        stusb4500_on_interrupt();
        LL_EXTI_ClearFlag_0_31(LL_EXTI_LINE_1);
    }
}

void EXTI2_IRQHandler(void) {
    if (LL_EXTI_IsActiveFlag_0_31(LL_EXTI_LINE_2)) {
        pwr_button_on_interrupt();
        LL_EXTI_ClearFlag_0_31(LL_EXTI_LINE_2);
    }
}

void EXTI3_IRQHandler(void) {
    if (LL_EXTI_IsActiveFlag_0_31(LL_EXTI_LINE_3)) {
        vol_down_button_on_interrupt();
        LL_EXTI_ClearFlag_0_31(LL_EXTI_LINE_3);
    }
}

void EXTI4_IRQHandler(void) {
    if (LL_EXTI_IsActiveFlag_0_31(LL_EXTI_LINE_4)) {
        vol_up_button_on_interrupt();
        LL_EXTI_ClearFlag_0_31(LL_EXTI_LINE_4);
    }
}

void EXTI9_5_IRQHandler(void) {
    if (LL_EXTI_IsActiveFlag_0_31(LL_EXTI_LINE_5)) {
        max17048_on_interrupt();
        LL_EXTI_ClearFlag_0_31(LL_EXTI_LINE_5);
    }
}