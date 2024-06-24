#include <console.h>
#include <stddef.h>

// reimplement new
void* operator new[](size_t size) {
    static int buffer[3];
    return buffer;
}

// reimplement delete
void operator delete(void *ptr, size_t size) {
    
}

int main() {
    int *t = new int[3];
    t[0] = '3';
    delete t;

    for(int i = 0;i < 3;i++) 
        console_putchar(t[i]);

    puts((char *)"Hello, world!");
    while(true) {}
}
