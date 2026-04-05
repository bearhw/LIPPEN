# ##### Makefile for Mahcine PASS
# -include config.mk

# # Disable built-in suffix/implicit rules so host 'as' isn't used accidentally.
# .SUFFIXES:
# MAKEFLAGS += -rR

# # =============================
# # LLVM tools (use your LLVM build)
# # =============================
# LLVM_CONFIG ?= /home/pac/llvm-project/build/bin/llvm-config
# LLVM_BINDIR := $(shell $(LLVM_CONFIG) --bindir)
# CLANG := $(LLVM_BINDIR)/clang
# OPT   := $(LLVM_BINDIR)/opt
# LLC   := $(LLVM_BINDIR)/llc

# # =============================
# # RISC-V GCC toolchain (for linking/runtime)
# # =============================
# RISCV_GCC     ?= /home/pac/Desktop/lalit/rocket-modified/chipyard/.conda-env/riscv-tools/bin/riscv64-unknown-elf-gcc
# GCC           ?= $(RISCV_GCC)
# RISCV_TRIPLE  ?= riscv64-unknown-elf
# RISCV_ARCH    ?= rv64gc
# RISCV_ABI     ?= lp64d
# RISCV_MATTR   ?= +m,+a,+f,+d,+c
# RISCV_SYSROOT ?= $(shell $(RISCV_GCC) --print-sysroot)
# RISCV_PREFIX  ?= $(shell dirname $(shell dirname $(shell $(RISCV_GCC) -print-libgcc-file-name)))
# OBJDUMP       ?= $(RISCV_TRIPLE)-objdump

# # Core compile/link flags
# CFLAGS  ?= -std=gnu99 -O2 -Wall
# CFLAGS  += -fno-common -fno-builtin -ffreestanding -mcmodel=medany
# LDFLAGS ?= -static

# # =============================
# # Chipyard libgloss (crt0/specs/linker)
# # =============================
# LIBGLOSS_DIR := /home/pac/Desktop/lalit/rocket-modified/chipyard/tests
# include $(LIBGLOSS_DIR)/libgloss.mk

# LIBGLOSS_LINK_FLAGS :=
# ifdef libgloss_ldflags
#   LIBGLOSS_LINK_FLAGS += $(libgloss_ldflags)
# endif
# ifdef libgloss_specs
#   LIBGLOSS_LINK_FLAGS += --specs=$(libgloss_specs)
# endif
# ifdef libgloss_linker_script
#   LIBGLOSS_LINK_FLAGS += -T $(libgloss_linker_script)
# endif
# # De-dup specs if libgloss already injected them
# LIBGLOSS_LINK_FLAGS_NO_SPECS := $(filter-out -specs=% --specs=%,$(LIBGLOSS_LINK_FLAGS))

# # =============================
# # Apps / sources
# # =============================
# PROGRAMS ?= test
# APPS := $(PROGRAMS)

# # Keep intermediates to inspect IR/MIR/obj
# .SECONDARY:

# .PHONY: all clean distclean riscv-llvm asm obj ll mir

# # =============================
# # Top-level targets
# # =============================
# all: riscv-llvm
# riscv-llvm: $(addsuffix .llvm.riscv,$(APPS))

# # Final ELF (link with GCC + libgloss)
# %.llvm.riscv: %.riscv.o pac_runtime.o $(libgloss)
# 	@echo "=== [riscv-llvm] Linking $@ (GCC + libgloss) ==="
# 	$(GCC) $(LDFLAGS) -march=$(RISCV_ARCH) -mabi=$(RISCV_ABI) -mcmodel=medany \
# 		$^ -o $@ $(LIBGLOSS_LINK_FLAGS_NO_SPECS)

# # =============================
# # Direct compilation path (simpler)
# # =============================

