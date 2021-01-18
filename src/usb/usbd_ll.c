#include "usb/cdc.h"

#include <assert.h>

USBD_HandleTypeDef board_usb;

USBD_StatusTypeDef USBD_LL_Init(USBD_HandleTypeDef* pdev) {
    assert(pdev == &board_usb);

    board_pcd.pData = &board_usb;
    board_usb.pData = &board_pcd;

    if (HAL_PCD_Init(&board_pcd) != HAL_OK) {
        return USBD_FAIL;
    }

    HAL_PCDEx_PMAConfig(&board_pcd, 0x00, PCD_SNG_BUF, 0x18);
    HAL_PCDEx_PMAConfig(&board_pcd, 0x80, PCD_SNG_BUF, 0x58);

    HAL_PCDEx_PMAConfig(&board_pcd, 0x81, PCD_SNG_BUF, 0xC0);
    HAL_PCDEx_PMAConfig(&board_pcd, 0x01, PCD_SNG_BUF, 0x110);
    HAL_PCDEx_PMAConfig(&board_pcd, 0x82, PCD_SNG_BUF, 0x100);

    return USBD_OK;
}

static USBD_StatusTypeDef hal_to_usbd_status(HAL_StatusTypeDef hal_status) {
    switch (hal_status) {
        case HAL_OK:
            return USBD_OK;
        case HAL_BUSY:
            return USBD_BUSY;
        default:
            return USBD_FAIL;
    }
}

USBD_StatusTypeDef USBD_LL_DeInit(USBD_HandleTypeDef* pdev) {
    assert(pdev == &board_usb);
    return hal_to_usbd_status(HAL_PCD_DeInit(&board_pcd));
}

USBD_StatusTypeDef USBD_LL_Start(USBD_HandleTypeDef* pdev) {
    return hal_to_usbd_status(HAL_PCD_Start(pdev->pData));
}

USBD_StatusTypeDef USBD_LL_Stop(USBD_HandleTypeDef* pdev) {
    return hal_to_usbd_status(HAL_PCD_Stop(pdev->pData));
}

USBD_StatusTypeDef
  USBD_LL_OpenEP(USBD_HandleTypeDef* pdev, uint8_t ep_addr, uint8_t ep_type, uint16_t ep_mps) {
    return hal_to_usbd_status(HAL_PCD_EP_Open(pdev->pData, ep_addr, ep_mps, ep_type));
}

USBD_StatusTypeDef USBD_LL_CloseEP(USBD_HandleTypeDef* pdev, uint8_t ep_addr) {
    return hal_to_usbd_status(HAL_PCD_EP_Close(pdev->pData, ep_addr));
}

USBD_StatusTypeDef USBD_LL_FlushEP(USBD_HandleTypeDef* pdev, uint8_t ep_addr) {
    return hal_to_usbd_status(HAL_PCD_EP_Flush(pdev->pData, ep_addr));
}

USBD_StatusTypeDef USBD_LL_StallEP(USBD_HandleTypeDef* pdev, uint8_t ep_addr) {
    return hal_to_usbd_status(HAL_PCD_EP_SetStall(pdev->pData, ep_addr));
}

USBD_StatusTypeDef USBD_LL_ClearStallEP(USBD_HandleTypeDef* pdev, uint8_t ep_addr) {
    return hal_to_usbd_status(HAL_PCD_EP_ClrStall(pdev->pData, ep_addr));
}

uint8_t USBD_LL_IsStallEP(USBD_HandleTypeDef* pdev, uint8_t ep_addr) {
    PCD_HandleTypeDef* hpcd = (PCD_HandleTypeDef*)pdev->pData;

    if ((ep_addr & 0x80) == 0x80) {
        return hpcd->IN_ep[ep_addr & 0x7F].is_stall;
    } else {
        return hpcd->OUT_ep[ep_addr & 0x7F].is_stall;
    }
}

USBD_StatusTypeDef USBD_LL_SetUSBAddress(USBD_HandleTypeDef* pdev, uint8_t dev_addr) {
    return hal_to_usbd_status(HAL_PCD_SetAddress(pdev->pData, dev_addr));
}

USBD_StatusTypeDef
  USBD_LL_Transmit(USBD_HandleTypeDef* pdev, uint8_t ep_addr, uint8_t* pbuf, uint16_t size) {
    return hal_to_usbd_status(HAL_PCD_EP_Transmit(pdev->pData, ep_addr, pbuf, size));
}

USBD_StatusTypeDef
  USBD_LL_PrepareReceive(USBD_HandleTypeDef* pdev, uint8_t ep_addr, uint8_t* pbuf, uint16_t size) {
    return hal_to_usbd_status(HAL_PCD_EP_Receive(pdev->pData, ep_addr, pbuf, size));
}

uint32_t USBD_LL_GetRxDataSize(USBD_HandleTypeDef* pdev, uint8_t ep_addr) {
    return HAL_PCD_EP_GetRxCount((PCD_HandleTypeDef*)pdev->pData, ep_addr);
}
