#ifndef _LTE_DATA_TYPE_H_
#define _LTE_DATA_TYPE_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

typedef struct ltePCFICHConfig {
  uint16_t NCellID;
////   uint16_t NSizeGrid;  // 单位：RB
////   uint16_t NStartGrid; // 单位：RB
  uint16_t NDLRB;             // 单位：RB
  //uint16_t NFrame;
  // uint16_t NSubframe;
  // uint16_t NSlot;
  // uint8_t SymbolsPerSlot;
  //// uint16_t SlotsPerSubframe;
  //// uint16_t SlotsPerFrame;
  //// uint16_t SubCarrierSpacing; // KHz
  uint8_t  DuplexMode;        // 0:TDD，1:FDD
  uint8_t  CyclicPrefix;      // 0:Normal 1:Extended
} __attribute__((aligned(64))) ltePCFICHConfig;


#ifdef __cplusplus
}
#endif

#endif