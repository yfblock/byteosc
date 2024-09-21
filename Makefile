FIND_EXCLUDE_ARG := -not -path './build/*'
SRC_FILES := $(shell find . -name '*.cpp' $(FIND_EXCLUDE_ARG))
HEADER_FILES := $(shell find . -name '*.h' $(FIND_EXCLUDE_ARG))
ALL_FILES := $(SRC_FILES) $(HEADER_FILES)

CLANG_FORMAT := clang-format

ARCH := aarch64

all:
	ninja -C build 
	rust-objcopy --binary-architecture=aarch64 build/byteos \
		--strip-all -O binary build/byteos.bin

# format all code *.cpp *.h
fmt:
	@$(CLANG_FORMAT) -i $(ALL_FILES)
	@echo "All files have been formatted."

fdt:
	@qemu-system-$(ARCH) -M 128m -machine virt,dumpdtb=virt.out
	fdtdump virt.out

show-files:
	@echo "Source files: $(SRC_FILES)"
	@echo "Header files: $(HEADER_FILES)"
	@echo "All files: $(ALL_FILES)"

clean:
	rm build/byteos.bin
	ninja -C build clean

run:
	qemu-system-$(ARCH) \
		-machine virt \
		-cpu cortex-a72 \
		-kernel build/byteos.bin \
		-nographic \
		-D qemu.log \
		-d in_asm,int,pcall,cpu_reset,guest_errors

cmake:
	cmake -B build -S . -G Ninja -DCMAKE_EXPORT_COMPILE_COMMANDS=on

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
	"make gdb_server && echo '按任意键继续' && read -n 1" && \
	tmux split-window -h "gdb build/byteos -ex 'target remote localhost:1234' -ex 'disp /16i $pc' " && \
	tmux -2 attach-session -d

.PHONY: all fmt show-files clean run cmake gdb_server debug
