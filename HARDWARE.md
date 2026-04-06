# Chipyard — Setup, RTL Simulation, and FPGA

This document covers setting up Chipyard v1.8.0 with the paper's modifications,
running RTL simulation, and generating an FPGA bitstream for the Xilinx VCU118.

---

## Requirements

### Hardware
- Linux workstation with ≥ 16 GB RAM and ≥ 100 GB free disk space
- Xilinx VCU118 board (for FPGA only)

### Software
- Ubuntu 20.04 or 22.04
- Git ≥ 2.25
- Conda (Miniconda or Anaconda)
- Java 11 (`sudo apt install default-jdk`)
- Vivado 2021.2 (for FPGA only)

---

## Step 1: Clone and Initialize Chipyard

```bash
cd LIPPEN
git submodule update --init chipyard
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

## Step 2: Apply Patches

```bash
# Chipyard patch
cd chipyard
git apply ../patches/chipyard.patch
cd ..

# rocket-chip patch
cd chipyard/generators/rocket-chip
git apply ../../../patches/rocket-chip.patch
cd ../../..
```


---

## Step 3: Set Up the Toolchain

```bash
cd chipyard
./build-setup.sh riscv-tools
source env.sh
cd ..
```

---

## Step 4: RTL Simulation

### 4a. Build the simulator

```bash
cd chipyard/sims/verilator
make CONFIG=MyRoCCSimConfig
cd ../../..
```

RoCC design can be found under chipyard/generators/chipyard/src/main/scala/rocc/PAPrinceV2.scala

### 4b. Compile the test binary

```bash
cd chipyard/tests
make
cd ../..
```

This produces `foo_bar_prince.riscv` in the `tests/` directory.

### 4c. Run the simulation

```bash
cd chipyard/sims/verilator
make CONFIG=MyRoCCSimConfig \
  BINARY="$(pwd)/../../tests/foo_bar_prince.riscv" \
  -j8 run-binary +max-cycles=1000000000
```

---

## Step 5: FPGA Bitstream Generation (VCU118)


### 5a. Build the bitstream

```bash
cd chipyard/fpga
make SUB_PROJECT=vcu118 CONFIG=MyRoCCVCU118Config CONFIG_PACKAGE=chipyard.fpga.vcu118 bitstream
```

> **Estimated time:** 4–6 hours.

### 5b. Locate the bitstream

```
chipyard/fpga/generated-src/chipyard.fpga.vcu118.VCU118FPGATestHarness.MyRoCCVCU118Config/obj/VCU118FPGATestHarness.bit
```

### 5c. Program the VCU118

follow https://chipyard.readthedocs.io/en/latest/Prototyping/VCU118.html


---

## Troubleshooting

**Patch does not apply cleanly:**
```bash
git apply --check ../patches/chipyard.patch
git apply --whitespace=fix ../patches/chipyard.patch
```

**Submodule init fails:**
```bash
git submodule update --init --depth 1 generators/rocket-chip
```

**`build-setup.sh` fails mid-way:**
Re-running is safe — it resumes where it left off. Ensure ≥ 50 GB free disk space.

**Vivado license error:**
Ensure your license covers Virtex UltraScale+. Set `LM_LICENSE_FILE` for floating licenses.
