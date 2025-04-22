function check_arch()
    if not is_arch("aarch64", "riscv64") then
        raise("unsupported arch "..get_config("arch"))
    end
    cprint("${green}arch: ${reset}%s", get_config("arch"))
end