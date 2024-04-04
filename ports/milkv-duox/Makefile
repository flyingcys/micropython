# Include the core environment definitions; this will set $(TOP).
include ../../py/mkenv.mk

# Include py core make definitions.
include $(TOP)/py/py.mk

# Set makefile-level MicroPython feature configurations.
MICROPY_ROM_TEXT_COMPRESSION ?= 0

# Define toolchain and other tools.
CROSS_COMPILE ?= riscv64-unknown-elf-
DFU ?= $(TOP)/tools/dfu.py
PYDFU ?= $(TOP)/tools/pydfu.py

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
LDFLAGS += -nostdlib -T $(LDSCRIPT) -Map=$(@:.elf=.map) --cref
LDFLAGS += --gc-sections

# Define the required source files.
SRC_C += main.c
SRC_C += \
	drivers/csi_adc.c \
	driver/csi_common.c \
	drivers/csi_dma_consts.c \
	driver/csd_dma.c \
	drivers/csi_gpio_pin.c \
	driver/csd_gpio.c \
	drivers/csi_iic.c \
	driver/csd_pin.c \
	driver/csd_pwm.c \
	driver/csd_spi.c \
	driver/csd_target_get.c \
	driver/csd_uart.c \
	driver/csd_dma.c \
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
all: $(BUILD)/firmware.bin

$(BUILD)/firmware.elf: $(OBJ)
	$(ECHO) "LINK $@"
	$(Q)$(LD) $(LDFLAGS) -o $@ $^
	$(Q)$(SIZE) $@

$(BUILD)/firmware.bin: $(BUILD)/firmware.elf
	$(ECHO) "Create $@"
	$(Q)$(OBJCOPY) -O binary $^ $@

# Include remaining core make rules.
include $(TOP)/py/mkrules.mk
