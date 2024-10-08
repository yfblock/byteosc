#include <aarch64.h>
#include <arch.h>
#include <assert.h>
#include <buddy_alloc.h>
#include <console.h>
#include <driver.h>
#include <msr.h>

void cmain(size_t, uintptr_t);
/**
 * arch specific initialization. Enter the main() after initialization
 * @param hart_id The id of the current hart
 * @param dtb The pointer of the device tree binary.
 *
 */
void boot_main(size_t hart_id, uintptr_t dtb) {
    // Add heap memory to allocator
    add_heap_range((uintptr_t)&heap, HEAP_SIZE);
    dtb_parse_chosen(dtb);

    puts((char *)"Entering Kernel...\n");

    cmain(hart_id, dtb);
    puts((char *)"Run End. Shutdown...\n");
    shutdown();
}

/**
 * Jump to EL1 from EL2
 */
void drop_to_el1() {
    write_sp_el0(0);
    write_spsel(SP_ELx);
    CurrentEL_Reg el = current_el();
    debug("Current EL: %d", el.el);

    // Ensure the Exception Level before droping is 1.
    assert(el.el == 1);
}
