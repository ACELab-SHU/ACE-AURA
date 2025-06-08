#include "data_type.h"
#include "riscv_printf.h"
#include "venus.h"

typedef short __v4096i16 __attribute__((ext_vector_type(4096)));
typedef short __v2048i16 __attribute__((ext_vector_type(2048)));
typedef char  __v4096i8 __attribute__((ext_vector_type(4096)));

#define tabLength 31
#define INF       0x7F

int dmrs_singleA[14 * 4 * 4] = {
    INF, INF, INF, INF, INF, INF, INF, INF,
    INF, INF, INF, INF, INF, INF, INF, INF, //  1 symbol duration
    INF, INF, INF, INF, INF, INF, INF, INF,
    INF, INF, INF, INF, INF, INF, INF, INF, //  2 symbol duration
    0,   0,   0,   0,   0,   0,   0,   0,
    0,   0,   0,   0,   0,   0,   0,   0, //  3 symbol duration
    0,   0,   0,   0,   0,   0,   0,   0,
    0,   0,   0,   0,   0,   0,   0,   0, //  4 symbol duration
    0,   0,   0,   0,   0,   0,   0,   0,
    0,   0,   0,   0,   0,   0,   0,   0, //  5 symbol duration
    0,   0,   0,   0,   0,   0,   0,   0,
    0,   0,   0,   0,   0,   0,   0,   0, //  6 symbol duration
    0,   0,   0,   0,   0,   0,   0,   0,
    0,   0,   0,   0,   0,   0,   0,   0, //  7 symbol duration
    0,   0,   0,   0,   0,   7,   INF, INF,
    0,   7,   INF, INF, 0,   7,   INF, INF, //  8 symbol duration
    0,   0,   0,   0,   0,   7,   INF, INF,
    0,   7,   INF, INF, 0,   7,   INF, INF, //  9 symbol duration
    0,   0,   0,   0,   0,   9,   INF, INF,
    0,   6,   9,   INF, 0,   6,   9,   INF, // 10 symbol duration
    0,   0,   0,   0,   0,   9,   INF, INF,
    0,   6,   9,   INF, 0,   6,   9,   INF, // 11 symbol duration
    0,   0,   0,   0,   0,   9,   INF, INF,
    0,   6,   9,   INF, 0,   5,   8,   11, // 12 symbol duration
    0,   0,   0,   0,   0,   11,  INF, INF,
    0,   7,   11,  INF, 0,   5,   8,   11, // 13 symbol duration,  -1 represents l_1 = 11/12
    0,   0,   0,   0,   0,   11,  INF, INF,
    0,   7,   11,  INF, 0,   5,   8,   11 // 14 symbol duration,  -1 represents l_1 = 11/12
};

int dmrs_singleB[14 * 4 * 4] = {
    INF, INF, INF, INF, INF, INF, INF, INF,
    INF, INF, INF, INF, INF, INF, INF, INF, //  1 symbol duration
    0,   0,   0,   0,   0,   0,   0,   0,
    0,   0,   0,   0,   0,   0,   0,   0, //  2 symbol duration
    0,   0,   0,   0,   0,   0,   0,   0,
    0,   0,   0,   0,   0,   0,   0,   0, //  3 symbol duration
    0,   0,   0,   0,   0,   0,   0,   0,
    0,   0,   0,   0,   0,   0,   0,   0, //  4 symbol duration
    0,   0,   0,   0,   0,   4,   INF, INF,
    0,   4,   INF, INF, 0,   4,   INF, INF, //  5 symbol duration
    0,   0,   0,   0,   0,   4,   INF, INF,
    0,   4,   INF, INF, 0,   4,   INF, INF, //  6 symbol duration (extended CP, half slot)
    0,   0,   0,   0,   0,   4,   INF, INF,
    0,   4,   INF, INF, 0,   4,   INF, INF, //  7 symbol duration (normal CP, half slot)
    0,   0,   0,   0,   0,   6,   INF, INF,
    0,   3,   6,   INF, 0,   3,   6,   INF, //  8 symbol duration
    0,   0,   0,   0,   0,   7,   INF, INF,
    0,   4,   7,   INF, 0,   4,   7,   INF, //  9 symbol duration
    0,   0,   0,   0,   0,   7,   INF, INF,
    0,   4,   7,   INF, 0,   4,   7,   INF, // 10 symbol duration
    0,   0,   0,   0,   0,   8,   INF, INF,
    0,   4,   8,   INF, 0,   3,   6,   9, // 11 symbol duration
    0,   0,   0,   0,   0,   9,   INF, INF,
    0,   5,   9,   INF, 0,   3,   6,   9, // 12 symbol duration
    0,   0,   0,   0,   0,   9,   INF, INF,
    0,   5,   9,   INF, 0,   3,   6,   9, // 13 symbol duration
    INF, INF, INF, INF, INF, INF, INF, INF,
    INF, INF, INF, INF, INF, INF, INF, INF // 14 symbol duration
};

int dmrs_doubleA[14 * 2 * 4] = {
    INF, INF, INF, INF, INF, INF, INF, INF, //  1 symbol duration
    INF, INF, INF, INF, INF, INF, INF, INF, //  2 symbol duration
    INF, INF, INF, INF, INF, INF, INF, INF, //  3 symbol duration
    0,   0,   0,   0,   0,   0,   0,   0,   //  4 symbol duration
    0,   0,   0,   0,   0,   0,   0,   0,   //  5 symbol duration
    0,   0,   0,   0,   0,   0,   0,   0,   //  6 symbol duration
    0,   0,   0,   0,   0,   0,   0,   0,   //  7 symbol duration
    0,   0,   0,   0,   0,   0,   0,   0,   //  8 symbol duration
    0,   0,   0,   0,   0,   0,   0,   0,   //  9 symbol duration
    0,   0,   0,   0,   0,   8,   INF, INF, // 10 symbol duration
    0,   0,   0,   0,   0,   8,   INF, INF, // 11 symbol duration
    0,   0,   0,   0,   0,   8,   INF, INF, // 12 symbol duration
    0,   0,   0,   0,   0,   10,  INF, INF, // 13 symbol duration
    0,   0,   0,   0,   0,   10,  INF, INF  // 14 symbol duration
};

int dmrs_doubleB[14 * 2 * 4] = {
    INF, INF, INF, INF, INF, INF, INF, INF, //  1 symbol duration
    INF, INF, INF, INF, INF, INF, INF, INF, //  2 symbol duration
    INF, INF, INF, INF, INF, INF, INF, INF, //  3 symbol duration
    INF, INF, INF, INF, INF, INF, INF, INF, //  4 symbol duration
    0,   0,   0,   0,   0,   0,   0,   0,   //  5 symbol duration -
    0,   0,   0,   0,   0,   0,   0,   0,   //  6 symbol duration R15 (ECP, half slot)
    0,   0,   0,   0,   0,   0,   0,   0,   //  7 symbol duration R15 (NCP, half slot)
    0,   0,   0,   0,   0,   5,   INF, INF, //  8 symbol duration -
    0,   0,   0,   0,   0,   5,   INF, INF, //  9 symbol duration -
    0,   0,   0,   0,   0,   7,   INF, INF, // 10 symbol duration -
    0,   0,   0,   0,   0,   7,   INF, INF, // 11 symbol duration -
    0,   0,   0,   0,   0,   8,   INF, INF, // 12 symbol duration -
    0,   0,   0,   0,   0,   8,   INF, INF, // 13 symbol duration -
    INF, INF, INF, INF, INF, INF, INF, INF  // 14 symbol duration
};

#define recpSqrt2 0.70710678

