includes("utils/*.lua")
add_rules("mode.debug", "mode.release")
set_policy("check.auto_ignore_flags", false);

-- 设置默认的架构信息
set_plat("cross")
set_config("arch", "aarch64")

target("byteos")
    set_kind("binary")
    set_languages("clatest")
    add_rules("clang")
    set_toolchains("clang")

    on_run(function()
        import("core.base.task")
        print("run qemu "..get_config("arch"))
        task.run("qemu")
    end)

    -- Check the architecture supported
    on_load(check_arch)

    -- Include the architecture specific files
    if get_config("arch") then 
        add_includedirs(string.format("arch/%s/includes", get_config("arch")))
        add_files(string.format("arch/%s/asm/*.S", get_config("arch")))
        add_files(string.format("arch/%s/*.c", get_config("arch")))
    end 

    -- Include the module files
    add_includedirs(
        "includes",
        "drivers/includes",
        "$(buildir)/config",
        "libs/buddy_alloc",
        "libs/elf_parser/includes",
        "libs/lwext4/includes",
        "libs/smoldtb"
    )

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

    -- Generate the header that contains the VARIABLES
    set_configvar("BOOT_STACK_SIZE", get_config("BOOT_STACK_SIZE"))
    set_configvar("HEAP_SIZE", get_config("HEAP_SIZE"))    
    set_configdir("$(buildir)/config")
    add_configfiles("arch/config-include.h.in")
    after_build(function(target)
        print(target:targetfile())
        os.exec("llvm-objcopy -O binary "..target:targetfile().." build/byteos.bin")
    end)
target_end()
