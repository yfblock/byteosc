cmake_minimum_required(VERSION 3.10)

project(ByteOS C ASM CXX)

message(STATUS "==========================")
message(STATUS "     Compiling ByteOS     ")
message(STATUS "==========================")

# Declare Variables
set(LINKER_SCRIPT_DIR "${PROJECT_SOURCE_DIR}/linker")
set(LINKER_FILE "${LINKER_SCRIPT_DIR}/linker-general.ld")

set(CMAKE_SYSTEM_PROCESSOR riscv64)

# Use c11 standard
set(CMAKE_C_STANDARD 11)

set(CMAKE_ASM_COMPILER riscv64-unknown-linux-gnu-gcc)
set(CMAKE_C_COMPILER riscv64-unknown-linux-gnu-gcc)
set(CMAKE_CXX_COMPILER riscv64-unknown-linux-gnu-g++)

# Set compile options
set(CMAKE_ASM_FLAGS "-march=rv64imac -mabi=lp64 -mcmodel=medany")
set(CMAKE_C_FLAGS "-march=rv64imac -mabi=lp64 -nostdlib -O2 -mcmodel=medany")
set(CMAKE_CXX_FLAGS "-march=rv64imac -mabi=lp64 -nostdlib -O2 -mcmodel=medany")
set(CMAKE_EXE_LINKER_FLAGS "-march=rv64imac -mabi=lp64 -mcmodel=medany -T ${LINKER_FILE}")

# For vscode
add_definitions(-DCMAKE_EXPORT_COMPILE_COMMANDS=ON)

# set_source_files_properties(*.asm PROPERTIES LANGUAGE ASM)

# find all source files
file(GLOB_RECURSE ASM_SOURCES "${CMAKE_SOURCE_DIR}/*.S")
file(GLOB_RECURSE C_SOURCES "${CMAKE_SOURCE_DIR}/*.c")
list(FILTER C_SOURCES EXCLUDE REGEX "build/*")

# Set include folders
include_directories(includes arch/riscv/includes)

# add_executable(byteos ${C_SOURCES} ${ASM_SOURCES})
message(STATUS "C_SOURCE_FILES: ${C_SOURCES}")
add_executable(byteos ${ASM_SOURCES} ${C_SOURCES})
# No Stdlib and ignore stdlib headers
target_compile_options(byteos PRIVATE -nostdlib -Wno-builtin-declaration-mismatch)

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
