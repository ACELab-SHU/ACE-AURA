---
title: Programming Model
#tags: [getting_started]
last_updated: June 10, 2025
keywords: 
summary: "This guide outlines the workflow for creating and managing projects in the AceEcho framework, covering project setup, task file creation, BAS file configuration, and result analysis. It provides four steps for developing Venus applications in the AceEcho environment"
sidebar: mydoc_sidebar
permalink: mydoc_programming_model.html
folder: mydoc
---

{% include links.html %}

## <font style="color:rgb(17, 17, 17);">Usage steps</font>

### **Step 1: Create a project file**

  In the AceEcho/tasks path, create a folder.(e.g : AceProject)

{% include image.html file="step1.png" %}

### **Step 2: Create multiple task files**

  In the AceEcho/tasks/AceProject/ directory, create multiple .c files according to the required functions, and write detailed rules as shown in Venus_User_Guide.

{% include image.html file="step2.png" %}

### **Step 3: Create a bas file to connect multiple task files**

  In the AceEcho/tasks/AceProject/ directory, create an AceProject.bas file, and write the rules as shown in the Venus User Guide example: Create a bas file.

{% include image.html file="step3.png" %}

<blockquote style="background: #f0f8ff; border-left: 3px solid #4682b4; padding: 10px;">
💡 Note: Only one bas file is allowed to exist in a project. 
</blockquote>

### **Step 4: Observe the printed result after compilation**

  In the AceEcho/tasks/Debug/emular_vins_result/, multiple task files will be output according to the order of writing, and there are multiple txt files under each file, and txt will output the printed content of each instruction.

{% include image.html file="step4.png" %}

## <font style="color:rgb(17, 17, 17);">Example</font>

  The following is an example code, which implements the descrambling function of the channel part of the physical layer of 5G NR:

**Task_nrPRBS.c**

