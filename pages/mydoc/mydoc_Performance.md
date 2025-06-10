---
title: Performance with OpenAURA × Venus
keywords: documentation theme, jekyll, technical writers, help authoring tools, hat replacements
last_updated: July 3, 2016
#tags: [getting_started]
summary: "Comparison results with the performance of modern chips"
sidebar: mydoc_sidebar
permalink: mydoc_performance.html
folder: mydoc
---

## OpenAURA × Venus —— Unlocking Performance with Software-Hardware Co-Design



{% include image.html file="performance1.png" caption="" %}

<font style="color:#0C68CA;">This table conducts a multi - dimensional comparison of six mobile communication - related solutions: OAI, srsRAN, Amarisoft, AI - RAN, Modem + NPU, and OpenAURA + Venus.</font>

<font style="color:rgba(0, 0, 0, 0.85) !important;">In scenarios for researchers, OpenAURA + Venus can support UE simulation and RAN simulation, covering common scenarios in scientific research. In commercial scenarios, from private network construction, to carrier - grade RAN deployment and UE adaptation, it is also competent, meeting the full - process needs of commercial network deployment. For AI - native mobile networks, this solution can also provide support, helping to build an intelligent network architecture.</font>

<font style="color:rgba(0, 0, 0, 0.85) !important;">In terms of AI computing performance, although OpenAURA + Venus does not pursue extreme computing power like some solutions, it can effectively play a role in multi - task collaborative processing by virtue of its architectural design, meeting the AI - assisted needs in mobile communication scenarios. In terms of communication energy consumption, it performs excellently, which can reduce energy consumption while ensuring network functions, conforming to the development direction of green communication. At the deployment level, based on the Venus platform, compared with solutions relying on traditional platforms such as x86, GPU, and ASIC, it can adapt to different deployment environments flexibly with a more advantageous cost investment, demonstrating engineering value in cost control and platform flexibility. It provides an efficient and practice - adaptable technical path for the mobile communication system from scientific research to commercial implementation, helping to promote the innovative application and large - scale promotion of technologies in diverse scenarios.</font>



## <font style="color:rgba(0, 0, 0, 0.85);">Multi - Dimensional Performance Verification and Design Analysis of Venus Architecture in Wireless Baseband Scenarios</font>

{% include image.html file="performance2.png" caption="Fig. (a) Single Venus tile performance compared with Intel AVX, Arm Neon and TI C64x+ DSP. (b) DAG of the sample thread, including FFT, channel estimation and equalization, rate de-matching and polar decoder. (c) System throughput with different numbers of clusters and tiles of Venus.(d) Post-synthesis area breakdown with 2 clusters (10 tiles in each cluster) configuration" %}

<font style="color:#0C68CA;">This set of figures serves as the core verification of the Venus architecture’s </font>**<font style="color:#0C68CA;">“Domain - Specific Architecture (DSA) adaptation to wireless baseband scenarios”.</font>**

#### <font style="color:rgb(0, 0, 0) !important;">1. Performance Comparison (a): Breaking the Limitations of General - Purpose Architectures</font>

<font style="color:rgba(0, 0, 0, 0.85) !important;">The single Venus tile, compared with Intel AVX (x86), Arm Neon (ARM), and TI DSP, achieves significantly higher speedup in wireless baseband - specific tasks such as FFT and channel equalization. This is because Venus uses </font>**<font style="color:rgb(0, 0, 0) !important;">UVP vector extensions</font>**<font style="color:rgba(0, 0, 0, 0.85) !important;"> (supporting non - power - of - two data lengths and dynamic register grouping) to solve the pain point of “high strip - mining overhead” in general - purpose instruction sets (as described in the TCAS1 - UVP paper), enabling more efficient mapping from algorithms to hardware.</font>

#### <font style="color:rgb(0, 0, 0) !important;">2. Task Flow (b): Hardware Collaboration Driven by Dataflow</font>

<font style="color:rgba(0, 0, 0, 0.85) !important;">The DAG diagram, prototyped on 5G baseband processing, connects the entire process of </font>**<font style="color:rgb(0, 0, 0) !important;">FFT (signal conversion) → channel estimation/equalization (signal compensation) → polar decoding (higher - layer protocols)</font>**<font style="color:rgba(0, 0, 0, 0.85) !important;">. Venus achieves task - level parallelism through the </font>**<font style="color:rgb(0, 0, 0) !important;">L2 scheduler (accounting for 89%, see (d))</font>**<font style="color:rgba(0, 0, 0, 0.85) !important;">, automatically decomposing Python algorithms into dataflow tasks (echoing the “multi - layer dataflow programming model” in VENUS_Magazine), verifying the scheduling flexibility of “software - defined hardware”.</font>

#### <font style="color:rgb(0, 0, 0) !important;">3. Throughput (c): Engineering Bottlenecks in Multi - core Scaling</font>

