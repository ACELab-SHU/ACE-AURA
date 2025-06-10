---
title: "OpenAURA"
keywords: sample homepage
# tags: [getting_started]
sidebar: mydoc_sidebar
permalink: index.html
summary:  
---



{% include image.html file="introduction1.png" %}

# What is OpenAURA？

OpenAURA<font style="color:rgba(0, 0, 0, 0.85) !important;"> is an open-source development platform led by the Advanced Communication and Computing Electronics Lab (ACE Lab), focusing on solving the key challenges of deep integration between wireless communications and artificial intelligence (AI) in the 5G/6G era. Built upon </font>**<font style="color:rgb(0, 0, 0) !important;">AURA (AI-Unified Radio Architecture)</font>**<font style="color:rgba(0, 0, 0, 0.85) !important;">, it aims to create a </font>**<font style="color:rgb(0, 0, 0) !important;">software-hardware decoupled, highly programmable ecosystem</font>**<font style="color:rgba(0, 0, 0, 0.85) !important;"> that enables real-time AI-powered signal processing, driving the communication field toward "AI-Native" transformation.</font>

## <font style="color:rgb(0, 0, 0) !important;">Core Positioning and Goals</font>

### <font style="color:rgb(0, 0, 0) !important;">1. AURA Architecture as the Foundation</font>

* <font style="color:rgba(0, 0, 0, 0.85) !important;">The </font>**<font style="color:rgb(0, 0, 0) !important;">AURA architecture</font>**<font style="color:rgba(0, 0, 0, 0.85) !important;"> employs a heterogeneous many-core design based on the </font>**<font style="color:rgb(0, 0, 0) !important;">RISC-V extended instruction set</font>**<font style="color:rgba(0, 0, 0, 0.85) !important;">, integrating vector computing units (e.g., Venus processor) and AI accelerators. This enables collaborative computing between communication signal processing (e.g., OFDM modulation, polar coding/decoding) and deep learning models (e.g., channel estimation neural networks).</font>
* <font style="color:rgba(0, 0, 0, 0.85) !important;">Through a </font>**<font style="color:rgb(0, 0, 0) !important;">multi-level dataflow scheduling mechanism</font>**<font style="color:rgba(0, 0, 0, 0.85) !important;"> (L1 hardware scheduling for real-time tasks, L2 software scheduling for complex workflows), it optimizes data flow and task allocation, overcoming the "memory wall" and fragmented computing resources in traditional architectures.</font>

### <font style="color:rgb(0, 0, 0) !important;">2. Full-Stack Development Toolchain</font>

* **<font style="color:rgb(0, 0, 0) !important;">Compiler (Zoozve)</font>**<font style="color:rgba(0, 0, 0, 0.85) !important;">: An LLVM-based cross-platform toolchain that compiles Python-like code into RISC-V instructions, with built-in register allocation optimization and asymmetric instruction coalescing to enhance execution efficiency.</font>
* **<font style="color:rgb(0, 0, 0) !important;">Simulator (Echo)</font>**<font style="color:rgba(0, 0, 0, 0.85) !important;">: Provides </font>**<font style="color:rgb(0, 0, 0) !important;">link-level simulation capabilities</font>**<font style="color:rgba(0, 0, 0, 0.85) !important;">, supporting floating-point/fixed-point result comparison and hardware cycle-level performance prediction to accelerate algorithm validation (e.g., 5G random access procedure simulation).</font>
* **<font style="color:rgb(0, 0, 0) !important;">Real-Time Scheduler</font>**<font style="color:rgba(0, 0, 0, 0.85) !important;">: Integrates hardware data dependency tables with software task DAG analysis to achieve microsecond-level task scheduling, ensuring low-latency collaboration between communication protocol stacks and AI operators.</font>

### <font style="color:rgb(0, 0, 0) !important;">3. Multi-Scenario Application Support</font>

* **<font style="color:rgb(0, 0, 0) !important;">Communication Protocols</font>**<font style="color:rgba(0, 0, 0, 0.85) !important;">: Covers scenarios such as 5G/LTE physical layer, GNSS baseband, and LORA, providing configurable operator libraries (e.g., FFT, channel equalization, rate matching).</font>
* **<font style="color:rgb(0, 0, 0) !important;">AI Integration</font>**<font style="color:rgba(0, 0, 0, 0.85) !important;">: Supports joint optimization of neural network models and communication algorithms, such as using deep learning for channel prediction and beam management to improve spectral efficiency and link reliability.</font>

## <font style="color:rgb(0, 0, 0) !important;">Key Technical Features</font>

