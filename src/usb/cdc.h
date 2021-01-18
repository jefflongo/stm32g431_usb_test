#pragma once

#include <stdbool.h>
#include <usbd_cdc.h>
#include <usbd_core.h>

void usb_cdc_init(void);

bool usb_cdc_print(char const* buf);
bool usb_cdc_write(uint8_t const* str, size_t len);

extern USBD_DescriptorsTypeDef cdc_descriptors;

extern PCD_HandleTypeDef board_pcd;
extern USBD_HandleTypeDef board_usb;
extern USBD_CDC_ItfTypeDef board_cdc;
