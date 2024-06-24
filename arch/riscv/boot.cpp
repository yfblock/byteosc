#include <console.h>
#include <stddef.h>
#include <stdint.h>

// reimplement new
void* operator new[](size_t size) {
    static int buffer[3];
    return buffer;
}

// reimplement delete
void operator delete(void *ptr, size_t size) {
    
}

extern "C" void boot_main(size_t hart_id, uintptr_t dtb) {
    int *t = new int[3];
    t[0] = '3';
    delete t;

    for(int i = 0;i < 3;i++) 
        console_putchar(t[i]);

    puts((char *)"Hello, world!\n");
    int main(size_t, uintptr_t);
    main(hart_id, dtb);
}
