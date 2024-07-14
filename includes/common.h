#pragma once

#include <stddef.h>
#include <stdint.h>
// TIPS: only for syscall number
#include <syscall.h>
// #include <ctype.h>
// #define _ERRNO_H
// #include <bits/errno.h>
// #include <bits/stat.h>
// #include <bits/poll.h>
// #include <bits/fcntl.h>
// #include <bits/time.h>
// #include <bits/time64.h>
// #include <bits/sigaction.h>
// #include <signal.h>
#include <assert.h>

#define STR_HELPER(x) #x
#define STR(x) STR_HELPER(x)
#define CONCAT(x, y) x y

#define NEWLINE(x) CONCAT(x, "\n")
#define CPP_VERSION_STR STR(__cplusplus)

#define BIT(x) (1 << (x))

// TODO: use [[gnu::constructor]] instead of __attribute__((constructor))
#define CTOR __attribute__((constructor))
typedef void (*init_func)();
extern init_func __start_init_array[];
extern init_func __stop_init_array[];

// TODO: implement the CTOR_ITER and use it.
#define for_each_init(iter)                                                    \
    for(init_func *iter = __start_init_array; iter != __stop_init_array; iter++)
