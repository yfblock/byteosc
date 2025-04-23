cmake_minimum_required(VERSION 3.10)
project(ByteOS C ASM)

message(STATUS "==========================")
message(STATUS "     Compiling ByteOS     ")
message(STATUS "==========================")

# ----------------------------------------------------------------
# Set Kernel Options
# ----------------------------------------------------------------

# Declare Variables
set(CMAKE_SYSTEM_PROCESSOR ${KernelArch})

# Use c23 standard
set(CMAKE_C_STANDARD 23)
set(CMAKE_OBJCOPY llvm-objcopy)

# For vscode
add_definitions(-DCMAKE_EXPORT_COMPILE_COMMANDS=ON)

# Include the CMake modules
include(config.cmake)
include(utils/utils.cmake)
include(utils/compile.cmake)

# find all source files
set(SOURCE_DIRS
    arch/${KernelArch}
    drivers
    kernel
    libs/buddy_alloc
    libs/smoldtb
    libs/std_impl
    libs/lwext4
    libs/elf_parser
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
    libs/lwext4/includes
    libs/elf_parser/includes
    libs/vfs/includes
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
    includes/inttypes.h
    includes/stdio.h
    includes/stdlib.h
    libs/lwext4/lwext4.c
    libs/elf_parser/includes/elf_parser.h
    libs/std_impl/tinyprintf.c
    libs/vfs/includes/vfs.h
    libs/lwext4/fs.c
    kernel/tests.c
)

target_link_libraries(byteos -static)

add_custom_target(byteos_bin
    COMMAND ${CMAKE_OBJCOPY} 
    -O binary $<TARGET_FILE:byteos> 
    ${PROJECT_BINARY_DIR}/byteos.bin
)

include(utils/command.cmake)
