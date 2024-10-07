#include <buddy_alloc.h>
#include <common.h>
#include <console.h>
#include <driver.h>
#include <stdint.h>

#define THR 0x0
#define LSR 0x5

#define LSR_THRE BIT(5)
#define LSR_RDR BIT(0)

#define UART_REG(base, x) ((volatile uint8_t *)((base) + (x)))

/**
 * write a character to serial.
 */
void uart_drv_putchar(udevice_t *device, char c) {
    while((*UART_REG(device->reg_addr, LSR) & LSR_THRE) == 0)
        ;

    *UART_REG(device->reg_addr, THR) = c;
}

/**
 * read a character from serial.
 */
char uart_drv_getchar(udevice_t *device) {
    while((*UART_REG(device->reg_addr, LSR) & LSR_RDR) == 0)
        ;

    return (char)*UART_REG(device->reg_addr, THR);
}

static serial_dri_t US16550A = {
    .getchar = uart_drv_getchar, .putchar = uart_drv_putchar
};

static const struct udevice_id us16550_ids[] = {
    { .compatible = "ns16550a" },
    {}
};

static const serial_dri_t uart16550_ops = {
    .putchar = uart_drv_putchar,
    .getchar = uart_drv_getchar
};

udevice_t* uart16550_init(dtb_node_t *node);
DEFINE_DRIVER SERIAL =  {
    .ids = us16550_ids,
    .uclass = UCLASS_SERIAL,
    .probe = uart16550_init,
    .ops = (void *)&uart16550_ops
};

udevice_t* uart16550_init(dtb_node_t *node) {
    dtb_prop_t *reg_prop = dtb_find_prop(node, "reg");
    udevice_t *device = calloc(1, sizeof(udevice_t));
    device->driver = &SERIAL;
    device->reg_addr = dtb_read_prop_cell(reg_prop->first_cell, 2);
    device->uclass = SERIAL.uclass;
    return device;
}
