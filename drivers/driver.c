//
// Created by yfblock on 10/7/24.
//
#include <driver.h>
#include <string.h>
#include <console.h>
#include <smoldtb.h>

driver_link_t *dri_serial = nullptr;

/**
 * @param name the driver compatible will be found.
 * @return the driver related to the specified compatible.
 */
driver_t *find_compatible(const char *name) {
    for_each_compatible(driver, id) {
        if (strcmp(name, id->compatible) == 0) {
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
        // NOTE: DO NOT DO THIS! This is a hack for testing purposes for I can
        // make print pretty trees and check all properties are read correctly.
        // There's a reason these structs are opaque to calling code, and their
        // underlying definitions can change at any time.
        printf("%s  | %s: %p", indent_buff, prop->name, prop->first_cell);
        // for(int i = 0;i < prop->length;i+=sizeof(*prop->first_cell)) {
        //     printf(" %x", *(prop->first_cell + i/sizeof(*prop->first_cell)));
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
 * This function will be called recursively.
 * @param rnode The root node will be parsed.
 */
void probe_dtb(dtb_node_t *rnode) {
    auto dtb_child = dtb_get_child(rnode);
    while (dtb_child != nullptr) {
        probe_dtb(dtb_child);
        dtb_prop_t *compatible_prop = dtb_find_prop(dtb_child, "compatible");
        if (compatible_prop == nullptr) goto end;
        // print_node(dtb_child, 0);
        for (int i = 0;;i++) {
            const char *compatible = dtb_read_string(compatible_prop, i);
            // Break if it not any valid props.
            if (compatible == nullptr) break;
            // Find driver for the specified compatible name.
            const driver_t *driver = find_compatible(compatible);
            if (driver == nullptr) continue;
            // Init driver if it has a valid init function.
            if (driver->probe != nullptr) {
                driver->probe(dtb_child);
            }
        }
    end:
        dtb_child = dtb_get_sibling(dtb_child);
    }
}
