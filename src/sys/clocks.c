#include "sys/clocks.h"

#include <assert.h>
#include <stm32g4xx_hal.h>
#include <stm32g4xx_ll_pwr.h>
#include <stm32g4xx_ll_rcc.h>
#include <stm32g4xx_ll_system.h>
#include <stm32g4xx_ll_utils.h>

void system_clock_init(void) {
    LL_FLASH_SetLatency(LL_FLASH_LATENCY_7);
    LL_PWR_SetRegulVoltageScaling(LL_PWR_REGU_VOLTAGE_SCALE1);

    // Enable HSI (16 MHz)
    LL_RCC_HSI_Enable();
    while (!LL_RCC_HSI_IsReady())
        ;
    LL_RCC_HSI_SetCalibTrimming(64);

    // Configure main PLL at HSI * 9.375 == 150 MHz
    LL_RCC_PLL_ConfigDomain_SYS(LL_RCC_PLLSOURCE_HSI, LL_RCC_PLLM_DIV_4, 75, LL_RCC_PLLR_DIV_2);
    LL_RCC_PLL_EnableDomain_SYS();
    LL_RCC_PLL_Enable();
    while (!LL_RCC_PLL_IsReady())
        ;

    // Switch system to run from PLL
    LL_RCC_SetSysClkSource(LL_RCC_SYS_CLKSOURCE_PLL);
    LL_RCC_SetAHBPrescaler(LL_RCC_SYSCLK_DIV_2);
    while (LL_RCC_GetSysClkSource() != LL_RCC_SYS_CLKSOURCE_STATUS_PLL)
        ;

    // 1 microsecond delay
    CoreDebug->DEMCR |= CoreDebug_DEMCR_TRCENA_Msk;
    DWT->CTRL |= DWT_CTRL_CYCCNTENA_Msk;
    DWT->CYCCNT = 0;
    while (DWT->CYCCNT < 100)
        ;

    // Increase AHB to full system clock speed (150 MHz)
    LL_RCC_SetAHBPrescaler(LL_RCC_SYSCLK_DIV_1);
    LL_RCC_SetAPB1Prescaler(LL_RCC_APB1_DIV_1);
    LL_RCC_SetAPB2Prescaler(LL_RCC_APB1_DIV_1);
    LL_SetSystemCoreClock(150000000);

    // [Re-]Initialize HAL systick
    assert(HAL_InitTick(TICK_INT_PRIORITY) == HAL_OK);
}

// set up USB clocked from HSI48
void usb_clk_init(void) {
    LL_RCC_HSI48_Enable();
    while (!LL_RCC_HSI48_IsReady())
        ;
    LL_RCC_SetUSBClockSource(LL_RCC_USB_CLKSOURCE_HSI48);
}

void usb_bus_suspend(void) {
    SCB->SCR |= (uint32_t)((uint32_t)(SCB_SCR_SLEEPDEEP_Msk | SCB_SCR_SLEEPONEXIT_Msk));
}

void usb_bus_resume(void) {
    SCB->SCR &= (uint32_t) ~((uint32_t)(SCB_SCR_SLEEPDEEP_Msk | SCB_SCR_SLEEPONEXIT_Msk));

    system_clock_init();
    usb_clk_init();
}