# # Generate object file directly from C source
# # This avoids the multi-step IR->OPT->MIR flow and ensures single pass through backend
# %.riscv.o: %.c
# 	@echo "=== [riscv-llvm] Compiling $< directly to object ==="
# 	$(CLANG) -O2 -c \
# 	         -target $(RISCV_TRIPLE) -march=$(RISCV_ARCH) -mabi=$(RISCV_ABI) \
# 	         --gcc-toolchain=$(RISCV_PREFIX) --sysroot=$(RISCV_SYSROOT) \
# 	         -ffreestanding -fno-builtin -fno-common -mcmodel=medany \
# 	         -I$(RISCV_SYSROOT)/include -I$(RISCV_SYSROOT)/include/newlib \
# 	         $< -o $@
# 	@echo "=== [riscv-llvm] Disassembling $@ ==="
# 	@$(OBJDUMP) -d $@ | grep -A 40 "<foo>:" || true

# # Alternative: Generate assembly to inspect
# %.riscv.s: %.c
# 	@echo "=== [riscv-llvm] Compiling $< to assembly ==="
# 	$(CLANG) -O2 -S \
# 	         -target $(RISCV_TRIPLE) -march=$(RISCV_ARCH) -mabi=$(RISCV_ABI) \
# 	         --gcc-toolchain=$(RISCV_PREFIX) --sysroot=$(RISCV_SYSROOT) \
# 	         -ffreestanding -fno-builtin -fno-common -mcmodel=medany \
# 	         -I$(RISCV_SYSROOT)/include -I$(RISCV_SYSROOT)/include/newlib \
# 	         $< -o $@
# 	@echo "=== [riscv-llvm] Checking for PAC instructions in $@ ==="
# 	@grep -nE 'rocc_prince_sign|rocc_prince_auth|\bret\b|jalr' $@ || true

# asm: $(addsuffix .riscv.s,$(APPS))
# obj: $(addsuffix .riscv.o,$(APPS))

# # =============================
# # Optional: IR inspection targets
# # =============================

# # Generate LLVM IR
# %.riscv.ll: %.c
# 	@echo "=== [riscv-llvm] Generating LLVM IR: $< ==="
# 	$(CLANG) -O2 -S -emit-llvm \
# 	         -target $(RISCV_TRIPLE) -march=$(RISCV_ARCH) -mabi=$(RISCV_ABI) \
# 	         --gcc-toolchain=$(RISCV_PREFIX) --sysroot=$(RISCV_SYSROOT) \
# 	         -ffreestanding -fno-builtin -fno-common -mcmodel=medany \
# 	         -I$(RISCV_SYSROOT)/include -I$(RISCV_SYSROOT)/include/newlib \
# 	         $< -o $@

# ll: $(addsuffix .riscv.ll,$(APPS))

# # =============================
# # ROCC runtime
# # =============================
# pac_runtime.o: pac_runtime.c pac_runtime.h
# 	$(GCC) $(CFLAGS) -c $< -o $@

# # =============================
# # Housekeeping
# # =============================
# clean:
# 	rm -f *.o *.s *.ll *.mir
# 	rm -f $(addsuffix .riscv.ll,$(APPS)) \
# 	      $(addsuffix .riscv.s,$(APPS)) \
# 	      $(addsuffix .riscv.o,$(APPS))
# 	rm -f $(addsuffix .llvm.riscv,$(APPS))
# 	$(if $(libgloss),rm -rf $(libgloss_builddir)/)

# distclean: clean
# 	@true



	





































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
PROGRAMS ?= test
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












































##### Makefile for Verilator with IR and MIR
# -include config.mk

# # Disable built-in suffix/implicit rules so host 'as' isn't used accidentally.
# .SUFFIXES:
# MAKEFLAGS += -rR

# # =============================
# # LLVM tools (use your LLVM build)
# # =============================
# LLVM_CONFIG ?= /home/pac/llvm-project/build/bin/llvm-config
# LLVM_BINDIR := $(shell $(LLVM_CONFIG) --bindir)
# CLANG := $(LLVM_BINDIR)/clang
# OPT   := $(LLVM_BINDIR)/opt
# LLC   := $(LLVM_BINDIR)/llc

