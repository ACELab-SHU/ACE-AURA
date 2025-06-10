/**
 * ****************************************
 * @file        Task_nrTBS.c
 * @brief       caculate TB sise
 * @author      yuanfeng
 * @date        2024.7.28
 * @copyright   ACE-Lab(Shanghai University)
 * ****************************************
 */
#include <stdint.h>

#include "data_type.h"
#include "riscv_printf.h"
#include "venus.h"
#include "vmath.h"

/*
 * min()/max()/clamp() macros that also do
 * strict type-checking.. See the
 * "unnecessary" pointer comparison.
 */
// #define min(x, y)                                                                                                      \
//   ({                                                                                                                   \
//     typeof(x) _min1 = (x);                                                                                             \
//     typeof(y) _min2 = (y);                                                                                             \
//     (void)(&_min1 == &_min2);                                                                                          \
//     _min1 < _min2 ? _min1 : _min2;                                                                                     \
//   })
//
// #define max(x, y) \
//   ({ \
//     typeof(x) _max1 = (x); \
//     typeof(y) _max2 = (y); \
//     (void)(&_max1 == &_max2); \
//     _max1 > _max2 ? _max1 : _max2; \
//   })

typedef short __v2048i16 __attribute__((ext_vector_type(2048)));
typedef short __v4096i16 __attribute__((ext_vector_type(4096)));
typedef char  __v4096i8 __attribute__((ext_vector_type(4096)));

static uint32_t tbsTable[93] = {
    24,   32,   40,   48,   56,   64,   72,   80,   88,   96,   104,  112,  120,  128,  136,  144,  152,  160,  168,
    176,  184,  192,  208,  224,  240,  256,  272,  288,  304,  320,  336,  352,  368,  384,  408,  432,  456,  480,
    504,  528,  552,  576,  608,  640,  672,  704,  736,  768,  808,  848,  888,  928,  984,  1032, 1064, 1128, 1160,
    1192, 1224, 1256, 1288, 1320, 1352, 1416, 1480, 1544, 1608, 1672, 1736, 1800, 1864, 1928, 2024, 2088, 2152, 2216,
    2280, 2408, 2472, 2536, 2600, 2664, 2728, 2792, 2856, 2976, 3104, 3240, 3368, 3496, 3624, 3752, 3824};

VENUS_INLINE uint32_t getTBSFromTable(uint32_t ndInfo) {
  for (int i = 0; i < 93; ++i)
    if (tbsTable[i] >= ndInfo)
      return tbsTable[i];

  return 0;
}

VENUS_INLINE uint32_t nrTBS(uint32_t modulation, uint32_t nlayers, uint32_t nPRB, uint32_t nREPerPRB, float tcr,
                            double xOh, double tbScaling) {
  uint32_t nREPrime  = nREPerPRB - xOh;
  uint32_t nRE       = min(nREPerPRB, 156) * nPRB;
  float    nInfo     = tbScaling * nRE * tcr * modulation * nlayers;
  int      nInfo_int = nInfo * 1000;
  if (nInfo <= 3824.0) {
    float n          = max(3.0, floor_log2(nInfo) - 6);
    int   n_int      = n * 1000;
    float ndInfo     = max(24.0, (1 << (int)n) * floor(nInfo / (1 << (int)n)));
    int   ndInfo_int = ndInfo * 1000;
    return getTBSFromTable((uint32_t)ndInfo);
  } else {
    float    n      = floor_log2(nInfo - 24) - 5;
    float    ndInfo = max(3840.0, (1 << (int)n) * round((nInfo - 24) / (1 << (int)n)));
    uint32_t c;
    if (tcr <= 0.25)
      c = (uint32_t)(ceil((nInfo + 24) / 3816));
    else {
      if (nInfo > 8424)
        c = (uint32_t)(ceil((nInfo + 24) / 8424));
      else
        c = 1;
    }
    uint32_t vout = (uint32_t)(8 * c * ceil((ndInfo + 24) / (8 * c)) - 24);
    return vout;
  }
}

int Task_nrTBS(nrPDSCHConfig pdsch, short_struct in_nREPerPRB, double_struct in_xOh, double_struct in_tbScaling) {
  uint32_t modulation = pdsch.Modulation;
  uint32_t nlayers    = pdsch.NumLayers;
  float    tcr        = pdsch.TargetCodeRate;
  uint32_t nPRBs      = pdsch.PRBSet_length;
  uint32_t nREPerPRB  = in_nREPerPRB.data;
  double   xOh        = in_xOh.data;
  double   tbScaling  = in_tbScaling.data;
  int      res        = nrTBS(modulation, nlayers, nPRBs, nREPerPRB, tcr, xOh, tbScaling);
  // int      res        = nrTBS(2, 1, 10, 108, 0.6631, 0, 1);
  // printf("res:%d\n", &res);

  int_struct out_res;
  out_res.data = res;

  vreturn(&out_res, sizeof(out_res));
}