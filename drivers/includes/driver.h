#pragma once
#include <driver-header.h>
#include <serial.h>
#include <smoldtb.h>
#include <stdint.h>

typedef struct _driver_link_t driver_link_t;
struct _driver_link_t {
    driver_link_t *next;
    serial_dri_t *ops;
};

extern driver_link_t *dri_serial;
extern udevice_t *stdout;

extern driver_t __start_drivers[];
extern driver_t __stop_drivers[];
#define DEFINE_DRIVER static __attribute__((used, section("drivers"))) driver_t
#define for_each_compatible(iter, cp_name)                                     \
    for(driver_t *iter = __start_drivers; iter != __stop_drivers; iter++)      \
        for(const struct udevice_id *cp_name = driver->ids;                    \
            cp_name->compatible != nullptr; cp_name++)
driver_t *find_compatible(const char *name);
void probe_dtb(dtb_node_t *rnode);
void print_node(dtb_node_t *node, size_t indent);
void dtb_parse_chosen(uintptr_t dtb_ptr);
