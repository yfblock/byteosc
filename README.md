# ByteOS CPP reimplementations

> using cmake

## Prepare

1. `cmake`
2. `make`
3. `ninja`
4. `riscv64-unknown-linux-gnu-gcc`
5. `riscv64-unknown-linux-gnu-g++`
6. `clang-format(optional)`

## How to run

```shell
cmake -B build -S . -G Ninja -DCMAKE_EXPORT_COMPILE_COMMANDS=on
make
make run
```

## Contribute your code

Before you push your code, you must run `make format` first.
