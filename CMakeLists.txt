cmake_minimum_required(VERSION 3.10)

project(ByteOS C ASM)

message(STATUS "==========================")
message(STATUS "     Compiling ByteOS     ")
message(STATUS "==========================")

# ----------------------------------------------------------------
# Set Kernel Options
# ----------------------------------------------------------------
set(KernelArch ${KERNEL_TARGET})

# Declare Variables
set(LINKER_SCRIPT_DIR "${PROJECT_SOURCE_DIR}/linker")
set(LINKER_FILE "${LINKER_SCRIPT_DIR}/linker-${KernelArch}.ld")

set(CMAKE_SYSTEM_PROCESSOR ${KernelArch})

# Use c23 standard
set(CMAKE_C_STANDARD 23)

set(CMAKE_OBJCOPY llvm-objcopy)

# set(TOOLS_PREFIX "${KernelArch}-linux-gnu-")
message(STATUS "TOOLS_PREFIX ${TOOLS_PREFIX}")
set(CMAKE_ASM_COMPILER ${TOOLS_PREFIX}gcc)
set(CMAKE_C_COMPILER ${TOOLS_PREFIX}gcc)
# set(CMAKE_ASM_COMPILER clang)
# set(CMAKE_C_COMPILER clang)
# set(CMAKE_EXE_LINKER_FLAGS -fuse-ld=lld)

# Set compile options
set(CMAKE_C_FLAGS "")
set(CMAKE_EXE_LINKER_FLAGS "-T ${LINKER_FILE} -nostdlib -nostartfiles -ffreestanding")
list(REMOVE_ITEM CMAKE_C_IMPLICIT_LINK_LIBRARIES stdc)

add_compile_options(
    # -------------------------------------------
    # Compile Options for Warnings
    # -------------------------------------------
    # -Wall
    -Werror

    # -Wmissing-declarations
    -Wundef
    -Wpointer-arith
    -Wno-nonnull

    # -------------------------------------------
    # Compile Options for Machine
    # -------------------------------------------
    -O2
    -fno-builtin
    -ffreestanding

    # -------------------------------------------
    # Compile Options for architecture
    # -------------------------------------------
    # -target ${KernelArch}-elf
)

# Aarch64 options
if(KernelArch STREQUAL "aarch64")
    add_compile_options(
        -mgeneral-regs-only
    )
elseif(KernelArch STREQUAL "riscv64")
    add_compile_options(
        -mcmodel=medany
        # -march=rv64gc
        # -march=rv64imac
        # -mabi=lp64
        # -mfloat-abi=hard
        # -msoft-float
    )
endif()

# For vscode
add_definitions(-DCMAKE_EXPORT_COMPILE_COMMANDS=ON)

include(config.cmake)

# Macro to add source files to Variables
# WARN: Dont use function to replace macro.
macro(add_source_directories)
    FOREACH(dir_name ${SOURCE_DIRS})
        file(GLOB_RECURSE FUNC_ASM_SOURCES "${CMAKE_SOURCE_DIR}/${dir_name}/*.S")
        file(GLOB_RECURSE FUNC_C_SOURCES "${CMAKE_SOURCE_DIR}/${dir_name}/*.c")
        list(APPEND ASM_SOURCES ${FUNC_ASM_SOURCES})
        list(APPEND C_SOURCES ${FUNC_C_SOURCES})
    ENDFOREACH()
endmacro()

# find all source files
set(SOURCE_DIRS
    arch/${KernelArch}
    drivers
    kernel
    libs/buddy_alloc
    libs/smoldtb
    libs/std_impl
)
add_source_directories()

# Set include folders
include_directories(
    includes
    arch/${KernelArch}/includes
    drivers/includes
    libs/smoldtb
    libs/memory
    libs/std_impl
    libs/buddy_alloc
    ${CMAKE_CURRENT_BINARY_DIR}/generated
    # libs/lwext4/includes
)

# --------------------------------------
# Generate executable files
# --------------------------------------

add_executable(byteos ${ASM_SOURCES} ${C_SOURCES} ${CPP_SOURCES}
        drivers/driver.c
        drivers/includes/driver-header.h
        drivers/rtc/pl031.c
        drivers/block/virtio-blk.c
        drivers/virtio.c
        drivers/includes/rtc.h
)
target_link_libraries(byteos -static)
# No Stdlib and ignore stdlib headers
target_compile_options(
    byteos
    PRIVATE
    -nostdlib
)

add_custom_target(byteos_bin
    COMMAND ${CMAKE_OBJCOPY} -O binary $<TARGET_FILE:byteos> ${PROJECT_BINARY_DIR}/byteos.bin
)

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
    -usb
    -device usb-ehci,id=ehci
#    -device usb-tablet,bus=usb-bus.0
    -device usb-host,bus=ehci.0
#    -device usb-storage,bus=ehci.0,drive=x0

    # --------------------------------------
    # Virtio Devices
    # --------------------------------------
    -global virtio-mmio.force-legacy=false
    -drive
    file=mount.img,if=none,format=raw,id=x0
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

# TODO: Generate config header from the toml file.
add_custom_target(gen_config
    COMMAND ${CMAKE_SOURCE_DIR}/utils/gen_config.py
        ${CMAKE_SOURCE_DIR}/configs/${CONFIG_FILE}
        ${CMAKE_BINARY_DIR}/generated/gen_config.h
)
add_dependencies(byteos gen_config)
