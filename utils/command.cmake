# --------------------------------------
# QEMU Execution Options
# --------------------------------------
set(QEMU_EXEC qemu-system-${CMAKE_SYSTEM_PROCESSOR})

configure_file(
    ${CMAKE_CURRENT_SOURCE_DIR}/arch/config-include.h.in
    ${CMAKE_CURRENT_BINARY_DIR}/generated/config-include.h
)

if(KernelArch STREQUAL "aarch64")
    list(APPEND QEMU_EXEC -machine virt -cpu cortex-a72)
elseif(KernelArch STREQUAL "riscv64")
    list(APPEND QEMU_EXEC -machine virt)
endif()

set(QEMU_EXEC_CMD ${QEMU_EXEC}
    -kernel ${PROJECT_BINARY_DIR}/byteos.bin
    -nographic
    -D qemu.log
    -d in_asm,int,pcall,cpu_reset,guest_errors

    # --------------------------------------
    # Machine Config
    # --------------------------------------
    # -usbc
    # -device usb-ehci,id=ehci
#    -device usb-tablet,bus=usb-bus.0
    # -device usb-host,bus=ehci.0
#    -device usb-storage,bus=ehci.0,drive=x0

    # --------------------------------------
    # Virtio Devices
    # --------------------------------------
    -global virtio-mmio.force-legacy=false
    -drive
    file=${CMAKE_CURRENT_BINARY_DIR}/mount.img,if=none,format=raw,id=x0
    -device
    virtio-blk-device,drive=x0,bus=virtio-mmio-bus.0
)

add_custom_target(fdt
    COMMAND ${QEMU_EXEC_CMD}
        -machine virt,dumpdtb=virt.out
        &&
        fdtdump virt.out
    # --------------------------------------
    # Other Arguments
    # --------------------------------------
    COMMENT "Display Device Tree For the qemu configuration"
    USES_TERMINAL
)

add_custom_target(run
    COMMAND ${QEMU_EXEC_CMD}
    # --------------------------------------
    # Other Arguments
    # --------------------------------------
    DEPENDS byteos_bin
    COMMENT "Running byteos in QEMU"
    USES_TERMINAL
)

add_custom_target(dbg
        COMMAND ${QEMU_EXEC_CMD} -s -S
        # --------------------------------------
        # Other Arguments
        # --------------------------------------
        DEPENDS byteos_bin
        COMMENT "Running byteos in QEMU"
        USES_TERMINAL
)

add_custom_target(gdb
        COMMAND gdb byteos -ex 'target remote localhost:1234' -ex 'disp /16i $pc'
        # --------------------------------------
        # Other Arguments
        # --------------------------------------
        DEPENDS byteos
        COMMENT "Debugging byteos through gdb"
        USES_TERMINAL
)

file(GLOB_RECURSE FMT_C_FILES "${CMAKE_SOURCE_DIR}/*.c")
list(FILTER FMT_C_FILES EXCLUDE REGEX ".*/build/.*")
file(GLOB_RECURSE FMT_HEADER_FILES "${CMAKE_SOURCE_DIR}/*.h" EXCLUDE REGEX ".*/build/.*")
list(FILTER FMT_HEADER_FILES EXCLUDE REGEX ".*/build/.*")
add_custom_target(fmt
    COMMAND clang-format
        -i
        ${FMT_C_FILES}
        ${FUNC_HEADER_FILES}
)
