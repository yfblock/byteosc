#include <arch.h>
#include <smoldtb.h>
#include <console.h>
#include <kernel.h>

void print_node(dtb_node* node, size_t indent)
{
    const size_t indent_scale = 2;
    if (node == NULL)
        return;

    char indent_buff[indent + 1];
    for (size_t i = 0; i < indent; i++)
        indent_buff[i] = ' ';
    indent_buff[indent] = 0;
    
    dtb_node_stat stat;
    dtb_stat_node(node, &stat);
    printf("%s[+] %s: %u siblings, %x children, %x properties.\r\n", indent_buff, 
        stat.name, stat.sibling_count, stat.child_count, stat.prop_count);

    for (size_t i = 0; i < stat.prop_count; i++)
    {
        dtb_prop* prop = dtb_get_prop(node, i);
        if (prop == NULL)
            break;
        //NOTE: DO NOT DO THIS! This is a hack for testing purposes for I can make print pretty
        //trees and check all properties are read correctly. There's a reason these structs are
        //opaque to calling code, and their underlying definitions can change at any time.
        const char* name = *(const char**)prop;
        printf("%s  | %s\r\n", indent_buff, name);
    }

    dtb_node* child = dtb_get_child(node);
    while (child != NULL)
    {
        print_node(child, indent + indent_scale);
        child = dtb_get_sibling(child);
    }
}

int main(size_t hart_id, uintptr_t dtb) {
    puts((char *)"Enter Real main function\n");
    printf("Hello %s%c\n", "World", '!');
    printf("root ptr: 0x%x\n", dtb);
    dtb_init(dtb, dtb_ops {
        .on_error = [](const char *str) -> void { puts((char *)str); }
    });

    dtb_node* root = dtb_find("/");
    printf("root ptr: 0x%x\n", root);
    while (root != NULL)
    {
        printf("Node");
        print_node(root, 0);
        root = dtb_get_sibling(root);
    }

    while(true) {}
}

void puts(char *str) {
    for(;*str != '\0';str++) 
        console_putchar(*str);
}
