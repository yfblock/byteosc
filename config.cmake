# define log level
# 0: trace 1: debug 2: info 3: warning 4: error 5:off
add_definitions(-DCONFIG_LOG_LEVEL=1)

# HEAP SIZE FOR ALLOC
add_definitions(-DHEAP_SIZE=0x200020)

# SIZE OF BOOT STACK
add_definitions(-DBOOT_STACK_SIZE=0x10000)
