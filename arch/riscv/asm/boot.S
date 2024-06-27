#include <define.h>
.section .text
.global _start
_start:
    # li      a7, 0x1
    # li      a0, '3'
    # ecall

    mv      s0, a0
    mv      s1, a1

    la      a0, _sbss
    li      a1, 0
    la      a2, _ebss
    sub     a2, a2, a0

    jal     memset

    mv      a0, s0
    mv      a1, s1
    la      sp, _boot_stack_top
    la      s0, boot_main
    jalr    s0

# Set Boot Stack
.section .bss
.global _boot_stack
.global _boot_stack_top
_boot_stack:
    .space BOOT_STACK_SIZE
_boot_stack_top:
