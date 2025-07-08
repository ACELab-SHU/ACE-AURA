---
title: 5G Cell Search
#tags: [special_layouts]
keywords: 5G, NR, Cell Search, MIB, SIB, SSS, PSS, BCH, CCH, SCH

last_updated: June 11, 2025
summary: "This document describes the multi-stage link flow of 5G cell search, which can be adapted to a variety of scenarios."
sidebar: mydoc_sidebar
permalink: mydoc_5g_baseband.html
toc: false
folder: mydoc
---

{% include links.html %}

<div style="text-align: center;">
  {% include image.html file="5G0.png" %}
</div>

<h2 id="rMNS6"><font style="color:rgba(0, 0, 0, 0.9);">The Multi-Stage Access Process of Contemporary Wireless Communication</font></h2>
<font style="color:rgba(0, 0, 0, 0.9);">
&emsp;&emsp;Contemporary wireless communication protocols, especially the 5G New Radio (NR) technology, adopt a multi-stage process to ensure the reliability and efficiency of communication. This staged approach covers the entire process from initial synchronization to system information acquisition, involving several key steps and operations.</font>

<div style="text-align: center;">
  {% include image.html file="5G1.png" %}
</div>

<h3 id="NdMiB"><font style="color:rgba(0, 0, 0, 0.9);">1. Initial Synchronization</font></h3>
<br>
<span style="background: #FFD6E7; padding: 4px 8px; border-radius: 4px; display: block; width: 100%; box-sizing: border-box;">
<b>Purpose:</b><br>
&emsp;&emsp;⭕ First critical step in random access procedure.<br>
&emsp;&emsp;⭕ Establishes basic alignment between UE and network.<br>
&emsp;&emsp;⭕ Enables subsequent access operations.
</span>
<br>

<span style="
  background: #D8F3DC; 
  padding: 4px 8px; 
  border-radius: 4px; 
  display: block;
  width: 100%;
  box-sizing: border-box;">
<b>PSS Detection (Primary Synchronization)</b>
</span><br>

+ **Main Functions:**
  - Achieves symbol-level timing synchronization, determining the start position of each OFDM symbol. 
  - Provides a preliminary reference for frequency alignment.
+ **Implementation:**
  - The UE matches the received signal with three locally stored standard PSS sequences.
  - Identifies symbol boundaries by finding the position with the strongest signal correlation.
  - Detection accuracy can reach ±0.5 microseconds.
<br><br>

<span style="background: #D8F3DC; padding: 4px 8px; border-radius: 4px; display: block;width: 100%;box-sizing: border-box;">
<b>SSS Detection (Secondary Synchronization)</b>
</span>
<br>

+ **Main Functions:**
  - Completes frame synchronization, determining the boundaries of 10ms radio frames. 
  - Identifies the cell group number and, together with PSS, determines the Physical Cell Identifier (PCI) :
<div style="text-align: center;">
  {% include image.html file="5G3.png" max-width = '400' %}
</div>
 
+ **Implementation:**
  - Detects the SSS signal based on PSS synchronization. 
  - Determines cell group information by matching 336 possible SSS sequences.<br><br>

<span style="background: #D8F3DC; padding: 4px 8px; border-radius: 4px; display: block;width: 100%;box-sizing: border-box;">
<b>Frequency Compensation (Key Addition)</b>
</span><br>

+ **Need for Compensation:**
  - Device movement or hardware differences can cause frequency offset in the received signal.  
  - Uncompensated frequency offset severely impacts signal demodulation quality.  
+ **Compensation Methods:**
  - Coarse Compensation Phase:  
    * Estimates large-range frequency offset using the cyclic prefix characteristics of the PSS signal.  
    * Compensates for typical frequency offsets (±1kHz to ±20kHz).  
  - Fine Compensation Phase:  
    * Performs precise adjustments using reference signals (DMRS) in the PBCH.  
    * Controls residual frequency offset to within ±300Hz (for 15kHz subcarrier spacing).  
+ **Special Scenario Handling:**
  - Enables Doppler shift prediction algorithms during high-speed movement.  
  - Uses dynamic tracking mechanisms to adapt to continuous frequency offset changes.  
<br>    
    
<span style="
  background-color: #FFF8E1;  
  padding: 4px 8px;  
  border-radius: 4px; 
  display: inline-block; 
  box-shadow: 0 1px 2px rgba(0,0,0,1); 
">
  <span> ⚠️<b>Notes:</b><br>
&emsp;&emsp;● The entire synchronization process is typically completed within 20-40ms, a critical factor in ensuring 5G's low-latency performance.<br>
&emsp;&emsp;● In poor channel conditions, the system activates repeated detection mechanisms to ensure reliability.
  </span>


<br>
<h3 id="dA2ia"><font style="color:rgba(0, 0, 0, 0.9);">2. Master Information Acquisition</font></h3>

<span style="background: #FFD6E7; padding: 4px 8px; border-radius: 4px; display: block;width: 100%;box-sizing: border-box;">
<b>Objective:</b> Decode MIB to obtain basic network configuration.
</span><br>

<span style="background: #D8F3DC; padding: 4px 8px; border-radius: 4px; display: block;width: 100%;box-sizing: border-box;">
<b>Signal Processing Chain</b>
</span>

<div style="text-align: center;">
  {% include image.html file="5G2.png" %}
</div>
&emsp;&emsp;**a.** The UE locates the time-frequency resources of the Physical Broadcast Channel (PBCH) through the synchronized SSB (Synchronization Signal Block).<br>
&emsp;&emsp;**b.** It performs channel estimation using the DM-RS (Demodulation Reference Signals) within PBCH and compensates for wireless channel effects through frequency-domain equalization techniques.<br>
&emsp;&emsp;**c.** The QPSK-modulated PBCH data undergoes demodulation and rate dematching, with coded bits being recovered through Polar Decoding. <br>
&emsp;&emsp;**d.** CRC verification is performed on the decoded data to extract critical MIB information including system frame number, subcarrier spacing, and CORESET#0 configuration. 


