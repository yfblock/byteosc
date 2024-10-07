#pragma once

#include <driver.h>

/**
 * Real Time Clock Driver Interface
 */
typedef struct {
    uint32_t (*get_time)();
} rtc_dri_t;
