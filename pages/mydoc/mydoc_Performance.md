---
title: Performance with ACE-Echo × Venus
keywords: Performance, Venus, Echo, comparison

last_updated: July 8, 2025
#tags: [getting_started]
summary: "Comparison results with the performance of modern chips"
sidebar: mydoc_sidebar
permalink: mydoc_performance.html
folder: mydoc
---




<h2 id="ydnAu"><font style="color:rgb(0, 0, 0);">1. </font><font style="color:rgba(0, 0, 0, 0.85) !important;">Venus </font><font style="color:rgba(0, 0, 0, 0.85) !important;">vs</font><font style="color:rgba(0, 0, 0, 0.85) !important;"> Other Instruction Set </font><font style="color:rgba(0, 0, 0, 0.85);">Architectures</font></h2>

**<font style="color:rgb(0, 0, 0) !important;background-color:#FBDE28;">Venus Speedups: 2.3× (FFT), >2× (Matrix), Linear Scaling (Multi - Lane)</font>**

**<font style="color:rgb(0, 0, 0) !important;background-color:#FBDE28;">Venus vs Others: ~1× (General - Purpose), 1.8×/0.7× (DSP), >1.5× (All 5G + AI Tasks)</font>**

<div style="text-align: center;">
  {% include image.html file="performance2.png" caption="Fig. Single Venus tile performance compared with Intel AVX, Arm Neon and TI C64x+ DSP." %}
</div>


<h3 id="test-design"><font style="color:rgb(0, 0, 0) !important;">· </font><font style="color:rgb(0, 0, 0);">Test Design</font></h3>
**<font style="color:rgb(0, 0, 0) !important;">（1）Task Selection</font>**<font style="color:rgba(0, 0, 0, 0.85) !important;"></font>

&emsp;&emsp;<font style="color:rgba(0, 0, 0, 0.85);">○ </font><font style="color:#0C68CA;">5 tasks from</font><font style="color:rgba(0, 0, 0, 0.85) !important;"> </font><font style="color:#0C68CA;">5G physical layer</font><font style="color:rgba(0, 0, 0, 0.85) !important;"> (FFT, Channel Estimation, Channel Equalization, Rate De - matching, Polar Decoder) ；</font>

&emsp;&emsp;<font style="color:rgba(0, 0, 0, 0.85);">○ </font><font style="color:rgba(0, 0, 0, 0.85) !important;"></font><font style="color:#0C68CA;">3 AI tasks</font><font style="color:rgba(0, 0, 0, 0.85) !important;"> (Matrix Multiplication, 2D - Convolution, Max - pooling).</font>

**<font style="color:rgb(0, 0, 0) !important;">（2）Parameter Configuration</font>**<font style="color:rgba(0, 0, 0, 0.85) !important;"></font>

&emsp;&emsp;<font style="color:rgba(0, 0, 0, 0.85);">○ </font><font style="color:#0C68CA;">For 5G tasks</font><font style="color:rgba(0, 0, 0, 0.85) !important;">: 20 MHz bandwidth, 96 RBs, MCS = 28; FFT uses radix - 2 decimation - in - time; channel estimation/equalization adopts least - squared method; polar decoding applies belief propagation. </font>

&emsp;&emsp;<font style="color:rgba(0, 0, 0, 0.85);">○ </font><font style="color:#0C68CA;">For AI tasks</font><font style="color:rgba(0, 0, 0, 0.85) !important;">: matrix multiplication (64×32 & 32×256 matrices); 2D convolution kernel (16×16); max - pooling window (16×16).</font>

**<font style="color:rgb(0, 0, 0) !important;">（3）Comparison Architectures</font>**<font style="color:rgba(0, 0, 0, 0.85) !important;"></font>

&emsp;&emsp;<font style="color:rgba(0, 0, 0, 0.85) !important;">Intel AVX (x86 vector extensions), </font>

&emsp;&emsp;<font style="color:rgba(0, 0, 0, 0.85) !important;">Arm Neon (ARM vector instructions), </font>

