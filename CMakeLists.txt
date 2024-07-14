cmake_minimum_required(VERSION 3.10)

project(ByteOS C ASM CXX)

message(STATUS "==========================")
message(STATUS "     Compiling ByteOS     ")
message(STATUS "==========================")
#----------------------------------------------------------------
# Set Kernel Options 
#----------------------------------------------------------------
set(ToolChainPrefix )
set(KernelArch aarch64)

# Declare Variables
set(LINKER_SCRIPT_DIR "${PROJECT_SOURCE_DIR}/linker")
set(LINKER_FILE "${LINKER_SCRIPT_DIR}/linker-${KernelArch}.ld")

set(CMAKE_SYSTEM_PROCESSOR ${KernelArch})

# Use c11 standard
set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 14)

set(CMAKE_ASM_COMPILER ${ToolChainPrefix}gcc)
set(CMAKE_C_COMPILER ${ToolChainPrefix}gcc)
set(CMAKE_CXX_COMPILER ${ToolChainPrefix}g++)

# Set compile options
# set(CMAKE_ASM_FLAGS "-march=rv64imac -mabi=lp64 -mcmodel=medany")
set(CMAKE_C_FLAGS "-nostdlib -O2")
set(CMAKE_CXX_FLAGS "-Wno-main -nostdlib -O2")
set(CMAKE_EXE_LINKER_FLAGS "-T ${LINKER_FILE} ")

add_compile_options(
    #-------------------------------------------
    # Compile Options for Warnings
    #-------------------------------------------
    # -Wall
    -Werror
    # -Wmissing-declarations
    -Wundef
    -Wpointer-arith
    -Wno-nonnull
    #-------------------------------------------
    # Compile Options for Machine
    #-------------------------------------------
    # -march=rv64gc
    -mabi=lp64 
    # -mcmodel=medany

    # Aarch64 options
    -mgeneral-regs-only
)

# For vscode
add_definitions(-DCMAKE_EXPORT_COMPILE_COMMANDS=ON)

include(config.cmake)

# set_source_files_properties(*.asm PROPERTIES LANGUAGE ASM)

# find all source files
file(GLOB_RECURSE ASM_SOURCES "${CMAKE_SOURCE_DIR}/*.S")
file(GLOB_RECURSE C_SOURCES "${CMAKE_SOURCE_DIR}/*.c")
file(GLOB_RECURSE CPP_SOURCES "${CMAKE_SOURCE_DIR}/*.cpp")
list(FILTER ASM_SOURCES EXCLUDE REGEX "build/*")
list(FILTER C_SOURCES EXCLUDE REGEX "build/*")
list(FILTER CPP_SOURCES EXCLUDE REGEX "build/*")
list(FILTER ASM_SOURCES EXCLUDE REGEX "arch/riscv64/*")
list(FILTER C_SOURCES EXCLUDE REGEX "arch/riscv64/*")
list(FILTER CPP_SOURCES EXCLUDE REGEX "arch/riscv64/*")

# Set include folders
include_directories(
    includes 
    arch/${KernelArch}/includes
    drivers/includes
    libs/smoldtb 
    libs/memory
    libs/std_impl
    libs/buddy_alloc
)

# add_executable(byteos ${C_SOURCES} ${ASM_SOURCES})
message(STATUS "C_SOURCE_FILES: ${C_SOURCES}")
add_executable(byteos ${ASM_SOURCES} ${C_SOURCES} ${CPP_SOURCES})
# No Stdlib and ignore stdlib headers
target_compile_options(
    byteos 
    PRIVATE 
    -nostdlib 
    -Wno-builtin-declaration-mismatch
)

add_custom_target(run
    COMMAND qemu-system-${CMAKE_SYSTEM_PROCESSOR}
            -machine virt
            -kernel ${PROJECT_BINARY_DIR}/byteos
            -nographic
            -D qemu.log
            -d in_asm,int,pcall,cpu_reset,guest_errors
    DEPENDS byteos
    COMMENT "Running byteos in QEMU"
)
