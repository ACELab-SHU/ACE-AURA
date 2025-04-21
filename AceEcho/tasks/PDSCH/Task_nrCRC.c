/**
 * ****************************************
 * @file        Task_nrCRC.c
 * @brief       Crc Decode
 * @author      yuanfeng
 * @date        2024.7.26
 * @copyright   ACE-Lab(Shanghai University)
 * ****************************************
 */

#include "data_type.h"
#include "riscv_printf.h"
#include "venus.h"

typedef short __v2048i16 __attribute__((ext_vector_type(2048)));
typedef char  __v4096i8 __attribute__((ext_vector_type(4096)));

int Task_nrCRC(__v4096i8 vin, short_struct in_fullLen, short_struct in_pariLen, __v4096i8 poly) {
  int fullLen = in_fullLen.data;
  int pariLen = in_pariLen.data + 1;
  int msgLen  = fullLen - pariLen + 1;
  int tmp;

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
  VSPM_OPEN();
  int crc_result = *(volatile unsigned char *)(compare_result_addr);
  VSPM_CLOSE();

  short_struct out_crc_result;
  out_crc_result.data = crc_result;
  printf("crc_result: %hd\n", &crc_result);

  vreturn(buf, sizeof(buf), &out_crc_result, sizeof(out_crc_result));
}