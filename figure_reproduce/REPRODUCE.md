To fully reproduce the figure 4 in the paper, we divide the process into 3 steps

HARDWARE

for the FPGA RISC-V based tests, you need to have 6 different bitstreams; one for each design.

as mentioned in this part (https://github.com/bearhw/LIPPEN/blob/main/HARDWARE.md#step-5-fpga-bitstream-generation-vcu118) on how to make 
bitstreams, run the following commands:

cd chipyard/fpga
make SUB_PROJECT=vcu118 CONFIG=MyRoCCVCU118Config CONFIG_PACKAGE=chipyard.fpga.vcu118 bitstream --> for rocket lippen
make SUB_PROJECT=vcu118 CONFIG=QarmaRoCCVCU118Config CONFIG_PACKAGE=chipyard.fpga.vcu118 bitstream --> for rocket pac
make SUB_PROJECT=vcu118 CONFIG=LargeBoomPAPrinceVCU118Config CONFIG_PACKAGE=chipyard.fpga.vcu118 bitstream --> for boom lippen
make SUB_PROJECT=vcu118 CONFIG=LargeBoomPAQarmaVCU118Config CONFIG_PACKAGE=chipyard.fpga.vcu118 bitstream --> for boom pac
make SUB_PROJECT=vcu118 CONFIG=LargeBoomPAPrinceVCU118CustomConfig CONFIG_PACKAGE=chipyard.fpga.vcu118 bitstream --> for boom lippen with no RAS
make SUB_PROJECT=vcu118 CONFIG=LargeBoomPAPrinceVCU118CustomConfig3 CONFIG_PACKAGE=chipyard.fpga.vcu118 bitstream --> for boom lippen with no RAS and no BTB

SOFTWARE

Then, for each design, the microbenchmarks needs to be running and data to be collected. microbenchmarks need to be compiled and risc-v binaries to be generated
once. they can be reused for each design.

return address protection microbenchmarks:
1. simply run the makefile to get the instrumented binaries
2. to get the uninstrumented baseline binaries, same makefile can be used but it must point to a new unmidified llvm.
   1. clone llvm
   2. cd llvm-project
      git checkout llvmorg-18.1.6
      mkdir -p build && cd build
      cmake -G Ninja -S ../llvm -B . \
        -DLLVM_ENABLE_PROJECTS="clang;lld" \
        -DLLVM_TARGETS_TO_BUILD="RISCV;X86" \
        -DCMAKE_BUILD_TYPE=Release \
        -DLLVM_ENABLE_ASSERTIONS=ON \
        -DCMAKE_INSTALL_PREFIX="$PWD/install"
      
      ninja -j$(nproc)
   3. modify line 21 of the makefile and update LLVM_CONFIG with the address of the new unmodified llvm
   4. make
   5. binaries you get at this stage are the baseline uninstrumented . Modify their name and add "_NOPAC" in the end of their name.

  data pointer protection microbenchmarks:
  since liipen instructions are manually inserted, just follow the command in micro-benchmark readme to make the binary. the program here contains 
  both instrumented and non-instrumented.

PLOTTING
plot.py with comments provide guidence on where to put each data.
