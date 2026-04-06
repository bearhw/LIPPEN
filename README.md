# [PAPER TITLE] — ISCA 2026 Artifact Evaluation

## Overview

This repository contains the artifact for the ISCA 2026 paper **[PAPER TITLE]**.
The artifact demonstrates how pointer encryption can be implemented in chipyard with an ROCC module, provides the llvm compiler toolchain that automatically instruments programs and protect return addresses, and micor-bench mark c codes to compare the performance against commercial pointer authentication mechanisms.

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
