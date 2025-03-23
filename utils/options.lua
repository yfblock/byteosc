option()
    after_check(function(option)
        option:set("configvar", option:name(), option:value(), {quote=false})
    end)

option("arch")
    set_default("aarch64")
    set_values("aarch64", "riscv64")
    on_check(function(option)
        if not option:value() then
            option:set("aarch64")
        end
        print("option check arch: ", option:value())
    end)

option("test")
    set_default(false)
    set_description("Enable debugging mode")

option("BOOT_STACK_SIZE")
    set_description("default size of the boot stack")
    set_default("0x10000")


option("HEAP_SIZE")
    set_description("default size of the heap")
    set_default("0x200000")
