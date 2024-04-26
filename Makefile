#--------------------------------------Makefile-------------------------------------
BUILD_DIR = ./build
SRC_DIR = ./src

CFILES = $(wildcard $(SRC_DIR)/*.c)
OFILES = $(CFILES:$(SRC_DIR)/%.c=$(BUILD_DIR)/%.o)

GCCFLAGS = -Wall -O2 -ffreestanding -nostdinc -nostdlib -nostartfiles

all: clean kernel8.img run

$(BUILD_DIR)/boot.o: $(SRC_DIR)/boot.S
	aarch64-none-elf-gcc $(GCCFLAGS) -c $< -o $@

$(BUILD_DIR)/%.o: $(SRC_DIR)/%.c
	aarch64-none-elf-gcc $(GCCFLAGS) -c $< -o $@

kernel8.img: $(BUILD_DIR)/boot.o $(OFILES)
	aarch64-none-elf-ld -nostdlib $(BUILD_DIR)/boot.o $(OFILES) -T $(SRC_DIR)/link.ld -o $(BUILD_DIR)/kernel8.elf
	aarch64-none-elf-objcopy -O binary $(BUILD_DIR)/kernel8.elf kernel8.img

clean:
	del .\build\kernel8.elf .\build\*.o .\build\*.img

# Run emulation with QEMU
run: 
	qemu-system-aarch64 -M raspi3 -kernel kernel8.img -serial stdio -display none

