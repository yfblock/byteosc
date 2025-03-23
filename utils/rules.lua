rule("arch.aarch64")
    on_load(function (target)
        target:add(
            "cflags",
            "-Werror",
            "-Wundef",
            "-Wpointer-arith",
            "-Wno-nonnull",
            "-O2",
            "-fno-builtin",
            "-ffreestanding",
            
            "-mgeneral-regs-only"
        )

        target:add(
            "ldflags",
            "-T linker/linker-aarch64.ld",
            "-nostdlib",
            "-nostartfiles",
            "-ffreestanding"
        )
    end)