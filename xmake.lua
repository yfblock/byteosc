add_rules("mode.debug", "mode.release")

set_arch("aarch64")
set_options()

target("byteos")
    set_kind("binary")
    set_languages("clatest")

    add_cflags(
        "-Werror",
        "-Wundef",
        "-Wpointer-arith",
        "-Wno-nonnull",
        "-O2",
        "-fno-builtin",
        "-ffreestanding",
        
        "-mgeneral-regs-only"
    )

    add_ldflags(
        "-T",
        "linker/linker-aarch64.ld",
        "-nostdlib",
        "-nostartfiles",
        "-ffreestanding"
    )

    add_options("test", "BOOT_STACK_SIZE", "HEAP_SIZE")

    add_includedirs(
        "includes",
        "drivers/includes",
        "arch/aarch64/includes",
        "$(buildir)/config",
        
        "libs/buddy_alloc",
        "libs/elf_parser/includes",
        "libs/lwext4/includes",
        "libs/smoldtb"
    )

    add_files("arch/aarch64/asm/*.S")
    add_files("arch/aarch64/*.c")
    add_files("kernel/*.c")
    add_files("drivers/*.c")
    add_files("drivers/block/*.c")
    add_files("drivers/rtc/*.c")
    add_files("drivers/serial/*.c")
    add_files("libs/buddy_alloc/*.c")
    add_files("libs/elf_parser/src/*.c")
    add_files("libs/lwext4/src/*.c")
    add_files("libs/smoldtb/*.c")
    add_files("libs/std_impl/*.c")

    
    set_configdir("$(buildir)/config")
    add_configfiles(
        "arch/config-include.h.xmake.in", { filename = "config-include.h"}
    )
    after_build(function(target)
        print(target:targetfile())
        os.exec("llvm-objcopy -O binary "..target:targetfile().." build/byteos.bin")
    end)


option()
    after_check(function(option)
        option:set("configvar", option:name(), option:value(), {quote=false})
    end)

option("arch")
    set_default("aarch64")

option("test")
    set_default(false)
    set_description("Enable debugging mode")

option("BOOT_STACK_SIZE")
    set_description("default size of the boot stack")
    set_default("0x10000")


option("HEAP_SIZE")
    set_description("default size of the heap")
    set_default("0x200000")


task("qemu")

    -- 设置运行脚本
    on_run(function ()
        os.exec([[
            qemu-system-aarch64 -machine virt -cpu cortex-a72 
            -kernel build/byteos.bin
            --nographic

            -D qemu.log
            -d in_asm,int,pcall,cpu_reset,guest_errors
        ]])
    end)

    set_menu {
        usage = "xmake qemu [options]",

        description = "run kernel in the qemu",

        options = {

        }
    }
