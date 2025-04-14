#include "data_type.h"
#include "riscv_printf.h"
#include "venus.h"

typedef short __v2048i16 __attribute__((ext_vector_type(2048)));
typedef char  __v4096i8 __attribute__((ext_vector_type(4096)));

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

// VENUS_INLINE uint16_t
// selectBaseGraph(uint16_t KBar, double R)
// {
//     // 3GPP 38.212 7.2.2 LDPC base graph selection
//     if (KBar <= 292 || (KBar <= 3824 && R <= 0.67) || R <= 0.25) {
//         return 2;
//     }
//     else {
//         return 1;
//     }
// }

// VENUS_INLINE uint16_t
// selectLiftSize(uint16_t KBar, uint16_t BGn)
// {
//     // select kb 3GPP 38.212 section 5.2.2
//     uint16_t Kb;
//     if (BGn == 1)
//         Kb = 22;
//     else {
//         if (KBar > 640)
//             Kb = 10;
//         else if (KBar > 560)
//             Kb = 9;
//         else if (KBar > 192)
//             Kb = 8;
//         else
//             Kb = 6;
//     }

//     // select lifting size and shifting set index based on 3GPP Table 5.3.2-1
//     uint16_t Zc = 384;
//     uint16_t candiZc;
//     for (unsigned int i = 0; i < 8; i++) {
//         for (unsigned int j = 0; j < 8; j++) {
//             candiZc = liftSizeTable[i*8 + j];
//             if (candiZc * Kb == KBar)
//                 return candiZc;
//             else if (candiZc * Kb > KBar && candiZc < Zc)
//                 Zc = candiZc;
//         }
//     }

//     return Zc;
// }

// VENUS_INLINE uint8_t
// selectShiftSet(uint16_t Zc)
// {
//     if (Zc >= 2 && Zc <= 384) return -1;
//     for (unsigned int i = 0; i < 8; i++) {
//         for (unsigned int j = 0; j < 8; j++) {
//             if (liftSizeTable[i*8 + j] == Zc)
//                 return i;
//         }
//     }
//     return -1;
// }

// VENUS_INLINE struct LDPC
// ldpc_init(uint16_t tbs, float coderate)
// {
//     struct LDPC ldpc_handler;
//     uint16_t L, Bd, Lcb;
//     // int infolen = tbs.data;
//     uint16_t infolen = tbs;
// 	ldpc_handler.mR = coderate;

//     // select base graph based on 3GPP 38.212 7.2.2
//     ldpc_handler.mBGn = selectBaseGraph(infolen, ldpc_handler.mR);

//     //crc type
//     if (infolen > 3824) {
//         L  = 24;
//         ldpc_handler.CRC = CRC24A;
//     }
//     else {
//         L  = 16;
//         ldpc_handler.CRC = CRC16;
//     }
//     int B = infolen + L;

//     //KBar
//     int Kcb = (ldpc_handler.mBGn == 1)?8448:3840;

//     if (B <= Kcb) {
//         Lcb = 0;
//         ldpc_handler.mC = 1;
//         Bd = B;
//     }
//     else {
//         Lcb = 24;
//         ldpc_handler.mC = B / (Kcb - L) + 1;
//         Bd = B + ldpc_handler.mC * L;
//     }

//     ldpc_handler.mKBar = (Bd - 1) / ldpc_handler.mC + 1;
//     ldpc_handler.mL    = L;
//     ldpc_handler.mLcb  = Lcb;

//     // select lifting size
//     ldpc_handler.mZc = selectLiftSize(ldpc_handler.mKBar, ldpc_handler.mBGn);

//     // select shifting set
//     ldpc_handler.mSetIdx = selectShiftSet(ldpc_handler.mZc);

//     // systematic bits length and parity bits length
//     if (ldpc_handler.mBGn == 1) {
//         ldpc_handler.mK = 22 * ldpc_handler.mZc;
//         ldpc_handler.mN = 68 * ldpc_handler.mZc;
//     }
//     else {
//         ldpc_handler.mK = 10 * ldpc_handler.mZc;
//         ldpc_handler.mN = 52 * ldpc_handler.mZc;
//     }

