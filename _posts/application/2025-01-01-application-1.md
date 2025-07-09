---
title:  "🗞️UVP Makes It: 5G PBCH MIB Decoded Successfully"
published: true
category: application
permalink: application-1.html
summary: false
tags: [5G,PBCH,MIB,decode]
---


&emsp;&emsp;On January 1, 2025, <b>our team successfully decoded the Master Information Block (MIB) in the 5G Physical Broadcast Channel (PBCH) </b>using a system based on the UVP architecture in the laboratory. This achievement validates the application value of UVP in practical wireless communication scenarios.
&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;

<div style="text-align: center;">
    {% include image.html file="application1_1.png" caption="Adetailedviewof the SoC architecture and evaluation board setup."  max-width = "450" %}
</div>

<div style="text-align: center;">
    {% include image.html file="application1_2.png" caption="Test setup for the evaluation board receiving and processing PBCH signals transmitted by abasestation." %}
</div>


&emsp;&emsp;At that time, the UVP system deployed on the VCU118 evaluation board was ready. The system adopts a 32-lane, Reg2048 configuration, tightly coupled with a lightweight RISC-V scalar core, and manages the collaborative work of various components through bus interconnection. The Digital Front-End (DFE) is responsible for real-time signal processing, the memory is used for code execution, and the serial port supports debugging and offline code access.

&emsp;&emsp;After the test started, the UVP system processed the PBCH signal according to the procedure: first, it performed OFDM demodulation, completed 3 FFTs and related pre- and post-processing steps, followed by SSS search, DMRS search and other links. Among them, DMRS search involves a large number of correlation operations and pseudo-random sequence generation, requiring extensive permutation and reduction summation operations. Subsequently, channel estimation, channel equalization demodulation, bit descrambling and channel decoding are performed in sequence. The channel decoding adopts the belief propagation algorithm of polar codes, and its latency is greatly affected by the signal quality; the worse the signal quality, the more iterations are required.

&emsp;&emsp;Throughout the process, UVP efficiently handled the vector operations in each link by virtue of its features such as supporting non-power-of-two register grouping, larger physical register files, and classifying vector instructions into symmetric and asymmetric categories with custom load/store schemes. Finally, the system successfully decoded the MIB, and at a frequency of 400MHz, the entire PBCH decoding process was completed within milliseconds.


<div style="text-align: center;">
    {% include image.html file="application1_3.png" caption=""  max-width = "500" %}
</div>














{% include links.html %}










