FIND_EXCLUDE_ARG := -not -path './build/*'
SRC_FILES := $(shell find . -name '*.cpp' $(FIND_EXCLUDE_ARG))
HEADER_FILES := $(shell find . -name '*.h' $(FIND_EXCLUDE_ARG))
ALL_FILES := $(SRC_FILES) $(HEADER_FILES)

CLANG_FORMAT := clang-format

all:
	ninja -C build 

# format all code *.cpp *.h
format:
	@$(CLANG_FORMAT) -i $(ALL_FILES)
	@echo "All files have been formatted."

fdt:
	@qemu-system-riscv64 -M 128m -machine virt,dumpdtb=virt.out
	fdtdump virt.out

show-files:
	@echo "Source files: $(SRC_FILES)"
	@echo "Header files: $(HEADER_FILES)"
	@echo "All files: $(ALL_FILES)"

clean: all
	ninja -C build clean

run:
	@qemu-system-riscv64 \
		-machine virt \
		-kernel build/byteos \
		-nographic \
		-D qemu.log \
		-d in_asm,int,pcall,cpu_reset,guest_errors

.PHONY: all format show-files clean run