```c
/**
 * ****************************************
 * @file        Task nrPRBS.c
 * @brief       5G NR DMRS (Demodulation Reference Signal) sequence generation implementation
 * ****************************************
 * Copyright (c) 2025 by ACE_Lab, All Rights Reserved.
 */

 #include "data_type.h"
 #include "riscv_printf.h"
 #include "venus.h"

 typedef short __v2048i16 __attribute__((ext_vector_type(2048)));
 typedef short __v4096i16 __attribute__((ext_vector_type(4096)));
 typedef char __v4096i8 __attribute__((ext_vector_type(4096)));

/**
 * @section DESCRIPTION
 * This Task implements the reference signal sequence generation 
 *    according to 3GPP TS 38.211
 * 
 * Features
 * - Supports both Type 1 and Type 2 DMRS configurations
 * - Implements gold sequence generation for pseudo-random DMRS sequences
 * - Provides configurable parameters for:
 *   * Physical layer cell identity (N_ID)
 *   * Slot/symbol allocation
 *   * Scrambling ID configuration
 *   * Antenna port mapping
 * 
 */

 int tabLength = 31;

 VENUS_INLINE __v4096i8
 nrPRBS(__v4096i8 seq1_vec, __v4096i8 init2_vec, __v2048i16 seq2_init_table_0_vec, __v2048i16 seq2_init_table_1_vec,
        __v2048i16 seq2_init_table_2_vec, __v2048i16 seq2_init_table_3_vec, __v2048i16 seq2_init_table_4_vec,
        __v2048i16 seq2_init_table_5_vec, __v2048i16 seq2_init_table_6_vec, __v2048i16 seq2_init_table_7_vec,
        __v2048i16 seq2_init_table_8_vec, __v2048i16 seq2_init_table_9_vec, __v2048i16 seq2_init_table_10_vec,
        __v2048i16 seq2_init_table_11_vec, __v2048i16 seq2_init_table_12_vec, __v2048i16 seq2_init_table_13_vec,
        __v2048i16 seq2_init_table_14_vec, __v2048i16 seq2_init_table_15_vec, __v2048i16 seq2_init_table_16_vec,
        __v2048i16 seq2_init_table_17_vec, __v2048i16 seq2_init_table_18_vec, __v2048i16 seq2_init_table_19_vec,
        __v2048i16 seq2_trans_table_0_vec, __v2048i16 seq2_trans_table_1_vec, __v2048i16 seq2_trans_table_2_vec,
        __v2048i16 seq2_trans_table_3_vec, __v2048i16 seq2_trans_table_4_vec, __v2048i16 seq2_trans_table_5_vec,
        __v2048i16 seq2_trans_table_6_vec, int sequenceLength)
 {
   /*--------------------DMRS GENERATION--------------------*/
   __v4096i8 seq2_init_table_0_tmp;
   vclaim(seq2_init_table_0_tmp);
   vshuffle(seq2_init_table_0_tmp, seq2_init_table_0_vec, init2_vec, SHUFFLE_GATHER, tabLength);
   __v4096i8 seq2_init_table_1_tmp;
   vclaim(seq2_init_table_1_tmp);
   vshuffle(seq2_init_table_1_tmp, seq2_init_table_1_vec, init2_vec, SHUFFLE_GATHER, tabLength);
   __v4096i8 seq2_init_table_2_tmp;
   vclaim(seq2_init_table_2_tmp);
   vshuffle(seq2_init_table_2_tmp, seq2_init_table_2_vec, init2_vec, SHUFFLE_GATHER, tabLength);
   __v4096i8 seq2_init_table_3_tmp;
   vclaim(seq2_init_table_3_tmp);
   vshuffle(seq2_init_table_3_tmp, seq2_init_table_3_vec, init2_vec, SHUFFLE_GATHER, tabLength);
   __v4096i8 seq2_init_table_4_tmp;
   vclaim(seq2_init_table_4_tmp);
   vshuffle(seq2_init_table_4_tmp, seq2_init_table_4_vec, init2_vec, SHUFFLE_GATHER, tabLength);
   __v4096i8 seq2_init_table_5_tmp;
   vclaim(seq2_init_table_5_tmp);
   vshuffle(seq2_init_table_5_tmp, seq2_init_table_5_vec, init2_vec, SHUFFLE_GATHER, tabLength);
   __v4096i8 seq2_init_table_6_tmp;
   vclaim(seq2_init_table_6_tmp);
   vshuffle(seq2_init_table_6_tmp, seq2_init_table_6_vec, init2_vec, SHUFFLE_GATHER, tabLength);
   __v4096i8 seq2_init_table_7_tmp;
   vclaim(seq2_init_table_7_tmp);
   vshuffle(seq2_init_table_7_tmp, seq2_init_table_7_vec, init2_vec, SHUFFLE_GATHER, tabLength);
   __v4096i8 seq2_init_table_8_tmp;
   vclaim(seq2_init_table_8_tmp);
   vshuffle(seq2_init_table_8_tmp, seq2_init_table_8_vec, init2_vec, SHUFFLE_GATHER, tabLength);
   __v4096i8 seq2_init_table_9_tmp;
   vclaim(seq2_init_table_9_tmp);
   vshuffle(seq2_init_table_9_tmp, seq2_init_table_9_vec, init2_vec, SHUFFLE_GATHER, tabLength);
   __v4096i8 seq2_init_table_10_tmp;
   vclaim(seq2_init_table_10_tmp);
   vshuffle(seq2_init_table_10_tmp, seq2_init_table_10_vec, init2_vec, SHUFFLE_GATHER, tabLength);
   __v4096i8 seq2_init_table_11_tmp;
   vclaim(seq2_init_table_11_tmp);
   vshuffle(seq2_init_table_11_tmp, seq2_init_table_11_vec, init2_vec, SHUFFLE_GATHER, tabLength);
   __v4096i8 seq2_init_table_12_tmp;
   vclaim(seq2_init_table_12_tmp);
   vshuffle(seq2_init_table_12_tmp, seq2_init_table_12_vec, init2_vec, SHUFFLE_GATHER, tabLength);
   __v4096i8 seq2_init_table_13_tmp;
   vclaim(seq2_init_table_13_tmp);
   vshuffle(seq2_init_table_13_tmp, seq2_init_table_13_vec, init2_vec, SHUFFLE_GATHER, tabLength);
   __v4096i8 seq2_init_table_14_tmp;
   vclaim(seq2_init_table_14_tmp);
   vshuffle(seq2_init_table_14_tmp, seq2_init_table_14_vec, init2_vec, SHUFFLE_GATHER, tabLength);
   __v4096i8 seq2_init_table_15_tmp;
   vclaim(seq2_init_table_15_tmp);
   vshuffle(seq2_init_table_15_tmp, seq2_init_table_15_vec, init2_vec, SHUFFLE_GATHER, tabLength);
   __v4096i8 seq2_init_table_16_tmp;
   vclaim(seq2_init_table_16_tmp);
   vshuffle(seq2_init_table_16_tmp, seq2_init_table_16_vec, init2_vec, SHUFFLE_GATHER, tabLength);
   __v4096i8 seq2_init_table_17_tmp;
   vclaim(seq2_init_table_17_tmp);
   vshuffle(seq2_init_table_17_tmp, seq2_init_table_17_vec, init2_vec, SHUFFLE_GATHER, tabLength);
   __v4096i8 seq2_init_table_18_tmp;
   vclaim(seq2_init_table_18_tmp);
   vshuffle(seq2_init_table_18_tmp, seq2_init_table_18_vec, init2_vec, SHUFFLE_GATHER, tabLength);
   __v4096i8 seq2_init_table_19_tmp;
   vclaim(seq2_init_table_19_tmp);
   vshuffle(seq2_init_table_19_tmp, seq2_init_table_19_vec, init2_vec, SHUFFLE_GATHER, tabLength);
   __v4096i8 seq2_tmp;
   vclaim(seq2_tmp);
   vbrdcst(seq2_tmp, 0, MASKREAD_OFF, tabLength + 1);
   seq2_tmp = vxor(seq2_init_table_0_tmp, seq2_init_table_1_tmp, MASKREAD_OFF, tabLength);
   seq2_tmp = vxor(seq2_tmp, seq2_init_table_2_tmp, MASKREAD_OFF, tabLength);
   seq2_tmp = vxor(seq2_tmp, seq2_init_table_3_tmp, MASKREAD_OFF, tabLength);
   seq2_tmp = vxor(seq2_tmp, seq2_init_table_4_tmp, MASKREAD_OFF, tabLength);
   seq2_tmp = vxor(seq2_tmp, seq2_init_table_5_tmp, MASKREAD_OFF, tabLength);
   seq2_tmp = vxor(seq2_tmp, seq2_init_table_6_tmp, MASKREAD_OFF, tabLength);
   seq2_tmp = vxor(seq2_tmp, seq2_init_table_7_tmp, MASKREAD_OFF, tabLength);
   seq2_tmp = vxor(seq2_tmp, seq2_init_table_8_tmp, MASKREAD_OFF, tabLength);
   seq2_tmp = vxor(seq2_tmp, seq2_init_table_9_tmp, MASKREAD_OFF, tabLength);
   seq2_tmp = vxor(seq2_tmp, seq2_init_table_10_tmp, MASKREAD_OFF, tabLength);
   seq2_tmp = vxor(seq2_tmp, seq2_init_table_11_tmp, MASKREAD_OFF, tabLength);
   seq2_tmp = vxor(seq2_tmp, seq2_init_table_12_tmp, MASKREAD_OFF, tabLength);
   seq2_tmp = vxor(seq2_tmp, seq2_init_table_13_tmp, MASKREAD_OFF, tabLength);
   seq2_tmp = vxor(seq2_tmp, seq2_init_table_14_tmp, MASKREAD_OFF, tabLength);
   seq2_tmp = vxor(seq2_tmp, seq2_init_table_15_tmp, MASKREAD_OFF, tabLength);
   seq2_tmp = vxor(seq2_tmp, seq2_init_table_16_tmp, MASKREAD_OFF, tabLength);
   seq2_tmp = vxor(seq2_tmp, seq2_init_table_17_tmp, MASKREAD_OFF, tabLength);
   seq2_tmp = vxor(seq2_tmp, seq2_init_table_18_tmp, MASKREAD_OFF, tabLength);
   seq2_tmp = vxor(seq2_tmp, seq2_init_table_19_tmp, MASKREAD_OFF, tabLength);

   __v2048i16 seq2_shuffle_index;
   vclaim(seq2_shuffle_index);
   vrange(seq2_shuffle_index, tabLength);

   __v4096i8 seq2_vec;
   vclaim(seq2_vec);
   vshuffle(seq2_vec, seq2_shuffle_index, seq2_tmp, SHUFFLE_SCATTER, tabLength);

   // gengrate the rest seqs
   __v4096i8 seq2_trans_0_tmp;
   __v4096i8 seq2_trans_1_tmp;
   __v4096i8 seq2_trans_2_tmp;
   __v4096i8 seq2_trans_3_tmp;
   __v4096i8 seq2_trans_4_tmp;
   __v4096i8 seq2_trans_5_tmp;
   __v4096i8 seq2_trans_6_tmp;
   vclaim(seq2_trans_0_tmp);
   vclaim(seq2_trans_1_tmp);
   vclaim(seq2_trans_2_tmp);
   vclaim(seq2_trans_3_tmp);
   vclaim(seq2_trans_4_tmp);
   vclaim(seq2_trans_5_tmp);
   vclaim(seq2_trans_6_tmp);

   for (int i = 1; i < ((sequenceLength + tabLength - 1) / tabLength); i++)
   {
     vshuffle(seq2_trans_0_tmp, seq2_trans_table_0_vec, seq2_tmp, SHUFFLE_GATHER, tabLength);
     vshuffle(seq2_trans_1_tmp, seq2_trans_table_1_vec, seq2_tmp, SHUFFLE_GATHER, tabLength);
     vshuffle(seq2_trans_2_tmp, seq2_trans_table_2_vec, seq2_tmp, SHUFFLE_GATHER, tabLength);
     vshuffle(seq2_trans_3_tmp, seq2_trans_table_3_vec, seq2_tmp, SHUFFLE_GATHER, tabLength);
     vshuffle(seq2_trans_4_tmp, seq2_trans_table_4_vec, seq2_tmp, SHUFFLE_GATHER, tabLength);
     vshuffle(seq2_trans_5_tmp, seq2_trans_table_5_vec, seq2_tmp, SHUFFLE_GATHER, tabLength);
     vshuffle(seq2_trans_6_tmp, seq2_trans_table_6_vec, seq2_tmp, SHUFFLE_GATHER, tabLength);
     vbrdcst(seq2_tmp, 0, MASKREAD_OFF, tabLength);
     seq2_tmp = vxor(seq2_trans_0_tmp, seq2_trans_1_tmp, MASKREAD_OFF, tabLength);
     seq2_tmp = vxor(seq2_tmp, seq2_trans_2_tmp, MASKREAD_OFF, tabLength);
     seq2_tmp = vxor(seq2_tmp, seq2_trans_3_tmp, MASKREAD_OFF, tabLength);
     seq2_tmp = vxor(seq2_tmp, seq2_trans_4_tmp, MASKREAD_OFF, tabLength);
     seq2_tmp = vxor(seq2_tmp, seq2_trans_5_tmp, MASKREAD_OFF, tabLength);
     seq2_tmp = vxor(seq2_tmp, seq2_trans_6_tmp, MASKREAD_OFF, tabLength);
     seq2_shuffle_index = vsadd(seq2_shuffle_index, tabLength, MASKREAD_OFF, tabLength);
     vshuffle(seq2_vec, seq2_shuffle_index, seq2_tmp, SHUFFLE_SCATTER, tabLength);
   }

   // generate dmrs
   __v4096i8 seq;
   seq = vxor(seq1_vec, seq2_vec, MASKREAD_OFF, sequenceLength);
   seq = vmul(seq, -2, MASKREAD_OFF, sequenceLength);
   seq = vsadd(seq, 1, MASKREAD_OFF, sequenceLength);
   return seq;
 }

 int Task_nrPRBS(
     __v4096i8 seq1_vec, __v2048i16 seq2_init_table_0_vec, __v2048i16 seq2_init_table_1_vec,
     __v2048i16 seq2_init_table_2_vec, __v2048i16 seq2_init_table_3_vec, __v2048i16 seq2_init_table_4_vec,
     __v2048i16 seq2_init_table_5_vec, __v2048i16 seq2_init_table_6_vec, __v2048i16 seq2_init_table_7_vec,
     __v2048i16 seq2_init_table_8_vec, __v2048i16 seq2_init_table_9_vec, __v2048i16 seq2_init_table_10_vec,
     __v2048i16 seq2_init_table_11_vec, __v2048i16 seq2_init_table_12_vec, __v2048i16 seq2_init_table_13_vec,
     __v2048i16 seq2_init_table_14_vec, __v2048i16 seq2_init_table_15_vec, __v2048i16 seq2_init_table_16_vec,
     __v2048i16 seq2_init_table_17_vec, __v2048i16 seq2_init_table_18_vec, __v2048i16 seq2_init_table_19_vec,
     __v2048i16 seq2_trans_table_0_vec, __v2048i16 seq2_trans_table_1_vec, __v2048i16 seq2_trans_table_2_vec,
     __v2048i16 seq2_trans_table_3_vec, __v2048i16 seq2_trans_table_4_vec, __v2048i16 seq2_trans_table_5_vec,
     __v2048i16 seq2_trans_table_6_vec, short_struct nCellID, short_struct input_sequence_length)
 {

   uint16_t NID = nCellID.data;
   int sequenceLength = input_sequence_length.data;
   int init2 = NID % (1 << 31);

   __v4096i8 init2_vec;
   vclaim(init2_vec);
   vbrdcst(init2_vec, 0, MASKREAD_OFF, 32);
   vbarrier();
   VSPM_OPEN();
   int init2_vec_addr = vaddr(init2_vec);
   for (int i = 0; i < 31; ++i)
   {
     *(volatile char *)(init2_vec_addr + i) = init2 & 0b1;
     init2 = init2 >> 1;
   }
   VSPM_CLOSE();

   __v4096i8 seq;
   seq = nrPRBS(seq1_vec, init2_vec, seq2_init_table_0_vec, seq2_init_table_1_vec, seq2_init_table_2_vec,
                seq2_init_table_3_vec, seq2_init_table_4_vec, seq2_init_table_5_vec, seq2_init_table_6_vec,
                seq2_init_table_7_vec, seq2_init_table_8_vec, seq2_init_table_9_vec, seq2_init_table_10_vec,
                seq2_init_table_11_vec, seq2_init_table_12_vec, seq2_init_table_13_vec, seq2_init_table_14_vec,
                seq2_init_table_15_vec, seq2_init_table_16_vec, seq2_init_table_17_vec, seq2_init_table_18_vec,
                seq2_init_table_19_vec, seq2_trans_table_0_vec, seq2_trans_table_1_vec, seq2_trans_table_2_vec,
                seq2_trans_table_3_vec, seq2_trans_table_4_vec, seq2_trans_table_5_vec, seq2_trans_table_6_vec,
                sequenceLength);


   vreturn(seq, sequenceLength);
 }
```

