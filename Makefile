MAIN_TARGET = wii-pocket.elf

ifeq ($(OS),Windows_NT)
	FLASH_PROGRAM = STM32_Programmer_CLI.exe
else
	FLASH_PROGRAM = STM32_Programmer.sh
endif

WARNINGS = \
	-Werror=all \
	-Werror=extra \
	-Wfatal-errors \
	-Wno-unused-parameter # thanks, ST...

GCC_PREFIX = arm-none-eabi-
CPU = -mcpu=cortex-m4
FPU = -mfpu=fpv4-sp-d16
FLOAT_ABI = -mfloat-abi=hard

DEBUG = 1
OPT = -Og

LIBS = -lc -lm -lnosys
LD_SCRIPT = src/sys/stm32g431kbtx_flash.ld
DEFS = -DSTM32G431xx -DUSE_FULL_LL_DRIVER -DUSE_HAL_DRIVER
SOURCES = \
	src \
	ext/STM32CubeG4/Drivers/CMSIS/Device/ST/STM32G4xx/Source/Templates/gcc/startup_stm32g431xx.s \
	ext/STM32CubeG4/Drivers/CMSIS/Device/ST/STM32G4xx/Source/Templates/system_stm32g4xx.c \
	ext/STM32CubeG4/Drivers/STM32G4xx_HAL_Driver/Src/stm32g4xx_hal.c \
	ext/STM32CubeG4/Drivers/STM32G4xx_HAL_Driver/Src/stm32g4xx_hal_cortex.c \
	ext/STM32CubeG4/Drivers/STM32G4xx_HAL_Driver/Src/stm32g4xx_hal_pcd.c \
	ext/STM32CubeG4/Drivers/STM32G4xx_HAL_Driver/Src/stm32g4xx_hal_pcd_ex.c \
	ext/STM32CubeG4/Drivers/STM32G4xx_HAL_Driver/Src/stm32g4xx_ll_exti.c \
	ext/STM32CubeG4/Drivers/STM32G4xx_HAL_Driver/Src/stm32g4xx_ll_gpio.c \
	ext/STM32CubeG4/Drivers/STM32G4xx_HAL_Driver/Src/stm32g4xx_ll_i2c.c \
	ext/STM32CubeG4/Drivers/STM32G4xx_HAL_Driver/Src/stm32g4xx_ll_usb.c \
	ext/STM32CubeG4/Drivers/STM32G4xx_HAL_Driver/Src/stm32g4xx_ll_utils.c \
	ext/STM32CubeG4/Middlewares/ST/STM32_USB_Device_Library/Class/CDC/Src/usbd_cdc.c \
	ext/STM32CubeG4/Middlewares/ST/STM32_USB_Device_Library/Core/Src/usbd_core.c \
	ext/STM32CubeG4/Middlewares/ST/STM32_USB_Device_Library/Core/Src/usbd_ctlreq.c \
	ext/STM32CubeG4/Middlewares/ST/STM32_USB_Device_Library/Core/Src/usbd_ioreq.c
INC_DIRS = \
	src \
	src/app \
	src/conf \
	src/driver \
	src/i2c \
	ext/STM32CubeG4/Drivers/CMSIS/Core/Include \
	ext/STM32CubeG4/Drivers/CMSIS/Device/ST/STM32G4xx/Include \
	ext/STM32CubeG4/Drivers/STM32G4xx_HAL_Driver/Inc \
	ext/STM32CubeG4/Middlewares/ST/STM32_USB_Device_Library/Class/CDC/Inc \
	ext/STM32CubeG4/Middlewares/ST/STM32_USB_Device_Library/Core/Inc

# try to keep the rest of this more-or-less project independent:

BUILD_DIR ?= build

CC = $(GCC_PREFIX)gcc
AS = $(GCC_PREFIX)gcc -x assembler-with-cpp
CP = $(GCC_PREFIX)objcopy
SZ = $(GCC_PREFIX)size

SRCS := $(shell find $(SOURCES) -name '*.[cs]')
OBJS := $(SRCS:%=$(BUILD_DIR)/%.o)
DEPS := $(OBJS:.o=.d)

MCU = $(CPU) -mthumb $(FPU) $(FLOAT-ABI)
INCLUDES := $(addprefix -I,$(INC_DIRS))
OPT ?=

AS_DEFS ?= $(DEFS)
AS_INC_DIRS ?= 
AS_INCLUDES := $(addprefix -I,$(AS_INC_DIRS)) $(INCLUDES)
ASFLAGS = $(MCU) $(AS_DEFS) $(AS_INCLUDES) $(OPT) $(WARNINGS) -fdata-sections -ffunction-sections

C_DEFS ?= $(DEFS)
C_INC_DIRS ?=
C_INCLUDES := $(addprefix -I,$(C_INC_DIRS)) $(INCLUDES)
CFLAGS = $(MCU) $(C_DEFS) $(C_INCLUDES) $(OPT) $(WARNINGS) -fdata-sections -ffunction-sections
CFLAGS += -MMD -MP

ifeq ($(DEBUG), 1)
CFLAGS += -g -gdwarf-2
endif

LIB_DIRS ?= 
LDFLAGS = $(MCU) -specs=nano.specs -T$(LD_SCRIPT) $(LIB_DIRS) $(LIBS) \
	-Wl,-Map=$(BUILD_DIR)/$(TARGET).map,--cref -Wl,--gc-sections

MKDIR_P ?= mkdir -p

$(BUILD_DIR)/$(MAIN_TARGET): $(OBJS)
	$(CC) $(OBJS) -o $@ $(LDFLAGS)
	$(SZ) $@

$(BUILD_DIR)/%.s.o: %.s
	$(MKDIR_P) $(dir $@)
	$(AS) $(ASFLAGS) -c $< -o $@

$(BUILD_DIR)/%.c.o: %.c
	$(MKDIR_P) $(dir $@)
	$(CC) $(CFLAGS) -c $< -o $@

.PHONY: clean
clean:
	$(RM) -r $(BUILD_DIR)

-include $(DEPS)

flash: $(BUILD_DIR)/$(MAIN_TARGET)
	$(FLASH_PROGRAM) -c port=SWD -e all -w $(BUILD_DIR)/$(MAIN_TARGET) -v -rst