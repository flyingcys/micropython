# Include the core environment definitions; this will set $(TOP).
include ../../py/mkenv.mk

# Include py core make definitions.
include $(TOP)/py/py.mk

# Set makefile-level MicroPython feature configurations.
MICROPY_ROM_TEXT_COMPRESSION ?= 0

# Define toolchain and other tools.
CROSS_COMPILE ?= riscv64-unknown-elf-

## fip param
FIP_COMPRESS ?= lzma

BUILD_PLAT ?= tools/duo256
CHIP_CONF_PATH = ${BUILD_PLAT}/chip_conf.bin
MONITOR_PATH = tools/fw_dynamic.bin

FIPTOOL ?= tools/plat/cv181x/fiptool.py
BLCP_IMG_RUNADDR ?= 0x05200200
BLCP_PARAM_LOADADDR ?= 0
NAND_INFO = 00000000
NOR_INFO = $(shell printf '%72s' | tr ' ' 'FF')
DDR_PARAM_TEST_PATH = tools/test/cv181x/ddr_param.bin
BLCP_PATH = tools/test/empty.bin
BLCP_2ND_PATH = build/firmware.bin
LOADER_2ND_PATH= ${BUILD_PLAT}/u-boot-raw.bin

# Set CFLAGS.
CFLAGS += -Wall -Wextra -std=c99 -nostdlib 
CFLAGS += -march=rv64imafdc -mabi=lp64d -mcmodel=medany
CFLAGS += -std=gnu11 -ffunction-sections -fdata-sections -Wl,--gc-sections -Wno-pointer-to-int-cast -fno-builtin -Wno-missing-field-initializers -fdiagnostics-color=always

# Select debugging or optimisation build.
ifeq ($(DEBUG), 1)
CFLAGS += -Og
else
CFLAGS += -O2 -DNDEBUG
endif

LDSCRIPT = cv181x_lscript.ld

# Set linker flags.
LDFLAGS += -nostartfiles -fms-extensions -ffunction-sections -fdata-sections -Wl,--gc-sections,-Map=$@.map,-cref,-u,_start -T $(LDSCRIPT)

# Define the required source files.
SRC_C += main.c lib.c mphalport.c
SRC_C += \
	drivers/csi_adc.c \
	drivers/csi_common.c \
	drivers/csi_dma_consts.c \
	drivers/csi_dma.c \
	drivers/csi_gpio_pin.c \
	drivers/csi_gpio.c \
	drivers/csi_iic.c \
	drivers/csi_pin.c \
	drivers/csi_pwm.c \
	drivers/csi_spi.c \
	drivers/csi_target_get.c \
	drivers/csi_uart.c \
	drivers/csi_dma.c \
	hal/hal_dma.c \
	hal/hal_sysdma.c \
	hal/dw/dw_iic_ll.c \
	hal/dw/dw_spi_ll.c \
	hal/dw/dw_uart_ll.c \
	startup/interrupt.c \
	startup/tick.c

SRC_C += board/duo256/variant_pin_maps.c

SRC_S += startup/start.S 

INC += -I.
INC += -I$(TOP)
INC += -I$(BUILD)
INC += -Iinclude
INC += -Iinclude/csi
INC += -Iinclude/cvi
INC += -Iinclude/hal
INC += -Iinclude/hal/dw
INC += -Iboard/duo256

CFLAGS += $(INC)

# Define the required object files.
OBJ += $(PY_CORE_O)
OBJ += $(addprefix $(BUILD)/, $(SRC_C:.c=.o))
OBJ += $(addprefix $(BUILD)/, $(SRC_S:.S=.o))

# Define the top-level target, the main firmware.
all: $(BUILD)/fip.bin

$(BUILD)/firmware.elf: $(OBJ)
	$(ECHO) "LINK $@"
	$(Q)$(CROSS_COMPILE)gcc $(LDFLAGS) -o $@ $^
	$(Q)$(SIZE) $@

$(BUILD)/firmware.bin: $(BUILD)/firmware.elf
	$(ECHO) "Create $@"
	$(Q)$(OBJCOPY) -O binary $^ $@

$(BUILD)/fip.bin: $(BUILD)/firmware.bin
	$(ECHO) "Create $@"
	${Q}. ${BUILD_PLAT}/blmacros.env && \
	${FIPTOOL} -v genfip \
		'${BUILD_PLAT}/fip.bin' \
		--MONITOR_RUNADDR="$${MONITOR_RUNADDR}" \
		--BLCP_2ND_RUNADDR="$${BLCP_2ND_RUNADDR}" \
		--CHIP_CONF='${CHIP_CONF_PATH}' \
		--NOR_INFO='${NOR_INFO}' \
		--NAND_INFO='${NAND_INFO}'\
		--BL2='${BUILD_PLAT}/bl2.bin' \
		--BLCP_IMG_RUNADDR=${BLCP_IMG_RUNADDR} \
		--BLCP_PARAM_LOADADDR=${BLCP_PARAM_LOADADDR} \
		--BLCP=${BLCP_PATH} \
		--DDR_PARAM='${DDR_PARAM_TEST_PATH}' \
		--BLCP_2ND='${BLCP_2ND_PATH}' \
		--MONITOR='${MONITOR_PATH}' \
		--LOADER_2ND='${LOADER_2ND_PATH}' \
		--compress='${FIP_COMPRESS}'

# Include remaining core make rules.
include $(TOP)/py/mkrules.mk
