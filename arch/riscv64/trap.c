#include <common.h>
#include <console.h>
#include <riscv64.h>

/**
 * Trap handler.
 */
void trap_handler() {
    trap_exception scause   = (trap_exception)read_scause();
    uintptr_t      addr     = read_stval();
    uintptr_t      fault_ip = read_sepc();
    switch(scause) {
    case TRAP_EXCEPTION_LOAD_ACCESS_FAULT:
        debug("TRAP_EXCEPTION_LOAD_ACCESS_FAULT: 0x%x fault_ip: %x", addr,
              fault_ip);
        break;
    default:
        break;
    }
    debug("trap_handler: %x", scause);
}

/**
 * The entry point for the trap handler
 */
__attribute__((naked, aligned(4))) void trap_entry() {
    // trap_handler();
    __asm__ volatile("sret");
}
