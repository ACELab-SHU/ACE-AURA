/**
  ******************************************************************************
  * @file           : ltePCFICHIndices.c
  * @author         : XiaoxiaoChen
  * @brief          : LTE PCFICH Indices Generation
  * @attention      : Indices start from 0 of a slot
  * @date           : 2024/1/3
  ******************************************************************************
  */

#include "lte_data_type.h"
#include "riscv_printf.h"
#include "vmath.h"
#include "venus.h"
#include <stdint.h>

typedef char __v4096i8 __attribute__((ext_vector_type(4096)));
typedef short __v2048i16 __attribute__((ext_vector_type(2048)));

//17, 0, 50, 0, 1, 0,
int Task_ltePCFICHIndices(ltePCFICHConfig pdcfich) {
    uint16_t NCellID = pdcfich.NCellID;//17
    uint16_t NDLRB = pdcfich.NDLRB;//50
    uint8_t DuplexMode = pdcfich.DuplexMode;//1
    uint8_t CyclicPrefix = pdcfich.CyclicPrefix;//0
    //uint8_t SymbolsPerSlot = pdcfich.SymbolsPerSlot;//7
    //int NDLsymb = 7;      // 一个子帧符号数
    int NRBSC = 12;       //RB的子载波数
    //int l = 0;                //一时隙符号索引
    int v = 0;
    int v_shift = NCellID % 6;
    int K_prime = (NRBSC / 2) * (NCellID % (2 * NDLRB));;
    int pcfichindices[16];

    for (int i = 0; i < 4; i++)
    {
        pcfichindices[4 * i] = K_prime + floor(i * NDLRB / 2) * NRBSC / 2;
        int RB = pcfichindices[4 * i] / 12;
        int scRB = RB * 12;
        int sccrs = (v + v_shift) % 3;
        int n = 0;
        for(int q = pcfichindices[4 * i] % 12; q < 12; q++){
            if(q != sccrs && q != sccrs + 3 && q != sccrs + 6 && q != sccrs + 9){
                pcfichindices[4 * i + n] = scRB + q;
                n++;
            }
            if(n == 4) break;
        }
    }

    __v2048i16 out_pcfichindices;
    vclaim(out_pcfichindices);
    vbarrier();
    VSPM_OPEN();
    int out_pcfichindices_addr = vaddr(out_pcfichindices);
    for (int i = 0; i < 16; ++i)
    {
        *(volatile short *)(out_pcfichindices_addr + (i << 1)) = pcfichindices[i];
    }
    VSPM_CLOSE();

    // for(int i = 0;i<16;i++)
    // {
    //     printf("%d ",&pcfichindices[i]);
    // }

    vreturn(out_pcfichindices, 32); //8bit字节长度 sizeof 标量要加上取地址
}