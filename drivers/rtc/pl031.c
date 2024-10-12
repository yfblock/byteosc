#include <console.h>
#include <driver.h>

typedef volatile struct {
    uint32_t data;
} pl031_dri_t;

udevice_t *pl031_init(dtb_node_t *node) {
    dtb_prop_t *reg_prop = dtb_find_prop(node, "reg");
    auto dri = (pl031_dri_t *)dtb_read_prop_cell(reg_prop->first_cell, 2);
    debug("pl031 address %lx", dri);
    debug("pl031 data: %d", dri->data);

    return nullptr;
}

// clang-format off

static const struct udevice_id pl031_ids[] = {
    {.compatible = "arm,pl031"},
    {}
};

DEFINE_DRIVER SERIAL = {
    .ids = pl031_ids,
    .uclass = UCLASS_RTC,
    .probe = pl031_init
};

// clang-format on
