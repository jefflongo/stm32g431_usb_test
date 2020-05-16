#include "board.h"

#include <stm32g4xx_ll_bus.h>
#include <stm32g4xx_ll_exti.h>
#include <stm32g4xx_ll_gpio.h>
#include <stm32g4xx_ll_pwr.h>

void board_init(void) {
    LL_AHB2_GRP1_EnableClock(LL_AHB2_GRP1_PERIPH_GPIOA);
    LL_AHB2_GRP1_EnableClock(LL_AHB2_GRP1_PERIPH_GPIOB);

    LL_PWR_DisableDeadBatteryPD();

    LL_GPIO_ResetOutputPin(BOARD_LED_PORT, BOARD_LED_PIN);
    LL_GPIO_SetPinMode(BOARD_LED_PORT, BOARD_LED_PIN, LL_GPIO_MODE_OUTPUT);

    LL_GPIO_SetPinMode(CABLE_DET_PORT, CABLE_DET_PIN, LL_GPIO_MODE_INPUT);
    LL_GPIO_SetPinPull(CABLE_DET_PORT, CABLE_DET_PIN, LL_GPIO_PULL_UP);

    __HAL_RCC_SYSCFG_CLK_ENABLE();

    LL_EXTI_InitTypeDef config;
    config.Line_0_31 = LL_EXTI_LINE_10;
    config.Line_32_63 = 0;
    config.LineCommand = ENABLE;
    config.Mode = LL_EXTI_MODE_IT;
    config.Trigger = LL_EXTI_TRIGGER_FALLING;

    LL_EXTI_Init(&config);

    HAL_NVIC_SetPriority(CABLE_DET_IRQ, 1, 0);
    HAL_NVIC_EnableIRQ(CABLE_DET_IRQ);
}