# # =============================
# # Pass plugins (.so)
# # =============================
# # IR pass (exports pipeline key: rocc-pac)
# PAC_PASS_PLUGIN ?= /home/pac/llvm-project/build/lib/RoccPacReturnPass.so
# PAC_PASSES      ?= rocc-pac
# # PAC_PASS_PLUGIN ?= /home/pac/llvm-project/build/lib/RoccPacReturnLitePass.so
# # PAC_PASSES      ?= rocc-pac-lite

# # Your MIR pass is compiled in-tree into llc and registered as 'mir-set-ra'
# MIR_SINGLE_PASS ?= mir-set-ra

# # =============================
# # RISC-V GCC toolchain (for linking/runtime)
# # =============================
# RISCV_GCC     ?= /home/pac/Desktop/lalit/rocket-modified/chipyard/.conda-env/riscv-tools/bin/riscv64-unknown-elf-gcc
# GCC           ?= $(RISCV_GCC)
# RISCV_TRIPLE  ?= riscv64-unknown-elf
# RISCV_ARCH    ?= rv64gc
# RISCV_ABI     ?= lp64d
# RISCV_MATTR   ?= +m,+a,+f,+d,+c
# RISCV_SYSROOT ?= $(shell $(RISCV_GCC) --print-sysroot)
# RISCV_PREFIX  ?= $(shell dirname $(shell dirname $(shell $(RISCV_GCC) -print-libgcc-file-name)))
# OBJDUMP       ?= $(RISCV_TRIPLE)-objdump

# # Core compile/link flags
# CFLAGS  ?= -std=gnu99 -O1 -Wall
# CFLAGS  += -fno-common -fno-builtin -ffreestanding -mcmodel=medany
# LDFLAGS ?= -static

# # =============================
# # Chipyard libgloss (crt0/specs/linker)
# # =============================
# LIBGLOSS_DIR := /home/pac/Desktop/lalit/rocket-modified/chipyard/tests
# include $(LIBGLOSS_DIR)/libgloss.mk

# LIBGLOSS_LINK_FLAGS :=
# ifdef libgloss_ldflags
#   LIBGLOSS_LINK_FLAGS += $(libgloss_ldflags)
# endif
# ifdef libgloss_specs
#   LIBGLOSS_LINK_FLAGS += --specs=$(libgloss_specs)
# endif
# ifdef libgloss_linker_script
#   LIBGLOSS_LINK_FLAGS += -T $(libgloss_linker_script)
# endif
# # De-dup specs if libgloss already injected them
# LIBGLOSS_LINK_FLAGS_NO_SPECS := $(filter-out -specs=% --specs=%,$(LIBGLOSS_LINK_FLAGS))

# # =============================
# # Apps / sources
# # =============================
# PROGRAMS ?= test
# APPS := $(PROGRAMS)

# # Keep intermediates to inspect IR/MIR/obj
# .SECONDARY:

# .PHONY: all clean distclean riscv-llvm asm obj mir-pass ll mir

# # =============================
# # Top-level targets
# # =============================
# all: riscv-llvm
# riscv-llvm: $(addsuffix .llvm.riscv,$(APPS))

# # Final ELF (link with GCC + libgloss)
# %.llvm.riscv: %.riscv.o pac_runtime.o $(libgloss)
# 	@echo "=== [riscv-llvm] Linking $@ (GCC + libgloss) ==="
# 	$(GCC) $(LDFLAGS) -march=$(RISCV_ARCH) -mabi=$(RISCV_ABI) -mcmodel=medany \
# 		$^ -o $@ $(LIBGLOSS_LINK_FLAGS_NO_SPECS)

# # =============================
# # MIR flow: .ll -> (isel) .mir -> run MIR pass (or produce asm/obj)
# # =============================

# # 1) Instruction-select only → produce .mir
# #    Generate MIR from the PAC-instrumented and optimized IR (.riscv.opt.ll).
# %.riscv.isel.mir: %.riscv.opt.ll
# 	@echo "=== [riscv-llvm] Instruction select → MIR: $< ==="
# 	$(LLC) -mtriple=$(RISCV_TRIPLE) -mattr=$(RISCV_MATTR) \
# 	       -relocation-model=static -stop-after=instruction-select \
# 	       -o $@ $<

