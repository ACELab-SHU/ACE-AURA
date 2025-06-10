---
title: Venus User Guide
#tags: [getting_started]
last_updated: June 10, 2025
keywords: 
summary: "This guide provides essential information for working with the Venus framework, covering data types, intrinsic functions, and sample code for task and bas file creation."
sidebar: mydoc_sidebar
permalink: mydoc_venus_user_guide.html
folder: mydoc
---

## DataType

Venus supports two types of vector lengths (8-bit or 16-bit), with the longest vector length being 65535. The definition method is as follows:

```c
//Define a variable a as a vector of x elements, each of which is y bits wide.
//The definition length can be arbitrary.

__v(x)i(y) a

//Example:
// Define a 1024-element vector short16 where each element is 16 bits wide.
__v1024i16 short16;     
// Define a 512-element vector char8 where each element is 8 bits wide.
__v512i8 char8;         
```



## Intrinsics

### <font style="color:rgba(0, 0, 0, 0.85);">General Syntax Format</font>

```c
/*
input1  :vector1
input2  :vector2 or constant
MASKREAD_OFF/MASKREAD_ON (optional): Specifies whether to apply the mask register during computation. Elements are processed only if the corresponding mask bit is 1.
MASKWRITE_OFF/MASKWRITE_ON (optional): Controls whether the computation result is written to the mask register.
Length: Specifies the number of elements to process, up to the maximum vector length. If omitted, defaults to the full vector length.
*/

return_vector = vfunction(input1, input2, MASKREAD_OFF/MASKREAD_ON, MASKWRITE_OFF/MASKWRITE_ON，Length);

```

<font style="color:rgba(0, 0, 0, 0.85);"></font>

### <font style="color:rgb(0, 0, 0) !important;">Base Intrinsics</font>

**<font style="color:rgb(0, 0, 0) !important;">1. Addition</font>**<font style="color:rgb(0, 0, 0) !important;">（vadd,vsadd,vsaddu）</font>

```c
// When MASKWREAD_ON is set, only positions with a value of 1 are calculated, 
// positions with a value of 0 are filled with the vector in_1.

out = vadd(in_1, in_2, MASKREAD_OFF,Length);    // in_2 + in_1
out = vsadd(in_1, in_2, MASKREAD_OFF,Length);    // in_2 + in_1 using saturation
out = vsaddu(in_1, in_2, MASKREAD_OFF,Length);    // unsigned(in_2) + unsigned(in_1) using saturation
```

**<font style="color:rgba(0, 0, 0, 0.85);">2. Subtraction</font>**<font style="color:rgba(0, 0, 0, 0.85);">（vrsub,vsub,vssub,vssubu）</font>

```c
// When MASKWREAD_ON is set, only positions with a value of 1 are calculated,
// positions with a value of 0 are filled with the vector in_1.

out = vrsub(in_1, in_2, MASKREAD_OFF,Length);    // in_1 - in_2
out = vsub(in_1, in_2, MASKREAD_OFF,Length);    // in_2 - in_1
out = vssub(in_1, in_2, MASKREAD_OFF,Length);      // in_2 - in_1 using saturation
out = vssubu(in_1, in_2, MASKREAD_OFF,Length);     // unsigned(in_2) - unsigned(in_1) using saturation
```

**3. Multiplication**（vmul,vmulh,vmulhu,vmulhsu）

```c
// When MASKWREAD_ON is set, only positions with a value of 1 are calculated, 
// the value of the position vector a with a value of 0.

out = vmul (in_1, in_2, MASKREAD_OFF,Length);    // in_2 * in_1,outputs the lower 8 or 16 bits of the multiplication result.
out = vmulh(in_1, in_2, MASKREAD_OFF,Length);    // in_2 * in_1,outputs the high 8 or 16 bits of the multiplication result.
out = vmulhu(in_1, in_2, MASKREAD_OFF,Length);     // unsigned(in_2) * unsigned(in_1),outputs the lower 8 or 16 bits of the multiplication result.
out = vmulhsu(in_1, in_2, MASKREAD_OFF,Length);    // signed(in_2) * unsigned(in_1),outputs the lower 8 or 16 bits of the multiplication result.
```

**4. Division**（vdiv,vdivu）

