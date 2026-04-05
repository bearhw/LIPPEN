# [PAPER TITLE] — ISCA 2025 Artifact Evaluation

## Overview

This repository contains the artifact for the ISCA 2025 paper **[PAPER TITLE]**.
The artifact supports both **RTL simulation** and **FPGA deployment on VCU118**.

It is structured as follows:

```
LIPPEN/
├── chipyard/           # Chipyard v1.8.0 (git submodule)
├── patches/
│   ├── chipyard.patch      # Modifications to Chipyard (configs, RoCC accelerators, vsrc, tests)
│   └── rocket-chip.patch   # Modifications to rocket-chip (Configs.scala, LazyRoCC.scala)
└── README.md
```

---

## Requirements

### Hardware
- **RTL Simulation:** A Linux workstation with at least 16 GB RAM and 100 GB free disk space
- **FPGA:** Xilinx VCU118 board with Vivado 2021.1 or later installed

### Software
- Ubuntu 20.04 or 22.04 (recommended)
- Git ≥ 2.25
- Conda (Miniconda or Anaconda)
- Java 11 (`sudo apt install default-jdk`)
- Vivado 2021.1+ (for FPGA only)

### Estimated Setup Time
| Step | Time |
|---|---|
| Clone + patch | ~5 minutes |
| Chipyard submodule init | ~10–20 minutes |
| riscv-tools toolchain build | ~2–4 hours |
| RTL simulation | ~30–60 minutes per test |
| FPGA bitstream generation | ~4–6 hours |

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

### Verify patches applied cleanly

```bash
ls chipyard/generators/chipyard/src/main/scala/rocc/
ls chipyard/generators/chipyard/src/main/resources/vsrc/
ls chipyard/tests/
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
```

### 5b. Compile the test binary

```bash
cd chipyard/tests
make
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
