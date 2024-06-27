#include <common.h>
#include <console.h>

__attribute__((__noreturn__)) void __assert_fail(const char *assertion,
                                                 const char *file,
                                                 unsigned int line,
                                                 const char *function) {
    error("assertion failed: %s, function: %s \n file:%s:%d", assertion,
          function, file, line);
    shutdown();
    while(true) {
    }
}