```c
// When MASKWREAD_ON is set, only positions with a value of 1 are calculated, 
// positions with a value of 0 are filled with the vector in_1.

out = vdiv(in_1, in_2, MASKREAD_OFF,Length);    // in_2/in_1
out = vdivu(in_1, in_2, MASKREAD_OFF,Length);    // unsigned(in_2)/unsigned(in_1)
```

**<font style="color:rgb(0, 0, 0) !important;">5. Modulo</font>**<font style="color:rgb(0, 0, 0) !important;">（vrem,vremu）</font>

```c
out = vrem(in_1, in_2, MASKREAD_OFF,Length);    // in_2 % in_1
out = vremu(in_1, in_2, MASKREAD_OFF,Length);    // unsigned(in_2) % unsigned(in_1)
```

**<font style="color:rgb(0, 0, 0) !important;">6. Logical Operations</font>**<font style="color:rgb(0, 0, 0) !important;">（vand,vor,vxor）</font>

```c
// When MASKWREAD_ON is set, only positions with a value of 1 are calculated,
// positions with a value of 0 are filled with the vector in_1.

out = vand(in_1, in_2, MASKREAD_OFF,Length);    // Compute the bitwise AND of in_1 and in_2
out = vor(in_1, in_2, MASKREAD_OFF,Length);        // Compute the bitwise OR of in_1 and in_2
out = vxor(in_1, in_2, MASKREAD_OFF,Length);    // Compute the bitwise XOR of in_1 and in_2
```

**<font style="color:rgb(0, 0, 0) !important;">7. Arithmetic Shift Operations</font>**<font style="color:rgba(0, 0, 0, 0.85);">（vsll,vsrl,vsra）</font>

```c
out = vsll(in_1, constant, MASKREAD_OFF,Length);    // Shift signed integers in in_1 left by 'constant' bits
out = vsrl(in_1, constant, MASKREAD_OFF,Length);    // Shift unsigned integers in in_1 right by 'constant' bits
out = vsra(in_1, constant, MASKREAD_OFF,Length);    // Shift signed integers in in_1 right by 'constant' bits
```

**<font style="color:rgb(0, 0, 0) !important;">8. Comparison</font>**<font style="color:rgb(0, 0, 0) !important;"> （vseq,vsne,vsltu,vslt,vsleu,vsle,vsgtu,vsgt）</font>

```c
// When MASKWRITE_ON is set, the instruction has no return value.
// When MASKWREAD_ON is set, only positions with a value of 1 are compared, 
// positions with a value of 0 are filled with the vector a.
// Example: vseq(a, b, MASKREAD_OFF, MASKWRITE_ON);

out = vseq(a, b, MASKREAD_OFF, MASKWRITE_OFF,Length);    // b == a
out = vsne(a, b, MASKREAD_OFF, MASKWRITE_OFF,Length);    // b ≠ a
out = vsltu(a, b, MASKREAD_OFF, MASKWRITE_OFF,Length);     // (unsigned)b < (unsigned)a
out = vslt(a, b, MASKREAD_OFF, MASKWRITE_OFF,Length);    // b < a
out = vsleu(a, b, MASKREAD_OFF, MASKWRITE_OFF,Length);    // (unsigned)b ≤ (unsigned)a
out = vsle(a, b, MASKREAD_OFF, MASKWRITE_OFF,Length);    // b ≤ a
out = vsgtu(a, b, MASKREAD_OFF, MASKWRITE_OFF,Length);    // (unsigned)b ≥ (unsigned)a
out = vsgt(a, b, MASKREAD_OFF, MASKWRITE_OFF,Length);    // b ≥ a
```

<blockquote style="background: #f0f8ff; border-left: 3px solid #4682b4; padding: 10px;">
💡 Note: 
<p><font style="color:rgb(17, 17, 17);">Comparison functions must specify MASKWRITE_OFF / MASKWRITE_ON.</font> </p>
</blockquote>

**<font style="color:rgb(0, 0, 0) !important;">9. Composite Functions</font>**<font style="color:rgb(0, 0, 0) !important;">（vmuladd,vmulsub,vaddmul,vsubmul）</font>

```c
out = vmuladd(a,b,c, MASKREAD_OFF,Length);    // (b * a) + c
out = vmulsub(a,b,c, MASKREAD_OFF,Length);     // (b * a) - c
out = vaddmul(a,b,c, MASKREAD_OFF,Length);    // (b + a) * c
out = vsubmul(a,b,c, MASKREAD_OFF,Length);    // (b - a) * c
```

