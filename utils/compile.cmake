set(CMAKE_C_FLAGS "")
# Set Linker script
set(LINKER_SCRIPT_DIR "${PROJECT_SOURCE_DIR}/linker")
set(LINKER_FILE "${LINKER_SCRIPT_DIR}/linker-${KernelArch}.ld")

# set(TOOLS_PREFIX "${KernelArch}-linux-gnu-")
# message(STATUS "TOOLS_PREFIX ${TOOLS_PREFIX}")
# set(CMAKE_ASM_COMPILER ${TOOLS_PREFIX}gcc)
# set(CMAKE_C_COMPILER ${TOOLS_PREFIX}gcc)

# --------------------------------------
# Set Compiler Clang
# --------------------------------------
set(CMAKE_ASM_COMPILER clang)
set(CMAKE_C_COMPILER clang)
set(CMAKE_LINKER "ld.lld")
set(CMAKE_C_COMPILER_TARGET ${KernelArch}-linux-gnu)
set(CMAKE_ASM_COMPILER_TARGET ${KernelArch}-linux-gnu)
add_compile_options(--target=${KernelArch}-linux-gnu)
set(CMAKE_C_LINK_EXECUTABLE "<CMAKE_LINKER> <FLAGS> <CMAKE_C_LINK_FLAGS> <OBJECTS> -o <TARGET> <LINK_LIBRARIES> -T ${LINKER_FILE}")
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
    -fno-stack-protector
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
