# [PAPER TITLE] — ISCA 2026 Artifact Evaluation
## Overview
This repository contains the artifact for the ISCA 2025 paper **[PAPER TITLE]**.
The artifact supports both **RTL simulation** and **FPGA deployment on VCU118**.
It is structured as follows:
```
LIPPEN/
├── chipyard/           # Chipyard v1.8.0 (git submodule)
├── llvm-project/       # LLVM llvmorg-18.1.6 (git submodule)
├── llvm_tests/         # Test binaries for LLVM-based evaluation
├── patches/
│   ├── chipyard.patch      # Modifications to Chipyard (configs, RoCC accelerators, vsrc, tests)
│   ├── rocket-chip.patch   # Modifications to rocket-chip (Configs.scala, LazyRoCC.scala)
│   └── llvm.patch          # Modifications to LLVM (RISCV backend, custom passes)
└── README.md
```
---
## Requirements
### Hardware
- **RTL Simulation:** A Linux workstation with at least 16 GB RAM and 100 GB free disk space
- **FPGA:** Xilinx VCU118 board
### Software
- Ubuntu 20.04 or 22.04 (recommended)
- Git ≥ 2.25
- Conda (Miniconda or Anaconda)
- CMake ≥ 3.20 (`sudo apt install cmake`)
- Ninja (`sudo apt install ninja-build`)
- riscv64-linux-gnu cross toolchain (`sudo apt install gcc-riscv64-linux-gnu`)
- Vivado 2021.2 (for FPGA bitstream generation)
### Estimated Setup Time
| Step | Time |
|---|---|
| Clone + patch | ~5 minutes |
| Chipyard submodule init | ~10–20 minutes |
| riscv-tools toolchain build | ~2–4 hours |
| RTL simulation | ~30–60 minutes per test |
| FPGA bitstream generation | ~4–6 hours |
| LLVM submodule init | ~10–20 minutes |
| LLVM build | ~1–2 hours |
---
## Step 1: Clone the Repository
```bash
git clone https://github.com/bearhw/LIPPEN.git
cd LIPPEN
git submodule update --init chipyard
```
---
## Step 2: Initialize Chipyard's Submodules
```bash
cd chipyard
git submodule update --init \
  generators/rocket-chip \
  generators/testchipip \
  generators/sifive-blocks \
  generators/sifive-cache \
  generators/constellation \
  tools/barstools \
  tools/chisel-testers \
  software/firemarshal \
  fpga/fpga-shells
cd ..
```
---
## Step 3: Apply Patches
### 3a. Apply the Chipyard patch
```bash
cd chipyard
git apply ../patches/chipyard.patch
cd ..
```
### 3b. Apply the rocket-chip patch
```bash
cd chipyard/generators/rocket-chip
git apply ../../../patches/rocket-chip.patch
cd ../../..
```
---
## Step 4: Set Up the Toolchain
```bash
cd chipyard
# Install Conda environment and build riscv-tools
./build-setup.sh riscv-tools
# Activate the environment (add to ~/.bashrc for convenience)
source env.sh
cd ..
```
---
## Step 5: RTL Simulation
### 5a. Build the simulator
```bash
cd chipyard/sims/verilator
make CONFIG=MyRoCCSimConfig
cd ../../..
```
### 5b. Compile the test binary
```bash
cd chipyard/tests
make
cd ../..
```
This produces `foo_bar_prince.riscv` in the `tests/` directory.
### 5c. Run the simulation
```bash
cd chipyard/sims/verilator
make CONFIG=MyRoCCSimConfig \
  BINARY="$(pwd)/../../tests/foo_bar_prince.riscv" \
  -j8 run-binary +max-cycles=1000000000
```
---
## Step 6: FPGA Bitstream Generation (VCU118)

### 6a. Build the bitstream

```bash
cd chipyard/fpga
make SUB_PROJECT=vcu118 CONFIG=MyRoCCVCU118Config CONFIG_PACKAGE=chipyard.fpga.vcu118 bitstream
```

> **Estimated time:** 4–6 hours depending on your machine.

### 6b. Locate the bitstream

The generated bitstream will be at:
```
chipyard/fpga/generated-src/chipyard.fpga.vcu118.VCU118FPGATestHarness.MyRoCCVCU118Config/obj/VCU118FPGATestHarness.bit
```

### 6c. Program the VCU118

follow https://chipyard.readthedocs.io/en/latest/Prototyping/VCU118.html


---
## Step 7: Build LLVM

### 7a. Initialize the LLVM submodule

```bash
cd /path/to/LIPPEN
git submodule update --init llvm-project
```

> **Note:** The LLVM repo is ~3 GB. This may take 10–20 minutes depending on your connection.

### 7b. Apply the LLVM patch

```bash
cd llvm-project
git checkout llvmorg-18.1.6
git apply ../patches/llvm.patch
```

### 7c. Configure and build

```bash
mkdir -p build && cd build

cmake -G Ninja -S ../llvm -B . \
  -DLLVM_ENABLE_PROJECTS="clang;lld" \
  -DLLVM_TARGETS_TO_BUILD="RISCV;X86" \
  -DCMAKE_BUILD_TYPE=Release \
  -DLLVM_ENABLE_ASSERTIONS=ON \
  -DCMAKE_INSTALL_PREFIX="$PWD/install"

ninja -j$(nproc)
cd ../..
```

> **Estimated build time:** 1–2 hours. Requires at least 30 GB free disk space and 16 GB RAM.

### 7d. Compile the test binaries

```bash
cd llvm_tests
make LLVM_CONFIG=$(pwd)/../llvm-project/build/bin/llvm-config
```

This produces test binaries in the `llvm_tests/` directory, which can be copied to the FPGA board and run directly on Linux.

---
