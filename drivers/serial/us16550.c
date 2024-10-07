#include <../../includes/common.h>
#include <../../includes/console.h>
#include <../includes/driver.h>
#include <stdint.h>

#define THR 0x0
#define LSR 0x5

#define LSR_THRE BIT(5)
#define LSR_RDR BIT(0)

#define UART_REG(x) ((volatile uint8_t *)(0x10000000 + (x)))

/**
 * write a character to serial.
 */
void uart_drv_putchar(char c) {
    while((*UART_REG(LSR) & LSR_THRE) == 0)
        ;

    *UART_REG(THR) = c;
}

/**
 * read a character from serial.
 */
char uart_drv_getchar(void) {
    while((*UART_REG(LSR) & LSR_RDR) == 0)
        ;

    return (char)*UART_REG(THR);
}

void uart16550_init(dtb_node_t *node) {
    dtb_prop_t *reg_prop = dtb_find_prop(node, "reg");
    debug("address %lx", dtb_read_prop_cell(reg_prop->first_cell, 2));
}

static serial_dri_t US16550A = {
    .getchar = uart_drv_getchar, .putchar = uart_drv_putchar
};

static const struct udevice_id us16550_ids[] = {
    { .compatible = "us16550" },
    {}
};

DEFINE_DRIVER SERIAL =  {
    .ids = us16550_ids,
    .probe = uart16550_init
};
