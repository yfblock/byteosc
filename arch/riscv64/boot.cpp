#include <arch.h>
#include <assert.h>
#include <console.h>
#include <riscv64.h>
#include <stddef.h>
#include <stdint.h>

int cmain(size_t, uintptr_t);
/**
 * arch specific initialization. Enter the main() after initialization
 * @param hart_id The id of the current hart
 * @param dtb The pointer of the device tree binary.
 */
EXTERN void boot_main(size_t hart_id, uintptr_t dtb) {
    puts((char *)"Entering Kernel...\n");
    // Write the handler of the boot function.
    write_stvec(trap_entry);
    cmain(hart_id, dtb);
    puts((char *)"Run End. Shutdown...\n");
    shutdown();
}