//     // fillers length
//     ldpc_handler.mF = ldpc_handler.mK - ldpc_handler.mKBar;

//     ldpc_handler.mR = 1.0 * ldpc_handler.mK / ldpc_handler.mN;
//     // build up edges and shifts
//     if (ldpc_handler.mBGn == 1) {
//         for (unsigned i = 0; i < 316; i++) {
//             vNodesIdx_BG1[i]  = (uint16_t)(shiftTableBgn_1[i*10+1]);
//             nShifts_BG1[i]  = (uint16_t)(shiftTableBgn_1[i*10 + ldpc_handler.mSetIdx + 2] % ldpc_handler.mZc);
//         }
//     }
//     else {
//         for (unsigned i = 0; i < 197; i++) {
//             vNodesIdx_BG2[i]  = (uint16_t)(shiftTableBgn_2[i*10+1]);
//             nShifts_BG2[i]  = (uint16_t)(shiftTableBgn_2[i*10 + ldpc_handler.mSetIdx + 2] % ldpc_handler.mZc);
//         }
//     }

//     return ldpc_handler;
// }

VENUS_INLINE __v4096i8 nrLDPCDecoder_version3(uint16_t mBGn, uint16_t mZc, uint16_t mN, uint16_t mF, uint16_t mK,
                                     __v4096i8 LLR) {
  int nMaxLayer = (mBGn == 1) ? 46 : 42;
  int maxiter   = 5;
  int edgeStart;
  int edgeEnd;
  int nLayerEdges;
  int edgeIdx;
  int vNodeIdx;
  int nShifts;
  int ss;

  __v4096i8  VtoCmsg;
  __v4096i8  Buffer;
  __v4096i8  Buffer1;
  __v4096i8  CtoVmsg;
  __v4096i8  parity_0;
  __v4096i8  parity;
  __v4096i8  VtoCmsg_abs;
  __v4096i8  VtoCmsg_minsum;
  __v4096i8  minBuf;
  __v4096i8  cons_i8;
  __v2048i16 transIdx;
  __v2048i16 index;

  vclaim(VtoCmsg);
  vclaim(Buffer);
  vclaim(Buffer1);
  vclaim(CtoVmsg);
  vclaim(parity_0);
  vclaim(parity);
  vclaim(VtoCmsg_abs);
  vclaim(VtoCmsg_minsum);
  vclaim(minBuf);
  vclaim(cons_i8);
  vclaim(transIdx);
  vclaim(index);

  for (int iter = 0; iter < maxiter; iter++) {
    for (int iLayer = 0; iLayer < nMaxLayer; iLayer++) {
      edgeStart   = (mBGn == 1) ? BGlayer_1[iLayer * 2 + 0] : BGlayer_2[iLayer * 2 + 0];
      edgeEnd     = (mBGn == 1) ? BGlayer_1[iLayer * 2 + 1] : BGlayer_2[iLayer * 2 + 1];
      nLayerEdges = edgeEnd - edgeStart;

      for (int iEdge = 0; iEdge < nLayerEdges; ++iEdge) {
        edgeIdx  = edgeStart + iEdge;
        vNodeIdx = (mBGn == 1) ? vNodesIdx_BG1[edgeIdx] : vNodesIdx_BG2[edgeIdx];
        nShifts  = (mBGn == 1) ? nShifts_BG1[edgeIdx] : nShifts_BG2[edgeIdx];
        ss = mZc - nShifts;

        vrange(index, mZc);
        index = vsadd(index, nShifts - mZc, MASKREAD_OFF, mZc);
        index = vsadd(index, (ss == 0)?0:mZc, MASKREAD_OFF, (ss == 0)?10:ss);
        index = vsadd(index, mZc * vNodeIdx, MASKREAD_OFF, mZc);
        vshuffle(Buffer, index, LLR, SHUFFLE_GATHER, mZc);

        vrange(index, mZc);
        index = vsadd(index, iEdge * mZc, MASKREAD_OFF, mZc);
        vshuffle(VtoCmsg, index, Buffer, SHUFFLE_SCATTER, mZc);
      }

      vbrdcst(parity, 0, MASKREAD_OFF, nLayerEdges * mZc);
      vbrdcst(cons_i8, 0xFF, MASKREAD_OFF, nLayerEdges * mZc);
      vbrdcst(VtoCmsg_minsum, 0, MASKREAD_OFF, nLayerEdges * mZc);

      VtoCmsg_abs = vsadd(VtoCmsg, 0, MASKREAD_OFF, nLayerEdges * mZc);
      vsgt(VtoCmsg_abs, 0, MASKREAD_OFF, MASKWRITE_ON, nLayerEdges * mZc);
      VtoCmsg_abs = vxor(cons_i8, VtoCmsg_abs, MASKREAD_ON, nLayerEdges * mZc);

      for (int iEdge = 0; iEdge < nLayerEdges; ++iEdge) {
        vbrdcst(minBuf, INF, MASKREAD_OFF, mZc);
        vbrdcst(parity_0, 0, MASKREAD_OFF, mZc);
        for (int j = 0; j < nLayerEdges; j++) {
          if (j == iEdge)
            continue;
          vrange(index, mZc);
          index = vsadd(index, j * mZc, MASKREAD_OFF, mZc);
          vshuffle(Buffer, index, VtoCmsg_abs, SHUFFLE_GATHER, mZc);
          vshuffle(Buffer1, index, VtoCmsg, SHUFFLE_GATHER, mZc);

          vsle(minBuf, Buffer, MASKREAD_OFF, MASKWRITE_ON, mZc);
          minBuf = vxor(minBuf, minBuf, MASKREAD_ON, mZc);
          minBuf = vsadd(Buffer, minBuf, MASKREAD_ON, mZc);

          vsgt(Buffer1, 0, MASKREAD_OFF, MASKWRITE_ON, mZc);
          parity_0 = vxor(cons_i8, parity_0, MASKREAD_ON, mZc);
        }
        vrange(index, mZc);
        index = vsadd(index, iEdge * mZc, MASKREAD_OFF, mZc);
        vshuffle(VtoCmsg_minsum, index, minBuf, SHUFFLE_SCATTER, mZc);
        vshuffle(parity, index, parity_0, SHUFFLE_SCATTER, mZc);
      }
      vsgt(VtoCmsg_minsum, ALPHA, MASKREAD_OFF, MASKWRITE_ON, nLayerEdges * mZc);
      VtoCmsg_minsum = vxor(VtoCmsg_minsum, VtoCmsg_minsum, MASKREAD_ON, nLayerEdges * mZc);
      vsle(VtoCmsg_minsum, ALPHA, MASKREAD_OFF, MASKWRITE_ON, nLayerEdges * mZc);
      vbrdcst(cons_i8, -1 * ALPHA, MASKREAD_OFF, nLayerEdges * mZc);
      VtoCmsg_minsum = vsadd(cons_i8, VtoCmsg_minsum, MASKREAD_ON, nLayerEdges * mZc);
      vsne(VtoCmsg_minsum, 0, MASKREAD_OFF, MASKWRITE_ON, nLayerEdges * mZc);
      VtoCmsg_minsum = vxor(parity, VtoCmsg_minsum, MASKREAD_ON, nLayerEdges * mZc);
      VtoCmsg_minsum = vsra(VtoCmsg_minsum, 2, MASKREAD_OFF, nLayerEdges * mZc);

      vbrdcst(CtoVmsg, 0, MASKREAD_OFF, mN);
      for (int iEdge = 0; iEdge < nLayerEdges; ++iEdge) {
        edgeIdx  = edgeStart + iEdge;
        vNodeIdx = (mBGn == 1) ? vNodesIdx_BG1[edgeIdx] : vNodesIdx_BG2[edgeIdx];
        nShifts  = (mBGn == 1) ? nShifts_BG1[edgeIdx] : nShifts_BG2[edgeIdx];

        vrange(index, mZc);
        index = vsadd(index, 0 - nShifts, MASKREAD_OFF, mZc);
        index = vsadd(index, (nShifts == 0)?0:mZc, MASKREAD_OFF, (nShifts == 0)?10:mZc);
        index = vsadd(index, iEdge * mZc, MASKREAD_OFF, mZc);
        vshuffle(Buffer, index, VtoCmsg_minsum, SHUFFLE_GATHER, mZc);
        vrange(index, mZc);
        index = vsadd(index, mZc * vNodeIdx, MASKREAD_OFF, mZc);
        vshuffle(CtoVmsg, index, Buffer, SHUFFLE_SCATTER, mZc);
      }
      LLR = vsadd(LLR, CtoVmsg, MASKREAD_OFF, mN);
    }
  }

  vsle(LLR, 0, MASKREAD_OFF, MASKWRITE_ON, mN);
  vbrdcst(LLR, 1, MASKREAD_OFF, mN);
  vbrdcst(LLR, 0, MASKREAD_ON, mN);
  vrange(transIdx, mK - mF);
  vshuffle(LLR, transIdx, LLR, SHUFFLE_GATHER, mK - mF);

  return LLR;
}
VENUS_INLINE __v4096i8 nrLDPCDecoder(uint16_t mBGn, uint16_t mZc, uint16_t mN, uint16_t mF, uint16_t mK,
                                     __v4096i8 LLR) {
  int nMaxLayer = (mBGn == 1) ? 46 : 42;
  int maxiter   = 5;
  int edgeStart;
  int edgeEnd;
  int nLayerEdges;
  int edgeIdx;
  int vNodeIdx;
  int nShifts;
  int ss;

  __v4096i8  VtoCmsg;
  __v4096i8  Buffer;
  __v4096i8  Buffer1;
  __v4096i8  CtoVmsg;
  __v4096i8  parity_0;
  __v4096i8  parity;
  __v4096i8  VtoCmsg_abs;
  __v4096i8  VtoCmsg_minsum;
  __v4096i8  minBuf;
  __v4096i8  cons_i8;
  __v2048i16 transIdx;
  __v2048i16 index;

  vclaim(VtoCmsg);
  vclaim(Buffer);
  vclaim(Buffer1);
  vclaim(CtoVmsg);
  vclaim(parity_0);
  vclaim(parity);
  vclaim(VtoCmsg_abs);
  vclaim(VtoCmsg_minsum);
  vclaim(minBuf);
  vclaim(cons_i8);
  vclaim(transIdx);
  vclaim(index);

  for (int iter = 0; iter < maxiter; iter++) {
    for (int iLayer = 0; iLayer < nMaxLayer; iLayer++) {
      edgeStart   = (mBGn == 1) ? BGlayer_1[iLayer * 2 + 0] : BGlayer_2[iLayer * 2 + 0];
      edgeEnd     = (mBGn == 1) ? BGlayer_1[iLayer * 2 + 1] : BGlayer_2[iLayer * 2 + 1];
      nLayerEdges = edgeEnd - edgeStart;

      for (int iEdge = 0; iEdge < nLayerEdges; ++iEdge) {
        edgeIdx  = edgeStart + iEdge;
        vNodeIdx = (mBGn == 1) ? vNodesIdx_BG1[edgeIdx] : vNodesIdx_BG2[edgeIdx];
        nShifts  = (mBGn == 1) ? nShifts_BG1[edgeIdx] : nShifts_BG2[edgeIdx];
        ss = mZc - nShifts;

        vrange(index, mZc);
        index = vsadd(index, nShifts - mZc, MASKREAD_OFF, mZc);
        index = vsadd(index, (ss == 0)?0:mZc, MASKREAD_OFF, (ss == 0)?10:ss);
        index = vsadd(index, mZc * vNodeIdx, MASKREAD_OFF, mZc);
        vshuffle(Buffer, index, LLR, SHUFFLE_GATHER, mZc);

        vrange(index, mZc);
        index = vsadd(index, iEdge * mZc, MASKREAD_OFF, mZc);
        vshuffle(VtoCmsg, index, Buffer, SHUFFLE_SCATTER, mZc);
      }

      vbrdcst(parity, 0, MASKREAD_OFF, nLayerEdges * mZc);
      vbrdcst(cons_i8, 0xFF, MASKREAD_OFF, nLayerEdges * mZc);
      vbrdcst(VtoCmsg_minsum, 0, MASKREAD_OFF, nLayerEdges * mZc);

      VtoCmsg_abs = vsadd(VtoCmsg, 0, MASKREAD_OFF, nLayerEdges * mZc);
      vsgt(VtoCmsg_abs, 0, MASKREAD_OFF, MASKWRITE_ON, nLayerEdges * mZc);
      VtoCmsg_abs = vxor(cons_i8, VtoCmsg_abs, MASKREAD_ON, nLayerEdges * mZc);

      for (int iEdge = 0; iEdge < nLayerEdges; ++iEdge) {
        vbrdcst(minBuf, INF, MASKREAD_OFF, mZc);
        vbrdcst(parity_0, 0, MASKREAD_OFF, mZc);
        for (int j = 0; j < nLayerEdges; j++) {
          if (j == iEdge)
            continue;
          vrange(index, mZc);
          index = vsadd(index, j * mZc, MASKREAD_OFF, mZc);
          vshuffle(Buffer, index, VtoCmsg_abs, SHUFFLE_GATHER, mZc);
          vshuffle(Buffer1, index, VtoCmsg, SHUFFLE_GATHER, mZc);

          vsle(minBuf, Buffer, MASKREAD_OFF, MASKWRITE_ON, mZc);
          minBuf = vxor(minBuf, minBuf, MASKREAD_ON, mZc);
          minBuf = vsadd(Buffer, minBuf, MASKREAD_ON, mZc);

          vsgt(Buffer1, 0, MASKREAD_OFF, MASKWRITE_ON, mZc);
          parity_0 = vxor(cons_i8, parity_0, MASKREAD_ON, mZc);
        }
        vrange(index, mZc);
        index = vsadd(index, iEdge * mZc, MASKREAD_OFF, mZc);
        vshuffle(VtoCmsg_minsum, index, minBuf, SHUFFLE_SCATTER, mZc);
        vshuffle(parity, index, parity_0, SHUFFLE_SCATTER, mZc);
      }
      vsgt(VtoCmsg_minsum, ALPHA, MASKREAD_OFF, MASKWRITE_ON, nLayerEdges * mZc);
      VtoCmsg_minsum = vxor(VtoCmsg_minsum, VtoCmsg_minsum, MASKREAD_ON, nLayerEdges * mZc);
      vsle(VtoCmsg_minsum, ALPHA, MASKREAD_OFF, MASKWRITE_ON, nLayerEdges * mZc);
      vbrdcst(cons_i8, -1 * ALPHA, MASKREAD_OFF, nLayerEdges * mZc);
      VtoCmsg_minsum = vsadd(cons_i8, VtoCmsg_minsum, MASKREAD_ON, nLayerEdges * mZc);
      vsne(VtoCmsg_minsum, 0, MASKREAD_OFF, MASKWRITE_ON, nLayerEdges * mZc);
      VtoCmsg_minsum = vxor(parity, VtoCmsg_minsum, MASKREAD_ON, nLayerEdges * mZc);
      VtoCmsg_minsum = vsra(VtoCmsg_minsum, 2, MASKREAD_OFF, nLayerEdges * mZc);

      vbrdcst(CtoVmsg, 0, MASKREAD_OFF, mN);
      for (int iEdge = 0; iEdge < nLayerEdges; ++iEdge) {
        edgeIdx  = edgeStart + iEdge;
        vNodeIdx = (mBGn == 1) ? vNodesIdx_BG1[edgeIdx] : vNodesIdx_BG2[edgeIdx];
        nShifts  = (mBGn == 1) ? nShifts_BG1[edgeIdx] : nShifts_BG2[edgeIdx];

        vrange(index, mZc);
        index = vsadd(index, 0 - nShifts, MASKREAD_OFF, mZc);
        index = vsadd(index, (nShifts == 0)?0:mZc, MASKREAD_OFF, (nShifts == 0)?10:mZc);
        index = vsadd(index, iEdge * mZc, MASKREAD_OFF, mZc);
        vshuffle(Buffer, index, VtoCmsg_minsum, SHUFFLE_GATHER, mZc);
        vrange(index, mZc);
        index = vsadd(index, mZc * vNodeIdx, MASKREAD_OFF, mZc);
        vshuffle(CtoVmsg, index, Buffer, SHUFFLE_SCATTER, mZc);
      }
      LLR = vsadd(LLR, CtoVmsg, MASKREAD_OFF, mN);
    }
  }

  vsle(LLR, 0, MASKREAD_OFF, MASKWRITE_ON, mN);
  vbrdcst(LLR, 1, MASKREAD_OFF, mN);
  vbrdcst(LLR, 0, MASKREAD_ON, mN);
  vrange(transIdx, mK - mF);
  vshuffle(LLR, transIdx, LLR, SHUFFLE_GATHER, mK - mF);

  return LLR;
}

VENUS_INLINE __v4096i8 cbsRateRecoverLDPC(__v4096i8 vin, int mK, int mZc, int mF, int k0, int Ncb, int Qm, int E) {
  __v4096i8 vout;
  __v4096i8 interleave;
  __v4096i8 interleave_tmp;
  vclaim(vout);
  vclaim(interleave);
  vclaim(interleave_tmp);

  int k       = mK - 2 * mZc;
  int kd      = k - mF;
  int NBuffer = Ncb - mF;

  int i, j;

  __v4096i8  circ;
  __v4096i8  cons_i8;
  __v2048i16 index;
  __v2048i16 index0;
  vclaim(circ);
  vclaim(index);
  vclaim(index0);
  vclaim(cons_i8);

  vrange(index, E);
  index = vadd(index, k0 - E, MASKREAD_OFF, E);
  index = vadd(index, E, MASKREAD_OFF, E - k0);
  vshuffle(circ, index, vin, SHUFFLE_GATHER, E);

  vrange(index, E / Qm);
  vrange(index0, E / Qm);
  index = vmul(index, Qm, MASKREAD_OFF, E / Qm);
  for (i = 0; i < Qm; i++) {
    index = vadd(index, i, MASKREAD_OFF, E / Qm);
    vshuffle(interleave_tmp, index, circ, SHUFFLE_GATHER, E / Qm);
    index0 = vadd(index0, i * E / Qm, MASKREAD_OFF, E / Qm);
    vshuffle(interleave, index0, interleave_tmp, SHUFFLE_SCATTER, E / Qm);
  }

  if (E > NBuffer) {
    int remBits = E % NBuffer;

    vbrdcst(vout, INF, MASKREAD_OFF, k);
    vrange(index, kd);
    vshuffle(vout, index, interleave, SHUFFLE_GATHER, kd);
    vrange(index, NBuffer - kd);
    index = vadd(index, kd, MASKREAD_OFF, NBuffer - kd);
    vshuffle(interleave_tmp, index, interleave, SHUFFLE_GATHER, NBuffer - kd);
    vrange(index, NBuffer - kd);
    index = vadd(index, k, MASKREAD_OFF, NBuffer - kd);
    vshuffle(vout, index, interleave_tmp, SHUFFLE_SCATTER, NBuffer - kd);

    vrange(index, kd);
    index = vadd(index, NBuffer, MASKREAD_OFF, kd);
    vshuffle(interleave_tmp, index, interleave, SHUFFLE_GATHER, NBuffer - kd);
    vout = vadd(vout, interleave_tmp, MASKREAD_OFF, kd);

    int tmp = (remBits - kd) < 0 ? 0 : (remBits - kd);
    if (tmp)
    {
        vbrdcst(interleave_tmp, 0, MASKREAD_OFF, Ncb);
        vrange(index, tmp);
        index = vadd(index, NBuffer + kd, MASKREAD_OFF, tmp);
        vshuffle(interleave_tmp, index, interleave, SHUFFLE_GATHER, tmp);
        vout = vadd(vout, interleave_tmp, MASKREAD_OFF, kd);
    }
  } else {
    vbrdcst(vout, INF, MASKREAD_OFF, k);
    vrange(index, kd);
    vshuffle(vout, index, interleave, SHUFFLE_GATHER, kd);
    vrange(index, NBuffer - kd);
    index = vadd(index, kd, MASKREAD_OFF, NBuffer - kd);
    vshuffle(interleave_tmp, index, interleave, SHUFFLE_GATHER, NBuffer - kd);
    vrange(index, NBuffer - kd);
    index = vadd(index, k, MASKREAD_OFF, NBuffer - kd);
    vshuffle(vout, index, interleave_tmp, SHUFFLE_SCATTER, NBuffer - kd);
  }

  return vout;
}

typedef struct {
  uint16_t vNodesIdx_BG1[316];
  uint16_t nShifts_BG1[316];

  uint16_t vNodesIdx_BG2[197];
  uint16_t nShifts_BG2[197];
} __attribute__((aligned(64))) struct_LDPC_Table;

int Task_nrPDSCHDecoder(__v4096i8 msg, int_struct in_mK, int_struct in_mZc, int_struct in_mF, int_struct in_mN,
                        int_struct in_k0, int_struct in_Ncb, int_struct in_Qm, int_struct in_E, int_struct in_mBGn,
                        struct_LDPC_Table ldpcTable) {

  int mK   = in_mK.data;
  int mZc  = in_mZc.data;
  int mF   = in_mF.data;
  int mN   = in_mN.data;
  int k0   = in_k0.data;
  int Ncb  = in_Ncb.data;
  int Qm   = in_Qm.data;
  int E    = in_E.data;
  int mBGn = in_mBGn.data;
  for (int i = 0; i < 316; ++i) {
    vNodesIdx_BG1[i] = ldpcTable.vNodesIdx_BG1[i];
    nShifts_BG1[i]   = ldpcTable.nShifts_BG1[i];
  }
  for (int i = 0; i < 197; ++i) {
    vNodesIdx_BG2[i] = ldpcTable.vNodesIdx_BG2[i];
    nShifts_BG2[i]   = ldpcTable.nShifts_BG2[i];
  }

  printf("mk=%d\n", &mK);
  printf("mZc=%d\n", &mZc);
  printf("mF=%d\n", &mF);
  printf("mN=%d\n", &mN);
  printf("k0=%d\n", &k0);
  printf("Ncb=%d\n", &Ncb);
  printf("Qm=%d\n", &Qm);
  printf("E=%d\n", &E);
  printf("mBGn=%d\n", &mBGn);
  // for (int i = 0; i < 197; ++i) {
  //   printf("vNodesIdx_BG2[%d]:", &i);
  //   printf("%hd\n", &vNodesIdx_BG2[i]);
  //   printf("nShifts_BG2[%d]:", &i);
  //   printf("%hd\n", &nShifts_BG2[i]);
  // }

  __v4096i8 recover;
  vclaim(recover);
  recover = cbsRateRecoverLDPC(msg, mK, mZc, mF, k0, Ncb, Qm, E);
  recover = vmul(recover, 1, MASKREAD_OFF, Ncb);
  // VENUS_PRINTVEC_CHAR(recover, Ncb);

  // ldpc decode
  __v4096i8  LLR;
  __v4096i8  decoded;
  __v2048i16 index;
  vclaim(LLR);
  vclaim(decoded);
  vclaim(index);

  vrange(index, Ncb);
  index = vadd(index, 2 * mZc, MASKREAD_OFF, Ncb);
  vshuffle(LLR, index, recover, SHUFFLE_SCATTER, Ncb);

  decoded = nrLDPCDecoder(mBGn, mZc, mN, mF, mK, LLR);
  decoded = vmul(decoded, 1, MASKREAD_OFF, mK - mF);
  // VENUS_PRINTVEC_CHAR(decoded, 100);

  // VENUS_PRINT_CHAR(decoded, mN);
  short_struct pdschbits_length;
  pdschbits_length.data = mK - mF;

  vreturn(decoded, sizeof(decoded), &pdschbits_length, sizeof(pdschbits_length));
}