# # 2) Single-pass test on MIR (no code emission)
# #    Mirrors: llc --passes=mir-set-ra input.mir -o /dev/null
# %.riscv.mirpass: %.riscv.isel.mir
# 	@echo "=== [riscv-llvm] Running MIR pass only: $(MIR_SINGLE_PASS) on $< ==="
# 	$(LLC) --passes=$(MIR_SINGLE_PASS) -o /dev/null $<
# 	@echo "=== [riscv-llvm] Done (pass ran; no code emitted). ==="

# # Convenience: run single-pass test for all apps
# mir-pass: $(addsuffix .riscv.mirpass,$(APPS))

# # 3a) Produce assembly from MIR using the target's default pipeline
# #     Your in-tree 'mir-set-ra' runs automatically if wired in PassConfig.
# %.riscv.s: %.riscv.isel.mir
# 	@echo "=== [riscv-llvm] MIR → ASM (default pipeline) ==="
# 	$(LLC) -filetype=asm -o $@ $<
# 	@echo "=== grep for the shadow load into ra ==="
# 	@grep -nE '__pac_ra_shadow|\blui\b|\baddi\b|ld[[:space:]]+x1\b|\bret\b|jalr' $@ || true

# # 3b) Produce object from MIR using the default pipeline (preferred)
# %.riscv.o: %.riscv.isel.mir
# 	@echo "=== [riscv-llvm] MIR → OBJ (default pipeline) ==="
# 	$(LLC) -filetype=obj -o $@ $<

# # 3c) (Optional) Assemble .riscv.s → .riscv.o using the RISC-V toolchain (never host 'as')
# #     Only used if you explicitly build .riscv.s first and then want .riscv.o
# %.riscv.o: %.riscv.s
# 	@echo "=== [riscv-llvm] Assembling RISC-V ASM → OBJ with $(GCC) ==="
# 	$(GCC) -march=$(RISCV_ARCH) -mabi=$(RISCV_ABI) -mcmodel=medany -c $< -o $@

# asm: $(addsuffix .riscv.s,$(APPS))
# obj: $(addsuffix .riscv.o,$(APPS))

# # =============================
# # IR pass: insert PAC instrumentation
# # =============================
# %.riscv.ra.ll: %.riscv.ll
# 	@echo "=== [riscv-llvm] Inserting RA protection ($(strip $(PAC_PASSES))) ==="
# 	$(OPT) -load-pass-plugin $(PAC_PASS_PLUGIN) -passes='$(strip $(PAC_PASSES)),verify' $< -S -o $@

# # Optimize instrumented IR
# %.riscv.opt.ll: %.riscv.ra.ll
# 	@echo "=== [riscv-llvm] Optimizing IR (O1) ==="
# 	$(OPT) -O1 $< -S -o $@

# # Generate unoptimized IR for RISC-V target (headers via sysroot/toolchain)
# %.riscv.ll: %.c
# 	@echo "=== [riscv-llvm] Generating unoptimized LLVM IR: $< ==="
# 	$(CLANG) -O1 -S -emit-llvm \
# 	         -target $(RISCV_TRIPLE) -march=$(RISCV_ARCH) -mabi=$(RISCV_ABI) \
# 	         --gcc-toolchain=$(RISCV_PREFIX) --sysroot=$(RISCV_SYSROOT) \
# 	         -ffreestanding -fno-builtin -fno-common -mcmodel=medany \
# 	         -I$(RISCV_SYSROOT)/include -I$(RISCV_SYSROOT)/include/newlib \
# 	         $< -o $@

# # =============================
# # ROCC runtime
# # =============================
# pac_runtime.o: pac_runtime.c pac_runtime.h
# 	$(GCC) $(CFLAGS) -c $< -o $@

