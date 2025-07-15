ARCH := aarch64

PREFIX := $(ARCH)-linux-gnu-
CC := $(PREFIX)gcc

ALL_C_FILES := $(shell find . -name *.c -not -path './arch/*' | sed 's|^\./||') \
	$(shell find arch/${ARCH} -name *.c)
ALL_ASM_FILES := $(shell find . -name *.S -not -path './arch/*' | sed 's|^\./||') \
	$(shell find arch/${ARCH} -name *.S)
O_FILES := $(patsubst %.S,build/%.o,$(ALL_ASM_FILES)) $(patsubst %.c,build/%.o,$(ALL_C_FILES))

CFLAGS := -mgeneral-regs-only -Tlinker/linker-$(ARCH).ld

CFLAGS += -std=c23 -O2 -fno-builtin -ffreestanding -Wundef \
    -Wpointer-arith \
    -Wno-nonnull \
	-nostdlib \
	-nostartfiles


# Add includes folders to compiler flags
INCLUDE_DIRS := $(shell find . -name includes | sed 's|^\./||')
INCLUDE_FLAGS := $(addprefix -I,$(INCLUDE_DIRS))
CFLAGS += $(INCLUDE_FLAGS)

# Find files for fmt
CLANG_FORMAT := clang-format


all: build

build: build/byteos.bin

build/byteos.bin: build/byteos
	@llvm-objcopy -O binary $< $@

build/byteos: $(O_FILES)
	@$(CC) $(CFLAGS) -static -o $@ $^

build/%.o: ./%.c
	@mkdir -p $(shell dirname $@)
	@echo "CC $<"
	@$(CC) $(CFLAGS) -c -o $@ $<

build/%.o: ./%.S
	@mkdir -p $(shell dirname $@)
	@echo "CC $<"
	@$(CC) $(CFLAGS) -c -o $@ $<

# format all code *.cpp *.h
fmt: $(shell find . -name '*.c' -or -name '*.h' -not -path './build/*')
	@$(CLANG_FORMAT) -i -style=file $^
	@echo "All files have been formatted."

fdt:
	@qemu-system-$(ARCH) -M 128m -machine virt,dumpdtb=virt.out
	fdtdump virt.out

clean:
	@rm $(O_FILES)
	@rm build/byteos.bin
	@rm build/byteos

run: build
	qemu-system-$(ARCH) \
		-machine virt \
		-cpu cortex-a72 \
		-kernel build/byteos.bin \
		-nographic \
		-D qemu.log \
		-d in_asm,int,pcall,cpu_reset,guest_errors

gdb_server:
	qemu-system-$(ARCH) \
		-cpu cortex-a72 \
		-machine virt \
		-kernel build/byteos.bin \
		-nographic \
		-D qemu.log \
		-d in_asm,int,pcall,cpu_reset,guest_errors -s -S

debug: build
	@tmux new-session -d \
	"ninja -C build run && echo '按任意键继续' && read -n 1" && \
	tmux split-window -h "gdb build/byteos -ex 'target remote localhost:1234' -ex 'disp /16i $pc' " && \
	tmux -2 attach-session -d

QEMU_LOG=n
qemu: build/byteos.bin
	qemu-system-$(ARCH) \
		-cpu cortex-a72 \
		-M virt -nographic \
		-D qemu.log \
		-d in_asm,int,pcall,cpu_reset,guest_errors \
		-kernel build/byteos.bin

.PHONY: all fmt show-files clean run cmake gdb_server debug
