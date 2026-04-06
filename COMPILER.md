# LLVM — Setup, Build, and Test Binaries

This document covers building the modified LLVM (llvmorg-18.1.6) with custom RISC-V backend
passes, and compiling the test binaries in `llvm_tests/`.

---

## Requirements

- Linux workstation with ≥ 16 GB RAM and ≥ 30 GB free disk space
- CMake ≥ 3.20 (`sudo apt install cmake`)
- Ninja (`sudo apt install ninja-build`)
- riscv64-linux-gnu cross toolchain (`sudo apt install gcc-riscv64-linux-gnu`)

---

## Step 1: Initialize the LLVM Submodule

```bash
cd LIPPEN
git submodule update --init llvm-project
```

> The LLVM repo is ~3 GB. This may take 10–20 minutes.

---

## Step 2: Apply the LLVM Patch

```bash
cd llvm-project
git checkout llvmorg-18.1.6
git apply ../patches/llvm.patch
cd ..
```

---

## Step 3: Configure and Build

```bash
cd llvm-project
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

> **Estimated build time:** 1–2 hours depending on core count.

---

## Step 4: Compile the Test Binaries

```bash
cd llvm_tests
make LLVM_CONFIG=$(pwd)/../llvm-project/build/bin/llvm-config
```

This produces the following binaries in `llvm_tests/`:
- `deep_recursive_entry.linux.riscv`
- `deep_recursive_return.linux.riscv`
- `loop.linux.riscv`
- `loop_nested.linux.riscv`
- `foo_bar.linux.riscv`

These are Linux RISC-V ELFs that can be copied to the VCU118 FPGA board and run directly.

---

## Troubleshooting

**Patch does not apply cleanly:**
```bash
cd llvm-project
git apply --check ../patches/llvm.patch
git apply --whitespace=fix ../patches/llvm.patch
```

**ninja build fails with missing `.inc` file:**
This is a build order issue. Make sure `intrinsics_gen` is listed as a dependency in the relevant `CMakeLists.txt`. The provided patch already includes this fix.

**`llvm-config` not found:**
Pass the full path explicitly:
```bash
make LLVM_CONFIG=/absolute/path/to/llvm-project/build/bin/llvm-config
```
