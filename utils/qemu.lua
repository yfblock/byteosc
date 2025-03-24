task("qemu")
    -- 设置运行脚本
    on_run(function ()
        import("core.project.config")
        -- task 中需要手动 load config
        config.load()
        local qemu_exec =  "qemu-system-"..config.arch()
        if is_arch("aarch64") then
            qemu_exec = qemu_exec .. " -machine virt -cpu cortex-a72 "
        elseif is_arch("riscv64") then
            qemu_exec = qemu_exec .. " -machine virt "
        end
        print("run qemu "..(config.arch() or "default"))
        os.exec(qemu_exec..[[
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
