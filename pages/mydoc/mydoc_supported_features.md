---
title: Supported features
#tags:
 # - getting_started
keywords: "features, capabilities, scalability, multichannel output, dita, hats, comparison, benefits"
last_updated: "June 10, 2025"
summary: "What you can do with ACE-Echo"
published: true
sidebar: mydoc_sidebar
permalink: mydoc_supported_features.html
folder: mydoc
---



<h2 id="wGsZX"><font style="color:rgb(0, 0, 0);">1. </font><font style="color:rgba(0, 0, 0, 0.85) !important;">ACE-Echo+Venus </font><font style="color:rgba(0, 0, 0, 0.85) !important;">vs</font><font style="color:rgba(0, 0, 0, 0.85) !important;"> Other Solutions</font></h2>


<div style="text-align: center;">
  {% include image.html file="performance1.png" caption="" %}
</div>


<h3 id="X0BDd"><font style="color:rgb(0, 0, 0) !important;">· Compared Solutions</font><font style="color:rgba(0, 0, 0, 0.85) !important;"></font></h3>
&emsp;&emsp;<font style="color:rgba(0, 0, 0, 0.85) !important;">OAI, srsRAN, Amarisoft, AI-RAN, Modem+NPU, ACE-Echo+Venus</font>

<h3 id="VdaIE"><font style="color:rgb(0, 0, 0) !important;">· 🚩 Key Advantages</font></h3>
&emsp;&emsp;<font style="color:rgba(0, 0, 0, 0.85);">○ </font><font style="color:rgba(0, 0, 0, 0.85) !important;">Supports </font><font style="color:#0C68CA;">UE simulation</font><font style="color:rgba(0, 0, 0, 0.85) !important;"> and </font><font style="color:#0C68CA;">RAN simulation</font><font style="color:rgba(0, 0, 0, 0.85) !important;"> in research scenarios, covering </font><font style="color:#0C68CA;">commercial private networks, carrier-grade RAN</font><font style="color:rgba(0, 0, 0, 0.85) !important;">, and </font><font style="color:#0C68CA;">full-process UE</font><font style="color:rgba(0, 0, 0, 0.85) !important;"> adaptation</font>

&emsp;&emsp;<font style="color:rgba(0, 0, 0, 0.85);">○ </font><font style="color:#0C68CA;">AI computing performance</font><font style="color:rgba(0, 0, 0, 0.85) !important;"> meets mobile communication needs, </font><font style="color:#0C68CA;">energy consumption</font><font style="color:rgba(0, 0, 0, 0.85) !important;"> outperforms x86/GPU solutions, and Venus-based deployment </font><font style="color:#0C68CA;">costs are lower</font>


<br><br><br>




<h2 id="IpMSh"><font style="color:rgb(0, 0, 0) !important;">2. Comprehensive Communication Protocol Support</font></h2>
<div style="text-align: center;">
  {% include image.html file="Supported_Features2.png" caption="" %}
</div>

<h3 id="h3Xvr"><font style="color:rgb(0, 0, 0) !important;">· 📶5G/LTE Physical Layer</font></h3>
&emsp;&emsp;<font style="color:rgba(0, 0, 0, 0.85) !important;">Supports OFDM modulation/demodulation, FFT/IFFT (up to 4096 subcarriers), polar coding/decoding, rate matching, scrambling/descrambling, channel estimation (LS/MMSE), and MIMO beamforming, compliant with 3GPP-standard procedures (e.g., random access, cell search).</font>

<h3 id="VuwVf"><font style="color:rgb(0, 0, 0) !important;">· 📡GNSS Baseband Development</font></h3>
&emsp;&emsp;<font style="color:rgba(0, 0, 0, 0.85) !important;">Enables signal acquisition, tracking, carrier phase recovery, pseudorange processing, and multi-constellation fusion for GPS, BeiDou, etc., with low-power modes for IoT and automotive applications.</font>

<h3 id="WwBGj"><font style="color:rgb(0, 0, 0) !important;">· 📍LORA Low-Power Wide-Area Networks (LPWAN)</font></h3>
&emsp;&emsp;<font style="color:rgba(0, 0, 0, 0.85) !important;">Optimized for chirp modulation/demodulation, spreading factors (SF7-SF12), adaptive data rate (ADR) control, and interference mitigation algorithms, with energy-efficient processing via Venus vector instructions.</font>

