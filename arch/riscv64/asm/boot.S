#include <define.h>
#include <asm/linkage.h>

.section .text
FUNC_START(_start)
    # STORE ARGUMENTS
    mv      s0, a0
    mv      s1, a1

    # CLEAR BSS SECTION
    # ENSURE IT NOT USING STACK
    lla     a0, _sbss
    li      a1, 0
    lla     a2, _ebss
    sub     a2, a2, a0
    jal     memset

    # LOAD BOOT STACK
    lla     sp, _boot_stack_top

    # JUMP TO MAIN ENTRY
    mv      a0, s0
    mv      a1, s1
    lla     s0, boot_main
    jalr    s0
    wfi
FUNC_END(_start)

# Set Boot Stack
.section .bss
DATA_START(_boot_stack)
    .space BOOT_STACK_SIZE
DATA_END(_boot_stack)
SYMBOL(_boot_stack_top)