**Task_nrDescramble.c**

```c
/**
 * ****************************************
 * @file        Task nrDescramble.c
 * @brief       5G NR Physical layer Descramble Module
 * ****************************************
  * Copyright (c) 2025 by ACE_Lab, All Rights Reserved.
 */

#include "data_type.h"
#include "riscv_printf.h"
#include "venus.h"

typedef short __v2048i16 __attribute__((ext_vector_type(2048)));
typedef short __v4096i16 __attribute__((ext_vector_type(4096)));
typedef char __v4096i8 __attribute__((ext_vector_type(4096)));

/**
 * @section DESCRIPTION
 * This Task implements the descrambling of received DMRS signals in 5G NR systems,
 * utilizing known DMRS sequences for channel estimation and equalization 
 * as specified in 3GPP TS 38.211.
 * 
 * Features
 * - Supports all 5G NR Physical layer descramblings
 */

int Task_nrDescramble(__v4096i8 demod,__v4096i8 scrambleseq, short_struct nfmod4, short_struct demod_length)
{


  int nf_mod4 = nfmod4.data;
  int length = demod_length.data;
  scrambleseq = vsadd(scrambleseq,0,MASKREAD_OFF,length);

  __v4096i8 seq_choose;
  vclaim(seq_choose);
  __v2048i16 nfmodindex;
  vclaim(nfmodindex);
  vrange(nfmodindex,length);
  nfmodindex = vsadd(nfmodindex,480*nf_mod4,MASKREAD_OFF,length);
  vshuffle(seq_choose,nfmodindex,scrambleseq,SHUFFLE_GATHER,length);

  __v4096i8 pbchllr;
  pbchllr = vmul(demod, seq_choose, MASKREAD_OFF, length);

  vreturn(pbchllr, length);
}

```

**nrDescramble.bas**

```basic
' '''''''''''''''''''''''''''''''''''''''''''
' @file        nrDescramble.bas
' @brief       5G NR Physical layer Descramble Project
' '''''''''''''''''''''''''''''''''''''''''''
dag dag1 = {
    [scrambleSeq] = Task_nrPRBS(seq1, seq2_init_table_0, seq2_init_table_1, seq2_init_table_2, seq2_init_table_3,  seq2_init_table_4, seq2_init_table_5, seq2_init_table_6,  seq2_init_table_7, seq2_init_table_8, seq2_init_table_9,  seq2_init_table_10, seq2_init_table_11,  seq2_init_table_12,  seq2_init_table_13, seq2_init_table_14,  seq2_init_table_15,  seq2_init_table_16, seq2_init_table_17,  seq2_init_table_18,  seq2_init_table_19, seq2_trans_table_0,  seq2_trans_table_1,  seq2_trans_table_2, seq2_trans_table_3,  seq2_trans_table_4,  seq2_trans_table_5,seq2_trans_table_6, nCellID, prbs_length)
    [llr] = Task_nrDescramble(demod, scrambleSeq, nfmod4, demod_length) 
END
```
