#pragma once
#include <driver-header.h>
#include <serial.h>
#include <stdint.h>
#include <smoldtb.h>

// typedef void (*dri_init_func)();

/**
 * The type of the driver.
 */
enum driver_type {
    DRIVER_SERIAL,
};

struct udevice_id {
    // Compatiable for drivers.
    const char *compatible;
};

typedef struct {
    // device ids
    const struct udevice_id *ids;
    // The type of the driver.
    void (*probe)(dtb_node_t *);
} driver_t;

typedef struct _driver_link_t driver_link_t;
struct _driver_link_t {
    driver_link_t *next;
    serial_dri_t *ops;
};

extern driver_link_t *dri_serial;

#define DEFINE_DRIVER static \
    __attribute__((used,section("drivers")))  driver_t
extern driver_t __start_drivers[];
extern driver_t __stop_drivers[];
#define for_each_compatible(iter, cp_name) \
    for(driver_t *iter = __start_drivers; iter != __stop_drivers; iter++) \
        for (const struct udevice_id* cp_name = driver->ids; \
            cp_name->compatible != nullptr; cp_name++)
driver_t *find_compatible(const char *name);
void probe_dtb(dtb_node_t *rnode);
void print_node(dtb_node_t *node, size_t indent);
