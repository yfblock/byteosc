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

# .global memset
# memset:
#     add     a2, a0, a2
# 0:
#     bge     a0, a2, 1f
#     sb      a1, 0(a0)
#     addi    a0, a0, 1
#     j       0b
# 1:
#     ret


# Set Boot Stack
_boot_stack:
    .space 4096 * 40
_boot_stack_top:
