#include "data_type.h"
#include "riscv_printf.h"
#include "venus.h"

typedef short __v9000i16 __attribute__((ext_vector_type(9000)));
typedef char  __v18000i8 __attribute__((ext_vector_type(18000)));

enum crcType { CRC24A, CRC24B, CRC24C, CRC16, CRC11, CRC6 };

#define BG1 0
#define BG2 1

#define FIX 64
#define INF 0x7F

#define ALPHA 5

#define BG1_LAYERSIZE 46
#define BG2_LAYERSIZE 42

#define MIN(a, b) (((a) < (b)) ? (0) : (a - b))
#define MAX(a, b) (((a) > (b)) ? (a) : (b))

uint16_t BGlayer_1[46 * 2] = {
    0,   19,  19,  38,  38,  57,  57,  76,  76,  79,  79,  87,  87,  96,  96,  103, 103, 113, 113, 122, 122, 129, 129,
    137, 137, 144, 144, 150, 150, 157, 157, 164, 164, 170, 170, 176, 176, 182, 182, 188, 188, 194, 194, 200, 200, 205,
    205, 210, 210, 216, 216, 221, 221, 226, 226, 230, 230, 235, 235, 240, 240, 245, 245, 250, 250, 255, 255, 260, 260,
    265, 265, 270, 270, 275, 275, 279, 279, 284, 284, 289, 289, 293, 293, 298, 298, 302, 302, 307, 307, 312, 312, 316};

uint16_t BGlayer_2[42 * 2] = {0,   8,   8,   18,  18,  26,  26,  36,  36,  40,  40,  46,  46,  52,  52,  58,  58,
                              62,  62,  67,  67,  72,  72,  77,  77,  81,  81,  86,  86,  91,  91,  95,  95,  100,
                              100, 105, 105, 109, 109, 113, 113, 117, 117, 121, 121, 124, 124, 128, 128, 132, 132,
                              135, 135, 140, 140, 143, 143, 147, 147, 150, 150, 155, 155, 158, 158, 162, 162, 166,
                              166, 170, 170, 174, 174, 178, 178, 181, 181, 185, 185, 189, 189, 193, 193, 197};

int LayerEdgesCnts_BG1[9] = {3, 4, 5, 6, 7, 8, 9, 10, 19};

int LayerEdgesCnts_BG2[6] = {3, 4, 5, 6, 8, 10};

int LayerEdgesCnts_Layers_BG1[9 * 19] = {
    1,  4,  0,  0,  0,  0,  0,  0,  0, 0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  5,  27, 37, 40, 42, 45, 0,  0,  0,  0,
    0,  0,  0,  0,  0,  0,  0,  0,  0, 18, 22, 23, 25, 26, 28, 29, 30, 31, 32, 33, 34, 35, 36, 38, 39, 41, 43, 44, 8,
    13, 16, 17, 18, 19, 20, 21, 24, 0, 0,  0,  0,  0,  0,  0,  0,  0,  0,  5,  7,  10, 12, 14, 15, 0,  0,  0,  0,  0,
    0,  0,  0,  0,  0,  0,  0,  0,  2, 5,  11, 0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  2,  6,
    9,  0,  0,  0,  0,  0,  0,  0,  0, 0,  0,  0,  0,  0,  0,  0,  0,  1,  8,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
    0,  0,  0,  0,  0,  0,  0,  4,  0, 1,  2,  3,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0};

int LayerEdgesCnts_Layers_BG2[6 * 21] = {
    6,  22, 25, 27, 29, 31, 37, 0,  0,  0,  0,  0,  0,  0,  0,  0,  0, 0, 0,  0,  0,  20, 4,  8,  12, 15,
    18, 19, 20, 21, 23, 24, 28, 32, 33, 34, 35, 36, 38, 39, 40, 41, 9, 9, 10, 11, 13, 14, 16, 17, 26, 30,
    0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  3,  5,  6,  7,  0,  0, 0, 0,  0,  0,  0,  0,  0,  0,  0,
    0,  0,  0,  0,  0,  0,  2,  0,  2,  0,  0,  0,  0,  0,  0,  0,  0, 0, 0,  0,  0,  0,  0,  0,  0,  0,
    0,  2,  1,  3,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, 0, 0,  0,  0,  0};

// table
uint16_t vNodesIdx_BG1[316];
uint16_t nShifts_BG1[316];

uint16_t vNodesIdx_BG2[197];
uint16_t nShifts_BG2[197];

struct LDPC {
  uint16_t     mKBar;   // mKBar = mK - mF
  uint16_t     mK;      //信息位长度
  double       mR;      //码率
  uint16_t     mBGn;    //基图类型
  uint16_t     mZc;     //上升因子
  uint16_t     mSetIdx; //上升因子所属的组
  uint16_t     mF;      //填充位
  uint16_t     mN;      //编码后的总长(未打掉前两列的长度)
  uint16_t     mC;      //分割后的码块数
  enum crcType CRC;     // CRC类型
  uint16_t     mL;      // CRC长度
  uint16_t     mLcb;    //码块分割添加的CRC长度
} __attribute__((aligned(64)));

#define FLOOR(n) ((((int)(n) < 0) && ((n) != (int)(n))) ? ((int)(n)-1) : (int)(n))
#define CEIL(n)  ((((int)(n) < 0) && ((n) != (int)(n))) ? ((int)(n) + 1) : (int)(n))

VENUS_INLINE __v18000i8 cbsRateRecoverLDPC(__v18000i8 vin, int mK, int mZc, int mF, int k0, int Ncb, int Qm, int E) {
  __v18000i8 vout;
  __v18000i8 interleave;
  __v18000i8 interleave_tmp;
  vclaim(vout);
  vclaim(interleave);
  vclaim(interleave_tmp);

  int k       = mK - 2 * mZc;
  int kd      = k - mF;
  int NBuffer = Ncb - mF;

  int i, j;

  __v18000i8 circ;
  __v18000i8 cons_i8;
  __v9000i16 index;
  __v9000i16 index0;
  vclaim(circ);
  vclaim(index);
  vclaim(index0);
  vclaim(cons_i8);

  vbrdcst(vout, 0, MASKREAD_OFF, 18000);
  vbrdcst(interleave, 0, MASKREAD_OFF, 18000);
  vbrdcst(interleave_tmp, 0, MASKREAD_OFF, 18000);

  vin = vsadd(vin, 0, MASKREAD_OFF, E);

  vrange(index, E);
  index = vsadd(index, k0 - E, MASKREAD_OFF, E);
  index = vsadd(index, E, MASKREAD_OFF, E - k0);
  vshuffle(circ, index, vin, SHUFFLE_GATHER, E);

  vrange(index, E / Qm);
  vrange(index0, E / Qm);
  index = vmul(index, Qm, MASKREAD_OFF, E / Qm);
  for (i = 0; i < Qm; i++) {
    index = vsadd(index, i, MASKREAD_OFF, E / Qm);
    vshuffle(interleave_tmp, index, circ, SHUFFLE_GATHER, E / Qm);
    index0 = vsadd(index0, i * E / Qm, MASKREAD_OFF, E / Qm);
    vshuffle(interleave, index0, interleave_tmp, SHUFFLE_SCATTER, E / Qm);
  }

  if (E > NBuffer) {
    int remBits = E % NBuffer;

    vbrdcst(vout, INF, MASKREAD_OFF, k);
    vrange(index, kd);
    vshuffle(vout, index, interleave, SHUFFLE_GATHER, kd);
    vrange(index, NBuffer - kd);
    index = vsadd(index, kd, MASKREAD_OFF, NBuffer - kd);
    vshuffle(interleave_tmp, index, interleave, SHUFFLE_GATHER, NBuffer - kd);
    vrange(index, NBuffer - kd);
    index = vsadd(index, k, MASKREAD_OFF, NBuffer - kd);
    vshuffle(vout, index, interleave_tmp, SHUFFLE_SCATTER, NBuffer - kd);

    vrange(index, kd);
    index = vsadd(index, NBuffer, MASKREAD_OFF, kd);
    vshuffle(interleave_tmp, index, interleave, SHUFFLE_GATHER, NBuffer - kd);
    vout = vsadd(vout, interleave_tmp, MASKREAD_OFF, kd);

    int tmp = (remBits - kd) < 0 ? 0 : (remBits - kd);
    vbrdcst(interleave_tmp, 0, MASKREAD_OFF, Ncb);
    vrange(index, tmp == 0 ? 10 : tmp);
    index = vsadd(index, NBuffer + kd, MASKREAD_OFF, tmp == 0 ? 10 : tmp);
    vshuffle(interleave_tmp, index, interleave, SHUFFLE_GATHER, tmp == 0 ? 10 : tmp);
    interleave_tmp = vmul(interleave_tmp, tmp == 0 ? 0 : 1, MASKREAD_OFF, kd);
    vout           = vsadd(vout, interleave_tmp, MASKREAD_OFF, kd);
  } else {
    vbrdcst(vout, INF, MASKREAD_OFF, k);
    vrange(index, kd);
    vshuffle(vout, index, interleave, SHUFFLE_GATHER, kd);
    vrange(index, NBuffer - kd);
    index = vsadd(index, kd, MASKREAD_OFF, NBuffer - kd);
    vshuffle(interleave_tmp, index, interleave, SHUFFLE_GATHER, NBuffer - kd);
    vrange(index, NBuffer - kd);
    index = vsadd(index, k, MASKREAD_OFF, NBuffer - kd);
    vshuffle(vout, index, interleave_tmp, SHUFFLE_SCATTER, NBuffer - kd);
  }

  return vout;
}

int Task_nrPDSCHRateRecover(__v18000i8 msg, int_struct in_mK, int_struct in_mZc, int_struct in_mF, int_struct in_k0,
                            int_struct in_Ncb, int_struct in_Qm, int_struct in_E) {

  int mK  = in_mK.data;
  int mZc = in_mZc.data;
  int mF  = in_mF.data;
  int k0  = in_k0.data;
  int Ncb = in_Ncb.data;
  int Qm  = in_Qm.data;
  int E   = in_E.data;

  __v18000i8 recover;
  vclaim(recover);
  recover = cbsRateRecoverLDPC(msg, mK, mZc, mF, k0, Ncb, Qm, E);

  __v18000i8 LLR;
  __v9000i16 index;
  vclaim(LLR);
  vclaim(index);

  vbrdcst(LLR, 0, MASKREAD_OFF, 2 * mZc);
  vrange(index, Ncb);
  index = vsadd(index, 2 * mZc, MASKREAD_OFF, Ncb);
  vshuffle(LLR, index, recover, SHUFFLE_SCATTER, Ncb);
  LLR = vsadd(LLR, 0, MASKREAD_OFF, 2 * mZc + Ncb);
  // vreturn(recover, sizeof(recover));
  // vreturn(recover, 9000);
  vreturn(LLR, 10000);
}