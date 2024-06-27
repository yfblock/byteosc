#ifndef __COMMON_H__
#define __COMMON_H__

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

#define compile_assert(name, expr)                                             \
    typedef int __assert_failed_##name[(expr) ? 1 : -1];

#endif