// typedef struct {
//   short data;
// } __attribute__((aligned(64))) short_struct;

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
       __v2048i16 seq2_trans_table_6_vec, int sequenceLength) {
  /*--------------------DMRS GENERATION--------------------*/
  // gengrate init 31 seq
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

  for (int i = 1; i < ((sequenceLength + tabLength - 1) / tabLength); i++) {
    vshuffle(seq2_trans_0_tmp, seq2_trans_table_0_vec, seq2_tmp, SHUFFLE_GATHER, tabLength);
    vshuffle(seq2_trans_1_tmp, seq2_trans_table_1_vec, seq2_tmp, SHUFFLE_GATHER, tabLength);
    vshuffle(seq2_trans_2_tmp, seq2_trans_table_2_vec, seq2_tmp, SHUFFLE_GATHER, tabLength);
    vshuffle(seq2_trans_3_tmp, seq2_trans_table_3_vec, seq2_tmp, SHUFFLE_GATHER, tabLength);
    vshuffle(seq2_trans_4_tmp, seq2_trans_table_4_vec, seq2_tmp, SHUFFLE_GATHER, tabLength);
    vshuffle(seq2_trans_5_tmp, seq2_trans_table_5_vec, seq2_tmp, SHUFFLE_GATHER, tabLength);
    vshuffle(seq2_trans_6_tmp, seq2_trans_table_6_vec, seq2_tmp, SHUFFLE_GATHER, tabLength);
    vbrdcst(seq2_tmp, 0, MASKREAD_OFF, tabLength);
    seq2_tmp           = vxor(seq2_trans_0_tmp, seq2_trans_1_tmp, MASKREAD_OFF, tabLength);
    seq2_tmp           = vxor(seq2_tmp, seq2_trans_2_tmp, MASKREAD_OFF, tabLength);
    seq2_tmp           = vxor(seq2_tmp, seq2_trans_3_tmp, MASKREAD_OFF, tabLength);
    seq2_tmp           = vxor(seq2_tmp, seq2_trans_4_tmp, MASKREAD_OFF, tabLength);
    seq2_tmp           = vxor(seq2_tmp, seq2_trans_5_tmp, MASKREAD_OFF, tabLength);
    seq2_tmp           = vxor(seq2_tmp, seq2_trans_6_tmp, MASKREAD_OFF, tabLength);
    seq2_shuffle_index = vsadd(seq2_shuffle_index, tabLength, MASKREAD_OFF, tabLength);
    vshuffle(seq2_vec, seq2_shuffle_index, seq2_tmp, SHUFFLE_SCATTER, tabLength);
  }

  // generate dmrs
  __v4096i8 seq;
  seq = vxor(seq1_vec, seq2_vec, MASKREAD_OFF, sequenceLength);
  seq = vmul(seq, -2, MASKREAD_OFF, sequenceLength);
  seq = vsadd(seq, 1, MASKREAD_OFF, sequenceLength);
  // seq = vaddmul(seq, -2, 1, MASKREAD_OFF, sequenceLength);
  return seq;
}

