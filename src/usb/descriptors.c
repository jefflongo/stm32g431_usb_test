#include "usb/cdc.h"

#include <string.h>

#define USBD_VID 1155
#define USBD_LANGID_STRING 1033
#define USBD_MANUFACTURER_STRING "STMicroelectronics"
#define USBD_PID 22336
#define USBD_PRODUCT_STRING "STM32 Virtual ComPort"
#define USBD_CONFIGURATION_STRING "CDC Config"
#define USBD_INTERFACE_STRING "CDC Interface"

static const uint8_t device_desc[] __attribute__((aligned(4))) = {
    0x12,                 // bLength
    USB_DESC_TYPE_DEVICE, // bDescriptorTypp
    0x00,                 // bcdUSB
    0x02,
    0x02,             // bDeviceClass
    0x02,             // bDeviceSubClass
    0x00,             // bDeviceProtocol
    USB_MAX_EP0_SIZE, // bMaxPacketSize
    LOBYTE(USBD_VID), // idVendor
    HIBYTE(USBD_VID), // idVendor
    LOBYTE(USBD_PID), // idProduct
    HIBYTE(USBD_PID), // idProduct
    0x00,             // bcdDevice rel. 2.00
    0x02,
    USBD_IDX_MFC_STR,           // Index of manufacturer  string
    USBD_IDX_PRODUCT_STR,       // Index of product string
    USBD_IDX_SERIAL_STR,        // Index of serial number string
    USBD_MAX_NUM_CONFIGURATION, // bNumConfigurations
};

static uint8_t* get_device_desc(USBD_SpeedTypeDef speed, uint16_t* length) {
    (void)speed;
    *length = sizeof(device_desc);
    return (uint8_t*)device_desc;
}

static const uint8_t lang_desc[] __attribute__((aligned(4))) = {
    USB_LEN_LANGID_STR_DESC,
    USB_DESC_TYPE_STRING,
    LOBYTE(USBD_LANGID_STRING),
    HIBYTE(USBD_LANGID_STRING),
};

static uint8_t* get_lang_desc(USBD_SpeedTypeDef speed, uint16_t* length) {
    (void)speed;
    *length = sizeof(lang_desc);
    return (uint8_t*)lang_desc;
}

static uint8_t global_string_desc[USBD_MAX_STR_DESC_SIZ] __attribute__((aligned));

static uint8_t* get_manufacturer_desc(USBD_SpeedTypeDef speed, uint16_t* length) {
    (void)speed;
    USBD_GetString((uint8_t*)USBD_MANUFACTURER_STRING, global_string_desc, length);
    return global_string_desc;
}

static uint8_t* get_product_desc(USBD_SpeedTypeDef speed, uint16_t* length) {
    (void)speed;
    USBD_GetString((uint8_t*)USBD_PRODUCT_STRING, global_string_desc, length);
    return global_string_desc;
}

static uint8_t* get_config_desc(USBD_SpeedTypeDef speed, uint16_t* length) {
    (void)speed;
    USBD_GetString((uint8_t*)USBD_CONFIGURATION_STRING, global_string_desc, length);
    return global_string_desc;
}

static uint8_t* get_interface_desc(USBD_SpeedTypeDef speed, uint16_t* length) {
    (void)speed;
    USBD_GetString((uint8_t*)USBD_INTERFACE_STRING, global_string_desc, length);
    return global_string_desc;
}

static void pack_int(uint32_t value, uint8_t* out, uint8_t len) {
    for (unsigned i = 0; i < len; i++) {
        if (((value >> 28)) < 0xA) {
            out[2 * i] = (value >> 28) + '0';
        } else {
            out[2 * i] = (value >> 28) + 'A' - 10;
        }
        out[2 * i + 1] = 0;

        value <<= 4;
    }
}

static uint8_t* get_serial_desc(USBD_SpeedTypeDef speed, uint16_t* length) {
    (void)speed;
    uint32_t ser[3];
    memcpy(ser, (uint32_t const*)0x1FFF7A10, sizeof(ser));

    // produce a 12-byte serial number in the same format as ST's generated code
    uint16_t i = 0;
    global_string_desc[i++] = 0;
    global_string_desc[i++] = USB_DESC_TYPE_STRING;

    ser[0] += ser[2];
    if (ser[0]) {
        pack_int(ser[0], &global_string_desc[i], 8);
        i += 8 * 2;
        pack_int(ser[1], &global_string_desc[i], 4);
        i += 4 * 2;
    }
    *length = global_string_desc[0] = i;

    return global_string_desc;
}

USBD_DescriptorsTypeDef cdc_descriptors = {
    .GetDeviceDescriptor = get_device_desc,
    .GetLangIDStrDescriptor = get_lang_desc,
    .GetManufacturerStrDescriptor = get_manufacturer_desc,
    .GetProductStrDescriptor = get_product_desc,
    .GetConfigurationStrDescriptor = get_config_desc,
    .GetInterfaceStrDescriptor = get_interface_desc,
    .GetSerialStrDescriptor = get_serial_desc,
};