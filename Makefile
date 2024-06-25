all:
	ninja -C build 
clean:
	ninja -C build clean
run:
	qemu-system-riscv64 \
		-machine virt \
		-kernel build/byteos \
		-nographic \
		-D qemu.log \
		-d in_asm,int,pcall,cpu_reset,guest_errors