int Task_nrPDSCHDMRS(
    nrPDSCHConfig pdsch, nrCarrierConfig carrier, __v4096i8 seq1_vec, __v2048i16 seq2_init_table_0_vec,
    __v2048i16 seq2_init_table_1_vec, __v2048i16 seq2_init_table_2_vec, __v2048i16 seq2_init_table_3_vec,
    __v2048i16 seq2_init_table_4_vec, __v2048i16 seq2_init_table_5_vec, __v2048i16 seq2_init_table_6_vec,
    __v2048i16 seq2_init_table_7_vec, __v2048i16 seq2_init_table_8_vec, __v2048i16 seq2_init_table_9_vec,
    __v2048i16 seq2_init_table_10_vec, __v2048i16 seq2_init_table_11_vec, __v2048i16 seq2_init_table_12_vec,
    __v2048i16 seq2_init_table_13_vec, __v2048i16 seq2_init_table_14_vec, __v2048i16 seq2_init_table_15_vec,
    __v2048i16 seq2_init_table_16_vec, __v2048i16 seq2_init_table_17_vec, __v2048i16 seq2_init_table_18_vec,
    __v2048i16 seq2_init_table_19_vec, __v2048i16 seq2_trans_table_0_vec, __v2048i16 seq2_trans_table_1_vec,
    __v2048i16 seq2_trans_table_2_vec, __v2048i16 seq2_trans_table_3_vec, __v2048i16 seq2_trans_table_4_vec,
    __v2048i16 seq2_trans_table_5_vec, __v2048i16 seq2_trans_table_6_vec) {
  __v2048i16 dmrs_shuffle_index;
  __v4096i8  dmrs_pdsch_real;
  __v4096i8  dmrs_pdsch_imag;
  __v4096i8  dmrs_seq;
  __v4096i8  dmrs_seq0;
  __v4096i8  dmrs_real_seq;
  __v4096i8  dmrs_imag_seq;
  __v4096i8  init_vec;
  vclaim(init_vec);
  vclaim(dmrs_shuffle_index);
  vclaim(dmrs_seq);
  vclaim(dmrs_seq0);
  vclaim(dmrs_real_seq);
  vclaim(dmrs_imag_seq);
  vclaim(dmrs_pdsch_real);
  vclaim(dmrs_pdsch_imag);

  int dmrssymbolset[4 * 2];

  uint16_t pdsch_NSizeBWP  = pdsch.NSizeBWP;
  uint16_t pdsch_NStartBWP = pdsch.NStartBWP;
  uint16_t nSizeGrid       = carrier.NSizeGrid;
  uint16_t nStartGrid      = carrier.NStartGrid;
  uint16_t nSizeBWP        = (pdsch_NSizeBWP == INF) ? nSizeGrid : pdsch_NSizeBWP;
  uint16_t nStartBWP       = (pdsch_NStartBWP == INF) ? nStartGrid : pdsch_NStartBWP;

  // lookupPDSCHDMRSSymbols
  uint16_t nsymbols;
  uint16_t ub, lb;
  uint16_t symbolSize   = pdsch.SymbolAllocation[1];
  uint16_t symbolStart  = pdsch.SymbolAllocation[0];
  uint16_t symbolEnd    = symbolStart + symbolSize - 1;
  uint16_t dmrsAddPos   = pdsch.DMRS.DMRSAdditionalPosition;
  uint16_t dmrsTypeAPos = pdsch.DMRS.DMRSTypeAPosition;
  uint16_t dmrsLength   = pdsch.DMRS.DMRSLength;
  uint16_t PRBSet[52];
  uint16_t PRBSetLength = pdsch.PRBSet_length;
  for (int i = 0; i < PRBSetLength; i++)
    PRBSet[i] = pdsch.PRBSet[i];

  ub = symbolEnd;
  if (pdsch.MappingType == 0) {
    lb = 0;
  } else {
    lb = symbolStart;
  }
  nsymbols = ub - lb + 1;

  if (pdsch.MappingType == 0) {
    if (pdsch.DMRS.DMRSLength == 1) {
      for (int i = 0; i < 4; i++)
        dmrssymbolset[i] = dmrs_singleA[(nsymbols - 1) * 4 * 4 + 4 * dmrsAddPos + i];
    } else {
      for (int i = 0; i < 4; i++)
        dmrssymbolset[i] = dmrs_doubleA[(nsymbols - 1) * 4 * 4 + 4 * dmrsAddPos + i];
    }
  } else {
    if (pdsch.DMRS.DMRSLength == 1) {
      for (int i = 0; i < 4; i++)
        dmrssymbolset[i] = dmrs_singleB[(nsymbols - 1) * 2 * 4 + 4 * dmrsAddPos + i];
    } else {
      for (int i = 0; i < 4; i++)
        dmrssymbolset[i] = dmrs_doubleB[(nsymbols - 1) * 2 * 4 + 4 * dmrsAddPos + i];
    }
  }

  if (pdsch.MappingType == 1) {
    for (int i = 0; i < 4; i++)
      dmrssymbolset[i] += symbolStart; // If type B (non slot-wise)
  } else {
    dmrssymbolset[0] = dmrsTypeAPos; // If type A (slot-wise) then 2 or 3
  }
  // Adjust for double-symbol DM-RS
  // In the operational system, if RRC configured with max length 2
  // then the actual length is DCI signaled
  uint16_t ldash[8] = {INF};
  if (pdsch.DMRS.DMRSLength == 2) {
    uint16_t dmrssymbolset1[4];
    uint16_t dmrssymbolset2[4];
    for (int i = 0; i < 4; i++) {
      dmrssymbolset1[i] = dmrssymbolset[i];
      dmrssymbolset2[i] = (dmrssymbolset[i] == INF) ? INF : (dmrssymbolset[i] + 1);
    }
    for (int i = 0; i < 4; i++) {
      dmrssymbolset[i]     = dmrssymbolset1[i];
      dmrssymbolset[1 + i] = dmrssymbolset2[i];
    }

    for (int i = 1; i < 4; i++)
      ldash[i] = (dmrssymbolset[i] != INF) ? 1 : INF;
  }
  // For non-standard set-ups, only return the DM-RS symbol indices
  // that overlap the actual allocation indices
  // logicalMatrix = repmat(symbolset(:),1,numel(dmrssymbolset)) == repmat(dmrssymbolset,numel(symbolset),1);
  // ind = sum(logicalMatrix,1)==1;
  // dmrssymbolset = dmrssymbolset(ind);
  // ldash = ldash(ind);
  // for (int i = 0; i < 4; i++)
  // {
  //     if (dmrssymbolset[i] != INF)
  //     {

  //     }
  // }

  if (pdsch.MappingType == 1 && ((carrier.SymbolsPerSlot == 14) && pdsch.SymbolAllocation[1] == 6) ||
      ((carrier.SymbolsPerSlot == 12) && pdsch.SymbolAllocation[1] == 7))
    for (int i = 0; i < 4; i++)
      dmrssymbolset[i] = INF;

  /*---------------------------------------------------*/

  /* reserved RB */

  /*---------------------------------------------------*/
  // uint16_t ports = pdsch.DMRS.DMRSPortSet;

  __v2048i16 prbcell;
  vclaim(prbcell);

  uint16_t prbrefpoint      = (pdsch.DMRS.DMRSReferencePoint == CRB0) ? nStartBWP : 0;
  uint16_t ndmrsre          = 4 + (pdsch.DMRS.DMRSConfigurationType == 1) * 2;
  uint16_t nidnscid         = carrier.NCellID;
  uint16_t port2baseseq     = 1;                // All ports map to the same base DM-RS sequence set
  uint16_t basesequenceidxs = 1;                // Single sequence set for all ports
  uint16_t nscid            = pdsch.DMRS.NSCID; // Get the NSCID value

  // input parameters: cinit(need ibar_SSB and ncellid)
  int fractionLength = 7;
  int sequenceLength = ndmrsre * 2 * (prbrefpoint + PRBSet[PRBSetLength - 1] + 1);
  int prbsLength     = ndmrsre * 2 * (PRBSet[PRBSetLength - 1] - PRBSet[0] + 1);
  //   __v4096i8 seq1_vec;
  //   vclaim(seq1_vec);
  //   vbarrier();
  //   VSPM_OPEN();
  //   int seq1_vec_addr = vaddr(seq1_vec);
  //   // TODO:修改数据输入方式
  //   for (int i = 0; i < sequenceLength; ++i) {
  //     *(volatile char *)(seq1_vec_addr + i) = seq1[i];
  //   }
  //   VSPM_CLOSE();

  // Included the empty check to avoid run-time error in codegen
  // for empty PRB set with reshape function
  uint16_t portcdmgroup      = 0;
  uint16_t dmrsSymbolIdxSize = 0;
  uint16_t nslot             = carrier.NSlot % carrier.SlotsPerFrame;
  if (dmrsLength == 1) {
    if (dmrssymbolset[0] != INF) {
      unsigned int cinit = ((1 << 17) * (carrier.SymbolsPerSlot * nslot + dmrssymbolset[0] + 1) * (2 * nidnscid + 1) +
                            ((portcdmgroup / 2) << 17) + 2 * nidnscid + nscid) %
                           (1 << 31);
      vbrdcst(init_vec, 0, MASKREAD_OFF, 32);
      vbrdcst(init_vec, 0, MASKREAD_OFF, 32);
      vbrdcst(init_vec, 0, MASKREAD_OFF, 32);
      vbrdcst(init_vec, 0, MASKREAD_OFF, 32);
      vbarrier();
      VSPM_OPEN();
      int init_addr = vaddr(init_vec);
      for (int i = 0; i < 31; ++i) {
        *(volatile char *)(init_addr + i) = cinit & 0b1;
        cinit                             = cinit >> 1;
      }
      VSPM_CLOSE();

      /*--------------------DMRS GENERATION--------------------*/

      dmrs_seq0 = nrPRBS(seq1_vec, init_vec, seq2_init_table_0_vec, seq2_init_table_1_vec, seq2_init_table_2_vec,
                         seq2_init_table_3_vec, seq2_init_table_4_vec, seq2_init_table_5_vec, seq2_init_table_6_vec,
                         seq2_init_table_7_vec, seq2_init_table_8_vec, seq2_init_table_9_vec, seq2_init_table_10_vec,
                         seq2_init_table_11_vec, seq2_init_table_12_vec, seq2_init_table_13_vec, seq2_init_table_14_vec,
                         seq2_init_table_15_vec, seq2_init_table_16_vec, seq2_init_table_17_vec, seq2_init_table_18_vec,
                         seq2_init_table_19_vec, seq2_trans_table_0_vec, seq2_trans_table_1_vec, seq2_trans_table_2_vec,
                         seq2_trans_table_3_vec, seq2_trans_table_4_vec, seq2_trans_table_5_vec, seq2_trans_table_6_vec,
                         sequenceLength);

      vrange(dmrs_shuffle_index, prbsLength);
      dmrs_shuffle_index = vsadd(dmrs_shuffle_index, sequenceLength - prbsLength, MASKREAD_OFF, prbsLength);
      vshuffle(dmrs_seq, dmrs_shuffle_index, dmrs_seq0, SHUFFLE_GATHER, prbsLength);

      vrange(dmrs_shuffle_index, prbsLength / 2);
      dmrs_shuffle_index = vmul(dmrs_shuffle_index, 2, MASKREAD_OFF, prbsLength / 2);
      vshuffle(dmrs_real_seq, dmrs_shuffle_index, dmrs_seq, SHUFFLE_GATHER, prbsLength / 2);
      dmrs_shuffle_index = vsadd(dmrs_shuffle_index, 1, MASKREAD_OFF, prbsLength / 2);
      vshuffle(dmrs_imag_seq, dmrs_shuffle_index, dmrs_seq, SHUFFLE_GATHER, prbsLength / 2);

      vrange(dmrs_shuffle_index, prbsLength / 2);
      dmrs_shuffle_index = vsadd(dmrs_shuffle_index, prbsLength * dmrsSymbolIdxSize / 2, MASKREAD_OFF, prbsLength / 2);
      vshuffle(dmrs_pdsch_real, dmrs_shuffle_index, dmrs_real_seq, SHUFFLE_GATHER, prbsLength / 2);
      vshuffle(dmrs_pdsch_imag, dmrs_shuffle_index, dmrs_imag_seq, SHUFFLE_SCATTER, prbsLength / 2);

      dmrsSymbolIdxSize++;
    }
    if (dmrssymbolset[1] != INF) {
      unsigned int cinit = ((1 << 17) * (carrier.SymbolsPerSlot * nslot + dmrssymbolset[1] + 1) * (2 * nidnscid + 1) +
                            ((portcdmgroup / 2) << 17) + 2 * nidnscid + nscid) %
                           (1 << 31);
      vbrdcst(init_vec, 0, MASKREAD_OFF, 32);
      vbrdcst(init_vec, 0, MASKREAD_OFF, 32);
      vbrdcst(init_vec, 0, MASKREAD_OFF, 32);
      vbrdcst(init_vec, 0, MASKREAD_OFF, 32);
      vbarrier();
      VSPM_OPEN();
      int init_addr = vaddr(init_vec);
      for (int i = 0; i < 31; ++i) {
        *(volatile char *)(init_addr + i) = cinit & 0b1;
        cinit                             = cinit >> 1;
      }
      VSPM_CLOSE();

      /*--------------------DMRS GENERATION--------------------*/
      dmrs_seq0 = nrPRBS(seq1_vec, init_vec, seq2_init_table_0_vec, seq2_init_table_1_vec, seq2_init_table_2_vec,
                         seq2_init_table_3_vec, seq2_init_table_4_vec, seq2_init_table_5_vec, seq2_init_table_6_vec,
                         seq2_init_table_7_vec, seq2_init_table_8_vec, seq2_init_table_9_vec, seq2_init_table_10_vec,
                         seq2_init_table_11_vec, seq2_init_table_12_vec, seq2_init_table_13_vec, seq2_init_table_14_vec,
                         seq2_init_table_15_vec, seq2_init_table_16_vec, seq2_init_table_17_vec, seq2_init_table_18_vec,
                         seq2_init_table_19_vec, seq2_trans_table_0_vec, seq2_trans_table_1_vec, seq2_trans_table_2_vec,
                         seq2_trans_table_3_vec, seq2_trans_table_4_vec, seq2_trans_table_5_vec, seq2_trans_table_6_vec,
                         sequenceLength);

      vrange(dmrs_shuffle_index, prbsLength);
      dmrs_shuffle_index = vsadd(dmrs_shuffle_index, sequenceLength - prbsLength, MASKREAD_OFF, prbsLength);
      vshuffle(dmrs_seq, dmrs_shuffle_index, dmrs_seq0, SHUFFLE_GATHER, prbsLength);

      vrange(dmrs_shuffle_index, prbsLength / 2);
      dmrs_shuffle_index = vmul(dmrs_shuffle_index, 2, MASKREAD_OFF, prbsLength / 2);
      vshuffle(dmrs_real_seq, dmrs_shuffle_index, dmrs_seq, SHUFFLE_GATHER, prbsLength / 2);
      dmrs_shuffle_index = vsadd(dmrs_shuffle_index, 1, MASKREAD_OFF, prbsLength / 2);
      vshuffle(dmrs_imag_seq, dmrs_shuffle_index, dmrs_seq, SHUFFLE_GATHER, prbsLength / 2);

      vrange(dmrs_shuffle_index, prbsLength / 2);
      dmrs_shuffle_index = vsadd(dmrs_shuffle_index, prbsLength * dmrsSymbolIdxSize / 2, MASKREAD_OFF, prbsLength / 2);
      vshuffle(dmrs_pdsch_real, dmrs_shuffle_index, dmrs_real_seq, SHUFFLE_SCATTER, prbsLength / 2);
      vshuffle(dmrs_pdsch_imag, dmrs_shuffle_index, dmrs_imag_seq, SHUFFLE_SCATTER, prbsLength / 2);
      dmrsSymbolIdxSize++;
    }
    if (dmrssymbolset[2] != INF) {
      unsigned int cinit = ((1 << 17) * (carrier.SymbolsPerSlot * nslot + dmrssymbolset[2] + 1) * (2 * nidnscid + 1) +
                            ((portcdmgroup / 2) << 17) + 2 * nidnscid + nscid) %
                           (1 << 31);
      vbrdcst(init_vec, 0, MASKREAD_OFF, 32);
      vbrdcst(init_vec, 0, MASKREAD_OFF, 32);
      vbrdcst(init_vec, 0, MASKREAD_OFF, 32);
      vbrdcst(init_vec, 0, MASKREAD_OFF, 32);
      vbarrier();
      VSPM_OPEN();
      int init_addr = vaddr(init_vec);
      for (int i = 0; i < 31; ++i) {
        *(volatile char *)(init_addr + i) = cinit & 0b1;
        cinit                             = cinit >> 1;
      }
      VSPM_CLOSE();

      /*--------------------DMRS GENERATION--------------------*/
      dmrs_seq0 = nrPRBS(seq1_vec, init_vec, seq2_init_table_0_vec, seq2_init_table_1_vec, seq2_init_table_2_vec,
                         seq2_init_table_3_vec, seq2_init_table_4_vec, seq2_init_table_5_vec, seq2_init_table_6_vec,
                         seq2_init_table_7_vec, seq2_init_table_8_vec, seq2_init_table_9_vec, seq2_init_table_10_vec,
                         seq2_init_table_11_vec, seq2_init_table_12_vec, seq2_init_table_13_vec, seq2_init_table_14_vec,
                         seq2_init_table_15_vec, seq2_init_table_16_vec, seq2_init_table_17_vec, seq2_init_table_18_vec,
                         seq2_init_table_19_vec, seq2_trans_table_0_vec, seq2_trans_table_1_vec, seq2_trans_table_2_vec,
                         seq2_trans_table_3_vec, seq2_trans_table_4_vec, seq2_trans_table_5_vec, seq2_trans_table_6_vec,
                         sequenceLength);

      vrange(dmrs_shuffle_index, prbsLength);
      dmrs_shuffle_index = vsadd(dmrs_shuffle_index, sequenceLength - prbsLength, MASKREAD_OFF, prbsLength);
      vshuffle(dmrs_seq, dmrs_shuffle_index, dmrs_seq0, SHUFFLE_GATHER, prbsLength);

      vrange(dmrs_shuffle_index, prbsLength / 2);
      dmrs_shuffle_index = vmul(dmrs_shuffle_index, 2, MASKREAD_OFF, prbsLength / 2);
      vshuffle(dmrs_real_seq, dmrs_shuffle_index, dmrs_seq, SHUFFLE_GATHER, prbsLength / 2);
      dmrs_shuffle_index = vsadd(dmrs_shuffle_index, 1, MASKREAD_OFF, prbsLength / 2);
      vshuffle(dmrs_imag_seq, dmrs_shuffle_index, dmrs_seq, SHUFFLE_GATHER, prbsLength / 2);

      vrange(dmrs_shuffle_index, prbsLength / 2);
      dmrs_shuffle_index = vsadd(dmrs_shuffle_index, prbsLength * dmrsSymbolIdxSize / 2, MASKREAD_OFF, prbsLength / 2);
      vshuffle(dmrs_pdsch_real, dmrs_shuffle_index, dmrs_real_seq, SHUFFLE_SCATTER, prbsLength / 2);
      vshuffle(dmrs_pdsch_imag, dmrs_shuffle_index, dmrs_imag_seq, SHUFFLE_SCATTER, prbsLength / 2);
      dmrsSymbolIdxSize++;
    }
    if (dmrssymbolset[3] != INF) {
      unsigned int cinit = ((1 << 17) * (carrier.SymbolsPerSlot * nslot + dmrssymbolset[3] + 1) * (2 * nidnscid + 1) +
                            ((portcdmgroup / 2) << 17) + 2 * nidnscid + nscid) %
                           (1 << 31);
      vbrdcst(init_vec, 0, MASKREAD_OFF, 32);
      vbrdcst(init_vec, 0, MASKREAD_OFF, 32);
      vbrdcst(init_vec, 0, MASKREAD_OFF, 32);
      vbrdcst(init_vec, 0, MASKREAD_OFF, 32);
      vbarrier();
      VSPM_OPEN();
      int init_addr = vaddr(init_vec);
      for (int i = 0; i < 31; ++i) {
        *(volatile char *)(init_addr + i) = cinit & 0b1;
        cinit                             = cinit >> 1;
      }
      VSPM_CLOSE();

      /*--------------------DMRS GENERATION--------------------*/
      dmrs_seq0 = nrPRBS(seq1_vec, init_vec, seq2_init_table_0_vec, seq2_init_table_1_vec, seq2_init_table_2_vec,
                         seq2_init_table_3_vec, seq2_init_table_4_vec, seq2_init_table_5_vec, seq2_init_table_6_vec,
                         seq2_init_table_7_vec, seq2_init_table_8_vec, seq2_init_table_9_vec, seq2_init_table_10_vec,
                         seq2_init_table_11_vec, seq2_init_table_12_vec, seq2_init_table_13_vec, seq2_init_table_14_vec,
                         seq2_init_table_15_vec, seq2_init_table_16_vec, seq2_init_table_17_vec, seq2_init_table_18_vec,
                         seq2_init_table_19_vec, seq2_trans_table_0_vec, seq2_trans_table_1_vec, seq2_trans_table_2_vec,
                         seq2_trans_table_3_vec, seq2_trans_table_4_vec, seq2_trans_table_5_vec, seq2_trans_table_6_vec,
                         sequenceLength);

      vrange(dmrs_shuffle_index, prbsLength);
      dmrs_shuffle_index = vsadd(dmrs_shuffle_index, sequenceLength - prbsLength, MASKREAD_OFF, prbsLength);
      vshuffle(dmrs_seq, dmrs_shuffle_index, dmrs_seq0, SHUFFLE_GATHER, prbsLength);

      vrange(dmrs_shuffle_index, prbsLength / 2);
      dmrs_shuffle_index = vmul(dmrs_shuffle_index, 2, MASKREAD_OFF, prbsLength / 2);
      vshuffle(dmrs_real_seq, dmrs_shuffle_index, dmrs_seq, SHUFFLE_GATHER, prbsLength / 2);
      dmrs_shuffle_index = vsadd(dmrs_shuffle_index, 1, MASKREAD_OFF, prbsLength / 2);
      vshuffle(dmrs_imag_seq, dmrs_shuffle_index, dmrs_seq, SHUFFLE_GATHER, prbsLength / 2);

      vrange(dmrs_shuffle_index, prbsLength / 2);
      dmrs_shuffle_index = vsadd(dmrs_shuffle_index, prbsLength * dmrsSymbolIdxSize / 2, MASKREAD_OFF, prbsLength / 2);
      vshuffle(dmrs_pdsch_real, dmrs_shuffle_index, dmrs_real_seq, SHUFFLE_SCATTER, prbsLength / 2);
      vshuffle(dmrs_pdsch_imag, dmrs_shuffle_index, dmrs_imag_seq, SHUFFLE_SCATTER, prbsLength / 2);
      dmrsSymbolIdxSize++;
    }

    dmrs_pdsch_real = vmul(dmrs_pdsch_real, (int)(recpSqrt2 * (1 << fractionLength)), MASKREAD_OFF,
                           prbsLength * dmrsSymbolIdxSize / 2);

    dmrs_pdsch_imag = vmul(dmrs_pdsch_imag, (int)(recpSqrt2 * (1 << fractionLength)), MASKREAD_OFF,
                           prbsLength * dmrsSymbolIdxSize / 2);

  } else if (dmrsLength == 2) {
    if (dmrssymbolset[0] != INF) {
      unsigned int cinit = ((1 << 17) * (carrier.SymbolsPerSlot * nslot + dmrssymbolset[0] + 1) * (2 * nidnscid + 1) +
                            ((portcdmgroup / 2) << 17) + 2 * nidnscid + nscid) %
                           (1 << 31);
      vbrdcst(init_vec, 0, MASKREAD_OFF, 32);
      vbrdcst(init_vec, 0, MASKREAD_OFF, 32);
      vbrdcst(init_vec, 0, MASKREAD_OFF, 32);
      vbrdcst(init_vec, 0, MASKREAD_OFF, 32);
      vbarrier();
      VSPM_OPEN();
      int init_addr = vaddr(init_vec);
      for (int i = 0; i < 31; ++i) {
        *(volatile char *)(init_addr + i) = cinit & 0b1;
        cinit                             = cinit >> 1;
      }
      VSPM_CLOSE();

      /*--------------------DMRS GENERATION--------------------*/
      dmrs_seq0 = nrPRBS(seq1_vec, init_vec, seq2_init_table_0_vec, seq2_init_table_1_vec, seq2_init_table_2_vec,
                         seq2_init_table_3_vec, seq2_init_table_4_vec, seq2_init_table_5_vec, seq2_init_table_6_vec,
                         seq2_init_table_7_vec, seq2_init_table_8_vec, seq2_init_table_9_vec, seq2_init_table_10_vec,
                         seq2_init_table_11_vec, seq2_init_table_12_vec, seq2_init_table_13_vec, seq2_init_table_14_vec,
                         seq2_init_table_15_vec, seq2_init_table_16_vec, seq2_init_table_17_vec, seq2_init_table_18_vec,
                         seq2_init_table_19_vec, seq2_trans_table_0_vec, seq2_trans_table_1_vec, seq2_trans_table_2_vec,
                         seq2_trans_table_3_vec, seq2_trans_table_4_vec, seq2_trans_table_5_vec, seq2_trans_table_6_vec,
                         sequenceLength);

      vrange(dmrs_shuffle_index, prbsLength);
      dmrs_shuffle_index = vsadd(dmrs_shuffle_index, sequenceLength - prbsLength, MASKREAD_OFF, prbsLength);
      vshuffle(dmrs_seq, dmrs_shuffle_index, dmrs_seq0, SHUFFLE_GATHER, prbsLength);

      vrange(dmrs_shuffle_index, prbsLength / 2);
      dmrs_shuffle_index = vmul(dmrs_shuffle_index, 2, MASKREAD_OFF, prbsLength / 2);
      vshuffle(dmrs_real_seq, dmrs_shuffle_index, dmrs_seq, SHUFFLE_GATHER, prbsLength / 2);
      dmrs_shuffle_index = vsadd(dmrs_shuffle_index, 1, MASKREAD_OFF, prbsLength / 2);
      vshuffle(dmrs_imag_seq, dmrs_shuffle_index, dmrs_seq, SHUFFLE_GATHER, prbsLength / 2);

      vrange(dmrs_shuffle_index, prbsLength / 2);
      dmrs_shuffle_index = vsadd(dmrs_shuffle_index, prbsLength * dmrsSymbolIdxSize / 2, MASKREAD_OFF, prbsLength / 2);
      vshuffle(dmrs_pdsch_real, dmrs_shuffle_index, dmrs_real_seq, SHUFFLE_SCATTER, prbsLength / 2);
      vshuffle(dmrs_pdsch_imag, dmrs_shuffle_index, dmrs_imag_seq, SHUFFLE_SCATTER, prbsLength / 2);

      dmrsSymbolIdxSize++;
    }
    if (dmrssymbolset[1] != INF) {
      unsigned int cinit = ((1 << 17) * (carrier.SymbolsPerSlot * nslot + dmrssymbolset[1] + 1) * (2 * nidnscid + 1) +
                            ((portcdmgroup / 2) << 17) + 2 * nidnscid + nscid) %
                           (1 << 31);
      vbrdcst(init_vec, 0, MASKREAD_OFF, 32);
      vbrdcst(init_vec, 0, MASKREAD_OFF, 32);
      vbrdcst(init_vec, 0, MASKREAD_OFF, 32);
      vbrdcst(init_vec, 0, MASKREAD_OFF, 32);
      vbarrier();
      VSPM_OPEN();
      int init_addr = vaddr(init_vec);
      for (int i = 0; i < 31; ++i) {
        *(volatile char *)(init_addr + i) = cinit & 0b1;
        cinit                             = cinit >> 1;
      }
      VSPM_CLOSE();

      /*--------------------DMRS GENERATION--------------------*/
      dmrs_seq0 = nrPRBS(seq1_vec, init_vec, seq2_init_table_0_vec, seq2_init_table_1_vec, seq2_init_table_2_vec,
                         seq2_init_table_3_vec, seq2_init_table_4_vec, seq2_init_table_5_vec, seq2_init_table_6_vec,
                         seq2_init_table_7_vec, seq2_init_table_8_vec, seq2_init_table_9_vec, seq2_init_table_10_vec,
                         seq2_init_table_11_vec, seq2_init_table_12_vec, seq2_init_table_13_vec, seq2_init_table_14_vec,
                         seq2_init_table_15_vec, seq2_init_table_16_vec, seq2_init_table_17_vec, seq2_init_table_18_vec,
                         seq2_init_table_19_vec, seq2_trans_table_0_vec, seq2_trans_table_1_vec, seq2_trans_table_2_vec,
                         seq2_trans_table_3_vec, seq2_trans_table_4_vec, seq2_trans_table_5_vec, seq2_trans_table_6_vec,
                         sequenceLength);

      vrange(dmrs_shuffle_index, prbsLength);
      dmrs_shuffle_index = vsadd(dmrs_shuffle_index, sequenceLength - prbsLength, MASKREAD_OFF, prbsLength);
      vshuffle(dmrs_seq, dmrs_shuffle_index, dmrs_seq0, SHUFFLE_GATHER, prbsLength);

      vrange(dmrs_shuffle_index, prbsLength / 2);
      dmrs_shuffle_index = vmul(dmrs_shuffle_index, 2, MASKREAD_OFF, prbsLength / 2);
      vshuffle(dmrs_real_seq, dmrs_shuffle_index, dmrs_seq, SHUFFLE_GATHER, prbsLength / 2);
      dmrs_shuffle_index = vsadd(dmrs_shuffle_index, 1, MASKREAD_OFF, prbsLength / 2);
      vshuffle(dmrs_imag_seq, dmrs_shuffle_index, dmrs_seq, SHUFFLE_GATHER, prbsLength / 2);

      vrange(dmrs_shuffle_index, prbsLength / 2);
      dmrs_shuffle_index = vsadd(dmrs_shuffle_index, prbsLength * dmrsSymbolIdxSize / 2, MASKREAD_OFF, prbsLength / 2);
      vshuffle(dmrs_pdsch_real, dmrs_shuffle_index, dmrs_real_seq, SHUFFLE_SCATTER, prbsLength / 2);
      vshuffle(dmrs_pdsch_imag, dmrs_shuffle_index, dmrs_imag_seq, SHUFFLE_SCATTER, prbsLength / 2);
      dmrsSymbolIdxSize++;
    }
    if (dmrssymbolset[2] != INF) {
      unsigned int cinit = ((1 << 17) * (carrier.SymbolsPerSlot * nslot + dmrssymbolset[2] + 1) * (2 * nidnscid + 1) +
                            ((portcdmgroup / 2) << 17) + 2 * nidnscid + nscid) %
                           (1 << 31);
      vbrdcst(init_vec, 0, MASKREAD_OFF, 32);
      vbrdcst(init_vec, 0, MASKREAD_OFF, 32);
      vbrdcst(init_vec, 0, MASKREAD_OFF, 32);
      vbrdcst(init_vec, 0, MASKREAD_OFF, 32);
      vbarrier();
      VSPM_OPEN();
      int init_addr = vaddr(init_vec);
      for (int i = 0; i < 31; ++i) {
        *(volatile char *)(init_addr + i) = cinit & 0b1;
        cinit                             = cinit >> 1;
      }
      VSPM_CLOSE();

      /*--------------------DMRS GENERATION--------------------*/
      dmrs_seq0 = nrPRBS(seq1_vec, init_vec, seq2_init_table_0_vec, seq2_init_table_1_vec, seq2_init_table_2_vec,
                         seq2_init_table_3_vec, seq2_init_table_4_vec, seq2_init_table_5_vec, seq2_init_table_6_vec,
                         seq2_init_table_7_vec, seq2_init_table_8_vec, seq2_init_table_9_vec, seq2_init_table_10_vec,
                         seq2_init_table_11_vec, seq2_init_table_12_vec, seq2_init_table_13_vec, seq2_init_table_14_vec,
                         seq2_init_table_15_vec, seq2_init_table_16_vec, seq2_init_table_17_vec, seq2_init_table_18_vec,
                         seq2_init_table_19_vec, seq2_trans_table_0_vec, seq2_trans_table_1_vec, seq2_trans_table_2_vec,
                         seq2_trans_table_3_vec, seq2_trans_table_4_vec, seq2_trans_table_5_vec, seq2_trans_table_6_vec,
                         sequenceLength);

      vrange(dmrs_shuffle_index, prbsLength);
      dmrs_shuffle_index = vsadd(dmrs_shuffle_index, sequenceLength - prbsLength, MASKREAD_OFF, prbsLength);
      vshuffle(dmrs_seq, dmrs_shuffle_index, dmrs_seq0, SHUFFLE_GATHER, prbsLength);

      vrange(dmrs_shuffle_index, prbsLength / 2);
      dmrs_shuffle_index = vmul(dmrs_shuffle_index, 2, MASKREAD_OFF, prbsLength / 2);
      vshuffle(dmrs_real_seq, dmrs_shuffle_index, dmrs_seq, SHUFFLE_GATHER, prbsLength / 2);
      dmrs_shuffle_index = vsadd(dmrs_shuffle_index, 1, MASKREAD_OFF, prbsLength / 2);
      vshuffle(dmrs_imag_seq, dmrs_shuffle_index, dmrs_seq, SHUFFLE_GATHER, prbsLength / 2);

      vrange(dmrs_shuffle_index, prbsLength / 2);
      dmrs_shuffle_index = vsadd(dmrs_shuffle_index, prbsLength * dmrsSymbolIdxSize / 2, MASKREAD_OFF, prbsLength / 2);
      vshuffle(dmrs_pdsch_real, dmrs_shuffle_index, dmrs_real_seq, SHUFFLE_SCATTER, prbsLength / 2);
      vshuffle(dmrs_pdsch_imag, dmrs_shuffle_index, dmrs_imag_seq, SHUFFLE_SCATTER, prbsLength / 2);
      dmrsSymbolIdxSize++;
    }
    if (dmrssymbolset[3] != INF) {
      unsigned int cinit = ((1 << 17) * (carrier.SymbolsPerSlot * nslot + dmrssymbolset[3] + 1) * (2 * nidnscid + 1) +
                            ((portcdmgroup / 2) << 17) + 2 * nidnscid + nscid) %
                           (1 << 31);
      vbrdcst(init_vec, 0, MASKREAD_OFF, 32);
      vbrdcst(init_vec, 0, MASKREAD_OFF, 32);
      vbrdcst(init_vec, 0, MASKREAD_OFF, 32);
      vbrdcst(init_vec, 0, MASKREAD_OFF, 32);
      vbarrier();
      VSPM_OPEN();
      int init_addr = vaddr(init_vec);
      for (int i = 0; i < 31; ++i) {
        *(volatile char *)(init_addr + i) = cinit & 0b1;
        cinit                             = cinit >> 1;
      }
      VSPM_CLOSE();

      /*--------------------DMRS GENERATION--------------------*/
      dmrs_seq0 = nrPRBS(seq1_vec, init_vec, seq2_init_table_0_vec, seq2_init_table_1_vec, seq2_init_table_2_vec,
                         seq2_init_table_3_vec, seq2_init_table_4_vec, seq2_init_table_5_vec, seq2_init_table_6_vec,
                         seq2_init_table_7_vec, seq2_init_table_8_vec, seq2_init_table_9_vec, seq2_init_table_10_vec,
                         seq2_init_table_11_vec, seq2_init_table_12_vec, seq2_init_table_13_vec, seq2_init_table_14_vec,
                         seq2_init_table_15_vec, seq2_init_table_16_vec, seq2_init_table_17_vec, seq2_init_table_18_vec,
                         seq2_init_table_19_vec, seq2_trans_table_0_vec, seq2_trans_table_1_vec, seq2_trans_table_2_vec,
                         seq2_trans_table_3_vec, seq2_trans_table_4_vec, seq2_trans_table_5_vec, seq2_trans_table_6_vec,
                         sequenceLength);

      vrange(dmrs_shuffle_index, prbsLength);
      dmrs_shuffle_index = vsadd(dmrs_shuffle_index, sequenceLength - prbsLength, MASKREAD_OFF, prbsLength);
      vshuffle(dmrs_seq, dmrs_shuffle_index, dmrs_seq0, SHUFFLE_GATHER, prbsLength);

      vrange(dmrs_shuffle_index, prbsLength / 2);
      dmrs_shuffle_index = vmul(dmrs_shuffle_index, 2, MASKREAD_OFF, prbsLength / 2);
      vshuffle(dmrs_real_seq, dmrs_shuffle_index, dmrs_seq, SHUFFLE_GATHER, prbsLength / 2);
      dmrs_shuffle_index = vsadd(dmrs_shuffle_index, 1, MASKREAD_OFF, prbsLength / 2);
      vshuffle(dmrs_imag_seq, dmrs_shuffle_index, dmrs_seq, SHUFFLE_GATHER, prbsLength / 2);

      vrange(dmrs_shuffle_index, prbsLength / 2);
      dmrs_shuffle_index = vsadd(dmrs_shuffle_index, prbsLength * dmrsSymbolIdxSize / 2, MASKREAD_OFF, prbsLength / 2);
      vshuffle(dmrs_pdsch_real, dmrs_shuffle_index, dmrs_real_seq, SHUFFLE_SCATTER, prbsLength / 2);
      vshuffle(dmrs_pdsch_imag, dmrs_shuffle_index, dmrs_imag_seq, SHUFFLE_SCATTER, prbsLength / 2);
      dmrsSymbolIdxSize++;
    }
    if (dmrssymbolset[4] != INF) {
      unsigned int cinit = ((1 << 17) * (carrier.SymbolsPerSlot * nslot + dmrssymbolset[4] + 1) * (2 * nidnscid + 1) +
                            ((portcdmgroup / 2) << 17) + 2 * nidnscid + nscid) %
                           (1 << 31);
      vbrdcst(init_vec, 0, MASKREAD_OFF, 32);
      vbrdcst(init_vec, 0, MASKREAD_OFF, 32);
      vbrdcst(init_vec, 0, MASKREAD_OFF, 32);
      vbrdcst(init_vec, 0, MASKREAD_OFF, 32);
      vbarrier();
      VSPM_OPEN();
      int init_addr = vaddr(init_vec);
      for (int i = 0; i < 31; ++i) {
        *(volatile char *)(init_addr + i) = cinit & 0b1;
        cinit                             = cinit >> 1;
      }
      VSPM_CLOSE();

      /*--------------------DMRS GENERATION--------------------*/
      dmrs_seq0 = nrPRBS(seq1_vec, init_vec, seq2_init_table_0_vec, seq2_init_table_1_vec, seq2_init_table_2_vec,
                         seq2_init_table_3_vec, seq2_init_table_4_vec, seq2_init_table_5_vec, seq2_init_table_6_vec,
                         seq2_init_table_7_vec, seq2_init_table_8_vec, seq2_init_table_9_vec, seq2_init_table_10_vec,
                         seq2_init_table_11_vec, seq2_init_table_12_vec, seq2_init_table_13_vec, seq2_init_table_14_vec,
                         seq2_init_table_15_vec, seq2_init_table_16_vec, seq2_init_table_17_vec, seq2_init_table_18_vec,
                         seq2_init_table_19_vec, seq2_trans_table_0_vec, seq2_trans_table_1_vec, seq2_trans_table_2_vec,
                         seq2_trans_table_3_vec, seq2_trans_table_4_vec, seq2_trans_table_5_vec, seq2_trans_table_6_vec,
                         sequenceLength);

      vrange(dmrs_shuffle_index, prbsLength);
      dmrs_shuffle_index = vsadd(dmrs_shuffle_index, sequenceLength - prbsLength, MASKREAD_OFF, prbsLength);
      vshuffle(dmrs_seq, dmrs_shuffle_index, dmrs_seq0, SHUFFLE_GATHER, prbsLength);

      vrange(dmrs_shuffle_index, prbsLength / 2);
      dmrs_shuffle_index = vmul(dmrs_shuffle_index, 2, MASKREAD_OFF, prbsLength / 2);
      vshuffle(dmrs_real_seq, dmrs_shuffle_index, dmrs_seq, SHUFFLE_GATHER, prbsLength / 2);
      dmrs_shuffle_index = vsadd(dmrs_shuffle_index, 1, MASKREAD_OFF, prbsLength / 2);
      vshuffle(dmrs_imag_seq, dmrs_shuffle_index, dmrs_seq, SHUFFLE_GATHER, prbsLength / 2);

      vrange(dmrs_shuffle_index, prbsLength / 2);
      dmrs_shuffle_index = vsadd(dmrs_shuffle_index, prbsLength * dmrsSymbolIdxSize / 2, MASKREAD_OFF, prbsLength / 2);
      vshuffle(dmrs_pdsch_real, dmrs_shuffle_index, dmrs_real_seq, SHUFFLE_SCATTER, prbsLength / 2);
      vshuffle(dmrs_pdsch_imag, dmrs_shuffle_index, dmrs_imag_seq, SHUFFLE_SCATTER, prbsLength / 2);

      dmrsSymbolIdxSize++;
    }
    if (dmrssymbolset[5] != INF) {
      unsigned int cinit = ((1 << 17) * (carrier.SymbolsPerSlot * nslot + dmrssymbolset[5] + 1) * (2 * nidnscid + 1) +
                            ((portcdmgroup / 2) << 17) + 2 * nidnscid + nscid) %
                           (1 << 31);
      vbrdcst(init_vec, 0, MASKREAD_OFF, 32);
      vbrdcst(init_vec, 0, MASKREAD_OFF, 32);
      vbrdcst(init_vec, 0, MASKREAD_OFF, 32);
      vbrdcst(init_vec, 0, MASKREAD_OFF, 32);
      vbarrier();
      VSPM_OPEN();
      int init_addr = vaddr(init_vec);
      for (int i = 0; i < 31; ++i) {
        *(volatile char *)(init_addr + i) = cinit & 0b1;
        cinit                             = cinit >> 1;
      }
      VSPM_CLOSE();

      /*--------------------DMRS GENERATION--------------------*/
      dmrs_seq0 = nrPRBS(seq1_vec, init_vec, seq2_init_table_0_vec, seq2_init_table_1_vec, seq2_init_table_2_vec,
                         seq2_init_table_3_vec, seq2_init_table_4_vec, seq2_init_table_5_vec, seq2_init_table_6_vec,
                         seq2_init_table_7_vec, seq2_init_table_8_vec, seq2_init_table_9_vec, seq2_init_table_10_vec,
                         seq2_init_table_11_vec, seq2_init_table_12_vec, seq2_init_table_13_vec, seq2_init_table_14_vec,
                         seq2_init_table_15_vec, seq2_init_table_16_vec, seq2_init_table_17_vec, seq2_init_table_18_vec,
                         seq2_init_table_19_vec, seq2_trans_table_0_vec, seq2_trans_table_1_vec, seq2_trans_table_2_vec,
                         seq2_trans_table_3_vec, seq2_trans_table_4_vec, seq2_trans_table_5_vec, seq2_trans_table_6_vec,
                         sequenceLength);

      vrange(dmrs_shuffle_index, prbsLength);
      dmrs_shuffle_index = vsadd(dmrs_shuffle_index, sequenceLength - prbsLength, MASKREAD_OFF, prbsLength);
      vshuffle(dmrs_seq, dmrs_shuffle_index, dmrs_seq0, SHUFFLE_GATHER, prbsLength);

      vrange(dmrs_shuffle_index, prbsLength / 2);
      dmrs_shuffle_index = vmul(dmrs_shuffle_index, 2, MASKREAD_OFF, prbsLength / 2);
      vshuffle(dmrs_real_seq, dmrs_shuffle_index, dmrs_seq, SHUFFLE_GATHER, prbsLength / 2);
      dmrs_shuffle_index = vsadd(dmrs_shuffle_index, 1, MASKREAD_OFF, prbsLength / 2);
      vshuffle(dmrs_imag_seq, dmrs_shuffle_index, dmrs_seq, SHUFFLE_GATHER, prbsLength / 2);

      vrange(dmrs_shuffle_index, prbsLength / 2);
      dmrs_shuffle_index = vsadd(dmrs_shuffle_index, prbsLength * dmrsSymbolIdxSize / 2, MASKREAD_OFF, prbsLength / 2);
      vshuffle(dmrs_pdsch_real, dmrs_shuffle_index, dmrs_real_seq, SHUFFLE_SCATTER, prbsLength / 2);
      vshuffle(dmrs_pdsch_imag, dmrs_shuffle_index, dmrs_imag_seq, SHUFFLE_SCATTER, prbsLength / 2);
      dmrsSymbolIdxSize++;
    }
    if (dmrssymbolset[6] != INF) {
      unsigned int cinit = ((1 << 17) * (carrier.SymbolsPerSlot * nslot + dmrssymbolset[6] + 1) * (2 * nidnscid + 1) +
                            ((portcdmgroup / 2) << 17) + 2 * nidnscid + nscid) %
                           (1 << 31);
      vbrdcst(init_vec, 0, MASKREAD_OFF, 32);
      vbrdcst(init_vec, 0, MASKREAD_OFF, 32);
      vbrdcst(init_vec, 0, MASKREAD_OFF, 32);
      vbrdcst(init_vec, 0, MASKREAD_OFF, 32);
      vbarrier();
      VSPM_OPEN();
      int init_addr = vaddr(init_vec);
      for (int i = 0; i < 31; ++i) {
        *(volatile char *)(init_addr + i) = cinit & 0b1;
        cinit                             = cinit >> 1;
      }
      VSPM_CLOSE();

      /*--------------------DMRS GENERATION--------------------*/
      dmrs_seq0 = nrPRBS(seq1_vec, init_vec, seq2_init_table_0_vec, seq2_init_table_1_vec, seq2_init_table_2_vec,
                         seq2_init_table_3_vec, seq2_init_table_4_vec, seq2_init_table_5_vec, seq2_init_table_6_vec,
                         seq2_init_table_7_vec, seq2_init_table_8_vec, seq2_init_table_9_vec, seq2_init_table_10_vec,
                         seq2_init_table_11_vec, seq2_init_table_12_vec, seq2_init_table_13_vec, seq2_init_table_14_vec,
                         seq2_init_table_15_vec, seq2_init_table_16_vec, seq2_init_table_17_vec, seq2_init_table_18_vec,
                         seq2_init_table_19_vec, seq2_trans_table_0_vec, seq2_trans_table_1_vec, seq2_trans_table_2_vec,
                         seq2_trans_table_3_vec, seq2_trans_table_4_vec, seq2_trans_table_5_vec, seq2_trans_table_6_vec,
                         sequenceLength);

      vrange(dmrs_shuffle_index, prbsLength);
      dmrs_shuffle_index = vsadd(dmrs_shuffle_index, sequenceLength - prbsLength, MASKREAD_OFF, prbsLength);
      vshuffle(dmrs_seq, dmrs_shuffle_index, dmrs_seq0, SHUFFLE_GATHER, prbsLength);

      vrange(dmrs_shuffle_index, prbsLength / 2);
      dmrs_shuffle_index = vmul(dmrs_shuffle_index, 2, MASKREAD_OFF, prbsLength / 2);
      vshuffle(dmrs_real_seq, dmrs_shuffle_index, dmrs_seq, SHUFFLE_GATHER, prbsLength / 2);
      dmrs_shuffle_index = vsadd(dmrs_shuffle_index, 1, MASKREAD_OFF, prbsLength / 2);
      vshuffle(dmrs_imag_seq, dmrs_shuffle_index, dmrs_seq, SHUFFLE_GATHER, prbsLength / 2);

      vrange(dmrs_shuffle_index, prbsLength / 2);
      dmrs_shuffle_index = vsadd(dmrs_shuffle_index, prbsLength * dmrsSymbolIdxSize / 2, MASKREAD_OFF, prbsLength / 2);
      vshuffle(dmrs_pdsch_real, dmrs_shuffle_index, dmrs_real_seq, SHUFFLE_SCATTER, prbsLength / 2);
      vshuffle(dmrs_pdsch_imag, dmrs_shuffle_index, dmrs_imag_seq, SHUFFLE_SCATTER, prbsLength / 2);
      dmrsSymbolIdxSize++;
    }
    if (dmrssymbolset[7] != INF) {
      unsigned int cinit = ((1 << 17) * (carrier.SymbolsPerSlot * nslot + dmrssymbolset[7] + 1) * (2 * nidnscid + 1) +
                            ((portcdmgroup / 2) << 17) + 2 * nidnscid + nscid) %
                           (1 << 31);
      vbrdcst(init_vec, 0, MASKREAD_OFF, 32);
      vbrdcst(init_vec, 0, MASKREAD_OFF, 32);
      vbrdcst(init_vec, 0, MASKREAD_OFF, 32);
      vbrdcst(init_vec, 0, MASKREAD_OFF, 32);
      vbarrier();
      VSPM_OPEN();
      int init_addr = vaddr(init_vec);
      for (int i = 0; i < 31; ++i) {
        *(volatile char *)(init_addr + i) = cinit & 0b1;
        cinit                             = cinit >> 1;
      }
      VSPM_CLOSE();

      /*--------------------DMRS GENERATION--------------------*/
      dmrs_seq0 = nrPRBS(seq1_vec, init_vec, seq2_init_table_0_vec, seq2_init_table_1_vec, seq2_init_table_2_vec,
                         seq2_init_table_3_vec, seq2_init_table_4_vec, seq2_init_table_5_vec, seq2_init_table_6_vec,
                         seq2_init_table_7_vec, seq2_init_table_8_vec, seq2_init_table_9_vec, seq2_init_table_10_vec,
                         seq2_init_table_11_vec, seq2_init_table_12_vec, seq2_init_table_13_vec, seq2_init_table_14_vec,
                         seq2_init_table_15_vec, seq2_init_table_16_vec, seq2_init_table_17_vec, seq2_init_table_18_vec,
                         seq2_init_table_19_vec, seq2_trans_table_0_vec, seq2_trans_table_1_vec, seq2_trans_table_2_vec,
                         seq2_trans_table_3_vec, seq2_trans_table_4_vec, seq2_trans_table_5_vec, seq2_trans_table_6_vec,
                         sequenceLength);

      vrange(dmrs_shuffle_index, prbsLength);
      dmrs_shuffle_index = vsadd(dmrs_shuffle_index, sequenceLength - prbsLength, MASKREAD_OFF, prbsLength);
      vshuffle(dmrs_seq, dmrs_shuffle_index, dmrs_seq0, SHUFFLE_GATHER, prbsLength);

      vrange(dmrs_shuffle_index, prbsLength / 2);
      dmrs_shuffle_index = vmul(dmrs_shuffle_index, 2, MASKREAD_OFF, prbsLength / 2);
      vshuffle(dmrs_real_seq, dmrs_shuffle_index, dmrs_seq, SHUFFLE_GATHER, prbsLength / 2);
      dmrs_shuffle_index = vsadd(dmrs_shuffle_index, 1, MASKREAD_OFF, prbsLength / 2);
      vshuffle(dmrs_imag_seq, dmrs_shuffle_index, dmrs_seq, SHUFFLE_GATHER, prbsLength / 2);

      vrange(dmrs_shuffle_index, prbsLength / 2);
      dmrs_shuffle_index = vsadd(dmrs_shuffle_index, prbsLength * dmrsSymbolIdxSize / 2, MASKREAD_OFF, prbsLength / 2);
      vshuffle(dmrs_pdsch_real, dmrs_shuffle_index, dmrs_real_seq, SHUFFLE_SCATTER, prbsLength / 2);
      vshuffle(dmrs_pdsch_imag, dmrs_shuffle_index, dmrs_imag_seq, SHUFFLE_SCATTER, prbsLength / 2);
      dmrsSymbolIdxSize++;
    }

    dmrs_pdsch_real = vmul(dmrs_pdsch_real, (int)(recpSqrt2 * (1 << fractionLength)), MASKREAD_OFF,
                           prbsLength * dmrsSymbolIdxSize / 2);

    dmrs_pdsch_imag = vmul(dmrs_pdsch_imag, (int)(recpSqrt2 * (1 << fractionLength)), MASKREAD_OFF,
                           prbsLength * dmrsSymbolIdxSize / 2);
  }

  // uint16_t pdschdmrs_length = prbsLength * dmrsSymbolIdxSize / 2;
  // uint16_t dmrs_interval    = 2;
  vreturn(dmrs_pdsch_real, prbsLength * dmrsSymbolIdxSize / 2, dmrs_pdsch_imag, prbsLength * dmrsSymbolIdxSize / 2);
  // vreturn(dmrs_pdsch_real, sizeof(dmrs_pdsch_real), dmrs_pdsch_imag, sizeof(dmrs_pdsch_imag));
}
