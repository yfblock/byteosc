#include <define.h>

_start:
    mrs     x19, mpidr_el1
    and     x19, x19, #0xffffff     // get current CPU id
    mov     x20, x0                 // save DTB pointer
    cbz     x19, 1f

    b       .
1:
    # Set stack pointer register
    adrp    x8, _boot_stack_top
    mov     sp, x8

    # Call Main Function
    mov     x0, x19
    mov     x1, x20
    bl      boot_main

    # Infinite loop
    b       .
# Set Boot Stack
.section .bss
.global _boot_stack
.global _boot_stack_top
_boot_stack:
    .space BOOT_STACK_SIZE
_boot_stack_top:
