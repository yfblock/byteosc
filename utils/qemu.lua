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
        options = {}
    }
