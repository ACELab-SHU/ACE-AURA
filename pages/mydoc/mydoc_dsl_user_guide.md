---
title: DSL User Guide
#tags: [formatting]
keywords: dsl, dag, bas, user, guide, language
summary: "bas is"
sidebar: mydoc_sidebar
permalink: mydoc_dsl_user_guide.html
folder: mydoc

---

## DSL User Guide

In the realm of **wireless baseband processing**, **graph computing**, and **neural networks**, dataflow models are often *semi-static* with predictable and stable patterns, making them suitable for **domain-specific languages (DSLs)** and **domain-specific architectures (DSAs)**.

**Directed acyclic graphs (DAGs)** can formally represent these dataflow models, where:

* **Nodes** denote tasks
* **Directed edges** indicate communication between tasks

By utilizing **DAG scheduling**, we have developed an **innovative, highly scalable external DSL and toolchain** designed for **dataflow-driven heterogeneous systems** .

{% include image.html file="dsl_graph.png" caption=" DSL Workflow Graph." %}



## Data Types

> The following are vector data types: int, short, char, float, double.

## Language Structure

| Keyword        | Description                                                                                                                                                 |
| -------------- | ----------------------------------------------------------------------------------------------------------------------------------------------------------- |
| `parameter`    | Initializes vector data                                                                                                                                     |
| `dfe_data`     | DAG input data coming from the digital front end                                                                                                            |
| `dag_input`    | Uses the dag_input parameter as input, instantiated as the input for a specific node in the DAG, typically used for input binding when creating task nodes. |
| `return_value` | Marks the output result of a task in the DAG, usually bound to a specific task node to represent the node’s execution return value.                         |
| `dag`          | Connects various task nodes to build the entire DAG execution flow, coordinating task dependencies and execution order.                                     |

## 🚀 Quick Start

### Step 1: Define DAG input data

    ' PDSCH.bas 内容
    ' input parameter
    parameter short in1 = {1，2，3}
    parameter short in2 = {2}
    dag_input char dag_in1 = {1,2,3}
    dfe_data int dfe_in1 = {1,2,3}
    ' output parameter
    return_value short ncellid[1]

### Step 2: Define the data flow graph

    dag dag1 = {
        [a0] = task1(in1, in2, dag_in1, dfe_in1)
        [ncellid] = task2(a0)
    }
    
    dag dag1 = {
        [a0] = task1(in1, in2, dag_in1, dfe_in1)
        [ncellid] = task2(a0)
    }

### Step 3: End the code with END

    END

## DSL Compilation Workflow

# Configuration File Description([common.ck](http://common.ck/))

This configuration file defines the compilation environment, toolchain paths, and LLVM compilation parameters tailored for the Venus architecture. It is a core configuration script used for LLVM-based RISC-V custom backend development.

* * *

## 1. Path Configuration

| Variable    | Description                        | Example                                               |
| ----------- | ---------------------------------- | ----------------------------------------------------- |
| `LLVM_PATH` | Directory containing LLVM binaries | `/home/venus_llvm/llvm-project-cmake-build-debug/bin` |
| `RVPATH`    | Path to the RISC-V toolchain       | `/server17/ic/riscv32im`                              |
| `RM`        | Path to remove (delete) command    | `/usr/bin/rm`                                         |
| `PYTHON`    | Path to Python interpreter         | `/usr/bin/python3`                                    |

> Note: LLVM_PATH is defined multiple times; adjust it according to your environment.

* * *

## 2. Target and Toolchain Related

| Variable     | Description                        | Example                     |
| ------------ | ---------------------------------- | --------------------------- |
| `TARGET_DAG` | Name of the target dataflow graph  | `PDSCH`                     |
| `CC`         | Path to clang compiler executable  | `$(LLVM_PATH)/clang`        |
| `OPT`        | Path to llvm-opt optimization tool | `$(LLVM_PATH)/opt`          |
| `LLC`        | Path to llvm-llc compiler backend  | `$(LLVM_PATH)/llc`          |
| `DUMP`       | Path to llvm-objdump executable    | `$(LLVM_PATH)/llvm-objdump` |
| `CPY`        | Path to llvm-objcopy executable    | `$(LLVM_PATH)/llvm-objcopy` |

* * *

## 3. Venus Architecture Parameters

Venus is a custom architecture based on RISC-V. This section defines hardware resource sizes and compiler architecture options.

| Variable               | Description                                               | Example                                                                                   |
| ---------------------- | --------------------------------------------------------- | ----------------------------------------------------------------------------------------- |
| `VENUSROW`             | Number of rows in Venus hardware (register matrix size)   | `2048` (alternative `1024` commented out)                                                 |
| `VENUSLANE`            | Number of lanes in Venus hardware (degree of parallelism) | `32` (alternative `64` commented out)                                                     |
| `VENUS_VRFADDR`        | Base address of Venus hardware register file              | `0x80100000`                                                                              |
| `VenusInputStructAddr` | Base address of input data structure                      | `0x8002F000`                                                                              |
| `VENUSARCH`            | Target architecture options (for LLVM target selection)   | `--target=riscv32-unknown-elf --gcc-toolchain=/server17/ic/riscv32im -march=rv32imzvenus` |

