
// clang-format off
#ifndef ASM_NL
#define ASM_NL ;
#endif

#define GLOBAL(name)                        \
    .global name

#define FUNC_START(name)                    \
    GLOBAL(name)  ASM_NL                    \
    .type name, @function  ASM_NL           \
    name:

#define FUNC_END(name)                      \
    .size name, .- name

#define DATA_START(name)                    \
    GLOBAL(name)  ASM_NL                    \
    .type name, @object  ASM_NL             \
    name:

#define DATA_END(name)                      \
    .size name, .- name

#define SYMBOL(name)                        \
    GLOBAL(name)  ASM_NL                    \
    name:

// clang-format on
