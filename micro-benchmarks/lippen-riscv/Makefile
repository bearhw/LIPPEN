##### Makefile (Linux-only) for Machine PASS on RISC-V
# Builds a Linux user-space ELF using riscv64-linux-gnu toolchain and your LLVM/Clang.
# No libgloss, no newlib, no freestanding.
#
# Usage:
#   make clean
#   make              # builds $(PROGRAMS).linux.riscv
#
# Run on FPGA Linux:
#   scp test.linux.riscv <board>:/tmp/test
#   ssh <board> "chmod +x /tmp/test && /tmp/test [iters] [trials]"

-include config.mk

.SUFFIXES:
MAKEFLAGS += -rR

# =============================
# LLVM tools (use your LLVM build)
# =============================
LLVM_CONFIG ?= /home/pac/llvm-project/build/bin/llvm-config
LLVM_BINDIR := $(shell $(LLVM_CONFIG) --bindir)
CLANG := $(LLVM_BINDIR)/clang
OPT   := $(LLVM_BINDIR)/opt
LLC   := $(LLVM_BINDIR)/llc

# =============================
# Linux RISC-V toolchain
# =============================
RISCV_GCC     ?= /usr/bin/riscv64-linux-gnu-gcc
GCC           ?= $(RISCV_GCC)
RISCV_TRIPLE  ?= riscv64-linux-gnu
RISCV_ARCH    ?= rv64gc
RISCV_ABI     ?= lp64d

# Debian/Ubuntu cross toolchains often report sysroot as "/" (multiarch layout).
# Prefer --gcc-toolchain over --sysroot in that case.
RISCV_PREFIX  ?= $(shell dirname $(shell dirname $(shell $(RISCV_GCC) -print-libgcc-file-name)))

OBJDUMP       ?= riscv64-linux-gnu-objdump

# =============================
# Flags (Linux user-space)
# =============================
CFLAGS  ?= -std=gnu99 -O2 -Wall -Wextra
CFLAGS  += -march=$(RISCV_ARCH) -mabi=$(RISCV_ABI)
# Helps keep call/ret structure stable for microbenchmarks
CFLAGS  += -fno-omit-frame-pointer

# Add include paths / defines if needed (e.g., -DPRINCE_OPCODE=1)
CPPFLAGS ?=

# Link flags:
# - Default: dynamic (recommended on Linux rootfs)
# - For static: make LDFLAGS=-static (may require static libc availability)
LDFLAGS += -static  

# =============================
# Apps / sources
# =============================
PROGRAMS ?= deep_recursive_entry deep_recursive_return loop_s loop_l foo_bar loop_nested
APPS := $(PROGRAMS)

.SECONDARY:
.PHONY: all clean asm obj

all: $(addsuffix .linux.riscv,$(APPS))

# =============================
# Link final Linux ELF
# =============================
%.linux.riscv: %.riscv.o pac_runtime.o
	@echo "=== [linux] Linking $@ ==="
	$(GCC) $(LDFLAGS) -march=$(RISCV_ARCH) -mabi=$(RISCV_ABI) \
		$^ -o $@

# =============================
# Compile C -> object with Clang targeting Linux
# =============================
%.riscv.o: %.c
	@echo "=== [linux] Compiling $< ==="
	$(CLANG) -O2 -c \
	         -target $(RISCV_TRIPLE) \
	         -march=$(RISCV_ARCH) -mabi=$(RISCV_ABI) \
	         --gcc-toolchain=$(RISCV_PREFIX) \
	         $(CPPFLAGS) $(CFLAGS) \
	         $< -o $@
	@echo "=== [linux] Disassembling $@ (leaf snippet) ==="
	@$(OBJDUMP) -d $@ | sed -n '/<leaf>:/,+40p' || true

# Assembly dump (optional)
%.riscv.s: %.c
	@echo "=== [linux] Compiling $< to assembly ==="
	$(CLANG) -O2 -S \
	         -target $(RISCV_TRIPLE) \
	         -march=$(RISCV_ARCH) -mabi=$(RISCV_ABI) \
	         --gcc-toolchain=$(RISCV_PREFIX) \
	         $(CPPFLAGS) $(CFLAGS) \
	         $< -o $@
	@echo "=== [linux] Checking for key patterns in $@ ==="
	@grep -nE 'rocc_prince_sign|rocc_prince_auth|\bret\b|jalr' $@ || true

asm: $(addsuffix .riscv.s,$(APPS))
obj: $(addsuffix .riscv.o,$(APPS))

# =============================
# ROCC / PAC runtime (Linux-safe only)
# =============================
# IMPORTANT:
# pac_runtime.c must be Linux-safe (no libgloss/newlib startup, no baremetal-only syscalls).
# It should ideally just provide inline asm wrappers / helpers.
pac_runtime.o: pac_runtime.c pac_runtime.h
	@echo "=== [linux] Compiling pac_runtime.c ==="
	$(CLANG) -O2 -c \
	         -target $(RISCV_TRIPLE) \
	         -march=$(RISCV_ARCH) -mabi=$(RISCV_ABI) \
	         --gcc-toolchain=$(RISCV_PREFIX) \
	         $(CPPFLAGS) $(CFLAGS) \
	         $< -o $@

clean:
	rm -f *.o *.s *.ll *.mir
	rm -f $(addsuffix .riscv.o,$(APPS)) $(addsuffix .riscv.s,$(APPS))
	rm -f $(addsuffix .linux.riscv,$(APPS))