# # =============================
# # Housekeeping
# # =============================
# clean:
# 	rm -f *.o *.s
# 	rm -f $(addsuffix .riscv.ll,$(APPS)) \
# 	      $(addsuffix .riscv.ra.ll,$(APPS)) \
# 	      $(addsuffix .riscv.opt.ll,$(APPS)) \
# 	      $(addsuffix .riscv.isel.mir,$(APPS)) \
# 	      $(addsuffix .riscv.mirpass,$(APPS)) \
# 	      $(addsuffix .riscv.o,$(APPS))
# 	rm -f $(addsuffix .llvm.riscv,$(APPS))
# 	$(if $(libgloss),rm -rf $(libgloss_builddir)/)

# distclean: clean
# 	@true



























































# #for linux

# -include config.mk

# .SUFFIXES:
# MAKEFLAGS += -rR

# # =============================
# # LLVM tools (your local build)
# # =============================
# LLVM_CONFIG ?= /home/pac/llvm-project/build/bin/llvm-config
# LLVM_BINDIR := $(shell $(LLVM_CONFIG) --bindir)
# CLANG := $(LLVM_BINDIR)/clang
# OPT   := $(LLVM_BINDIR)/opt
# LLC   := $(LLVM_BINDIR)/llc

# # =============================
# # PAC IR pass plugin
# # =============================
# PAC_PASS_PLUGIN ?= /home/pac/llvm-project/build/lib/RoccPacReturnPass.so
# PAC_PASSES      ?= rocc-pac

# # Single MIR pass registered in llc
# MIR_SINGLE_PASS ?= mir-set-ra

# # =============================
# # RISC-V Linux toolchain
# # =============================
# RISCV_GCC     ?= /home/pac/Desktop/lalit/rocket-modified/chipyard/.conda-env/riscv-tools/bin/riscv64-unknown-linux-gnu-gcc
# GCC           ?= $(RISCV_GCC)
# RISCV_TRIPLE  ?= riscv64-unknown-linux-gnu
# RISCV_ARCH    ?= rv64gc
# RISCV_ABI     ?= lp64d
# RISCV_MATTR   ?= +m,+a,+f,+d,+c
# RISCV_SYSROOT := $(shell $(RISCV_GCC) --print-sysroot)
# RISCV_PREFIX  := $(shell dirname $(shell dirname $(shell $(RISCV_GCC) -print-libgcc-file-name)))
# OBJDUMP       ?= $(RISCV_TRIPLE)-objdump

# # =============================
# # Build flags (Linux user-space)
# # =============================
# # NOTE: No libgloss, no freestanding, no bare-metal specs/scripts
# override CFLAGS  := -std=gnu99 -O2 -Wall -Wextra -fno-omit-frame-pointer \
#                     -march=$(RISCV_ARCH) -mabi=$(RISCV_ABI)
# override LDFLAGS := -static

# # Optional: choose your RoCC opcode (must match Scala build)
# # e.g., OpcodeSet.custom1 => -DPRINCE_OPCODE=1
# CFLAGS += -DPRINCE_OPCODE=1

# # =============================
# # Apps / sources
# # =============================
# PROGRAMS ?= test
# APPS := $(PROGRAMS)
# .SECONDARY:

# .PHONY: all clean distclean riscv-llvm asm obj mir-pass ll mir

# # =============================
# # Top-level targets
# # =============================
# all: riscv-llvm
# riscv-llvm: $(addsuffix .llvm.riscv,$(APPS))

# # Final ELF (Linux static)
# %.llvm.riscv: %.riscv.o pac_runtime.o
# 	@echo "=== [riscv-llvm] Linking (Linux static): $@ ==="
# 	$(GCC) $(LDFLAGS) -march=$(RISCV_ARCH) -mabi=$(RISCV_ABI) $^ -o $@

# # =============================
# # MIR flow: .ll -> (isel) .mir -> passes -> obj/asm
# # =============================

# # 1) Instruction-select to MIR
# %.riscv.isel.mir: %.riscv.opt.ll
# 	@echo "=== [riscv-llvm] Instruction select → MIR: $< ==="
# 	$(LLC) -mtriple=$(RISCV_TRIPLE) -mattr=$(RISCV_MATTR) \
# 	       -relocation-model=static -stop-after=instruction-select \
# 	       -o $@ $<

