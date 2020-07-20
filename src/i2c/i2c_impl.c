#include "i2c_impl.h"

#include "board.h"
#include "i2c.h"

#include <stm32g4xx_ll_bus.h>
#include <stm32g4xx_ll_cortex.h>
#include <stm32g4xx_ll_gpio.h>
#include <stm32g4xx_ll_i2c.h>

#define NBYTES_MAX 255
#define CHECK_TIMER_ELAPSED(timer) (LL_SYSTICK_IsActiveCounterFlag() && timer-- == 0)
#define TIMED_CONDITION(condition, timeout, timer, on_timeout)                                     \
    do {                                                                                           \
        timer = timeout;                                                                           \
        while (!(condition)) {                                                                     \
            if (CHECK_TIMER_ELAPSED(timer)) on_timeout;                                            \
        }                                                                                          \
    } while (0);

bool _i2c_master_init_impl(void) {
    LL_GPIO_InitTypeDef gpio_config;

    // Enable GPIO clocks
    LL_AHB2_GRP1_EnableClock(LL_AHB2_GRP1_PERIPH_GPIOA);
    LL_AHB2_GRP1_EnableClock(LL_AHB2_GRP1_PERIPH_GPIOB);

    // Configure SDA/SCL pins
    gpio_config.Pin = BOARD_SDA_PIN;
    gpio_config.Mode = LL_GPIO_MODE_ALTERNATE;
    gpio_config.OutputType = LL_GPIO_OUTPUT_OPENDRAIN;
    gpio_config.Pull = LL_GPIO_PULL_UP;
    gpio_config.Speed = LL_GPIO_SPEED_FREQ_VERY_HIGH;
    gpio_config.Alternate = BOARD_SDA_AF;
    LL_GPIO_Init(BOARD_SDA_PORT, &gpio_config);

    gpio_config.Pin = BOARD_SCL_PIN;
    gpio_config.Mode = LL_GPIO_MODE_ALTERNATE;
    gpio_config.OutputType = LL_GPIO_OUTPUT_OPENDRAIN;
    gpio_config.Pull = LL_GPIO_PULL_UP;
    gpio_config.Speed = LL_GPIO_SPEED_FREQ_VERY_HIGH;
    gpio_config.Alternate = BOARD_SCL_AF;
    LL_GPIO_Init(BOARD_SCL_PORT, &gpio_config);

    // Enable I2C clock
    LL_APB1_GRP1_EnableClock(LL_APB1_GRP1_PERIPH_I2C1);

    // Configure I2C
    LL_I2C_InitTypeDef i2c_config = { .PeripheralMode = LL_I2C_MODE_I2C,
                                      .Timing = 0x20303E5D, // why ST
                                      .AnalogFilter = LL_I2C_ANALOGFILTER_ENABLE,
                                      .DigitalFilter = 0,
                                      .OwnAddress1 = 0,
                                      .TypeAcknowledge = LL_I2C_ACK,
                                      .OwnAddrSize = LL_I2C_OWNADDRESS1_7BIT };

    if (LL_I2C_Init(BOARD_I2C, &i2c_config) != SUCCESS) return false;
    LL_I2C_EnableAutoEndMode(BOARD_I2C);
    LL_I2C_SetOwnAddress2(BOARD_I2C, 0, LL_I2C_OWNADDRESS2_NOMASK);
    LL_I2C_DisableOwnAddress2(BOARD_I2C);
    LL_I2C_DisableGeneralCall(BOARD_I2C);
    LL_I2C_EnableClockStretching(BOARD_I2C);

    return true;
}

