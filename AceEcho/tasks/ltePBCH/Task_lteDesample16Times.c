/*
 * @author: shenyihao shenyihao@shu.edu.cn
 * @Date: 2024-11-28 16:43:43
 * @LastEditors: shenyihao shenyihao@shu.edu.cn
 * @LastEditTime: 2025-03-12 16:47:43
 * @FilePath: /5G-Lite-0530/LTE_ECHO/5g_lite/Tasks/ltePBCH/Task_inputSplit.c
 * @Description:
 *
 * Copyright (c) 2024 by ACE_Lab（Shanghai University）, All Rights Reserved.
 */

#include "riscv_printf.h"
#include "venus.h"

typedef char __v8192i8 __attribute__((ext_vector_type(8192)));
typedef short __v4096i16 __attribute__((ext_vector_type(4096)));
typedef char __v4096i8 __attribute__((ext_vector_type(4096)));
typedef short __v2048i16 __attribute__((ext_vector_type(2048)));

// int Task_lteDesample16Times(__v4096i8 dfe_output_real_0, __v4096i8 dfe_output_real_1, __v4096i8 dfe_output_real_2,
//                             __v4096i8 dfe_output_real_3, __v4096i8 dfe_output_real_4, __v4096i8 dfe_output_real_5,
//                             __v4096i8 dfe_output_real_6, __v4096i8 dfe_output_real_7, __v4096i8 dfe_output_real_8,
//                             __v4096i8 dfe_output_imag_0, __v4096i8 dfe_output_imag_1, __v4096i8 dfe_output_imag_2,
//                             __v4096i8 dfe_output_imag_3, __v4096i8 dfe_output_imag_4, __v4096i8 dfe_output_imag_5,
//                             __v4096i8 dfe_output_imag_6, __v4096i8 dfe_output_imag_7, __v4096i8 dfe_output_imag_8) {

//   __v4096i8 desampled_real_0;
//   __v4096i8 desampled_real_1;
//   __v4096i8 desampled_real_2;
//   __v4096i8 desampled_real_3;
//   __v4096i8 desampled_real_4;
//   __v4096i8 desampled_real_5;
//   __v4096i8 desampled_real_6;
//   __v4096i8 desampled_real_7;
//   __v4096i8 desampled_real_8;

//   __v4096i8 desampled_imag_0;
//   __v4096i8 desampled_imag_1;
//   __v4096i8 desampled_imag_2;
//   __v4096i8 desampled_imag_3;
//   __v4096i8 desampled_imag_4;
//   __v4096i8 desampled_imag_5;
//   __v4096i8 desampled_imag_6;
//   __v4096i8 desampled_imag_7;
//   __v4096i8 desampled_imag_8;

//   vclaim(desampled_real_0);
//   vclaim(desampled_real_1);
//   vclaim(desampled_real_2);
//   vclaim(desampled_real_3);
//   vclaim(desampled_real_4);
//   vclaim(desampled_real_5);
//   vclaim(desampled_real_6);
//   vclaim(desampled_real_7);
//   vclaim(desampled_real_8);

//   vclaim(desampled_imag_0);
//   vclaim(desampled_imag_1);
//   vclaim(desampled_imag_2);
//   vclaim(desampled_imag_3);
//   vclaim(desampled_imag_4);
//   vclaim(desampled_imag_5);
//   vclaim(desampled_imag_6);
//   vclaim(desampled_imag_7);
//   vclaim(desampled_imag_8);

//   __v2048i16 desample_index;
//   vclaim(desample_index);
//   vrange(desample_index, 138);
//   desample_index = vsll(desample_index, 4, MASKREAD_OFF, 138);

//   vshuffle(desampled_real_0, desample_index, dfe_output_real_0, SHUFFLE_GATHER, 137);
//   vshuffle(desampled_real_1, desample_index, dfe_output_real_1, SHUFFLE_GATHER, 138);
//   vshuffle(desampled_real_2, desample_index, dfe_output_real_2, SHUFFLE_GATHER, 137);
//   vshuffle(desampled_real_3, desample_index, dfe_output_real_3, SHUFFLE_GATHER, 137);
//   vshuffle(desampled_real_4, desample_index, dfe_output_real_4, SHUFFLE_GATHER, 137);
//   vshuffle(desampled_real_5, desample_index, dfe_output_real_5, SHUFFLE_GATHER, 138);
//   vshuffle(desampled_real_6, desample_index, dfe_output_real_6, SHUFFLE_GATHER, 137);
//   vshuffle(desampled_real_7, desample_index, dfe_output_real_7, SHUFFLE_GATHER, 137);
//   vshuffle(desampled_real_8, desample_index, dfe_output_real_8, SHUFFLE_GATHER, 137);

//   vshuffle(desampled_imag_0, desample_index, dfe_output_imag_0, SHUFFLE_GATHER, 137);
//   vshuffle(desampled_imag_1, desample_index, dfe_output_imag_1, SHUFFLE_GATHER, 138);
//   vshuffle(desampled_imag_2, desample_index, dfe_output_imag_2, SHUFFLE_GATHER, 137);
//   vshuffle(desampled_imag_3, desample_index, dfe_output_imag_3, SHUFFLE_GATHER, 137);
//   vshuffle(desampled_imag_4, desample_index, dfe_output_imag_4, SHUFFLE_GATHER, 137);
//   vshuffle(desampled_imag_5, desample_index, dfe_output_imag_5, SHUFFLE_GATHER, 138);
//   vshuffle(desampled_imag_6, desample_index, dfe_output_imag_6, SHUFFLE_GATHER, 137);
//   vshuffle(desampled_imag_7, desample_index, dfe_output_imag_7, SHUFFLE_GATHER, 137);
//   vshuffle(desampled_imag_8, desample_index, dfe_output_imag_8, SHUFFLE_GATHER, 137);

//   vreturn(desampled_real_0, 137, desampled_real_1, 138, desampled_real_2, 137, desampled_real_3, 137,
//   desampled_real_4,
//           137, desampled_real_5, 138, desampled_real_6, 137, desampled_real_7, 137, desampled_real_8, 137,
//           desampled_imag_0, 137, desampled_imag_1, 138, desampled_imag_2, 137, desampled_imag_3, 137,
//           desampled_imag_4, 137, desampled_imag_5, 138, desampled_imag_6, 137, desampled_imag_7, 137,
//           desampled_imag_8, 137);
// }

int Task_lteDesample16Times(__v4096i8 dfe_output_real_0, __v4096i8 dfe_output_imag_0) {

  __v4096i8 desampled_real_0;

  __v4096i8 desampled_imag_0;

  vclaim(desampled_real_0);

  vclaim(desampled_imag_0);

  __v2048i16 desample_index;
  vclaim(desample_index);
  vrange(desample_index, 138);
  desample_index = vsll(desample_index, 4, MASKREAD_OFF, 138);

  vshuffle(desampled_real_0, desample_index, dfe_output_real_0, SHUFFLE_GATHER, 137);

  vshuffle(desampled_imag_0, desample_index, dfe_output_imag_0, SHUFFLE_GATHER, 137);

  vreturn(desampled_real_0, 137, desampled_imag_0, 137);
}
