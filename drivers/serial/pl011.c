#include <../includes/driver.h>
#include <../../includes/console.h>

// DEFINE_COMPATIBLE PL011_DRI = {
//     .compatiables = "arm,pl011"
// };

void pl011_init(dtb_node_t *node) {
    dtb_prop_t *reg_prop = dtb_find_prop(node, "reg");
    debug("pl011 address %lx", dtb_read_prop_cell(reg_prop->first_cell, 2));

}

static const struct udevice_id pl011_ids[] = {
    { .compatible = "arm,pl011" },
    {}
};

DEFINE_DRIVER SERIAL =  {
    .ids = pl011_ids,
    .probe = pl011_init
};
