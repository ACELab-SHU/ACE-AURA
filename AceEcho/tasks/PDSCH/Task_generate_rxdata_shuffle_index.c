/**
 * ****************************************
 * @file        Task_generate_rxdata_shuffle_index.c
 * @brief       generate rxdata shuffle index(due to concat)
 * @author      yuanfeng
 * @date        2024.7.31
 * @copyright   ACE-Lab(Shanghai University)
 * ****************************************
 */

#include "data_type.h"
#include "riscv_printf.h"
#include "venus.h"

typedef char  __v15600i8 __attribute__((ext_vector_type(15600)));
typedef short __v7800i16 __attribute__((ext_vector_type(7800)));

int Task_generate_rxdata_shuffle_index(__v15600i8 rxData_real, __v15600i8 rxData_imag, short_struct in_nrb,
                                       short_struct in_symbol_length, short_struct in_concat_length) {

  short nrb             = in_nrb.data;
  short symbol_length   = in_symbol_length.data;
  short subcarrierPerRB = 12;
  short K               = nrb * subcarrierPerRB;
  short concat_lenth    = in_concat_length.data;

  // generate shuffle index
  __v7800i16 tmp_index;
  vclaim(tmp_index);
  vrange(tmp_index, K);

  __v7800i16 shuffle_index;
  vclaim(shuffle_index);
  vrange(shuffle_index, K);

  __v7800i16 rxData_shuffle_index;
  vclaim(rxData_shuffle_index);
  vshuffle(rxData_shuffle_index, shuffle_index, tmp_index, SHUFFLE_GATHER, K);

  for (int i = 1; i < symbol_length; ++i) {
    tmp_index     = vsadd(tmp_index, concat_lenth, MASKREAD_OFF, K);
    shuffle_index = vsadd(shuffle_index, K, MASKREAD_OFF, K);
    vshuffle(rxData_shuffle_index, shuffle_index, tmp_index, SHUFFLE_SCATTER, K);
  }
  __v15600i8 rxData_real0;
  __v15600i8 rxData_imag0;
  vclaim(rxData_real0);
  vclaim(rxData_imag0);
  rxData_real = vsadd(rxData_real, 0, MASKREAD_OFF, 1024 * symbol_length);
  rxData_imag = vsadd(rxData_imag, 0, MASKREAD_OFF, 1024 * symbol_length);

  vshuffle(rxData_real0, rxData_shuffle_index, rxData_real, SHUFFLE_GATHER, symbol_length * K);
  vshuffle(rxData_imag0, rxData_shuffle_index, rxData_imag, SHUFFLE_GATHER, symbol_length * K);

  vreturn(rxData_real0, symbol_length * K, rxData_imag0, symbol_length * K);
  // vreturn(rxData_shuffle_index, symbol_length * K * 2);
  // vreturn(rxData_shuffle_index, sizeof(rxData_shuffle_index));
}
