# Reproducing Figure 4

Full reproduction of Figure 4 involves three stages: **Hardware**, **Software**, and **Plotting**.

---

## Stage 1: Hardware

Six FPGA bitstreams are required — one per design. Refer to [Step 5: FPGA Bitstream Generation (VCU118)](https://github.com/bearhw/LIPPEN/blob/main/HARDWARE.md#step-5-fpga-bitstream-generation-vcu118) for setup details, then run the following commands:

```bash
cd chipyard/fpga

# Rocket LIPPEN
make SUB_PROJECT=vcu118 CONFIG=MyRoCCVCU118Config CONFIG_PACKAGE=chipyard.fpga.vcu118 bitstream

# Rocket PAC
make SUB_PROJECT=vcu118 CONFIG=QarmaRoCCVCU118Config CONFIG_PACKAGE=chipyard.fpga.vcu118 bitstream

# BOOM LIPPEN
make SUB_PROJECT=vcu118 CONFIG=LargeBoomPAPrinceVCU118Config CONFIG_PACKAGE=chipyard.fpga.vcu118 bitstream

# BOOM PAC
make SUB_PROJECT=vcu118 CONFIG=LargeBoomPAQarmaVCU118Config CONFIG_PACKAGE=chipyard.fpga.vcu118 bitstream

# BOOM LIPPEN (no RAS)
make SUB_PROJECT=vcu118 CONFIG=LargeBoomPAPrinceVCU118CustomConfig CONFIG_PACKAGE=chipyard.fpga.vcu118 bitstream

# BOOM LIPPEN (no RAS, no BTB)
make SUB_PROJECT=vcu118 CONFIG=LargeBoomPAPrinceVCU118CustomConfig3 CONFIG_PACKAGE=chipyard.fpga.vcu118 bitstream
```

---

## Stage 2: Software

The microbenchmarks need to be compiled once and the resulting RISC-V binaries can be reused across all six designs.

### Return Address Protection Microbenchmarks

**Instrumented binaries:**
1. Run `make` to build the instrumented binaries containing LIPPEN instructions, as described in the [Standard Benchmarks README](https://github.com/bearhw/LIPPEN/tree/main/micro-benchmarks/lippen-riscv#standard-benchmarks).

**Uninstrumented baseline binaries:**

To generate uninstrumented baseline binaries, the source files must be compiled using an unmodified LLVM toolchain. Follow the steps below:
1. Clone LLVM and check out the required version:
```bash
   git clone https://github.com/llvm/llvm-project.git
   cd llvm-project
   git checkout llvmorg-18.1.6
```

2. Build LLVM:
```bash
   mkdir -p build && cd build
   cmake -G Ninja -S ../llvm -B . \
     -DLLVM_ENABLE_PROJECTS="clang;lld" \
     -DLLVM_TARGETS_TO_BUILD="RISCV;X86" \
     -DCMAKE_BUILD_TYPE=Release \
     -DLLVM_ENABLE_ASSERTIONS=ON \
     -DCMAKE_INSTALL_PREFIX="$PWD/install"
   ninja -j$(nproc)
```

3. In the Makefile, update `LLVM_CONFIG` on line 21 to point to the newly built unmodified LLVM.

4. Run `make`.

5. Rename the resulting binaries by appending `_NOPAC` to their filenames — these are the uninstrumented baseline binaries.

### Data Pointer Protection Microbenchmarks

Since LIPPEN instructions are manually inserted, follow the compilation commands in the [Micro-Benchmarks README](../microbenchmarks/README.md). Each program contains both instrumented and uninstrumented code paths.

---

## Stage 3: Plotting

Open `plot.py` and follow the inline comments to insert the collected timing data into the appropriate placeholders. Running the script will generate Figure 4.