* **<font style="color:rgb(0, 0, 0) !important;">Seamless Heterogeneous Computing Integration</font>**<font style="color:rgba(0, 0, 0, 0.85) !important;">: Dynamically invokes Venus processors (communication-specific) and AI accelerators (e.g., Kuilong chip), shielding hardware differences through a unified programming model to allow developers to focus on algorithm design.</font>
* **<font style="color:rgb(0, 0, 0) !important;">Python-Like Programming Experience</font>**<font style="color:rgba(0, 0, 0, 0.85) !important;">: Offers a</font><font style="color:rgba(0, 0, 0, 0.85) !important;"> </font>**<font style="color:rgb(0, 0, 0) !important;">domain-specific programming language (DSL)</font>**<font style="color:rgba(0, 0, 0, 0.85) !important;"> </font><font style="color:rgba(0, 0, 0, 0.85) !important;">that supports visual workflow construction and hardware resource abstraction, reducing the entry barrier for developers without hardware expertise.</font>
* **<font style="color:rgb(0, 0, 0) !important;">Open Source and Community Ecosystem</font>**<font style="color:rgba(0, 0, 0, 0.85) !important;">: Benchmarked against projects like NVIDIA Sionna and Aerial RAN, it opensources code and documentation via GitHub, inviting academia and industry to contribute operators, optimization schemes, and application cases.</font>

## <font style="color:rgb(0, 0, 0) !important;">Applications and Impact</font>

* **<font style="color:rgb(0, 0, 0) !important;">Academic Research</font>**<font style="color:rgba(0, 0, 0, 0.85) !important;">: Provides an</font><font style="color:rgba(0, 0, 0, 0.85) !important;"> </font>**<font style="color:rgb(0, 0, 0) !important;">open-source testbed</font>**<font style="color:rgba(0, 0, 0, 0.85) !important;"> </font><font style="color:rgba(0, 0, 0, 0.85) !important;">for communication AI algorithms, enabling rapid prototyping (e.g., AI-aided signal detection algorithms) and shortening the cycle from research papers to hardware implementation.</font>
* **<font style="color:rgb(0, 0, 0) !important;">Industrial Deployment</font>**<font style="color:rgba(0, 0, 0, 0.85) !important;">: Helps equipment vendors and operators reduce 6G R&D costs, such as verifying AI-RAN (Intelligent Radio Access Network) solutions via the Echo simulator to avoid high trial-and-error costs of ASIC tape-out.</font>
* **<font style="color:rgb(0, 0, 0) !important;">Standardization Acceleration</font>**<font style="color:rgba(0, 0, 0, 0.85) !important;">: Accelerates the standardization of AI-driven communication technologies through open-source collaboration, such as introducing reference implementations of AI signal processing modules in 3GPP Rel-18 and beyond.</font>

# What is Venus?

{% include image.html file="introduction2_venus_hardware_architecture.png" caption=" Fig. Hardware architecture of Venus"%}

<font style="color:rgba(0, 0, 0, 0.85) !important;">Venus is a </font>**<font style="color:rgb(0, 0, 0) !important;">domain-specific processor</font>**<font style="color:rgba(0, 0, 0, 0.85) !important;"> developed by the Advanced Communication and Computing Electronics Lab (ACE Lab) for wireless communication applications, specifically targeting efficient hardware solutions for 4G/5G baseband signal processing. Its core design focuses on two critical dimensions: </font>**<font style="color:rgb(0, 0, 0) !important;">computational efficiency (parallelism optimization)</font>**<font style="color:rgba(0, 0, 0, 0.85) !important;"> and </font>**<font style="color:rgb(0, 0, 0) !important;">data communication efficiency (memory access mechanisms)</font>**<font style="color:rgba(0, 0, 0, 0.85) !important;">. Through a many-core architecture based on RISC-V instruction set extensions, multi-level hybrid scheduling strategies, and hardware-software co-design, Venus achieves deep optimization of vector operations, task scheduling, and memory management in baseband processing. Project outcomes include architecture prototype development, compiler toolchain construction, and link-level simulation validation, with the goal of driving breakthroughs in open architecture, low power consumption, and high-performance computing for wireless communications.</font>

<font style="color:rgba(0, 0, 0, 0.85) !important;"></font>

<font style="color:rgba(0, 0, 0, 0.85) !important;"></font>

<font style="color:rgba(0, 0, 0, 0.85) !important;"></font>

<font style="color:rgba(0, 0, 0, 0.85) !important;"></font>
