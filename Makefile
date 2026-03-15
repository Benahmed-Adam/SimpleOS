CXX = i686-linux-gnu-g++
ASM = nasm

SRC_DIR = src
OBJ_DIR = build/obj
BIN_DIR = build/bin
ISO_DIR = build/isodir

KERNEL_BIN = $(BIN_DIR)/myos.bin
DISK_IMG = myos.img

SRC_CXX = $(shell find $(SRC_DIR) -name "*.cpp")
SRC_ASM = $(shell find $(SRC_DIR) -name "*.s")

OBJ_CXX = $(patsubst $(SRC_DIR)/%.cpp, $(OBJ_DIR)/%.cpp.o, $(SRC_CXX))
OBJ_ASM = $(patsubst $(SRC_DIR)/%.s, $(OBJ_DIR)/%.s.o, $(SRC_ASM))
ALL_OBJ = $(OBJ_CXX) $(OBJ_ASM)

CXXFLAGS = -ffreestanding -O2 -Wall -Wextra -fno-exceptions -fno-rtti -I$(SRC_DIR)
ASFLAGS  = -f elf32
LDFLAGS  = -T linker.ld -ffreestanding -O2 -nostdlib -lgcc -z notext

.PHONY: all clean run iso

all: $(ISO_DIR)

$(DISK_IMG): $(KERNEL_BIN) $(SRC_DIR)/grub.cfg
	@mkdir -p $(ISO_DIR)/boot/grub
	cp $(KERNEL_BIN) $(ISO_DIR)/boot/
	cp $(SRC_DIR)/grub.cfg $(ISO_DIR)/boot/grub/
	grub-mkrescue -o $(DISK_IMG) $(ISO_DIR)

$(KERNEL_BIN): $(ALL_OBJ) linker.ld
	@mkdir -p $(BIN_DIR)
	$(CXX) -o $@ $(LDFLAGS) $(ALL_OBJ)

$(OBJ_DIR)/%.cpp.o: $(SRC_DIR)/%.cpp
	@mkdir -p $(dir $@)
	$(CXX) -c $< -o $@ $(CXXFLAGS)

$(OBJ_DIR)/%.s.o: $(SRC_DIR)/%.s
	@mkdir -p $(dir $@)
	$(ASM) $< -o $@ $(ASFLAGS)

clean:
	rm -rf build $(DISK_IMG)

run: $(DISK_IMG)
	qemu-system-i386 -audiodev sdl,id=speaker -machine pcspk-audiodev=speaker -drive format=raw,file=$(DISK_IMG)