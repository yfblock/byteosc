rule("arch.aarch64")
    on_load(function(target)
        target:add("cflags", [[
            -Werror
            -Wundef
            -Wpointer-arith
            -Wno-nonnull
            -O2
            -fno-builtin
            -ffreestanding
            -fno-stack-protector
            -mgeneral-regs-only
            ]])

        target:add("ldflags", [[
            -T linker/linker-aarch64.ld
            -static
            -nostdlib
            -nostartfiles
            -ffreestanding
            -fno-stack-protector
        ]])
    end)

rule("clang")
    on_load(function(target)
        target:add("ldflags", "-fuse-ld=lld", "-target " .. get_config("arch") .. "-linux-gnu")
        target:add("asflags", "-target " .. get_config("arch") .. "-linux-gnu")
        target:add("cflags", "-target " .. get_config("arch") .. "-linux-gnu")
    end)
