# [PAPER TITLE] — ISCA 2025 Artifact Evaluation

## Overview

This repository contains the artifact for the ISCA 2025 paper **[PAPER TITLE]**.
The artifact demonstrates [brief 1-2 sentence description of what the paper does].

The artifact is organized into three independent components, each with its own README:

| Component | Description | README |
|---|---|---|
| Chipyard (RISC-V SoC) | RTL simulation and FPGA bitstream on VCU118 | [CHIPYARD.md](CHIPYARD.md) |
| LLVM (RISC-V backend) | Custom LLVM passes and test binaries | [LLVM.md](LLVM.md) |
| Apple M1 Micro-benchmarks | PAC instruction benchmarks on ARM64 | [micro-benchmarks-m1/README.md](micro-benchmarks-m1/README.md) |

---

## Repository Structure

```
LIPPEN/
├── chipyard/               # Chipyard v1.8.0 (git submodule)
├── llvm-project/           # LLVM llvmorg-18.1.6 (git submodule)
├── llvm_tests/             # LLVM test binaries for RISC-V
├── pactight/               # PACTight (git submodule)
├── micro-benchmarks-m1/    # Apple M1 PAC micro-benchmarks
├── patches/
│   ├── chipyard.patch      # Chipyard modifications
│   ├── rocket-chip.patch   # rocket-chip modifications
│   ├── llvm.patch          # LLVM modifications
│   └── pactight.patch      # PACTight modifications
├── CHIPYARD.md
├── LLVM.md
└── README.md               # This file
```

---

## Quick Start

### Chipyard (RTL Simulation)

```bash
git clone https://github.com/bearhw/LIPPEN.git && cd LIPPEN
git submodule update --init chipyard
cd chipyard && git submodule update --init \
  generators/rocket-chip generators/testchipip generators/sifive-blocks \
  generators/sifive-cache generators/constellation tools/barstools \
  tools/chisel-testers software/firemarshal fpga/fpga-shells && cd ..
cd chipyard && git apply ../patches/chipyard.patch && cd ..
cd chipyard/generators/rocket-chip && git apply ../../../patches/rocket-chip.patch && cd ../../..
cd chipyard && ./build-setup.sh riscv-tools && source env.sh && cd ..
cd chipyard/sims/verilator && make CONFIG=MyRoCCSimConfig && cd ../../..
cd chipyard/tests && make && cd ../..
cd chipyard/sims/verilator
make CONFIG=MyRoCCSimConfig BINARY="$(pwd)/../../tests/foo_bar_prince.riscv" -j8 run-binary +max-cycles=1000000000
```

→ See [CHIPYARD.md](CHIPYARD.md) for full details including FPGA bitstream generation.

---

### LLVM

```bash
git submodule update --init llvm-project
cd llvm-project && git checkout llvmorg-18.1.6 && git apply ../patches/llvm.patch
mkdir -p build && cd build
cmake -G Ninja -S ../llvm -B . \
  -DLLVM_ENABLE_PROJECTS="clang;lld" \
  -DLLVM_TARGETS_TO_BUILD="RISCV;X86" \
  -DCMAKE_BUILD_TYPE=Release \
  -DLLVM_ENABLE_ASSERTIONS=ON \
  -DCMAKE_INSTALL_PREFIX="$PWD/install"
ninja -j$(nproc) && cd ../..
cd llvm_tests && make LLVM_CONFIG=$(pwd)/../llvm-project/build/bin/llvm-config
```

→ See [LLVM.md](LLVM.md) for full details.

---

### Apple M1 Micro-benchmarks

```bash
cd micro-benchmarks-m1
clang -O3 -arch arm64 -mbranch-protection=pac-ret+leaf loop.c -o loop_pac
otool -tvV ./loop_pac
```

→ See [micro-benchmarks-m1/README.md](micro-benchmarks-m1/README.md) for full details.

---

## Requirements Summary

| Component | Hardware | Software |
|---|---|---|
| Chipyard RTL sim | Linux, ≥16 GB RAM, ≥100 GB disk | Ubuntu 20.04/22.04, Conda, Java 11 |
| Chipyard FPGA | Xilinx VCU118 | Vivado 2021.2 |
| LLVM | Linux, ≥16 GB RAM, ≥30 GB disk | CMake ≥ 3.20, Ninja, riscv64-linux-gnu-gcc |
| M1 benchmarks | Apple M1 (ARM64 with PAC) | macOS, Xcode command line tools |

---

## Estimated Time

| Task | Time |
|---|---|
| Chipyard submodule init + patch | ~20 minutes |
| riscv-tools toolchain build | ~2–4 hours |
| RTL simulation | ~30–60 minutes |
| FPGA bitstream generation | ~4–6 hours |
| LLVM build | ~1–2 hours |
| M1 benchmarks compile | ~1 minute |
