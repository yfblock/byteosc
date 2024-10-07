#include <buddy_alloc.h>
#include <console.h>
#include <driver.h>
// Data Register
#define DR 0x00
#define FR 0x18

#define UART_REG(base, x) (volatile uint32_t *)((base) + (x))

udevice_t* pl011_init(dtb_node_t *node);

void pl011_putchar(udevice_t *device, char c) {
    while((*UART_REG(device->reg_addr, FR) & (1 << 5)) != 0)
        ;
    *UART_REG(device->reg_addr, DR) = c;
}

static const struct udevice_id pl011_ids[] = {
    { .compatible = "arm,pl011" },
    {}
};

static const serial_dri_t pl011_ops = {
    .putchar = pl011_putchar
};

DEFINE_DRIVER SERIAL =  {
    .ids = pl011_ids,
    .uclass = UCLASS_SERIAL,
    .probe = pl011_init,
    .ops =  (void *)&pl011_ops
};

udevice_t* pl011_init(dtb_node_t *node) {
    dtb_prop_t *reg_prop = dtb_find_prop(node, "reg");
    udevice_t *device = calloc(1, sizeof(udevice_t));
    device->driver = &SERIAL;
    device->reg_addr = dtb_read_prop_cell(reg_prop->first_cell, 2);
    device->uclass = SERIAL.uclass;
    return device;
}