<font style="color:rgba(0, 0, 0, 0.85) !important;">Testing the throughput under different numbers of clusters and tiles reveals the key to “compute - memory” collaboration: when the number of tiles is ≤ 10, the throughput grows linearly due to the </font>**<font style="color:rgb(0, 0, 0) !important;">SIMD parallelism of vector extensions</font>**<font style="color:rgba(0, 0, 0, 0.85) !important;">; however, when the number of clusters exceeds 10, the “L1 DMA Memory Wall” emerges (L1 DMA bandwidth only accounts for 5.1%, see (d)), reflecting the design constraints of the on - chip network (AXI fabrics) and memory subsystem, and verifying the engineering trade - off in the </font>**<font style="color:rgb(0, 0, 0) !important;">multi - core scalability</font>**<font style="color:rgba(0, 0, 0, 0.85) !important;"> of Venus.</font>

#### <font style="color:rgb(0, 0, 0) !important;">4. Area Breakdown (d): Resource Allocation for Domain - Specific Optimization</font>

<font style="color:rgba(0, 0, 0, 0.85) !important;">In the post - synthesis area proportion, “Venus Tiles (54.4%)” integrate customized vector units (such as dedicated permutation engines for polar decoding), and the “L2 scheduler (89%)” dominates task scheduling — which is consistent with the design in the papers of “improving energy efficiency through domain - specific hardware reuse”. Although the control module has a high proportion (reflecting “programmability overhead”), through the area efficiency of </font>**<font style="color:rgb(0, 0, 0) !important;">21.2 GOPS/mm²</font>**<font style="color:rgba(0, 0, 0, 0.85) !important;"> (data from TCAS1 - UVP), it verifies the hardware value of “customization for wireless baseband scenarios”.</font>



## <font style="color:rgba(0, 0, 0, 0.85);">Wireless Baseband Processing Solutions: Cross - Platform Comparison of Performance and Development Efficiency</font>

{% include image.html file="performance3.png" caption="" %}

<font style="background-color:#E7E9E8;">[13] Y. Shen, F. Yuan, S. Cao, Z. Jiang, and S. Zhou, “Parallel computing for energy-efficient baseband processing in O-RAN: Synchronization and OFDMimplementation based on SPMD,” in Proc. IEEE Glob. Commun. Conf. (GLOBECOM), pp. 2736–2741, IEEE, 2023. </font>_

_<font style="background-color:#E7E9E8;">[14] J. Hoydis, S. Cammerer, F. A. Aoudia, A. Vem, N. Binder, G. Marcus, and A. Keller, “Sionna: An open-source library for next-generation physical layer research,” arXiv preprint arXiv:2203.11854, 2022. </font>_

<font style="color:rgba(0, 0, 0, 0.85) !important;">Table 1 presents comparison of link-level performance and software-friendliness between Venusian and prior works. Apart from the Venusian platform, we implemented a BCH decoding baseline in C, running on an Arm CPU. For other benchmarks, Shen et al. [13] leverage the single program multiple data (SPMD) paradigm of Intel CPUs using Implicit SPMD Program Compiler (ISPC), while Sionna [14] utilizes GPU-based simulations through the TensorFlow framework.</font>

<font style="color:rgba(0, 0, 0, 0.85) !important;">Compared to the Arm baseline, Venusian achieves a 51.57× speedup while also significantly reducing programming effort. This latency improvement primarily stems from the SIMD capability of the Venusian hardware. In contrast, the Intel CPU exhibits similar latency to the Arm baseline without ISPC optimization, but improves only to 3.61× slower than Venusian when leveraging multi-core and SIMD techniques. Sionna, while offering rich visualization and analysis interfaces, demonstrates low computational efficiency – up to 292.61× slower than Venusian when deployed on GPUs – likely due to memory overhead incurred during CPU-GPU data transfers.</font>



## <font style="color:rgba(0, 0, 0, 0.85);">Latency Breakdown and Performance Optimization Verification of PBCH Decoding Under Venus Architecture</font>

{% include image.html file="performance4.png" caption="Fig. Latency breakdown of BCH Procedure @ 50 MHz, Lane32Reg1024 (DMRS: Demodulation Reference Signal)." %}

<font style="color:rgba(0, 0, 0, 0.85);">This figure presents the latency for each procedure of PBCH decoding under a frequency of 50 MHz. The most time-consuming module in PBCH is channel decoding, which involves a belief propagation algorithm for polar codes. The latency of polar decoding is directly influenced by signal quality: the worse the signal quality, the more iterations are required. Following channel decoding, OFDM demodulation and DMRS search are the next time-consuming operations. OFDM demodulation consists of three FFTs and associated pre- and post-processing steps. DMRS search involves performing correlation and generating pseudo-random sequences for all candidates, which requires extensive permutation and reduction summation. The remaining modules are less complex and consume significantly less time.</font>
