#pragma once
#include <smoldtb.h>

/**
 * The type of the driver.
 */
enum uclass_id {
    UCLASS_BLOCK,
    UCLASS_RTC,
    UCLASS_SERIAL,
    UCLASS_VIRTIO
};

struct udevice_id {
    // Compatible for drivers.
    const char *compatible;
};

typedef struct _udevice_id_t udevice_t;
typedef struct {
    // device ids
    const struct udevice_id *ids;
    // device class
    const enum uclass_id uclass;
    // The type of the driver.
    udevice_t* (*probe)(dtb_node_t *);
    // operations for the driver.
    void *ops;
} driver_t;

struct _udevice_id_t {
    driver_t *driver;
    // The final uclass
    enum uclass_id uclass;
    uintptr_t reg_addr;
};
