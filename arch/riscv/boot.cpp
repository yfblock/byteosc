#include <arch.h>
#include <console.h>
#include <stddef.h>
#include <stdint.h>

extern "C" void boot_main(size_t hart_id, uintptr_t dtb) {
    puts((char *)"Entering Kernel...\n");
    int main(size_t, uintptr_t);
    main(hart_id, dtb);
    puts((char *)"Run End. Shutdown...\n");
    shutdown();
}
