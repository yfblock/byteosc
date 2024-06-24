.section .text
.global _start
_start:
    # li      a7, 0x1
    # li      a0, '3'
    # ecall
    la      sp, _boot_stack_top
    la      s0, boot_main
    jalr    s0

# Set Boot Stack
_boot_stack:
    .space 4096 * 16
_boot_stack_top:
