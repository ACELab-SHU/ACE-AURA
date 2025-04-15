/*
 * @Author: shenyihao shenyihao@shu.edu.cn
 * @Date: 2025-03-12 20:18:57
 * @LastEditors: shenyihao shenyihao@shu.edu.cn
 * @LastEditTime: 2025-03-12 23:16:20
 * @FilePath: /VEMU/AceEcho/tasks/ltePBCH/Task_ltePBCHConcatExtract.c
 * @Description: 
 */
#include "riscv_printf.h"
#include "venus.h"

typedef char __v4096i8 __attribute__((ext_vector_type(4096)));
typedef short __v2048i16 __attribute__((ext_vector_type(2048)));

int Task_ltePBCHConcatExtract(__v4096i8 pbch_real_concat, __v4096i8 pbch_imag_concat, __v2048i16 rxData_shuffle_index){
    __v4096i8 pbch_data_real;
    __v4096i8 pbch_data_imag;
    vclaim(pbch_data_real);
    vclaim(pbch_data_imag);
    vshuffle(pbch_data_real, rxData_shuffle_index, pbch_real_concat, SHUFFLE_GATHER, 288);
    vshuffle(pbch_data_imag, rxData_shuffle_index, pbch_imag_concat, SHUFFLE_GATHER, 288);

    vreturn(pbch_data_real,288,pbch_data_imag,288);

}