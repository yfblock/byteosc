#include <common.h>
#include <serial.h>
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

    return *UART_REG(THR);
}

static serial_dri_t US16550A = {
    .getchar = uart_drv_getchar, .putchar = uart_drv_putchar
};

static driver_t SERIAL =  {
    .compatiables = "us16550"
};
