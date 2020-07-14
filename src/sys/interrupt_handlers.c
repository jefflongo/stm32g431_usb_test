#include "app_conf.h"
#include "board.h"
#include "bq24292i.h"
#include "max17048.h"
#include "stusb4500.h"
#include "sys/sys.h"

#include <stdbool.h>
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

static bq24292i_charge_state_t curr_status = BQ_STATE_NOT_CHARGING;
void EXTI0_IRQHandler(void) {
    if (LL_EXTI_IsActiveFlag_0_31(LL_EXTI_LINE_0)) {
        bq24292i_charge_state_t new_status;
        if (bq24292i_get_charge_state(&new_status) && new_status != curr_status) {
            switch (new_status) {
                case BQ_STATE_NOT_CHARGING:
                    printf("charger removed\r\n");
                    break;
                case BQ_STATE_PRECHARGE:
                case BQ_STATE_FAST_CHARGE:
                    printf("charge started\r\n");
                    break;
                case BQ_STATE_TERMINATED:
                    printf("charge complete\r\n");
                    break;
                default:
                    printf("invalid charge state\r\n");
                    break;
            }
            curr_status = new_status;
        }
        bq24292i_fault_t faults;
        if (bq24292i_check_faults(&faults) && (faults & ~BQ_FAULT_CHG)) {
            printf("BQ FAULT:");
            if (faults & BQ_FAULT_THERM) {
                printf(" THERMAL ");
            }
            if (faults & BQ_FAULT_BAT) {
                printf(" BATTERY ");
            }
            if (faults & BQ_FAULT_CHG && new_status != BQ_STATE_NOT_CHARGING) {
                printf(" CHARGE ");
            }
            if (faults & BQ_FAULT_BOOST) {
                printf(" BOOST ");
            }
            if (faults & BQ_FAULT_WATCHDOG) {
                printf(" WATCHDOG ");
            }
            printf("\r\n");
        }

        LL_EXTI_ClearFlag_0_31(LL_EXTI_LINE_0);
    }
}

void EXTI1_IRQHandler(void) {
    if (LL_EXTI_IsActiveFlag_0_31(LL_EXTI_LINE_1)) {
        NVIC_DisableIRQ(BOARD_CABLE_DET_IRQ);
        LL_EXTI_ClearFlag_0_31(LL_EXTI_LINE_1);
        stusb4500_negotiate(&stusb4500_config, true);
        // TODO: A result of the stusb resetting in the interrupt,
        // this is obviously temporary and bad
        SYS_DELAY_MS(5000);
        printf("usb cable detected\r\n");
        NVIC_EnableIRQ(BOARD_CABLE_DET_IRQ);
    }
}

void EXTI2_IRQHandler(void) {
    if (LL_EXTI_IsActiveFlag_0_31(LL_EXTI_LINE_2)) {
        SYS_DELAY_MS(20);
        if (!LL_GPIO_IsInputPinSet(BOARD_PWR_BTN_PORT, BOARD_PWR_BTN_PIN)) {
            printf("power button pressed\r\n");
            LL_GPIO_TogglePin(BOARD_PWR_ON_PORT, BOARD_PWR_ON_PIN);
            LL_GPIO_TogglePin(BOARD_DEBUG_N_PORT, BOARD_DEBUG_N_PIN);
        }
        LL_EXTI_ClearFlag_0_31(LL_EXTI_LINE_2);
    }
}

void EXTI3_IRQHandler(void) {
    if (LL_EXTI_IsActiveFlag_0_31(LL_EXTI_LINE_3)) {
        printf("volume down pressed\r\n");
        LL_EXTI_ClearFlag_0_31(LL_EXTI_LINE_3);
    }
}

void EXTI4_IRQHandler(void) {
    if (LL_EXTI_IsActiveFlag_0_31(LL_EXTI_LINE_4)) {
        printf("volume up pressed\r\n");
        LL_EXTI_ClearFlag_0_31(LL_EXTI_LINE_4);
    }
}

void EXTI9_5_IRQHandler(void) {
    if (LL_EXTI_IsActiveFlag_0_31(LL_EXTI_LINE_5)) {
        max17048_alert_t alerts;
        if (max17048_get_alerts(&alerts) && alerts) {
            printf("MAX ALERT:");
            if (alerts & MAX_ALERT_RESET) {
                printf(" RESET ");
            }
            if (alerts & MAX_ALERT_OVERVOLTED) {
                printf(" OVERVOLTED ");
            }
            if (alerts & MAX_ALERT_UNDERVOLTED) {
                printf(" UNDERVOLTED ");
            }
            if (alerts & MAX_ALERT_VOLTAGE_RESET) {
                printf(" VOLTAGE_RESET ");
            }
            if (alerts & MAX_ALERT_SOC_LOW) {
                printf(" LOW_BATTERY ");
            }
            if (alerts & MAX_ALERT_SOC_CHANGE) {
                printf(" SOC_CHANGED ");
            }
            printf("\r\n");
        }
        LL_EXTI_ClearFlag_0_31(LL_EXTI_LINE_5);
    }
}