* * *

## 4. Compilation Flags

### 4.1 Compile Phase Flags

| Variable       | Description                                      | Example                                                                                |
| -------------- | ------------------------------------------------ | -------------------------------------------------------------------------------------- |
| `VENUSCFLAGS`  | Compiler flags passed to clang for Venus backend | `-mllvm --venus -mllvm --venus-nr-row=$(VENUSROW) -mllvm --venus-nr-lane=$(VENUSLANE)` |
| `VENUSIRFLAGS` | Flags for generating LLVM IR                     | `-S -emit-llvm -Xclang -disable-O0-optnone`                                            |
| `CFLAGS`       | Combined compilation flags                       | `$(VENUSCFLAGS) $(VENUSARCH) $(VENUSIRFLAGS)`                                          |

### 4.2 Optimization Flags

| Variable        | Description                                                        | Example                                                                                                                                                    |
| --------------- | ------------------------------------------------------------------ | ---------------------------------------------------------------------------------------------------------------------------------------------------------- |
| `VENUSOPTFLAGS` | llvm-opt optimization flags including architecture-specific passes | `--mattr=+m,+zvenus --venus-nr-row=$(VENUSROW) --venus-nr-lane=$(VENUSLANE) --venus-istruct-baseaddr=$(VenusInputStructAddr) -S -passes=mem2reg,venusplit` |

### 4.3 LLVM Backend Compilation Flags

| Variable        | Description                                        | Example                                                                                                                                                           |
| --------------- | -------------------------------------------------- | ----------------------------------------------------------------------------------------------------------------------------------------------------------------- |
| `VENUSLLCFLAGS` | llvm-llc flags for register allocation and codegen | `-O3 --mattr=+m,+zvenus --venus-nr-row=$(VENUSROW) --venus-nr-lane=$(VENUSLANE) -regalloc=basic --venus-vrf-baseaddr=$(VENUS_VRFADDR) --enable-venus-alu-anlysis` |

### 4.4 Disassembly Flags

| Variable         | Description                        | Example                               |
| ---------------- | ---------------------------------- | ------------------------------------- |
| `VENUSDUMPFLAGS` | llvm-objdump flags for disassembly | `-d --mattr=+m,+zvenus -M no-aliases` |

### 4.5 Alternate Flags (Commented Out)

    # VENUSROW=1024
    # VENUSLANE=64
    # VENUSOPTFLAGS= --mattr=+m,+zvenus --venus-nr-row=$(VENUSROW) --venus-nr-lane=$(VENUSLANE) --venus-istruct-baseaddr=$(VenusInputStructAddr) -S -passes=mem2reg
    # VENUSLLCFLAGS2= -O3 --mattr=+m,+zvenus --venus-nr-row=$(VENUSROW) --venus-nr-lane=$(VENUSLANE) -regalloc=basic -venus-disable-vimls
    
    # 5. Configuration Example
    
    ```makefile
    LLVM_PATH=/home/xusiyi/new/llvm-project-cmake-build-debug/bin
    RVPATH=/server17/ic/riscv32im
    RM=/usr/bin/rm
    PYTHON=/usr/bin/python3
    TARGET_DAG=PDSCH
    
    CC=$(LLVM_PATH)/clang
    OPT=$(LLVM_PATH)/opt
    LLC=$(LLVM_PATH)/llc
    DUMP=$(LLVM_PATH)/llvm-objdump
    CPY=$(LLVM_PATH)/llvm-objcopy
    
    VENUSROW=2048
    VENUSLANE=32
    VENUS_VRFADDR=0x80100000
    VenusInputStructAddr=0x8002F000
    
    VENUSARCH= --target=riscv32-unknown-elf --gcc-toolchain=/server17/ic/riscv32im -march=rv32imzvenus
    VENUSCFLAGS= -mllvm --venus -mllvm --venus-nr-row=$(VENUSROW) -mllvm --venus-nr-lane=$(VENUSLANE)
    VENUSIRFLAGS= -S -emit-llvm -Xclang -disable-O0-optnone
    
    CFLAGS= $(VENUSCFLAGS) $(VENUSARCH) $(VENUSIRFLAGS)
    
    VENUSOPTFLAGS= --mattr=+m,+zvenus --venus-nr-row=$(VENUSROW) --venus-nr-lane=$(VENUSLANE) --venus-istruct-baseaddr=$(VenusInputStructAddr) -S -passes=mem2reg,venusplit
    
    VENUSLLCFLAGS= -O3 --mattr=+m,+zvenus --venus-nr-row=$(VENUSROW) --venus-nr-lane=$(VENUSLANE) -regalloc=basic --venus-vrf-baseaddr=$(VENUS_VRFADDR) --enable-venus-alu-anlysis
    
    VENUSDUMPFLAGS= -d --mattr=+m,+zvenus -M no-aliases

{% include links.html %}
