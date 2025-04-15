/**
 * ****************************************
 * @file        Task_nrEqualize.c
 * @brief       equalization using ZF
 * @author      yuanfeng
 * @date        2024.7.25
 * @copyright   ACE-Lab(Shanghai University)
 * ****************************************
 */

#include "riscv_printf.h"
#include "venus.h"

typedef short __v7168i16 __attribute__((ext_vector_type(7168)));
typedef char  __v14336i8 __attribute__((ext_vector_type(14336)));

typedef struct {
  short data;
} __attribute__((aligned(64))) short_struct;

int Task_nrEqualize(__v14336i8 rxData_real, __v14336i8 rxData_imag, __v14336i8 H_real, __v14336i8 H_imag,
                    __v7168i16 pbch_index, __v7168i16 rxData_shuffle_index, short_struct input_sequence_length,
                    short_struct input_nrbLength, short_struct input_data_symbol_length) {

  int   sequence_length    = input_sequence_length.data;
  short subcarrierLength   = input_nrbLength.data * 12;
  short data_symbol_length = input_data_symbol_length.data;

  int fractionLength = 6;
  int nVar           = 0;

  /*--------------------ExtractResources--------------------*/
  vshuffle(rxData_real, rxData_shuffle_index, rxData_real, SHUFFLE_GATHER, subcarrierLength * data_symbol_length);
  vshuffle(rxData_imag, rxData_shuffle_index, rxData_imag, SHUFFLE_GATHER, subcarrierLength * data_symbol_length);

  __v14336i8 rxSym_real;
  __v14336i8 rxSym_imag;

  vclaim(rxSym_real);
  vclaim(rxSym_imag);
  vshuffle(rxSym_real, pbch_index, rxData_real, SHUFFLE_GATHER, sequence_length);
  vshuffle(rxSym_imag, pbch_index, rxData_imag, SHUFFLE_GATHER, sequence_length);

  __v14336i8 Hest_real;
  __v14336i8 Hest_imag;

  vclaim(Hest_real);
  vclaim(Hest_imag);
  vshuffle(Hest_real, pbch_index, H_real, SHUFFLE_GATHER, sequence_length);
  vshuffle(Hest_imag, pbch_index, H_imag, SHUFFLE_GATHER, sequence_length);

  /*--------------------Equalization--------------------*/
  __v14336i8 csi;
  __v14336i8 csi_tmp1;
  __v14336i8 csi_tmp2;

  vsetshamt(fractionLength);
  csi_tmp1 = vmul(Hest_real, Hest_real, MASKREAD_OFF, sequence_length);
  csi_tmp2 = vmul(Hest_imag, Hest_imag, MASKREAD_OFF, sequence_length);
  vsetshamt(0);

  csi = vadd(csi_tmp1, csi_tmp2, MASKREAD_OFF, sequence_length);
  csi = vadd(csi, nVar, MASKREAD_OFF, sequence_length);

  __v14336i8 pbchEq_real;
  __v14336i8 pbchEq_imag;
  __v14336i8 pbchEq_tmp1;
  __v14336i8 pbchEq_tmp2;
  __v14336i8 pbchEq_tmp3;
  __v14336i8 pbchEq_tmp4;

  vsetshamt(fractionLength);
  Hest_real = vdiv(csi, Hest_real, MASKREAD_OFF, sequence_length);
  Hest_imag = vdiv(csi, Hest_imag, MASKREAD_OFF, sequence_length);
  vsetshamt(0);

  vsetshamt(fractionLength);
  pbchEq_tmp1 = vmul(Hest_real, rxSym_real, MASKREAD_OFF, sequence_length);
  pbchEq_tmp2 = vmul(Hest_imag, rxSym_imag, MASKREAD_OFF, sequence_length);
  vsetshamt(0);
  pbchEq_real = vadd(pbchEq_tmp1, pbchEq_tmp2, MASKREAD_OFF, sequence_length);

  vsetshamt(fractionLength);
  pbchEq_tmp3 = vmul(Hest_real, rxSym_imag, MASKREAD_OFF, sequence_length);
  pbchEq_tmp4 = vmul(Hest_imag, rxSym_real, MASKREAD_OFF, sequence_length);
  vsetshamt(0);
  pbchEq_imag = vsub(pbchEq_tmp4, pbchEq_tmp3, MASKREAD_OFF, sequence_length);

  vreturn(pbchEq_real, sequence_length, pbchEq_imag, sequence_length, csi, sequence_length);
}