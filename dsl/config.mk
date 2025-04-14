LLVM_PATH=/server17/ic/llvm-bin/bin
# LLVM_PATH=/home/xusiyi/sys_llvm/llvm-project-cmake-build-debug/bin
RVPATH=/server17/ic/riscv32im
RM=/usr/bin/rm
PYTHON=/usr/bin/python3
TARGET_DAG=ltePBCH

CC=$(LLVM_PATH)/clang
OPT=$(LLVM_PATH)/opt
LLC=$(LLVM_PATH)/llc
DUMP=$(LLVM_PATH)/llvm-objdump
CPY=$(LLVM_PATH)/llvm-objcopy
VENUSROW=2048
VENUSLANE=32
# VENUSROW=1024
# VENUSLANE=64
VENUS_VRFADDR=0x80100000
VenusInputStructAddr=0x8002F000
VENUSARCH= --target=riscv32-unknown-elf --gcc-toolchain=/server17/ic/riscv32im -march=rv32imzvenus
VENUSCFLAGS= -mllvm --venus -mllvm --venus-nr-row=$(VENUSROW) -mllvm --venus-nr-lane=$(VENUSLANE) 
VENUSIRFLAGS= -S -emit-llvm -Xclang -disable-O0-optnone
CFLAGS= $(VENUSCFLAGS) $(VENUSARCH) $(VENUSIRFLAGS)
VENUSOPTFLAGS= --mattr=+m,+zvenus --venus-nr-row=$(VENUSROW) --venus-nr-lane=$(VENUSLANE) --venus-istruct-baseaddr=$(VenusInputStructAddr) -S -passes=mem2reg,venusplit 
# VENUSOPTFLAGS= --mattr=+m,+zvenus --venus-nr-row=$(VENUSROW) --venus-nr-lane=$(VENUSLANE) --venus-istruct-baseaddr=$(VenusInputStructAddr) -S -passes=mem2reg 
VENUSLLCFLAGS= -O3 --mattr=+m,+zvenus --venus-nr-row=$(VENUSROW) --venus-nr-lane=$(VENUSLANE) -regalloc=basic --venus-vrf-baseaddr=$(VENUS_VRFADDR) --enable-venus-alu-anlysis 
VENUSDUMPFLAGS= -d --mattr=+m,+zvenus -M no-aliases
VENUSLLCFLAGS2= -O3 --mattr=+m,+zvenus --venus-nr-row=$(VENUSROW) --venus-nr-lane=$(VENUSLANE) -regalloc=basic -venus-disable-vimls
