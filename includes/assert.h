#ifndef _HEADER_ASSERT
#define _HEADER_ASSERT

__attribute__((__noreturn__)) void __assert_fail(const char  *assertion,
                                                 const char  *file,
                                                 unsigned int line,
                                                 const char  *function);
#define assert(expr)                                        \
    if(!(expr))                                             \
        __assert_fail(#expr, __FILE__, __LINE__, __func__);

#endif
