#pragma once
#include <aarch64.h>
#include <arch.h>

#define MRS_INLINE(x, var) \
    uint64_t var;          \
    MRS(x, var)

/**
 * CurrentEL Register definition
 */
typedef union {
    uint64_t raw;
    struct {
        uint64_t reserved2 : 2;  // Reserved bits [1: 0]
        uint64_t el        : 2;  // Current Exception Level [3: 2]
        uint64_t reserved  : 60; // Reserved bits [63: 4]
    };
} CurrentEL_Reg;

/**
 * Get the value of the currentEL register
 * link:
 * https://developer.arm.com/documentation/ddi0595/2021-06/AArch64-Registers/CurrentEL--Current-Exception-Level
 */
static inline CurrentEL_Reg current_el() {
    CurrentEL_Reg temp;
    MRS("currentEL", temp);
    return temp;
}

/**
 * Write the value of the SP_EL0 register
 */
static inline void write_sp_el0(size_t value) {
    MSR("sp_el0", value);
}

#define SP_EL0 0
#define SP_ELx 1
/**
 * Write the value of the SPSEL(Stack Pointer Select) register
 * link:
 * https://developer.arm.com/documentation/ddi0595/2021-06/AArch64-Registers/SPSel--Stack-Pointer-Select
 */
static inline void write_spsel(size_t value) {
    MSR("spsel", value);
}

/**
 * CNTHCTL_EL2, Counter-timer Hypervisor Control register
 * linke:
 * https://developer.arm.com/documentation/ddi0595/2021-06/AArch64-Registers/CNTHCTL-EL2--Counter-timer-Hypervisor-Control-register
 */
typedef union {
    uint64_t raw;
    struct {
        bool     EL0PCTEN : 1;
        bool     EL0VCTEN : 1;
        bool     EVNTEN   : 1;
        bool     EVNTDIR  : 1;
        uint64_t reserved : 3;
        bool     EVNTI    : 1;
        bool     EL0VTEN  : 1;
        bool     EL0PTEN  : 1;
        bool     EL1PCTEN : 1;
        bool     EL1PTEN  : 1;
        bool     ECV      : 1;
        bool     EL1TVT   : 1;
        bool     EL1TVCT  : 1;
        bool     EL1NVPCT : 1;
        bool     EL1NVVCT : 1;
        bool     EVNTIS   : 1;
    };
} CNTHCTL_EL2_Reg;

/**
 * Write data to CNTOFF_EL2(Counter-timer Virtual Offset register) register
 * link:
 * https://developer.arm.com/documentation/ddi0595/2021-06/AArch64-Registers/CNTVOFF-EL2--Counter-timer-Virtual-Offset-register
 */
static inline void write_cntoff_el2(size_t value) {
    MRS("CNTOFF_EL2", value);
}

/**
 * Write data to HCR_EL2(Hypervisor Configuration Register) register
 * link:
 * https://developer.arm.com/documentation/ddi0595/2021-06/AArch64-Registers/HCR-EL2--Hypervisor-Configuration-Register
 */
static inline void write_hcr_el2(size_t value) {
    MRS("HCR_EL2", value);
}
