#include <aarch64.h>
#include <arch.h>
#include <assert.h>
#include <console.h>
#include <msr.h>

// Data Register
#define DR 0x00
#define FR 0x18

#define PL011_UART_BASE 0x09000000
#define UART_REG(x) (volatile uint32_t *)(PL011_UART_BASE + (x))

void console_putchar(char c) {
    while((*UART_REG(FR) & (1 << 5)) != 0)
        ;
    *UART_REG(DR) = c;
}

void cmain(size_t, uintptr_t);
/**
 * arch specific initialization. Enter the main() after initialization
 * @param hart_id The id of the current hart
 * @param dtb The pointer of the device tree binary.
 *
 */
void boot_main(size_t hart_id, uintptr_t dtb) {
    puts((char *)"Entering Kernel...\n");
    debug("hart_id: %d  dtb: 0x%x\n", hart_id, dtb);
    // Write the handler of the boot function.

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
    debug("Current EL: %d", el.raw);
    debug("Current EL: %d", el.el);

    // Ensure the Exception Level before droping is 1.
    assert(el.el == 1);
    return;
}
