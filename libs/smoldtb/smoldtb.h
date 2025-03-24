#pragma once
#include <stddef.h>
#include <stdint.h>

typedef struct _dtb_node_t dtb_node_t;
typedef struct _dtb_prop_t dtb_prop_t;

/* Similar to nodes, properties are stored a singly linked list. */
struct _dtb_prop_t {
    const char     *name;
    const uint32_t *first_cell;
    size_t          length;
    dtb_prop_t     *next;
};

/* The tree is represented in horizontal slices, where all child nodes are
 * represented in a singly-linked list. Only a pointer to the first child is
 * stored in the parent, and the list is build using the node->sibling pointer.
 * For reference the pointer building the tree are:
 * - parent: go up one level
 * - sibling: the next node on this level. To access the previous node, access
 * the parent and then the child pointer and iterate to just before the target.
 * - child: the first child node.
 */
struct _dtb_node_t {
    dtb_node_t *parent;
    dtb_node_t *sibling;
    dtb_node_t *child;
    dtb_prop_t *props;

    const char *name;
    uint8_t     addr_cells;
    uint8_t     size_cells;
};

typedef struct {
    size_t a;
    size_t b;
} dtb_pair;

typedef struct {
    size_t a;
    size_t b;
    size_t c;
} dtb_triplet;

typedef struct {
    size_t a;
    size_t b;
    size_t c;
    size_t d;
} dtb_quad;

typedef struct {
    void *(*malloc)(size_t length);
    void  (*free)(void *ptr, size_t length);
    void  (*on_error)(const char *why);
} dtb_ops;

typedef struct {
    const char *name;
    size_t      child_count;
    size_t      prop_count;
    size_t      sibling_count;
} dtb_node_stat;

void        dtb_init(uintptr_t start, dtb_ops ops);

dtb_node_t *dtb_find_compatible(dtb_node_t *node, const char *str);
dtb_node_t *dtb_find_phandle(unsigned handle);
dtb_node_t *dtb_find(const char *path);
dtb_node_t *dtb_find_child(dtb_node_t *node, const char *name);
dtb_prop_t *dtb_find_prop(dtb_node_t *node, const char *name);

dtb_node_t *dtb_get_sibling(dtb_node_t *node);
dtb_node_t *dtb_get_child(dtb_node_t *node);
dtb_node_t *dtb_get_parent(dtb_node_t *node);
dtb_prop_t *dtb_get_prop(dtb_node_t *node, size_t index);
void        dtb_stat_node(dtb_node_t *node, dtb_node_stat *stat);

const char *dtb_read_string(dtb_prop_t *prop, size_t index);
size_t      dtb_read_prop_cell(const uint32_t *cells, size_t count);
size_t dtb_read_prop_values(dtb_prop_t *prop, size_t cell_count, size_t *vals);
size_t dtb_read_prop_pairs(dtb_prop_t *prop, dtb_pair layout, dtb_pair *vals);
size_t dtb_read_prop_triplets(dtb_prop_t *prop, dtb_triplet layout,
                              dtb_triplet *vals);
size_t dtb_read_prop_quads(dtb_prop_t *prop, dtb_quad layout, dtb_quad *vals);
dtb_node_t *dtb_find_accurate(const char *name);
