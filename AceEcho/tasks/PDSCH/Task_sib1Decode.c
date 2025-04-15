/*
 * @Author: YihaoShen
 * @Date: 2024-07-19 12:59:47
 * @LastEditors: YihaoShen
 * @LastEditTime: 2024-07-28 19:08:24
 * @Description: SIB1 prase to get TAC, cellIdentity
 *
 * Copyright (c) 2024 by ACE_Lab, All Rights Reserved.
 */

#include "data_type.h"
#include "riscv_printf.h"
#include "venus.h"
#include "vmath.h"

typedef short __v4096i16 __attribute__((ext_vector_type(4096)));
typedef short __v2048i16 __attribute__((ext_vector_type(2048)));
typedef char  __v4096i8 __attribute__((ext_vector_type(4096)));

/**
 * @Task_name: Task_sib1Decode
 * @input:
 *  1. sib1_bits
 * @output:
 *  1. TAC,cellIdentity...
 * */

int Task_sib1Decode(__v4096i8 sib1_bits) {

  uint8_t sib1bits[4096];
  vbarrier();
  VSPM_OPEN();
  int sib1_addr = vaddr(sib1_bits);
  for (int i = 0; i < 4096; i++) {
    sib1bits[i] = (*(volatile char *)(sib1_addr + i));
  }
  VSPM_CLOSE();

  if (sib1bits[0] != 0 || sib1bits[1] != 1) {
    // ERROR Print
  }
  uint8_t cellSelectionInfo        = sib1bits[2];
  uint8_t connEstFailureControl    = sib1bits[3];
  uint8_t si_SchedulingInfo        = sib1bits[4];
  uint8_t servingCellConfigCommon  = sib1bits[5];
  uint8_t ims_EmergencySupport     = sib1bits[6];
  uint8_t eCallOverIMS_Support     = sib1bits[7];
  uint8_t ue_TimersAndConstants    = sib1bits[8];
  uint8_t uac_BarringInfo          = sib1bits[9];
  uint8_t useFullResumeID          = sib1bits[10];
  uint8_t lateNonCriticalExtension = sib1bits[11];
  uint8_t nonCriticalExtension     = sib1bits[12];

  short index_offset = 13;
  if (cellSelectionInfo) {
    uint8_t q_RxLevMinOffset = sib1bits[index_offset];
    index_offset++;

    uint8_t q_RxLevMinSUL = sib1bits[index_offset];
    index_offset++;

    uint8_t q_QualMin = sib1bits[index_offset];
    index_offset++;

    uint8_t q_QualMinOffset = sib1bits[index_offset];
    index_offset++;

    short Q_RxLevMin =
        -70 + ((sib1bits[index_offset] << 5) + (sib1bits[index_offset + 1] << 4) + (sib1bits[index_offset + 2] << 3) +
               (sib1bits[index_offset + 3] << 2) + (sib1bits[index_offset + 4] << 1) + (sib1bits[index_offset + 5]));
    index_offset = index_offset + 6;

    if (q_RxLevMinOffset) {
      index_offset = index_offset + 3;
    }
    if (q_RxLevMinSUL) {
      index_offset = index_offset + 6;
    }
    if (q_QualMin) {
      index_offset = index_offset + 5;
    }
    if (q_QualMinOffset) {
      index_offset = index_offset + 3;
    }
  }
  uint8_t cellReservedForOtherUse      = sib1bits[index_offset];
  uint8_t cellReservedForFutureUse_r16 = sib1bits[index_offset + 1];
  uint8_t npn_IdentityInfoList_r16     = sib1bits[index_offset + 2];
  index_offset                         = index_offset + 3;

  // 此代码暂时默认PLMN_IdentityInfoList只有1个；
  uint8_t PLMN_IdentityInfoList = (sib1bits[index_offset] << 3) + (sib1bits[index_offset + 1] << 2) +
                                  (sib1bits[index_offset + 2] << 1) + (sib1bits[index_offset + 3]);
  index_offset = index_offset + 4;

  uint8_t trackingAreaCode = sib1bits[index_offset];
  uint8_t ranac            = sib1bits[index_offset + 1];
  index_offset             = index_offset + 2;

  // 此代码暂时默认plmn_IdentityList只有1个；
  uint8_t plmn_IdentityList = (sib1bits[index_offset] << 3) + (sib1bits[index_offset + 1] << 2) +
                              (sib1bits[index_offset + 2] << 1) + (sib1bits[index_offset + 3]);
  index_offset = index_offset + 4;

  uint8_t mcc = sib1bits[index_offset];
  index_offset++;

  char    MCC[3];
  uint8_t MCC_Digit_0;
  uint8_t MCC_Digit_1;
  uint8_t MCC_Digit_2;
  if (mcc) {
    MCC_Digit_0 = (sib1bits[index_offset] << 3) + (sib1bits[index_offset + 1] << 2) +
                  (sib1bits[index_offset + 2] << 1) + (sib1bits[index_offset + 3]);
    index_offset = index_offset + 4;
    MCC_Digit_1  = (sib1bits[index_offset] << 3) + (sib1bits[index_offset + 1] << 2) +
                  (sib1bits[index_offset + 2] << 1) + (sib1bits[index_offset + 3]);
    index_offset = index_offset + 4;
    MCC_Digit_2  = (sib1bits[index_offset] << 3) + (sib1bits[index_offset + 1] << 2) +
                  (sib1bits[index_offset + 2] << 1) + (sib1bits[index_offset + 3]);
    index_offset = index_offset + 4;
    MCC[0]       = MCC_Digit_0;
    MCC[1]       = MCC_Digit_1;
    MCC[2]       = MCC_Digit_2;
  }

  char    MNC[3];
  uint8_t MNC_Digit_0;
  uint8_t MNC_Digit_1;
  uint8_t MNC_Digit_2;
  uint8_t mnc_size = sib1bits[index_offset];
  index_offset++;
  if (mnc_size == 0) {
    MNC_Digit_0 = (sib1bits[index_offset] << 3) + (sib1bits[index_offset + 1] << 2) +
                  (sib1bits[index_offset + 2] << 1) + (sib1bits[index_offset + 3]);
    index_offset = index_offset + 4;
    MNC_Digit_1  = (sib1bits[index_offset] << 3) + (sib1bits[index_offset + 1] << 2) +
                  (sib1bits[index_offset + 2] << 1) + (sib1bits[index_offset + 3]);
    index_offset = index_offset + 4;
    MNC[0]       = MNC_Digit_0;
    MNC[1]       = MNC_Digit_1;
  } else {
    MNC_Digit_0 = (sib1bits[index_offset] << 3) + (sib1bits[index_offset + 1] << 2) +
                  (sib1bits[index_offset + 2] << 1) + (sib1bits[index_offset + 3]);
    index_offset = index_offset + 4;
    MNC_Digit_1  = (sib1bits[index_offset] << 3) + (sib1bits[index_offset + 1] << 2) +
                  (sib1bits[index_offset + 2] << 1) + (sib1bits[index_offset + 3]);
    index_offset = index_offset + 4;
    MNC_Digit_2  = (sib1bits[index_offset] << 3) + (sib1bits[index_offset + 1] << 2) +
                  (sib1bits[index_offset + 2] << 1) + (sib1bits[index_offset + 3]);
    index_offset = index_offset + 4;
    MNC[0]       = MNC_Digit_0;
    MNC[1]       = MNC_Digit_1;
    MNC[2]       = MNC_Digit_2;
  }

  int TAC = 0;
  if (trackingAreaCode) {
    for (int i = 0; i < 24; i++) {
      TAC = TAC + (sib1bits[index_offset + i] << (23 - i));
    }
    index_offset = index_offset + 24;
  }
  if (ranac) {
    index_offset = index_offset + 8;
  }

  long cellIdentity = 0;
  for (int i = 0; i < 36; i++) {
    /* code */
    cellIdentity = cellIdentity + (sib1bits[index_offset + i] << (35 - i));
  }
  index_offset = index_offset + 36;

  // out TAC(int)、cellIdentity(long)、MCC(char[3])、MNC(char[3])

  int_struct out_TAC;
  out_TAC.data = TAC;
  int_struct out_cellIdentity;
  out_cellIdentity.data = cellIdentity;

  MCC_MNC out_MCC_MNC;
  out_MCC_MNC.MCC[0]  = MCC[0];
  out_MCC_MNC.MCC[1]  = MCC[1];
  out_MCC_MNC.MCC[2]  = MCC[2];
  out_MCC_MNC.MNC[0]  = MNC[0];
  out_MCC_MNC.MNC[1]  = MNC[1];
  out_MCC_MNC.MNC[2]  = MNC[2];
  out_MCC_MNC.MNC_Len = mnc_size;

  vreturn(&out_TAC, sizeof(out_TAC), &out_cellIdentity, sizeof(out_cellIdentity), &out_MCC_MNC, sizeof(out_MCC_MNC));
}
