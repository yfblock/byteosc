#include <define.h>
#include <asm/linkage.h>

FUNC_START(_start)
    mrs     x19, mpidr_el1
    and     x19, x19, #0xffffff     // get current CPU id
    mov     x20, x0                 // save DTB pointer
    cbz     x19, .Lnot_primary

    b       .
.Lnot_primary:
    # Set stack pointer register
    adrp    x8, _boot_stack_top
    mov     sp, x8

    bl     drop_to_el1

    # Call Main Function
    mov     x0, x19
    mov     x1, x20
    bl      boot_main

    # Infinite loop
    b       .
FUNC_END(_start)

# Set Boot Stack
.section .bss
DATA_START(_boot_stack)
    .space BOOT_STACK_SIZE
DATA_END(_boot_stack)
SYMBOL(_boot_stack_top)
