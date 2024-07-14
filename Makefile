FIND_EXCLUDE_ARG := -not -path './build/*'
SRC_FILES := $(shell find . -name '*.cpp' $(FIND_EXCLUDE_ARG))
HEADER_FILES := $(shell find . -name '*.h' $(FIND_EXCLUDE_ARG))
ALL_FILES := $(SRC_FILES) $(HEADER_FILES)

CLANG_FORMAT := clang-format

ARCH := aarch64

all:
	ninja -C build 

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

clean: all
	ninja -C build clean

run:
	qemu-system-$(ARCH) \
		-machine virt \
		-cpu cortex-a53 \
		-kernel build/byteos \
		-nographic \
		-D qemu.log \
		-d in_asm,int,pcall,cpu_reset,guest_errors

cmake:
	cmake -B build -S . -G Ninja -DCMAKE_EXPORT_COMPILE_COMMANDS=on

.PHONY: all fmt show-files clean run cmake
