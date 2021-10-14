RPI_VERSION ?= 4

ARMGNU ?=

COPS = -DRPI_VERSION=$(RPI_VERSION) -Wall -nostdlib -nostartfiles -ffreestanding -fstack-protector-strong \
	   -Iinclude -mgeneral-regs-only

ASMOPS = -Iinclude

BUILD_DIR = build
SRC_DIR = src
LIB_DIR = libc

all : kernel8.img

clean :
	rm -rf $(BUILD_DIR) *.img

BCM4345C0.o: BCM4345C0.hcd
	aarch64-none-elf-objcopy -I binary -O elf64-littleaarch64 -B aarch64 $< $@

$(BUILD_DIR)/%_c.o: $(SRC_DIR)/%.c
	mkdir -p $(@D)
	$(ARMGNU)-gcc $(COPS) -MMD -c $< -o $@

$(BUILD_DIR)/%_lib.o: $(LIB_DIR)/%.c
	mkdir -p $(@D)
	$(ARMGNU)-gcc $(COPS) -MMD -c $< -o $@

$(BUILD_DIR)/%_s.o: $(SRC_DIR)/%.S
	mkdir -p $(@D)
	$(ARMGNU)-gcc $(COPS) -MMD -c $< -o $@

C_FILES = $(wildcard $(SRC_DIR)/*.c)
ASM_FILES = $(wildcard $(SRC_DIR)/*.S)
LIB_FILES = $(wildcard $(LIB_DIR)/*.c)
OBJ_FILES = $(C_FILES:$(SRC_DIR)/%.c=$(BUILD_DIR)/%_c.o)
OBJ_FILES += $(ASM_FILES:$(SRC_DIR)/%.S=$(BUILD_DIR)/%_s.o)
OBJ_FILES += $(LIB_FILES:$(LIB_DIR)/%.c=$(BUILD_DIR)/%_lib.o)

DEP_FILES = $(OBJ_FILES:%.o=%.d)
-include $(DEP_FILES)

kernel8.img: $(SRC_DIR)/linker.ld $(OBJ_FILES) BCM4345C0.o
	@echo "Building IceOS for RPI $(value RPI_VERSION)"

	$(ARMGNU)-ld -T $(SRC_DIR)/linker.ld -o $(BUILD_DIR)/kernel8.elf $(OBJ_FILES) BCM4345C0.o
	$(ARMGNU)-objcopy $(BUILD_DIR)/kernel8.elf -O binary kernel8.img

	cp kernel8.img kernel8-rpi4.img

armstub/build/armstub_s.o: armstub/src/armstub.S
	mkdir -p $(@D)
	$(ARMGNU)-gcc $(COPS) -MMD -c $< -o  $@

armstub: armstub/build/armstub_s.o
	$(ARMGNU)-ld --section-start=.text=0 -o armstub/build/armstub.elf armstub/build/armstub_s.o
	$(ARMGNU)-objcopy armstub/build/armstub.elf -O binary armstub-new.bin	