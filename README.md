
This repository is the official artifact for the paper "LIPPEN: A Lightweight In-Place Pointer Encryption
Architecture for Pointer Integrity," Erfan Iravani, Lalit Prasad Peri, Mohannad Ismail, Charitha Tumkur Siddalingaradhya, Changwoo Min, Elif Bilge Kavun, and Wenjie Xiong, ISCA 2026.

## Background
Memory corruption attacks still threaten modern systems through both control-flow hijacking and data-oriented exploitation. Widely used in-place defenses such as ARM Pointer Authentication Codes (PAC) protect pointers without extra memory, but they rely on short authentication codes stored in unused pointer bits. This limits security entropy and leaves them vulnerable to brute-force forgery. Similar entropy limits affect other in-place pointer protection schemes as well.

Our paper presents LIPPEN, a pointer protection architecture that fully encrypts 64-bit pointers with a lightweight block cipher instead of attaching a small authentication tag. This removes the low-entropy weakness of PAC-style designs while preserving a similar software interface and compatibility with existing protection policies. We implement LIPPEN on a RISC-V Rocket Chip FPGA prototype and show that it can provide stronger security with comparable or better performance.

## Overview

This repository contains the artifact for the ISCA 2026 paper "LIPPEN: A Lightweight In-Place Pointer Encryption
Architecture for Pointer Integrity".
The artifact demonstrates how pointer encryption can be implemented in chipyard with an ROCC module, provides the llvm compiler toolchain that automatically instruments programs and protect return addresses, and micor-bench mark c codes to compare the performance against commercial pointer authentication mechanisms.

The artifact is organized into three independent components, each with its own README:

| Component | Description | README |
|---|---|---|
| Chipyard (RISC-V SoC) | RTL simulation and FPGA bitstream on VCU118 | [HARDWARE.md](HARDWARE.md) |
| LLVM (RISC-V backend) | Custom LLVM passes and test binaries | [COMPILER.md](COMPILER.md) |
| Micro-benchmarks | hand-crafted micro-benchmarks on both ARM64 and RISC-V | [micro-benchmarks/m1/README.md](micro-benchmarks/m1/README.md) |

---

## Repository Structure

```
LIPPEN/
├── chipyard/               # Chipyard v1.8.0 (git submodule)
├── llvm-project/           # LLVM llvmorg-18.1.6 (git submodule)
├── micro-benchmarks/             # LLVM test binaries for RISC-V
│   ├── lippen-riscv        # LLVM test binaries for RISC-V
│   └── m1                  # Apple M1 PAC micro-benchmarks
├── pactight/               # PACTight (git submodule)
├── patches/
│   ├── chipyard.patch      # Chipyard modifications
│   ├── rocket-chip.patch   # rocket-chip modifications
│   ├── llvm.patch          # LLVM modifications
│   └── pactight.patch      # PACTight modifications
├── HARDWARE.md
├── COMPILER.md
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

---

## License

This project is licensed under the GNU General Public License v3.0. See the LICENSE file for details.

---

## Citation

@inproceedings{lippen2026isca,

  title     = {LIPPEN: A Lightweight In-Place Pointer Encryption Architecture for Pointer Integrity},
  
  author    = {Iravani, Erfan and Peri, Lalit Prasad and Ismail, Mohannad and Siddalingaradhya, Charitha Tumkur and Min, Changwoo and Kavun, Elif Bilge and Xiong, Wenjie},
  
  booktitle = {Proceedings of the 53rd Annual International Symposium on Computer Architecture},
 
  year      = {2026}

}