&emsp;&emsp;<font style="color:rgba(0, 0, 0, 0.85) !important;">TI C64x + DSP (dedicated signal processor),  </font>

&emsp;&emsp;<font style="color:rgba(0, 0, 0, 0.85) !important;">Venus tiles (16/32/64 lanes).</font>

**<font style="color:rgb(0, 0, 0) !important;">（4）Optimization Strategies</font>**<font style="color:rgba(0, 0, 0, 0.85) !important;"></font>

&emsp;&emsp;<font style="color:rgba(0, 0, 0, 0.85);">○ </font><font style="color:#0C68CA;">Intel AVX/Arm Neon</font><font style="color:rgba(0, 0, 0, 0.85) !important;">: built - in intrinsics + compiler + digital library; </font>

&emsp;&emsp;<font style="color:rgba(0, 0, 0, 0.85);">○ </font><font style="color:#0C68CA;">TI DSP</font><font style="color:rgba(0, 0, 0, 0.85) !important;">: compiler - only; </font>

&emsp;&emsp;<font style="color:rgba(0, 0, 0, 0.85);">○ </font><font style="color:#0C68CA;">Venus</font><font style="color:rgba(0, 0, 0, 0.85) !important;">: customized vector extensions (complex shuffle units, extended vector instructions).</font>

<h3 id="performance-results"><font style="color:rgb(0, 0, 0) !important;">· </font><font style="color:rgb(0, 0, 0);">📈 Performance Results</font></h3>
**<font style="color:rgb(0, 0, 0) !important;">（1）Single - lane Competitiveness (16 - lane)</font>**<font style="color:rgba(0, 0, 0, 0.85) !important;"></font>

&emsp;&emsp;<font style="color:rgba(0, 0, 0, 0.85);">○ </font><font style="color:#0C68CA;">FFT</font><font style="color:rgba(0, 0, 0, 0.85) !important;">: Venus (radix - 2 optimization + data rearrangement) → 2.3× speedup vs TI C64x + DSP. </font>

&emsp;&emsp;<font style="color:rgba(0, 0, 0, 0.85);">○ </font><font style="color:#0C68CA;">Matrix Multiplication</font><font style="color:rgba(0, 0, 0, 0.85) !important;">: customized vector parallelism → >2× speedup vs Arm Neon (shows DSA advantage over general - purpose architectures).</font>

**<font style="color:rgb(0, 0, 0) !important;">（2）Multi - lane Scalability (32/64 - lane)</font>**<font style="color:rgba(0, 0, 0, 0.85) !important;"></font>