bool _i2c_master_write_impl(
  const uint8_t device, const uint8_t reg, const void* buf, uint16_t len) {
    const uint8_t* pbuf = (uint8_t*)buf;
    if (!pbuf || len == 0) return false;

    uint32_t timer;

    // Start write condition
    LL_I2C_HandleTransfer(
      BOARD_I2C,
      device << 1,
      LL_I2C_ADDRSLAVE_7BIT,
      1,
      LL_I2C_MODE_RELOAD,
      LL_I2C_GENERATE_START_WRITE);

    // Transmit register address
    TIMED_CONDITION(LL_I2C_IsActiveFlag_TXIS(BOARD_I2C), I2C_TIMEOUT, timer, return false);
    LL_I2C_TransmitData8(BOARD_I2C, reg);
    // Wait for 1 byte transaction completion
    TIMED_CONDITION(LL_I2C_IsActiveFlag_TCR(BOARD_I2C), I2C_TIMEOUT, timer, return false);

    // Transmit payload
    do {
        // Reload NBYTES or last transaction and configure auto stop condition
        LL_I2C_HandleTransfer(
          BOARD_I2C,
          device << 1,
          LL_I2C_ADDRSLAVE_7BIT,
          len > NBYTES_MAX ? NBYTES_MAX : len,
          len > NBYTES_MAX ? LL_I2C_MODE_RELOAD : LL_I2C_MODE_AUTOEND,
          LL_I2C_GENERATE_NOSTARTSTOP);

        while (len % (NBYTES_MAX + 1)) {
            // Transmit data byte
            TIMED_CONDITION(LL_I2C_IsActiveFlag_TXIS(BOARD_I2C), I2C_TIMEOUT, timer, return false);
            LL_I2C_TransmitData8(BOARD_I2C, *pbuf++);
            len--;
        }
        // Wait for bulk transaction completion if not generating a stop condition
        if (len) {
            TIMED_CONDITION(LL_I2C_IsActiveFlag_TCR(BOARD_I2C), I2C_TIMEOUT, timer, return false);
        }
    } while (len);

    // Wait for stop condition
    TIMED_CONDITION(LL_I2C_IsActiveFlag_STOP(BOARD_I2C), I2C_TIMEOUT, timer, return false);
    LL_I2C_ClearFlag_STOP(BOARD_I2C);

    return true;
}

bool _i2c_master_read_impl(const uint8_t device, const uint8_t reg, void* buf, uint16_t len) {
    uint8_t* pbuf = (uint8_t*)buf;
    if (!pbuf || len == 0) return false;

    uint32_t timer;

    // Start write condition
    LL_I2C_HandleTransfer(
      BOARD_I2C,
      device << 1,
      LL_I2C_ADDRSLAVE_7BIT,
      1,
      LL_I2C_MODE_SOFTEND,
      LL_I2C_GENERATE_START_WRITE);

    // Transmit register address
    TIMED_CONDITION(LL_I2C_IsActiveFlag_TXIS(BOARD_I2C), I2C_TIMEOUT, timer, return false);
    LL_I2C_TransmitData8(BOARD_I2C, reg);
    // Wait for 1 byte transaction completion
    TIMED_CONDITION(LL_I2C_IsActiveFlag_TC(BOARD_I2C), I2C_TIMEOUT, timer, return false);

    // Start read condition
    LL_I2C_HandleTransfer(
      BOARD_I2C,
      device << 1,
      LL_I2C_ADDRSLAVE_7BIT,
      len > NBYTES_MAX ? NBYTES_MAX : len,
      len > NBYTES_MAX ? LL_I2C_MODE_RELOAD : LL_I2C_MODE_AUTOEND,
      LL_I2C_GENERATE_START_READ);

    // Receive payload
    do {
        while (len % NBYTES_MAX) {
            // Receive data byte
            TIMED_CONDITION(LL_I2C_IsActiveFlag_RXNE(BOARD_I2C), I2C_TIMEOUT, timer, return false);
            *pbuf++ = LL_I2C_ReceiveData8(BOARD_I2C);
            len--;
        }
        // Wait for bulk transaction completion if not generating a stop condition
        if (len) {
            TIMED_CONDITION(LL_I2C_IsActiveFlag_TCR(BOARD_I2C), I2C_TIMEOUT, timer, return false);
            // Reload NBYTES or last transaction and configure auto stop condition
            LL_I2C_HandleTransfer(
              BOARD_I2C,
              device << 1,
              LL_I2C_ADDRSLAVE_7BIT,
              len > NBYTES_MAX ? NBYTES_MAX : len,
              len > NBYTES_MAX ? LL_I2C_MODE_RELOAD : LL_I2C_MODE_AUTOEND,
              LL_I2C_GENERATE_NOSTARTSTOP);
        }
    } while (len);

    // Wait for stop condition
    TIMED_CONDITION(LL_I2C_IsActiveFlag_STOP(BOARD_I2C), I2C_TIMEOUT, timer, return false);
    LL_I2C_ClearFlag_STOP(BOARD_I2C);

    return true;
}