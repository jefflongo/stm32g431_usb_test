#include "board.h"

#include <stm32g4xx_ll_bus.h>
#include <stm32g4xx_ll_exti.h>
#include <stm32g4xx_ll_gpio.h>
#include <stm32g4xx_ll_pwr.h>
#include <stm32g4xx_ll_system.h>

void board_init(void) {
    LL_AHB2_GRP1_EnableClock(LL_AHB2_GRP1_PERIPH_GPIOA);
    LL_AHB2_GRP1_EnableClock(LL_AHB2_GRP1_PERIPH_GPIOB);
    LL_AHB2_GRP1_EnableClock(LL_AHB2_GRP1_PERIPH_GPIOF);

    LL_PWR_DisableDeadBatteryPD();

    LL_GPIO_SetPinMode(BOARD_BQ_INT_PORT, BOARD_BQ_INT_PIN, LL_GPIO_MODE_INPUT);
    LL_GPIO_SetPinPull(BOARD_BQ_INT_PORT, BOARD_BQ_INT_PIN, LL_GPIO_PULL_UP);

    LL_GPIO_SetPinMode(BOARD_CABLE_DET_PORT, BOARD_CABLE_DET_PIN, LL_GPIO_MODE_INPUT);
    LL_GPIO_SetPinPull(BOARD_CABLE_DET_PORT, BOARD_CABLE_DET_PIN, LL_GPIO_PULL_UP);

    LL_GPIO_SetPinMode(BOARD_DEBUG_N_PORT, BOARD_DEBUG_N_PIN, LL_GPIO_MODE_OUTPUT);
    LL_GPIO_ResetOutputPin(BOARD_DEBUG_N_PORT, BOARD_DEBUG_N_PIN);

    LL_GPIO_SetPinMode(BOARD_LED_PORT, BOARD_LED_PIN, LL_GPIO_MODE_OUTPUT);
    LL_GPIO_ResetOutputPin(BOARD_LED_PORT, BOARD_LED_PIN);

    LL_GPIO_SetPinMode(BOARD_MAX_INT_PORT, BOARD_MAX_INT_PIN, LL_GPIO_MODE_INPUT);
    LL_GPIO_SetPinPull(BOARD_MAX_INT_PORT, BOARD_MAX_INT_PIN, LL_GPIO_PULL_UP);

    LL_GPIO_SetPinMode(BOARD_PWR_BTN_PORT, BOARD_PWR_BTN_PIN, LL_GPIO_MODE_INPUT);
    LL_GPIO_SetPinPull(BOARD_PWR_BTN_PORT, BOARD_PWR_BTN_PIN, LL_GPIO_PULL_UP);

    LL_GPIO_SetPinMode(BOARD_VOL_DOWN_PORT, BOARD_VOL_DOWN_PIN, LL_GPIO_MODE_INPUT);
    LL_GPIO_SetPinPull(BOARD_VOL_DOWN_PORT, BOARD_VOL_DOWN_PIN, LL_GPIO_PULL_UP);

    LL_GPIO_SetPinMode(BOARD_VOL_UP_PORT, BOARD_VOL_UP_PIN, LL_GPIO_MODE_INPUT);
    LL_GPIO_SetPinPull(BOARD_VOL_UP_PORT, BOARD_VOL_UP_PIN, LL_GPIO_PULL_UP);

    LL_GPIO_SetPinMode(BOARD_PWR_ON_PORT, BOARD_PWR_ON_PIN, LL_GPIO_MODE_OUTPUT);
    LL_GPIO_SetOutputPin(BOARD_PWR_ON_PORT, BOARD_PWR_ON_PIN);

    __HAL_RCC_SYSCFG_CLK_ENABLE();

    LL_EXTI_InitTypeDef config;

    LL_SYSCFG_SetEXTISource(LL_SYSCFG_EXTI_PORTA, LL_SYSCFG_EXTI_LINE0);
    config.Line_0_31 = LL_EXTI_LINE_0;
    config.Line_32_63 = 0;
    config.LineCommand = ENABLE;
    config.Mode = LL_EXTI_MODE_IT;
    config.Trigger = LL_EXTI_TRIGGER_FALLING;
    LL_EXTI_Init(&config);
    HAL_NVIC_SetPriority(BOARD_BQ_INT_IRQ, 1, 0);
    HAL_NVIC_EnableIRQ(BOARD_BQ_INT_IRQ);

    LL_SYSCFG_SetEXTISource(LL_SYSCFG_EXTI_PORTF, LL_SYSCFG_EXTI_LINE1);
    config.Line_0_31 = LL_EXTI_LINE_1;
    config.Line_32_63 = 0;
    config.LineCommand = ENABLE;
    config.Mode = LL_EXTI_MODE_IT;
    config.Trigger = LL_EXTI_TRIGGER_FALLING;
    LL_EXTI_Init(&config);
    HAL_NVIC_SetPriority(BOARD_CABLE_DET_IRQ, 1, 0);
    HAL_NVIC_EnableIRQ(BOARD_CABLE_DET_IRQ);

    LL_SYSCFG_SetEXTISource(LL_SYSCFG_EXTI_PORTA, LL_SYSCFG_EXTI_LINE2);
    config.Line_0_31 = LL_EXTI_LINE_2;
    config.Line_32_63 = 0;
    config.LineCommand = ENABLE;
    config.Mode = LL_EXTI_MODE_IT;
    config.Trigger = LL_EXTI_TRIGGER_RISING_FALLING;
    LL_EXTI_Init(&config);
    HAL_NVIC_SetPriority(BOARD_PWR_BTN_IRQ, 1, 0);
    HAL_NVIC_EnableIRQ(BOARD_PWR_BTN_IRQ);

    LL_SYSCFG_SetEXTISource(LL_SYSCFG_EXTI_PORTB, LL_SYSCFG_EXTI_LINE3);
    config.Line_0_31 = LL_EXTI_LINE_3;
    config.Line_32_63 = 0;
    config.LineCommand = ENABLE;
    config.Mode = LL_EXTI_MODE_IT;
    config.Trigger = LL_EXTI_TRIGGER_FALLING;
    LL_EXTI_Init(&config);
    HAL_NVIC_SetPriority(BOARD_VOL_DOWN_IRQ, 1, 0);
    HAL_NVIC_EnableIRQ(BOARD_VOL_DOWN_IRQ);

    LL_SYSCFG_SetEXTISource(LL_SYSCFG_EXTI_PORTB, LL_SYSCFG_EXTI_LINE4);
    config.Line_0_31 = LL_EXTI_LINE_4;
    config.Line_32_63 = 0;
    config.LineCommand = ENABLE;
    config.Mode = LL_EXTI_MODE_IT;
    config.Trigger = LL_EXTI_TRIGGER_FALLING;
    LL_EXTI_Init(&config);
    HAL_NVIC_SetPriority(BOARD_VOL_UP_IRQ, 1, 0);
    HAL_NVIC_EnableIRQ(BOARD_VOL_UP_IRQ);

    LL_SYSCFG_SetEXTISource(LL_SYSCFG_EXTI_PORTA, LL_SYSCFG_EXTI_LINE5);
    config.Line_0_31 = LL_EXTI_LINE_5;
    config.Line_32_63 = 0;
    config.LineCommand = ENABLE;
    config.Mode = LL_EXTI_MODE_IT;
    config.Trigger = LL_EXTI_TRIGGER_FALLING;
    LL_EXTI_Init(&config);
    HAL_NVIC_SetPriority(BOARD_MAX_INT_IRQ, 1, 0);
    HAL_NVIC_EnableIRQ(BOARD_MAX_INT_IRQ);
}
