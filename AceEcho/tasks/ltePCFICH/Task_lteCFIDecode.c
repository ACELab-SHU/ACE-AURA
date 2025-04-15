/**
  ******************************************************************************
  * @file           : Task_lteCFIDecode.c
  * @author         : XiaoxiaoChen
  * @brief          : LTE PCFICH CFI Decode
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

typedef struct {
  short data;
} __attribute__((aligned(64))) short_struct1;

int Task_lteCFIDecode(__v4096i8 softbit) {

    __v2048i16 index_one;
    vclaim(index_one);
    vbrdcst(index_one,1,MASKREAD_OFF,1);
    __v4096i8 firstBit;
    vclaim(firstBit);
    vshuffle(firstBit,index_one,softbit,SHUFFLE_GATHER,1);
    vbarrier();
    VSPM_OPEN();
    int firstBit_addr = vaddr(firstBit);
    int firstBit0 = *(volatile char *)(firstBit_addr + 0);
    VSPM_CLOSE();

    __v2048i16 index_two;
    vclaim(index_two);
    vbrdcst(index_two,2,MASKREAD_OFF,1);
    __v4096i8 secondBit;
    vclaim(secondBit);
    vshuffle(secondBit,index_two,softbit,SHUFFLE_GATHER,1);
    vbarrier();
    VSPM_OPEN();
    int secondBit_addr = vaddr(secondBit);
    int secondBit1 = *(volatile char *)(secondBit_addr + 1);
    VSPM_CLOSE();

    // printf("%d \n",&firstBit0);
    // printf("%d \n",&secondBit1);

    int cfidecison = (firstBit0 << 1) | secondBit1;

    short_struct1 cfi;
    switch (cfidecison)
    {
    case 0: // '00' -> 0
      cfi.data = 4;
      break;
    case 1: // '01' -> 1
      cfi.data = 1;
      break;
    case 2: // '10' -> 2
      cfi.data = 2;
      break;
    case 3: // '11' -> 3
      cfi.data = 3;
      break;
    default:
      printf("Invalid bits\n",NULL);
      //return -1;
    }
  printf("cfi:%hd \n",&cfi.data);  
  vreturn(&cfi, sizeof(cfi));
}
