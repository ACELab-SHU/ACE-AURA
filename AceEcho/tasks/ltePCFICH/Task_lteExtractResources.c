/**
  ******************************************************************************
  * @file           : Task_lteExtractResources.c
  * @author         : XiaoxiaoChen
  * @brief          : LTE PCFICH Resources Extraction
  * @attention      : Indices start from 0 of a slot
  * @date           : 2024/1/6
  ******************************************************************************
  */

#include "riscv_printf.h"
#include "venus.h"
#include "stdint.h"
#include "data_type.h"
#include "vmath.h"

typedef short __v2048i16 __attribute__((ext_vector_type(2048)));
typedef short __v4096i16 __attribute__((ext_vector_type(4096)));
typedef char  __v4096i8 __attribute__((ext_vector_type(4096)));

//short         rxSymbolLength             = 600;

// char rxSymbol[600] = {1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1};//一个符号的数据
// short pcfichindices[16] = {102,103,105,106,252,253,255,256,402,403,405,406,552,553,555,556};//CFI对应索引

int Task_lteExtractResources(__v4096i8 dfe_input_real_0, __v4096i8 dfe_input_imag_0, __v2048i16 out_pcfichindices) {
    //__v4096i8 pcfichHest;
    //__v2048i16 index_rxS;
    //vclaim(pcfichHest);
    //vclaim(index_rxS);

    // __v2048i16 index_0;
    // vclaim(index_0);
    // vbrdcst(index_0,0,MASKREAD_OFF,16);
    // vsadd(out_pcfichindices,index_0,MASKREAD_OFF,16);

    // __v4096i8 index_zero;
    // vclaim(index_zero);
    // vbrdcst(index_zero,0,MASKREAD_OFF,600);
    // vsadd(dfe_input_real_0,index_zero,MASKREAD_OFF,600);
    // vsadd(dfe_input_imag_0,index_zero,MASKREAD_OFF,600);
    __v4096i8 dfe_output_real_0;
    vclaim(dfe_output_real_0);
    vshuffle(dfe_output_real_0, out_pcfichindices, dfe_input_real_0, SHUFFLE_GATHER, 16);
   
    __v4096i8 dfe_output_imag_0;
    vclaim(dfe_output_imag_0);
    vshuffle(dfe_output_imag_0, out_pcfichindices, dfe_input_imag_0, SHUFFLE_GATHER, 16);

    vreturn(dfe_output_real_0, 16, dfe_output_imag_0, 16);
}
