#include "usb/cdc.h"

#include "sys/clocks.h"

#include <assert.h>
#include <stm32g4xx_hal.h>
#include <string.h>
#include <usbd_cdc.h>
#include <usbd_core.h>

#define RX_BUFFER_SIZE 1024
#define TX_BUFFER_SIZE 1024

void usb_cdc_init(void) {
    usb_clk_init();

    assert(USBD_Init(&board_usb, &cdc_descriptors, DEVICE_FS) == USBD_OK);
    assert(USBD_RegisterClass(&board_usb, USBD_CDC_CLASS) == USBD_OK);
    assert(USBD_CDC_RegisterInterface(&board_usb, &board_cdc) == USBD_OK);
    assert(USBD_Start(&board_usb) == USBD_OK);
}

static uint8_t cdc_rx_buffer[RX_BUFFER_SIZE];
static uint8_t cdc_tx_buffer[TX_BUFFER_SIZE];

static int8_t cdc_init(void) {
    USBD_CDC_SetRxBuffer(&board_usb, cdc_rx_buffer);
    USBD_CDC_SetTxBuffer(&board_usb, cdc_tx_buffer, 0);
    return USBD_OK;
}

static int8_t cdc_deinit(void) {
    return USBD_OK;
}

static int8_t cdc_control(uint8_t cmd, uint8_t* pbuf, uint16_t length) {
    return USBD_OK;
}

static int8_t cdc_receive(uint8_t* Buf, uint32_t* Len) {
    USBD_CDC_SetRxBuffer(&board_usb, &Buf[0]);
    USBD_CDC_ReceivePacket(&board_usb);
    return USBD_OK;
}

USBD_CDC_ItfTypeDef board_cdc = {
    .Init = cdc_init,
    .DeInit = cdc_deinit,
    .Control = cdc_control,
    .Receive = cdc_receive,
};

bool usb_cdc_print(char const* buf) {
    return usb_cdc_write((uint8_t const*)buf, strlen(buf));
}

bool usb_cdc_write(uint8_t const* buf, size_t len) {
    USBD_CDC_HandleTypeDef* cdc = (USBD_CDC_HandleTypeDef*)board_usb.pClassData;
    if (cdc->TxState != 0) {
        return false;
    }

    // TODO: better buffer management
    if (len > TX_BUFFER_SIZE) return false;
    memcpy(cdc_tx_buffer, buf, len);
    USBD_CDC_SetTxBuffer(&board_usb, cdc_tx_buffer, len);

    return USBD_CDC_TransmitPacket(&board_usb);
}