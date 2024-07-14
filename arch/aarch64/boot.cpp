#include <arch.h>
#include <assert.h>
#include <console.h>

// Data Register
#define DR 0x00
#define FR 0x18

#define PL011_UART_BASE 0x09000000
#define UART_REG(x) (volatile uint32_t *)(PL011_UART_BASE + (x))

void console_putchar(char c) {
    while(*UART_REG(FR) & (1 << 5) != 0)
        ;
    *UART_REG(DR) = c;
}

/**
 * arch specific initialization. Enter the main() after initialization
 * @param hart_id The id of the current hart
 * @param dtb The pointer of the device tree binary.
 */
extern "C" void boot_main(size_t hart_id, uintptr_t dtb) {
    puts((char *)"Entering Kernel...\n");
    debug("hart_id: %d  dtb: 0x%x\n", hart_id, dtb);
    // Write the handler of the boot function.

    int main(size_t, uintptr_t);
    main(hart_id, dtb);
    puts((char *)"Run End. Shutdown...\n");
    shutdown();
}