&emsp;&emsp;<font style="color:rgba(0, 0, 0, 0.85) !important;">Linear speedup growth with lanes (matches Amdahl's law). </font>

&emsp;&emsp;_<font style="color:rgba(0, 0, 0, 0.85) !important;">E.g., Polar Decoder: 5.7× (64 - lane vs 16 - lane);  2D - Convolution: >6 (64 - lane vs Intel AVX) → verifies hardware parallelism gain for hybrid loads.</font>_

**<font style="color:rgb(0, 0, 0) !important;">（3）Task Adaptation Differences</font>**<font style="color:rgba(0, 0, 0, 0.85) !important;"></font>

&emsp;&emsp;<font style="color:rgba(0, 0, 0, 0.85);">○ </font><font style="color:#0C68CA;">General - purpose (AVX/Neon)</font><font style="color:rgba(0, 0, 0, 0.85) !important;">: ~1× speedup in “irregular parallel” tasks (Polar Decoder, limited by software scheduling). </font>

&emsp;&emsp;<font style="color:rgba(0, 0, 0, 0.85);">○ </font><font style="color:#0C68CA;">Dedicated DSP (TI C64x +)</font><font style="color:rgba(0, 0, 0, 0.85) !important;">: 1.8× in 5G (Channel Equalization), 0.7× in AI (Max - pooling, architecture mismatch). </font>

&emsp;&emsp;<font style="color:rgba(0, 0, 0, 0.85);">○ </font><font style="color:#0C68CA;">Venus (DSA - based)</font><font style="color:rgba(0, 0, 0, 0.85) !important;">: >1.5× speedup in all “5G + AI” tasks → more balanced adaptation.</font>

<br><br><br> 



<h2 id="KGO7T"><font style="color:rgb(0, 0, 0);">2. </font><font style="color:rgba(0, 0, 0, 0.85) !important;">Venus </font><font style="color:rgba(0, 0, 0, 0.85) !important;">vs</font><font style="color:rgba(0, 0, 0, 0.85) !important;"> Other Hardware</font></h2>

**<font style="color:rgb(0, 0, 0) !important;background-color:#FBDE28;">Venusian Performance: 51.57× Faster Than Arm Baseline; Intel CPU/GPU Are 3.61×/292.61× Slower</font>**


<div style="text-align: center;">
  {% include image.html file="performance3.png" caption="Table: COMPARISON WITH EARLIER WORKS FOR PERFORMANCE AND SOFTWARE-FRIENDLINESS"  max-width = '500'%}
</div>

_<font style="background-color:#E7E9E8;">[13] Y. Shen, F. Yuan, S. Cao, Z. Jiang, and S. Zhou, “Parallel computing for energy-efficient baseband processing in O-RAN: Synchronization and OFDMimplementation based on SPMD,” in Proc. IEEE Glob. Commun. Conf. (GLOBECOM), pp. 2736–2741, IEEE, 2023. </font>_

_<font style="background-color:#E7E9E8;">[14] J. Hoydis, S. Cammerer, F. A. Aoudia, A. Vem, N. Binder, G. Marcus, and A. Keller, “Sionna: An open-source library for next-generation physical layer research,” arXiv preprint arXiv:2203.11854, 2022. </font>_

<h3 id="bP1Fs"><font style="color:rgb(0, 0, 0) !important;">· </font><font style="color:rgba(0, 0, 0, 0.85);">Comparison Objects</font></h3>

&emsp;&emsp;<font style="color:rgba(0, 0, 0, 0.85);">Baseline (Zynq (Cortex - A9) ), </font>

&emsp;&emsp;<font style="color:rgba(0, 0, 0, 0.85);">[13] (Intel CPU, Intel CPU (w/ ISPC) ), </font>

&emsp;&emsp;<font style="color:rgba(0, 0, 0, 0.85);">[14] (Nvidia GPU, Intel CPU ), and Venusian (FPGA).</font>

<h3 id="k5fme"><font style="color:rgb(0, 0, 0) !important;">· </font><font style="color:rgba(0, 0, 0, 0.85);">Test Content</font></h3>
&emsp;&emsp;<font style="color:rgba(0, 0, 0, 0.85);">Compare link - level performance and software - friendliness. Except Venusian, a BCH decoding baseline runs on Arm CPU (C - implemented). </font>

&emsp;&emsp;<font style="color:rgba(0, 0, 0, 0.85);">[13] uses SPMD paradigm with Intel CPUs + ISPC compiler; </font>

&emsp;&emsp;<font style="color:rgba(0, 0, 0, 0.85);">[14]'s Sionna leverages TensorFlow for GPU - based simulation.</font>

<h3 id="ILqjy"><font style="color:rgb(0, 0, 0) !important;">· </font><font style="color:rgba(0, 0, 0, 0.85);">📈 Performance</font></h3>
&emsp;&emsp;<font style="color:rgba(0, 0, 0, 0.85);">○ Vs. Arm baseline, </font><font style="color:#0C68CA;">Venusian achieves 51.57× speedup</font><font style="color:rgba(0, 0, 0, 0.85);">, reducing programming effort. Latency improvement comes from its hardware SIMD capability.</font>  

&emsp;&emsp;<font style="color:rgba(0, 0, 0, 0.85);">○ Intel CPU has similar latency to Arm baseline without ISPC. After optimization, it's still </font><font style="color:#0C68CA;">3.61× slower than Venusian</font><font style="color:rgba(0, 0, 0, 0.85);"> (with multi - core & SIMD).</font>  
&emsp;&emsp;<font style="color:rgba(0, 0, 0, 0.85);">○ Sionna offers rich visualization but low efficiency. On GPU, it's up to </font><font style="color:#0C68CA;">292.61× slower than Venusian</font><font style="color:rgba(0, 0, 0, 0.85);">, likely due to CPU - GPU data transfer overhead.</font>  

<br><br><br>

<h2 id="bYBFc"><font style="color:rgb(0, 0, 0);">3. UVP </font><font style="color:rgb(0, 0, 0);">vs</font><font style="color:rgb(0, 0, 0);"> Ara </font></h2>
**<font style="color:rgb(0, 0, 0) !important;background-color:#FBDE28;">UVP Performance Blows Away Ara: Far Fewer Cycles in Matmul/FFT, Up to 3.0× Speedup</font>**

<div style="text-align: center;">
  {% include image.html file="performance4.png" caption="Fig. Comparison of UVP with Ara in the number of clock cycles across various configurations and kernels. Note that we compare the L-lane Ara with the 4L-lane UVP (as shown in the legend, with pairs organized in columns) to ensure a fair comparison, given the equal number of execution units.  " %}
</div>




<h3 id="yaRgi"><font style="color:rgb(0, 0, 0) !important;">· </font><font style="color:rgba(0, 0, 0, 0.85);">Objective</font></h3>
&emsp;&emsp;<font style="color:rgba(0, 0, 0, 0.85);">Verify UVP's clock cycle optimization and performance acceleration vs. Ara architecture, under different lane configs and computing kernels (matmul, FFT).</font>

<h3 id="Egatt"><font style="color:rgb(0, 0, 0) !important;">· </font><font style="color:rgba(0, 0, 0, 0.85);">Test Content</font></h3>
&emsp;&emsp;<font style="color:rgba(0, 0, 0, 0.85);">○ </font><font style="color:#0C68CA;">Tasks & Configs</font><font style="color:rgba(0, 0, 0, 0.85);">：Select matmul (e.g., (33,9,129)) and FFT (e.g., 512/1024 points) tasks. Compare Ara (2/4/8/16 lanes) and UVP (8/16/32/64 lanes, ensuring fair execution unit comparison: L - lane Ara → 4L - lane UVP).</font>

&emsp;&emsp;<font style="color:rgba(0, 0, 0, 0.85);">○</font><font style="color:#0C68CA;"> Metrics</font><font style="color:rgba(0, 0, 0, 0.85);">：Use clock cycles to evaluate efficiency; speedup reflects UVP's performance gain over Ara.</font>

<h3 id="NTCaN"><font style="color:rgb(0, 0, 0) !important;">· </font><font style="color:rgba(0, 0, 0, 0.85);">📈 Performance</font></h3>
**<font style="color:rgba(0, 0, 0, 0.85);">（1）Clock Cycle Optimization</font>**<font style="color:rgba(0, 0, 0, 0.85);"></font>

&emsp;&emsp;<font style="color:rgba(0, 0, 0, 0.85);">○ In matmul (33,9,129), UVP (8 lanes) has </font>**<font style="color:rgba(0, 0, 0, 0.85);">far fewer</font>**<font style="color:rgba(0, 0, 0, 0.85);"> cycles than Ara (2 lanes); more UVP lanes (16/32/64) → </font>**<font style="color:rgba(0, 0, 0, 0.85);">fewer</font>**<font style="color:rgba(0, 0, 0, 0.85);"> cycles. </font>

&emsp;&emsp;<font style="color:rgba(0, 0, 0, 0.85);">○  Same for FFT (fft512): </font>**<font style="color:rgba(0, 0, 0, 0.85);">UVP cycles < Ara</font>**<font style="color:rgba(0, 0, 0, 0.85);"> for all configs.</font>

**<font style="color:rgba(0, 0, 0, 0.85);">（2）Speedup</font>**<font style="color:rgba(0, 0, 0, 0.85);"></font>

&emsp;&emsp;<font style="color:rgba(0, 0, 0, 0.85);">○ </font><font style="color:#0C68CA;">Matmul</font><font style="color:rgba(0, 0, 0, 0.85);">: 1.1× - 3.0× (more significant when matrix dims ≠ powers of two, ≥1.4×). </font>

&emsp;&emsp;<font style="color:rgba(0, 0, 0, 0.85);">○ </font><font style="color:#0C68CA;">FFT</font><font style="color:rgba(0, 0, 0, 0.85);">: 1.2× - 1.3×. If butterfly input exceeds Ara's vector register capacity, UVP benefits more from flexible RGs and larger VRFs.</font>  



<br><br><br>



<h2 id="VSef3"><font style="color:rgb(0, 0, 0);">4、PBCH Decoding Latency Breakdown</font></h2>

<div style="text-align: center;">
  {% include image.html file="performance5.png" caption="Fig. Latency breakdown of BCH Procedure @ 50 MHz, Lane32Reg1024 (DMRS: Demodulation Reference Signal)." max-width = 500 %}
</div>


<h3 id="GnesM"><font style="color:rgb(0, 0, 0) !important;">· </font><font style="color:rgba(0, 0, 0, 0.85);">Test Scenario</font></h3>
&emsp;&emsp;<font style="color:rgba(0, 0, 0, 0.85);">Analyze PBCH decoding latency at 50 MHz with Lane32Reg1024 config (including DMRS).</font>

<h3 id="HIdx4"><font style="color:rgb(0, 0, 0) !important;">· </font><font style="color:rgba(0, 0, 0, 0.85);">Task Decomposition</font></h3>
&emsp;&emsp;<font style="color:rgba(0, 0, 0, 0.85);">Break down PBCH decoding into 9 sub - procedures: OFDM Demodulation, Channel Estimation, Bit Descrambling, SSS Search, Channel Equalization, DMRS Search, Demodulation, Channel Decoding, and Others—covering the full link from signal preprocessing to data decoding.</font>

<h3 id="TP7tK"><font style="color:rgb(0, 0, 0) !important;">· </font><font style="color:rgba(0, 0, 0, 0.85);">📈 Core Latency Characteristics</font></h3>
&emsp;&emsp;<font style="color:rgba(0, 0, 0, 0.85);">○ </font>**<font style="color:rgba(0, 0, 0, 0.85);">Most Time - Consuming</font>**<font style="color:rgba(0, 0, 0, 0.85);">：</font><font style="color:#0C68CA;">Channel Decoding</font><font style="color:rgba(0, 0, 0, 0.85);"> (Polar code belief propagation), latency depends on signal quality (worse signal → more iterations → higher latency).</font>

&emsp;&emsp;<font style="color:rgba(0, 0, 0, 0.85);">○ </font>**<font style="color:rgba(0, 0, 0, 0.85);">Next Critical</font>**<font style="color:rgba(0, 0, 0, 0.85);">：</font><font style="color:#0C68CA;">OFDM Demodulation</font><font style="color:rgba(0, 0, 0, 0.85);"> (3 FFTs + time - frequency conversions) and </font><font style="color:#0C68CA;">DMRS Search</font><font style="color:rgba(0, 0, 0, 0.85);"> (candidate sequence correlation + pseudo - random generation via complex permutations/summations).</font>

&emsp;&emsp;<font style="color:rgba(0, 0, 0, 0.85);">○ </font>**<font style="color:rgba(0, 0, 0, 0.85);">Low - Impact</font>**<font style="color:rgba(0, 0, 0, 0.85);">：</font><font style="color:#0C68CA;">Bit Descrambling, SSS Search, etc.</font><font style="color:rgba(0, 0, 0, 0.85);">, have low complexity and minimal impact on total latency.</font>
