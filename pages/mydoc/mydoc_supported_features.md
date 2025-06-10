---
title: Supported features
#tags:
 # - getting_started
keywords: "features, capabilities, scalability, multichannel output, dita, hats, comparison, benefits"
last_updated: "June 10, 2025"
summary: "What you can do with OpenAURA"
published: true
sidebar: mydoc_sidebar
permalink: mydoc_supported_features.html
folder: mydoc
---

<font style="color:#0C68CA;">OpenAura supports the following core features, covering communication protocols, AI integration, development toolchains, and multi-scenario deployment:</font>

## <font style="color:rgb(0, 0, 0) !important;">Comprehensive Communication Protocol Support</font>

* **<font style="color:rgb(0, 0, 0) !important;">5G/LTE Physical Layer</font>**<font style="color:rgba(0, 0, 0, 0.85) !important;"> </font><font style="color:rgba(0, 0, 0, 0.85) !important;">Supports OFDM modulation/demodulation, FFT/IFFT (up to 4096 subcarriers), polar coding/decoding, rate matching, scrambling/descrambling, channel estimation (LS/MMSE), and MIMO beamforming, compliant with 3GPP-standard procedures (e.g., random access, cell search).</font>
* **<font style="color:rgb(0, 0, 0) !important;">GNSS Baseband Development</font>**<font style="color:rgba(0, 0, 0, 0.85) !important;"> </font><font style="color:rgba(0, 0, 0, 0.85) !important;">Enables signal acquisition, tracking, carrier phase recovery, pseudorange processing, and multi-constellation fusion for GPS, BeiDou, etc., with low-power modes for IoT and automotive applications.</font>
* **<font style="color:rgb(0, 0, 0) !important;">LORA Low-Power Wide-Area Networks (LPWAN)</font>**<font style="color:rgba(0, 0, 0, 0.85) !important;"> </font><font style="color:rgba(0, 0, 0, 0.85) !important;">Optimized for chirp modulation/demodulation, spreading factors (SF7-SF12), adaptive data rate (ADR) control, and interference mitigation algorithms, with energy-efficient processing via Venus vector instructions.</font>
* **<font style="color:rgb(0, 0, 0) !important;">Configurable Operator Library</font>**<font style="color:rgba(0, 0, 0, 0.85) !important;"> </font><font style="color:rgba(0, 0, 0, 0.85) !important;">Provides reusable operator modules (e.g.,fft_4096, polar_decoder) configurable via Python APIs or DSL scripts.</font>

## <font style="color:rgb(0, 0, 0);"></font><font style="color:rgb(0, 0, 0) !important;">Deep AI-Communication Fusion</font>

* **<font style="color:rgb(0, 0, 0) !important;">Neural Network Integration</font>**<font style="color:rgba(0, 0, 0, 0.85) !important;"> </font><font style="color:rgba(0, 0, 0, 0.85) !important;">Compatible with TensorFlow, PyTorch, etc., with pre-built AI operators for communication tasks:</font>
  * **<font style="color:rgb(0, 0, 0) !important;">Channel Prediction</font>**<font style="color:rgba(0, 0, 0, 0.85) !important;">: LSTM/Transformer networks for time-varying channel prediction.</font>
  * **<font style="color:rgb(0, 0, 0) !important;">Beam Management</font>**<font style="color:rgba(0, 0, 0, 0.85) !important;">: Reinforcement learning (RL) for dynamic beam selection in mmWave systems.</font>
  * **<font style="color:rgb(0, 0, 0) !important;">Signal Detection</font>**<font style="color:rgba(0, 0, 0, 0.85) !important;">: CNN-based joint detection and decoding for noisy channels.</font>
* **<font style="color:rgb(0, 0, 0) !important;">Hardware-Accelerated Inference</font>**<font style="color:rgba(0, 0, 0, 0.85) !important;"> </font><font style="color:rgba(0, 0, 0, 0.85) !important;">Utilizes Venus’s custom instructions (e.g., saturation arithmetic, complex number operations) to accelerate AI inference on edge devices, supporting quantized neural networks (QNNs) for low-latency, low-power fixed-point deployment.</font>

## <font style="color:rgba(0, 0, 0, 0.85) !important;"></font><font style="color:rgb(0, 0, 0) !important;">Flexible Development and Deployment</font>

* **<font style="color:rgb(0, 0, 0) !important;">Multi-Language Support</font>**
  * **<font style="color:rgb(0, 0, 0) !important;">Python API</font>**<font style="color:rgba(0, 0, 0, 0.85) !important;">: High-level interface for rapid prototyping (e.g., script-based 5G-LTE uplink simulation).</font>
  * **<font style="color:rgb(0, 0, 0) !important;">DSL (Domain-Specific Language)</font>**<font style="color:rgba(0, 0, 0, 0.85) !important;">: Visual workflow editor for defining task DAGs, ideal for complex protocol design.</font>
  * **<font style="color:rgb(0, 0, 0) !important;">C/C++</font>**<font style="color:rgba(0, 0, 0, 0.85) !important;">: Low-level access for optimizing performance-critical modules (e.g., real-time control loops).</font>
* **<font style="color:rgb(0, 0, 0) !important;">Multi-Hardware Targets</font>**
  * **<font style="color:rgb(0, 0, 0) !important;">Venus Processor</font>**<font style="color:rgba(0, 0, 0, 0.85) !important;">: Native support for RISC-V domain-specific hardware, with cycle-accurate simulation via Echo.</font>
  * **<font style="color:rgb(0, 0, 0) !important;">CPU/GPU</font>**<font style="color:rgba(0, 0, 0, 0.85) !important;">: Fallback for general-purpose computing in debugging or resource-constrained environments.</font>
  * **<font style="color:rgb(0, 0, 0) !important;">FPGA/ASIC</font>**<font style="color:rgba(0, 0, 0, 0.85) !important;">: Supports RTL export for hardware acceleration (coming in v1.0).</font>

## <font style="color:rgb(0, 0, 0);"></font><font style="color:rgb(0, 0, 0) !important;">Performance Validation and Debugging</font>

* **<font style="color:rgb(0, 0, 0) !important;">Link-Level Simulation</font>**<font style="color:rgba(0, 0, 0, 0.85) !important;"> </font><font style="color:rgba(0, 0, 0, 0.85) !important;">Echo simulator provides side-by-side floating-point (reference) and fixed-point (hardware-ready) result comparison, supporting SNR-BER analysis and large-scale network modeling (e.g., 100+ UE simulations).</font>
* **<font style="color:rgb(0, 0, 0) !important;">Real-Time Performance Metrics</font>**<font style="color:rgba(0, 0, 0, 0.85) !important;"> </font><font style="color:rgba(0, 0, 0, 0.85) !important;">Monitors throughput (bps/Hz), latency (cycles/bit), energy efficiency (pJ/bit), and resource utilization (vector lane occupancy, memory bandwidth), generating FPGA resource estimates and ASIC power consumption reports.</font>


