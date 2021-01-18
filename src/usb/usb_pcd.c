#include "board.h"
#include "sys/clocks.h"
#include "usb/cdc.h"

#include <assert.h>
#include <stm32g4xx_ll_gpio.h>

PCD_HandleTypeDef board_pcd = {
  .Instance = USB,
  .Init = {
    .dev_endpoints = 8,
    .speed = PCD_SPEED_FULL,
    .phy_itface = PCD_PHY_EMBEDDED,
    .Sof_enable = DISABLE,
    .low_power_enable = DISABLE,
    .lpm_enable = DISABLE,
    .battery_charging_enable = ENABLE,
  },
};

void HAL_PCD_MspInit(PCD_HandleTypeDef* hpcd) {
    LL_GPIO_SetPinMode(BOARD_USB_DP_PORT, BOARD_USB_DP_PIN, LL_GPIO_MODE_ANALOG);
    LL_GPIO_SetPinPull(BOARD_USB_DP_PORT, BOARD_USB_DP_PIN, LL_GPIO_PULL_NO);

    LL_GPIO_SetPinMode(BOARD_USB_DM_PORT, BOARD_USB_DM_PIN, LL_GPIO_MODE_ANALOG);
    LL_GPIO_SetPinPull(BOARD_USB_DM_PORT, BOARD_USB_DM_PIN, LL_GPIO_PULL_NO);

    __HAL_RCC_USB_CLK_ENABLE();

    HAL_NVIC_SetPriority(USB_LP_IRQn, 0, 0);
    HAL_NVIC_EnableIRQ(USB_LP_IRQn);
}

void HAL_PCD_MspDeInit(PCD_HandleTypeDef* hpcd) {
    __HAL_RCC_USB_CLK_DISABLE();
}

void HAL_PCD_SetupStageCallback(PCD_HandleTypeDef* hpcd) {
    USBD_LL_SetupStage((USBD_HandleTypeDef*)hpcd->pData, (uint8_t*)hpcd->Setup);
}

void HAL_PCD_DataOutStageCallback(PCD_HandleTypeDef* hpcd, uint8_t epnum) {
    USBD_LL_DataOutStage((USBD_HandleTypeDef*)hpcd->pData, epnum, hpcd->OUT_ep[epnum].xfer_buff);
}

void HAL_PCD_DataInStageCallback(PCD_HandleTypeDef* hpcd, uint8_t epnum) {
    USBD_LL_DataInStage((USBD_HandleTypeDef*)hpcd->pData, epnum, hpcd->IN_ep[epnum].xfer_buff);
}

void HAL_PCD_SOFCallback(PCD_HandleTypeDef* hpcd) {
    USBD_LL_SOF((USBD_HandleTypeDef*)hpcd->pData);
}

void HAL_PCD_ResetCallback(PCD_HandleTypeDef* hpcd) {
    USBD_SpeedTypeDef speed = USBD_SPEED_FULL;

    assert(hpcd->Init.speed == PCD_SPEED_FULL);

    USBD_LL_SetSpeed((USBD_HandleTypeDef*)hpcd->pData, speed);
    USBD_LL_Reset((USBD_HandleTypeDef*)hpcd->pData);
}

void HAL_PCD_SuspendCallback(PCD_HandleTypeDef* hpcd) {
    USBD_LL_Suspend((USBD_HandleTypeDef*)hpcd->pData);

    if (hpcd->Init.low_power_enable) {
        usb_bus_suspend();
    }
}

void HAL_PCD_ResumeCallback(PCD_HandleTypeDef* hpcd) {
    if (hpcd->Init.low_power_enable) {
        usb_bus_resume();
    }

    USBD_LL_Resume((USBD_HandleTypeDef*)hpcd->pData);
}

void HAL_PCD_ISOOUTIncompleteCallback(PCD_HandleTypeDef* hpcd, uint8_t epnum) {
    USBD_LL_IsoOUTIncomplete((USBD_HandleTypeDef*)hpcd->pData, epnum);
}

void HAL_PCD_ISOINIncompleteCallback(PCD_HandleTypeDef* hpcd, uint8_t epnum) {
    USBD_LL_IsoINIncomplete((USBD_HandleTypeDef*)hpcd->pData, epnum);
}

void HAL_PCD_ConnectCallback(PCD_HandleTypeDef* hpcd) {
    USBD_LL_DevConnected((USBD_HandleTypeDef*)hpcd->pData);
}

void HAL_PCD_DisconnectCallback(PCD_HandleTypeDef* hpcd) {
    USBD_LL_DevDisconnected((USBD_HandleTypeDef*)hpcd->pData);
}

void USB_LP_IRQHandler(void) {
    HAL_PCD_IRQHandler(&board_pcd);
}
