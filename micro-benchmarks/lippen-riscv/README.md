# Micro-Benchmarks for RISC-V (FPGA)

These benchmarks are designed to run on an FPGA using our Chipyard design.

## Building the Binaries

### Standard Benchmarks

To compile the RISC-V binaries for the following benchmarks, simply run `make`:

* `loop_l.c`
* `loop_s.c`
* `loop_nested.c`
* `deep_recursive_entry.c`
* `deep_recursive_return.c`

### Data Benchmarks

For `data_loop.c` and `data_unrolled32.c`, compile manually with the following commands:

```bash
riscv64-linux-gnu-gcc -O2 -static -fno-unroll-loops -fno-tree-vectorize -fno-inline -Wall \
    data_loop.c -o data_loop.riscv

riscv64-linux-gnu-gcc -O2 -static -fno-unroll-loops -fno-tree-vectorize -fno-inline -Wall \
    data_unrolled32.c -o data_unrolled32.riscv
```