<h3 id="H6wD8"><font style="color:rgb(0, 0, 0) !important;">· Configurable Operator Library</font></h3>
&emsp;&emsp;<font style="color:rgba(0, 0, 0, 0.85) !important;">Provides reusable operator modules (e.g.,fft_4096, polar_decoder) configurable via Python APIs or DSL scripts.</font>

<br><br><br>





<h2 id="le4RI"><font style="color:rgba(0, 0, 0, 0.85) !important;">3. </font><font style="color:rgb(0, 0, 0) !important;">Flexible Development and Deployment</font></h2>
<div style="text-align: center;">
  {% include image.html file="Supported_Features4.png" caption="" %}
</div>


<h3 id="NJbmN"><font style="color:rgb(0, 0, 0) !important;">· Multi-Language Support</font></h3>
&emsp;&emsp;<font style="color:rgba(0, 0, 0, 0.85);">○ </font>**<font style="color:rgb(0, 0, 0) !important;">Python API</font>**<font style="color:rgba(0, 0, 0, 0.85) !important;">: High-level interface for rapid prototyping (e.g., script-based 5G-LTE uplink simulation).</font>

&emsp;&emsp;<font style="color:rgba(0, 0, 0, 0.85);">○ </font>**<font style="color:rgb(0, 0, 0) !important;">DSL (Domain-Specific Language)</font>**<font style="color:rgba(0, 0, 0, 0.85) !important;">: Visual workflow editor for defining task DAGs, ideal for complex protocol design.</font>

&emsp;&emsp;<font style="color:rgba(0, 0, 0, 0.85);">○ </font>**<font style="color:rgb(0, 0, 0) !important;">C/C++</font>**<font style="color:rgba(0, 0, 0, 0.85) !important;">: Low-level access for optimizing performance-critical modules (e.g., real-time control loops).</font>



<h3 id="dGV5G"><font style="color:rgb(0, 0, 0) !important;">· Multi-Hardware Targets</font></h3>
&emsp;&emsp;<font style="color:rgba(0, 0, 0, 0.85);">○ </font>**<font style="color:rgb(0, 0, 0) !important;">Venus Processor</font>**<font style="color:rgba(0, 0, 0, 0.85) !important;">: Native support for RISC-V domain-specific hardware, with cycle-accurate simulation via Echo.</font>

&emsp;&emsp;<font style="color:rgba(0, 0, 0, 0.85);">○ </font>**<font style="color:rgb(0, 0, 0) !important;">CPU/GPU</font>**<font style="color:rgba(0, 0, 0, 0.85) !important;">: Fallback for general-purpose computing in debugging or resource-constrained environments.</font>

&emsp;&emsp;<font style="color:rgba(0, 0, 0, 0.85);">○ </font>**<font style="color:rgb(0, 0, 0) !important;">FPGA/ASIC</font>**<font style="color:rgba(0, 0, 0, 0.85) !important;">: Supports RTL export for hardware acceleration (coming in v1.0).</font>




<br><br><br>



<h2 id="UjTtM"><font style="color:rgb(0, 0, 0);">4. </font><font style="color:rgb(0, 0, 0) !important;">Performance Validation and Debugging</font></h2>
<div style="text-align: center;">
  {% include image.html file="Supported_Features5.png" caption="" %}
</div>


<h3 id="e2MYf"><font style="color:rgb(0, 0, 0) !important;">· Link-Level Simulation</font></h3>
<font style="color:rgba(0, 0, 0, 0.85) !important;">Echo simulator provides side-by-side floating-point (reference) and fixed-point (hardware-ready) result comparison, supporting SNR-BER analysis and large-scale network modeling (e.g., 100+ UE simulations).</font>

<h3 id="KsLts"><font style="color:rgb(0, 0, 0) !important;">· Real-Time Performance Metrics</font></h3>
<font style="color:rgba(0, 0, 0, 0.85) !important;">Monitors throughput (bps/Hz), latency (cycles/bit), energy efficiency (pJ/bit), and resource utilization (vector lane occupancy, memory bandwidth), generating FPGA resource estimates and ASIC power consumption reports.</font>

**<font style="color:rgb(0, 0, 0) !important;"></font>**

**<font style="color:rgb(0, 0, 0) !important;"></font>**

**<font style="color:rgb(0, 0, 0) !important;"></font>**

