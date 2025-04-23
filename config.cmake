#--------------------------------------------------
# Definitions For C AND C++ Code.
# Recommended using CONFIG_XXX as the specification
# for definitions.
# Use as macros for C and C++ code
#--------------------------------------------------

# define log level
# 0: trace 1: debug 2: info 3: warning 4: error 5:off
add_definitions(-DCONFIG_LOG_LEVEL=1)

set(BOOT_STACK_SIZE "0x10000" CACHE STRING "the size of the boot stack")
set(HEAP_SIZE "0x200000" CACHE STRING "the size of the heap")