**<font style="color:rgb(0, 0, 0) !important;">10. Complex Multiplication Function</font>**<font style="color:rgb(0, 0, 0) !important;">（vcmxmul）</font>

```c
__v4096i8 *cmxreal_part = &tempWnResult_real;
__v4096i8 *cmximag_part = &tempWnResult_imag;

vcmxmul(cmximag_part, cmxreal_part, tempWnResult_real, tempWnResult_imag, 
        sin_stage0, cos_stage0, MASKREAD_OFF,calculate_length);
//(tempWnResult_real + tempWnResult_imag * i) * (cos_stage0 + sin_stage0 * i) 
// = (cmxreal_part + cmximag_part * i)
```



### <font style="color:rgb(0, 0, 0) !important;">Extended intrinsics</font>

**<font style="color:rgb(0, 0, 0) !important;">1. Gather/Scatter</font>**<font style="color:rgb(0, 0, 0) !important;">（vshuffle）</font>

```c
vshuffle(out, index, in, SHUFFLE_GATHER, Length);   // Void function. Places in[index(i)] into out(i) for i ∈ [0, Length-1].
vshuffle(out, index, in, SHUFFLE_SCATTER, Length);  // Void function. Writes in(i) to out[index(i)] for i ∈ [0, Length-1].
```

**<font style="color:rgb(0, 0, 0) !important;">2. Incremental Assignment Function</font>**<font style="color:rgb(0, 0, 0) !important;">（vrange）</font>

```c
//Fill vector in_and_out with {0,1,2,…,Length-1}. Void function.
vrange(in_and_out, Length); 
```

<blockquote style="background: #f0f8ff; border-left: 3px solid #4682b4; padding: 10px;">
💡 Note: <font style="color:rgba(0, 0, 0, 0.85);">vrange only supports 16-bit data.</font>
</blockquote>

**<font style="color:rgb(0, 0, 0) !important;">3. Broadcast Function</font>**<font style="color:rgb(0, 0, 0) !important;">（vbrdcst）</font>

```c
// Void function. Broadcasts 'constant' to all elements of vector in_and_out.
vbrdcst(in_and_out, constant, MASKREAD_OFF,Length);  
```

**<font style="color:rgb(0, 0, 0) !important;">4. Shift and Output Function for Multiply and Divide</font>**<font style="color:rgb(0, 0, 0) !important;">（vsetshamt）</font>

```c
/* Description: 
When performing Venus multiplication with 8-bit vectors a and b, the result of a*b is 16 bits. 
Standard multiplication instructions can only output the lower 8 bits or high 8 bits of this result. 
To extract an arbitrary 8-bit segment from the 16-bit product, use the vsetshamt(constant) instruction beforehand. 
This configures subsequent multiplication operations (e.g., vmul, vmulh) to apply shifts before output:
    - vmul: Outputs the lower 8 bits of (a*b >> constant).
    - vmulh: Outputs the upper 8 bits of (a*b << constant).
*/
vsetshamt(constant);
out = vmul(a, b, MASKREAD_OFF,Length);
```

**<font style="color:rgba(0, 0, 0, 0.85);">5. Declaration</font>**<font style="color:rgba(0, 0, 0, 0.85);">（vclaim）</font>

```c
// Void function. Ensures the address space of in_and_out is preserved and not eliminated by compiler optimizations.
// Typically placed after the variable definition.
vclaim(in_and_out);
```

**<font style="color:rgb(0, 0, 0) !important;">6. Address Retrieval Function</font>**<font style="color:rgb(0, 0, 0) !important;">（vaddr）</font>

```c
// Returns the starting address of the vector,out is an integer.
out = vaddr(vector);    
```

**<font style="color:rgba(0, 0, 0, 0.85);">7. Memory Locking Function</font>**<font style="color:rgba(0, 0, 0, 0.85);">（vbarrier）</font>

```c
// Example: memory locking for move the array data

// Transfer of 16-bit array
VSPM_OPEN();
vbarrier();
int testdata_16bit_a_addr = vaddr(testdata_16bit_a);
for (int i = 0; i < 8; i++)
{
    *(volatile unsigned short *)(testdata_16bit_a_addr + (i << 1)) = testdata_16bit_1[i];
}
int testdata_16bit_b_addr = vaddr(testdata_16bit_b);
for (int i = 0; i < 8; i++)
{
    *(volatile unsigned short *)(testdata_16bit_b_addr + (i << 1)) = testdata_16bit_2[i];
}
    VSPM_CLOSE();

// Transfer of 8-bit array
VSPM_OPEN();
vbarrier();
int testdata_8bit_a_addr = vaddr(testdata_8bit_a);
for (int i = 0; i < 8; i++)
{
    *(volatile unsigned char *)(testdata_8bit_a_addr + i) = testdata_8bit_1[i];
}
int testdata_8bit_b_addr = vaddr(testdata_8bit_b);
for (int i = 0; i < 8; i++)
{
    *(volatile unsigned char *)(testdata_8bit_b_addr + i) = testdata_8bit_2[i];
}
VSPM_CLOSE();

```

**<font style="color:rgb(0, 0, 0) !important;">8. Mask Inversion Function</font>**<font style="color:rgb(0, 0, 0) !important;">（vmnot）</font>

```c
vmnot(mask_reg); // Inverting the value of the mask register
```

**<font style="color:rgba(0, 0, 0, 0.85);">9. Vector Return Function</font>**<font style="color:rgba(0, 0, 0, 0.85);">（vreturn）</font>

```c
typedef struct{
    short data;
}attribute_((aligned(64)))short_struct;

__v2048i16 Vector;
short length = 256;
short_struct constant;
constant.data = 10;

vreturn (&constant,sizeof(constant),Vector,length,...,...);
//The scalar data needs to be put into the short_struct structure, 
//and the vector data needs to return the vector length length.
```

**<font style="color:rgb(0, 0, 0) !important;">10. Vector Operations</font>**<font style="color:rgb(0, 0, 0) !important;">（vredmin,vredmax,vredsum）</font>

```c
/*compute the minimum\maximum values all elements and 
the sum of all elements in a single vector.
*/

vector_b = vredmin(vector_a,MASKRED_OFF,length);
//Find the minimum value of the first length variables in the vector_a, 
//and store the value in the first variable of the vector_b.

vector_b = vredmax(vector_a,MASKRED_OFF,length);
//the usage method is the same as vredmin.
//You can invert the variable and replace it with find Minimum.

vector_b = vredsum(vector_a,MASKRED_OFF,length);
//add up the first length variables in the vector_a
//and the results are stored in the first four variables of the vector_b.
```

## A Sample Code for Single TASK File Creation

