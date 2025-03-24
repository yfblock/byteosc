#include <assert.h>
#include <buddy_alloc.h>
#include <console.h>
#include <driver.h>
#include <smoldtb.h>
#include <stdio.h>
#include <string.h>

driver_link_t *dri_serial = nullptr;
serial_dri_t  *stdout_ops = nullptr;
udevice_t     *stdout     = nullptr;

/**
 * @param name the driver compatible will be found.
 * @return the driver related to the specified compatible.
 */
driver_t      *find_compatible(const char *name) {
    for_each_compatible(driver, id) {
        if(strcmp(name, id->compatible) == 0) {
            return driver;
        }
    }
    return nullptr;
}

/**
 * Print DTB Nodes for Debugging.
 */
void print_node(dtb_node_t *node, size_t indent) {
    const size_t indent_scale = 2;
    if(node == NULL)
        return;

    char indent_buff[indent + 1];
    for(size_t i = 0; i < indent; i++)
        indent_buff[i] = ' ';
    indent_buff[indent] = 0;

    dtb_node_stat stat;
    dtb_stat_node(node, &stat);
    printf("%s[+] %s: %ld siblings, %ld children, %lx properties.\r\n",
           indent_buff, stat.name, stat.sibling_count, stat.child_count,
           stat.prop_count);

    for(size_t i = 0; i < stat.prop_count; i++) {
        dtb_prop_t *prop = dtb_get_prop(node, i);
        if(prop == NULL)
            break;
        // NOTE: DO NOT DO THIS! This is a hack for testing purposes for I
        // can make print pretty trees and check all properties are read
        // correctly. There's a reason these structs are opaque to calling
        // code, and their underlying definitions can change at any time.
        printf("%s  | %s: %p", indent_buff, prop->name, prop->first_cell);
        // for(int i = 0;i < prop->length;i+=sizeof(*prop->first_cell)) {
        //     printf(" %x", *(prop->first_cell +
        //     i/sizeof(*prop->first_cell)));
        // }
        printf("\r\n");
    }

    dtb_node_t *child = dtb_get_child(node);
    while(child != NULL) {
        print_node(child, indent + indent_scale);
        child = dtb_get_sibling(child);
    }
}

/**
 * Find a driver though the given device tree node.
 * @param node the node will be parsed
 * @return the driver pointer.
 */
driver_t *detect_device(dtb_node_t *node) {
    dtb_prop_t *compatible_prop = dtb_find_prop(node, "compatible");
    if(compatible_prop == nullptr)
        return nullptr;
    // print_node(dtb_child, 0);
    for(int i = 0;; i++) {
        const char *compatible = dtb_read_string(compatible_prop, i);
        // Break if it not any valid props.
        if(compatible == nullptr)
            break;
        // Find driver for the specified compatible name.
        driver_t *driver = find_compatible(compatible);
        if(driver == nullptr)
            continue;
        return driver;
    }
    return nullptr;
}

/**
 * This function will be called recursively.
 * @param rnode The root node will be parsed.
 */
void probe_dtb(dtb_node_t *rnode) {
    dtb_node_t *dtb_child = dtb_get_child(rnode);
    while(dtb_child != nullptr) {
        probe_dtb(dtb_child);
        const driver_t *driver = detect_device(dtb_child);
        // Init driver if it has a valid init function.
        if(driver != nullptr && driver->probe != nullptr) {
            driver->probe(dtb_child);
        }
        dtb_child = dtb_get_sibling(dtb_child);
    }
}

/**
 * put a character to the serial.
 * @param c the char will be puted.
 */
void console_putchar(char c) {
    if(stdout_ops == nullptr)
        return;
    stdout_ops->putchar(stdout, c);
}

/**
 * Parse chosen device from device tree.
 * @param dtb_ptr the device tree will be loaded and parsed.
 */
void dtb_parse_chosen(uintptr_t dtb_ptr) {
    dtb_ops dtb_ops_impl = {
        .malloc = malloc, .free = free_len, .on_error = puts};
    dtb_init(dtb_ptr, dtb_ops_impl);

    dtb_node_t *mnode = dtb_find("/chosen");
    if(mnode == nullptr)
        return;

    dtb_prop_t *prop = dtb_find_prop(mnode, "stdout-path");
    if(prop != nullptr) {
        dtb_node_t *stdout_node = dtb_find_accurate(dtb_read_string(prop, 0));
        if(stdout_node != nullptr) {
            driver_t *driver = detect_device(stdout_node);
            if(driver != nullptr) {
                // Initialize the stdout serial.
                stdout = driver->probe(stdout_node);
                assert(stdout->driver->ops != nullptr);
                stdout_ops = stdout->driver->ops;
            }
        }
    }
}
