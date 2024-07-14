#pragma once

/**
 *
 */
typedef struct {
    // Interrupt handler.
    void (*interrupt_handler)();
} dri_int_t;