<span style="
  background-color: #E1F5FE;  
  padding: 4px 8px;  
  border-radius: 4px; 
  display: block;width: 100%;box-sizing: border-box; 
  box-shadow: 0 1px 2px rgba(0,0,0,1); 
">
  <span> ⛳️<b>Key Parameters:</b><br>
&emsp;&emsp;● CORESET0 Location<br>
&emsp;&emsp;● SSB Periodicity<br>
&emsp;&emsp;● System Bandwidth
  </span>

<span style="
  background-color: #FFF8E1;  
  padding: 4px 8px;  
  border-radius: 4px; 
  display: block;width: 100%;box-sizing: border-box; 
  box-shadow: 0 1px 2px rgba(0,0,0,1); 
">
  <span> ⚠️<b>Notes:</b><br>
&emsp;&emsp;● The entire process must be completed within an 80ms time window while maintaining robust processing capabilities against frequency-selective fading and phase noise.<br>
&emsp;&emsp;● PBCH employs a 20ms retransmission mechanism to enhance decoding reliability.
  </span>

<h3 id="NePiQ"> <font style="color:rgba(0, 0, 0, 0.9);">3. Sysyem Information Acquisition</font></h3>
<span style=
  "background: #FFD6E7; 
    padding: 4px 8px; 
    border-radius: 4px; 
    display: block;width: 100%;box-sizing: border-box;">
<b>Objective:</b> Obtain complete access parameters via SIB1.
</span><br>

<span style="background: #D8F3DC; padding: 4px 8px; border-radius: 4px; display: block;width: 100%;box-sizing: border-box;">
<b>Signal Processing Chain</b>
</span>

&emsp;&emsp;The 5G NR access procedure follows a strict sequence where Control Channel (CCH) identification must precede Shared Channel (SCH) localization. The UE first decodes the CCH to obtain Downlink Control Information (DCI), which contains the necessary scheduling parameters. Using this DCI, the UE then locates and decodes the SCH to acquire System Information Blocks (SIBs). This sequential approach ensures efficient and reliable network access by maintaining proper synchronization between control and data plane operations.

<div style="text-align: center;">
  {% include image.html file="5G4.png" max-width = '400'%}
</div>

+ **<font style="color:rgba(0, 0, 0, 0.9);">Identification of Control Channel (CCH)</font>**<br>
&emsp;&emsp;**a.**  <font style="color:rgba(0, 0, 0, 0.9);">After achieving time-frequency synchronization through synchronization signals, the UE determines the physical resource location of CCH based on CORESET configuration information obtained from BCH decoding. </font><br>
&emsp;&emsp;**b.** <font style="color:rgba(0, 0, 0, 0.9);">Channel estimation is performed using DM-RS reference signals to calculate the channel response matrix, and equalization algorithms such as MMSE are applied to eliminate multipath interference. </font><br>
&emsp;&emsp;**d.** <font style="color:rgba(0, 0, 0, 0.9);">Blind detection is conducted on candidate PDCCHs, where operations including demodulation, descrambling, and CRC verification are performed to identify valid DCI formats from multiple candidate aggregation levels. </font><br>
&emsp;&emsp;**d.** <font style="color:rgba(0, 0, 0, 0.9);">The DCI content containing critical information such as scheduling grants is parsed. The entire process must be completed within millisecond-level time constraints while maintaining robust processing capabilities against frequency offsets and phase noise.</font>

<div style="text-align: center;">
  {% include image.html file="5G5.png" max-width = '650' %}
</div>

+ **<font style="color:rgba(0, 0, 0, 0.9);">Localization of Shared Channel (SCH)</font>**<br>
&emsp;&emsp;**a.** <font style="color:rgba(0, 0, 0, 0.9);">Based on the time-frequency resource location indicated by the DCI, the UE receives SIB1 data on the PDSCH and performs channel estimation and MMSE equalization using DM-RS. </font><br>
&emsp;&emsp;**b.** <font style="color:rgba(0, 0, 0, 0.9);">The QPSK/16QAM-modulated data is then demodulated, and the transport block is recovered through LDPC decoding. </font><br>
&emsp;&emsp;**c.** <font style="color:rgba(0, 0, 0, 0.9);">The UE parses the cell access parameters and other SIB scheduling information contained in SIB1. </font><br>

<div style="text-align: center;">
  {% include image.html file="5G6.png" max-width = '500' %}
</div>

<span style="
  background-color: #FFF8E1;  
  padding: 4px 8px;  
  border-radius: 4px; 
  display: block;width: 100%;box-sizing: border-box; 
  box-shadow: 0 1px 2px rgba(0,0,0,1); 
">
  <span> ⚠️<b>Notes:</b><br>
&emsp;&emsp;● The entire process must comply with the 160ms periodicity constraint and supports HARQ retransmission mechanisms to ensure reliability.<br>
&emsp;&emsp;● SIB1 transmission adopts a combined fixed-period and window-based mechanism to facilitate efficient system information acquisition by the UE.
  </span>

<h2 id="LEe7M">Summary</h2>
<font>&emsp;&emsp;The 5G NR employs a phased access mechanism that ensures rapid and reliable network access for UEs through a progressive three-stage process: Initial Synchronization establishes time-frequency references, Master Information Acquisition decodes critical configuration parameters, and System Information Acquisition completes full network information retrieval. This strictly sequential procedure features low-latency flexible processing mechanisms that not only adapt to communication requirements in complex wireless environments but also provide reliable technical support for diverse future service scenarios.</font>
