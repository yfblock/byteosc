#pragma once
#include <driver.h>
#include <interrupt.h>

/**
 * Serial Driver Interface
 */
typedef struct {
    // Get a character from the serial.
    char (*getchar)(udevice_t *);
    // Put a character to the serial.
    void (*putchar)(udevice_t *, char);
} serial_dri_t;
