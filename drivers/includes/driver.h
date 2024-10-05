#pragma once

/**
 * The type of the driver.
 */
enum driver_type {
    DRIVER_SERIAL,
};

typedef struct {
    // Compatiable for drivers.
    const char *compatiables;
    // The type of the driver.
} driver_t;
