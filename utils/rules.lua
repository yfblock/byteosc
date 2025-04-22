rule("clang")
    on_load(function(target)
        local arch = get_config("arch")
        local target_flag = "-target " .. arch .. "-linux-gnu"
        target:add(
            "ldflags", 
            "-fuse-ld=lld", 
            target_flag, 
            "-T linker/linker-"..arch..".ld"
        )
        target:add("ldflags", [[
            -static
            -nostdlib
            -nostartfiles
            -ffreestanding
            -fno-stack-protector
        ]])
        target:add("asflags", target_flag)
        target:add("cflags", target_flag)
        target:add("cflags", [[
            -Werror
            -Wundef
            -Wpointer-arith
            -Wno-nonnull
            -O2
            -fno-builtin
            -ffreestanding
            -fno-stack-protector
        ]])

        if arch == "aarch64" then
            target:add("cflags", "-mgeneral-regs-only")
        elseif arch == "riscv64" then
            target:add("cflags", "-mcmodel=medany")
        end
    end)