```c
#include "data_type.h"
#include "riscv_printf.h"
#include "venus.h"

typedef short __v2048i16 __attribute__((ext_vector_type(2048)));
typedef char  __v4096i8 __attribute__((ext_vector_type(4096)));

/**
 * @section DESCRIPTION
 * This Task checks the input data vector for a CRC error for 5G New Radio (NR)
 * physical channels as specified in 3GPP TS 38.212 .
 * 
 * Features
 * - Supports all 5G NR CRC polynomials (CRC24A, CRC24B, CRC24C, CRC16, CRC11, CRC6)
 *
 * @param[in]     tmp_vin            : A 4096i8 vector for storing the input bit sequence.
 * @param[in]     in_fullLen         : A short struct for storing length of input data in bits.
 * @param[in]     in_pariLen         : A short struct for storing length of CRC data in bits.
 * @param[in]     poly               : A 4096i8 vector (stored table in bas) for storing the CRC generation polynomial.
 * @param[out]    out_crc_result     : A short struct for storing error detection result.
 * @param[out]    buf                : A 4096i8 vector for storing calculated CRC value (optional).
 * 
 */

int Task_nrCRC(__v4096i8 tmp_vin, short_struct in_fullLen, short_struct in_pariLen, __v4096i8 poly) {
  int fullLen = in_fullLen.data + 24;
  int pariLen = in_pariLen.data + 1;
  int msgLen  = fullLen - pariLen + 1;
  int tmp;

  __v4096i8 vin;
  vclaim(vin);
  vbrdcst(vin, 1, MASKREAD_OFF, fullLen);

  __v2048i16 vin_shuffle_index;
  vclaim(vin_shuffle_index);
  vrange(vin_shuffle_index, fullLen);
  vin_shuffle_index = vsadd(vin_shuffle_index, 24, MASKREAD_OFF, fullLen);

  vshuffle(vin, vin_shuffle_index, tmp_vin, SHUFFLE_SCATTER, fullLen);

  __v4096i8  buf;
  __v4096i8  msg;
  __v2048i16 index;
  vclaim(buf);
  vclaim(msg);
  vclaim(index);

  vrange(index, msgLen);
  vbrdcst(msg, 0, MASKREAD_OFF, fullLen);
  vshuffle(msg, index, vin, SHUFFLE_GATHER, msgLen);

  for (int i = 0; i < msgLen; i++) {
    int m_addr = vaddr(msg);
    vbarrier();
    VSPM_OPEN();
    unsigned int addr = m_addr + i;
    tmp               = *(volatile unsigned char *)(addr);
    VSPM_CLOSE();

    if (tmp == 1) {
      vrange(index, pariLen);
      index = vsadd(index, i, MASKREAD_OFF, pariLen);
      vshuffle(buf, index, msg, SHUFFLE_GATHER, pariLen);
      buf = vxor(buf, poly, MASKREAD_OFF, pariLen);
      vshuffle(msg, index, buf, SHUFFLE_SCATTER, pariLen);
    }
  }

  vrange(index, pariLen - 1);
  index = vsadd(index, msgLen, MASKREAD_OFF, pariLen - 1);
  vshuffle(buf, index, msg, SHUFFLE_GATHER, pariLen - 1);

  __v4096i8 si_rnti;
  vclaim(si_rnti);
  vbrdcst(si_rnti, 1, MASKREAD_OFF, pariLen - 1);
  vbrdcst(si_rnti, 0, MASKREAD_OFF, 8);
  buf = vxor(buf, si_rnti, MASKREAD_OFF, pariLen - 1);

  __v2048i16 shuffle_index;
  vclaim(shuffle_index);
  vrange(shuffle_index, pariLen);
  shuffle_index = vsadd(shuffle_index, fullLen - pariLen + 1, MASKREAD_OFF, pariLen - 1);

  __v4096i8 compare;
  vclaim(compare);
  vshuffle(compare, shuffle_index, vin, SHUFFLE_GATHER, pariLen - 1);

  __v4096i8 compare_result;
  compare_result = vsne(buf, compare, MASKREAD_OFF, MASKWRITE_OFF, pariLen - 1);

  compare_result          = vredsum(compare_result, MASKREAD_OFF, pariLen - 1);
  int compare_result_addr = vaddr(compare_result);
  vbarrier();
  VSPM_CLOSE();
  int crc_result = *(volatile unsigned char *)(compare_result_addr);
  VSPM_CLOSE();

  short_struct out_crc_result;
  out_crc_result.data = crc_result;

  vreturn(buf, sizeof(buf), &out_crc_result, sizeof(out_crc_result));
}
```

<blockquote style="background: #f0f8ff; border-left: 3px solid #4682b4; padding: 10px;">
💡Notes:
<p><font style="color:rgb(17, 17, 17);">Try not to have scalar operations before vector operations, and add vclaim when they occur.</font>
</p>
</blockquote>

## A Sample Code for BAS File Creation

```basic
'Define vectors
parameter char in_vec1 = {1,2,3,...}         'no ";" ending
parameter short in_vec2 = {1,2,3,...}    
'Define constants
parameter short constant = {1}     

dfedata char dfe_input_0[4096]  'the input value of dfe
dfedata char dfe_input_1[4096]     'the input value of dfe
dag_input short NCELLID2[1]         'the input value of dag

return_value short subFrameNum[1]  'dag return value

dag dag1 = {
  [dfe_output_0] = Task_example(dfe_input_0, dfe_input_1,in_vec1,in_vec2,constant)
  ' _v4096i16 Task_example(_v4096i8 in_1, __v4096i8 in_2,__v409618 in_3,__v2048i16 in_4, short_struct const)
  ' typedef struct {short data;}__attribute_((aligned(64))) short_struct;
}

END
```



<blockquote style="background: #f0f8ff; border-left: 3px solid #4682b4; padding: 10px;">
💡Notes:
<p>1、<font style="color:rgb(17, 17, 17);">A single task in a bas file cannot be printed.</font>
<p>2、<font style="color:rgb(17, 17, 17);">Each line of code can not be added“ ; ”.</font>
<p>3、<font style="color:rgb(17, 17, 17);">Comments can not be added to the bas variable definition.</font>
<p>4、"<font style="color:rgb(17, 17, 17);">data" can not be used as a variable name for bas.</font>
</blockquote>
