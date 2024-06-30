#pragma once

#define percpu __attribute__((section(".percpu")))

#define percpu_start()
#define percpu_stop()
#define percpu_offset(name)

// TODO: improve percpu implementation
#define percpu_define(T, name, value)                                          \
    T *percpu_##name() {                                                       \
        static percpu T define = value;                                        \
        return &define;                                                        \
    }
