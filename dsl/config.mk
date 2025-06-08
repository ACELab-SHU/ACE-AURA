# LLVM path in your computer
LLVM_PATH=/home/syh_gyh/文档/venus-llvm-project-venus/llvm-bin/bin

# RISC-V gnu toolchain path in your computer
RVPATH=/home/syh_gyh/SYH_Project/opt-riscv32-glibc-ima

# change the target dag
TARGET_DAG=ltePBCHDag1_hw

RM=/usr/bin/rm
PYTHON=/usr/bin/python3
mode=debug

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
VenusInputStructAddr=0x80023000
VENUSARCH= --target=riscv32-unknown-elf --gcc-toolchain=$(RVPATH) -march=rv32imzvenus
VENUSCFLAGS= -mllvm --venus -mllvm --venus-nr-row=$(VENUSROW) -mllvm --venus-nr-lane=$(VENUSLANE) 
VENUSIRFLAGS= -S -emit-llvm -Xclang -disable-O0-optnone
CFLAGS= $(VENUSCFLAGS) $(VENUSARCH) $(VENUSIRFLAGS)
VENUSOPTFLAGS= --mattr=+m,+zvenus --venus-nr-row=$(VENUSROW) --venus-nr-lane=$(VENUSLANE) --venus-istruct-baseaddr=$(VenusInputStructAddr) -S -passes=mem2reg,venusplit 
# VENUSOPTFLAGS= --mattr=+m,+zvenus --venus-nr-row=$(VENUSROW) --venus-nr-lane=$(VENUSLANE) --venus-istruct-baseaddr=$(VenusInputStructAddr) -S -passes=mem2reg 
VENUSLLCFLAGS= -O3 --mattr=+m,+zvenus --venus-nr-row=$(VENUSROW) --venus-nr-lane=$(VENUSLANE) -regalloc=basic --venus-vrf-baseaddr=$(VENUS_VRFADDR) --enable-venus-alu-anlysis 
VENUSDUMPFLAGS= -d --mattr=+m,+zvenus -M no-aliases
VENUSLLCFLAGS2= -O3 --mattr=+m,+zvenus --venus-nr-row=$(VENUSROW) --venus-nr-lane=$(VENUSLANE) -regalloc=basic -venus-disable-vimls