# # 2) Run a single MIR pass (no output artifact besides status)
# %.riscv.mirpass: %.riscv.isel.mir
# 	@echo "=== [riscv-llvm] Running MIR pass: $(MIR_SINGLE_PASS) on $< ==="
# 	$(LLC) --passes=$(MIR_SINGLE_PASS) -o /dev/null $<
# 	@echo "=== [riscv-llvm] Done. ==="

# mir-pass: $(addsuffix .riscv.mirpass,$(APPS))

# # 3a) MIR → ASM (optional)
# %.riscv.s: %.riscv.isel.mir
# 	@echo "=== [riscv-llvm] MIR → ASM (default pipeline) ==="
# 	$(LLC) -filetype=asm -o $@ $<
# 	@echo "=== grep for PAC patterns (optional) ==="
# 	@grep -nE '__pac_ra_shadow|\blui\b|\baddi\b|ld[[:space:]]+x1\b|\bret\b|jalr' $@ || true

# # 3b) MIR → OBJ (preferred)
# %.riscv.o: %.riscv.isel.mir
# 	@echo "=== [riscv-llvm] MIR → OBJ (default pipeline) ==="
# 	$(LLC) -filetype=obj -o $@ $<

# # 3c) Assemble .s → .o with the Linux toolchain (if you used .s)
# %.riscv.o: %.riscv.s
# 	@echo "=== [riscv-llvm] ASM → OBJ with $(GCC) (Linux) ==="
# 	$(GCC) -march=$(RISCV_ARCH) -mabi=$(RISCV_ABI) -c $< -o $@

# asm: $(addsuffix .riscv.s,$(APPS))
# obj: $(addsuffix .riscv.o,$(APPS))

# # =============================
# # IR pass: PAC instrumentation
# # =============================
# %.riscv.ra.ll: %.riscv.ll
# 	@echo "=== [riscv-llvm] Insert PAC passes: $(strip $(PAC_PASSES)) ==="
# 	$(OPT) -load-pass-plugin $(PAC_PASS_PLUGIN) -passes='$(strip $(PAC_PASSES)),verify' $< -S -o $@

# # Optimize instrumented IR
# %.riscv.opt.ll: %.riscv.ra.ll
# 	@echo "=== [riscv-llvm] Optimize IR (O1) ==="
# 	$(OPT) -O1 $< -S -o $@

# # Generate unoptimized LLVM IR targeting Linux (uses Linux sysroot/toolchain)
# %.riscv.ll: %.c
# 	@echo "=== [riscv-llvm] Emit LLVM IR for Linux: $< ==="
# 	$(CLANG) -O1 -S -emit-llvm \
# 	         -target $(RISCV_TRIPLE) -march=$(RISCV_ARCH) -mabi=$(RISCV_ABI) \
# 	         --gcc-toolchain=$(RISCV_PREFIX) --sysroot=$(RISCV_SYSROOT) \
# 	         -fno-omit-frame-pointer \
# 	         $< -o $@

# # =============================
# # Runtime
# # =============================
# pac_runtime.o: pac_runtime.c pac_runtime.h
# 	$(GCC) $(CFLAGS) -c $< -o $@

# # =============================
# # Housekeeping
# # =============================
# clean:
# 	rm -f *.o *.s
# 	rm -f $(addsuffix .riscv.ll,$(APPS)) \
# 	      $(addsuffix .riscv.ra.ll,$(APPS)) \
# 	      $(addsuffix .riscv.opt.ll,$(APPS)) \
# 	      $(addsuffix .riscv.isel.mir,$(APPS)) \
# 	      $(addsuffix .riscv.mirpass,$(APPS)) \
# 	      $(addsuffix .riscv.o,$(APPS))
# 	rm -f $(addsuffix .llvm.riscv,$(APPS))

# distclean: clean
# 	@true
