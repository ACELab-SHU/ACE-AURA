/*
 * @Author: YihaoShen
 * @Date: 2024-07-28 18:36:27
 * @LastEditors: YihaoShen
 * @LastEditTime: 2024-07-29 13:04:38
 * @Description: nrPDSCHIndices Generation
 *
 * Copyright (c) 2024 by ACE_Lab, All Rights Reserved.
 */
#include "data_type.h"
#include "riscv_printf.h"
#include "venus.h"
#include <stdint.h>

typedef short __v2048i16 __attribute__((ext_vector_type(2048)));
typedef char  __v4096i8 __attribute__((ext_vector_type(4096)));

#define INF 0xFFFF

typedef struct fTABLE {
  uint8_t MappingTypeB; // Flag to indicate if mapping type is set to 'B' or not (0 or 1). Default is the indication of
                        // mapping type B for PDSCH
  uint8_t IntraSlotFreqHoppingFlag; // Flag to indicate the presence of intra-slot frequency hopping (Default 0)
} __attribute__((aligned(64))) fTABLE;

// // input for test
// nrCarrierConfig carrier;
// nrPDSCHConfig   pdsch;
// carrier.NCellID           = 0;
// carrier.SubCarrierSpacing = 15;
// carrier.CyclicPrefix      = 0;
// carrier.NSizeGrid         = 24;
// carrier.NStartGrid        = 0;
// carrier.NSlot             = 1;
// carrier.NFrame            = 434;
// carrier.SymbolsPerSlot    = 14;
// carrier.SlotsPerSubframe  = 1;
// carrier.SlotsPerFrame     = 10;
// carrier.SubCarrierSpacing = 15;

// pdsch.Modulation          = 2;
// pdsch.NumLayers           = 1;
// pdsch.MappingType         = 0;
// pdsch.SymbolAllocation[0] = 2;
// pdsch.SymbolAllocation[1] = 12;
// pdsch.PRBSet_length       = 10;
// for (int i = 0; i < pdsch.PRBSet_length; i++) {
//   /* code */
//   pdsch.PRBSet[i] = i;
// }
// pdsch.VRBToPRBInterleaving = 0;
// pdsch.VRBBundleSize        = 2;
// pdsch.RNTI                 = 65535;
// pdsch.EnablePTRS           = 0;

// pdsch.DMRS.DMRSConfigurationType          = 1;
// pdsch.DMRS.DMRSReferencePoint             = 0;
// pdsch.DMRS.NumCDMGroupsWithoutData        = 2;
// pdsch.DMRS.DMRSTypeAPosition              = 2;
// pdsch.DMRS.DMRSAdditionalPosition         = 2;
// pdsch.DMRS.DMRSLength                     = 1;
// pdsch.DMRS.CustomSymbolSet_length         = 0;
// pdsch.DMRS.DMRSPortSet[0]                 = 0;
// pdsch.DMRS.NSCID                          = 0;
// pdsch.DMRS.CDMGroups                      = 0;
// pdsch.DMRS.DeltaShifts[0]                 = 0;
// pdsch.DMRS.DeltaShifts_length             = 1;
// pdsch.DMRS.FrequencyWeights[0]            = 1;
// pdsch.DMRS.FrequencyWeights[1]            = 1;
// pdsch.DMRS.TimeWeights[0]                 = 1;
// pdsch.DMRS.TimeWeights[1]                 = 1;
// pdsch.DMRS.DMRSSubcarrierLocations_length = 6;
// pdsch.DMRS.DMRSSubcarrierLocations[0]     = 0;
// pdsch.DMRS.DMRSSubcarrierLocations[1]     = 2;
// pdsch.DMRS.DMRSSubcarrierLocations[2]     = 4;
// pdsch.DMRS.DMRSSubcarrierLocations[3]     = 6;
// pdsch.DMRS.DMRSSubcarrierLocations[4]     = 8;
// pdsch.DMRS.DMRSSubcarrierLocations[5]     = 10;
// pdsch.DMRS.CDMLengths[0]                  = 1;
// pdsch.DMRS.CDMLengths[1]                  = 1;
// pdsch.ReservedPRBSetLen                   = 0;

/**
 * @Task_name: Task_nrPDSCHIndices
 * @input:
 *  1. pdsch —— nrPDSCHConfig
 *  2. carrier —— nrCarrierConfig
 * @output:
 *  1. pdsch_index_out —— pdsch info index
 * */

int Task_nrPDSCHIndices(nrPDSCHConfig pdsch, nrCarrierConfig carrier) {

  VSPM_OPEN();

  uint16_t pdsch_NSizeBWP  = pdsch.NSizeBWP;
  uint16_t pdsch_NStartBWP = pdsch.NStartBWP;
  uint16_t nSizeGrid       = carrier.NSizeGrid;
  uint16_t nStartGrid      = carrier.NStartGrid;
  uint16_t symbperslot     = carrier.SymbolsPerSlot;
  uint16_t nSizeBWP        = (pdsch_NSizeBWP == 0) ? nSizeGrid : pdsch_NSizeBWP;
  uint16_t nStartBWP       = (pdsch_NStartBWP == 0) ? nStartGrid : pdsch_NStartBWP;
  uint16_t nRBSC           = 12;

  uint8_t nPDSCHSym;
  uint8_t nPDSCHStart;
  uint8_t symbolset[14]; // nPDSCHSym
  // initializeResources
  if (pdsch.SymbolAllocation[1]) {
    nPDSCHStart = pdsch.SymbolAllocation[0];
    nPDSCHSym   = pdsch.SymbolAllocation[1];
    for (int i = 0; i < nPDSCHSym; ++i) {
      symbolset[i] = nPDSCHStart + i;
    }
  }

  // Assign the structure ftable to pass into the initializeResources
  // internal function
  fTABLE ftable;
  ftable.MappingTypeB             = pdsch.MappingType == 1 ? 1 : 0;
  ftable.IntraSlotFreqHoppingFlag = 0;

  uint8_t  dmrssymbolset[14]; // dmrssymbolsetTempCount
  uint16_t dmrssymbolset_length;
  uint8_t  ldash[14]; // dmrssymbolsetTempCount; // Treat as single-symbol
  uint16_t ldash_length;

  if (pdsch.DMRS.CustomSymbolSet_length) {
    // Custom symbol locations
    uint8_t dmrssymbolsetTemp[14];
    uint8_t dmrssymbolsetTempCount = 0;
    for (int i = 0; i < nPDSCHSym; ++i) {
      for (int j = 0; j < pdsch.DMRS.CustomSymbolSet_length; ++j) {
        if ((pdsch.DMRS.CustomSymbolSet[j] < symbperslot) && (symbolset[i] == pdsch.DMRS.CustomSymbolSet[j])) {
          dmrssymbolsetTemp[dmrssymbolsetTempCount++] = symbolset[i];
        }
      }
    }

    for (int i = 0; i < dmrssymbolsetTempCount; ++i) {
      dmrssymbolset[i] = dmrssymbolsetTemp[i];
    }
  } else {

    uint8_t nsymbols;
    if (nPDSCHSym) {
      uint8_t lb = 255, ub = 0;
      for (int i = 0; i < nPDSCHSym; ++i) {
        if (lb > symbolset[i])
          lb = symbolset[i];
        if (ub < symbolset[i])
          ub = symbolset[i];
      }
      if (!ftable.MappingTypeB)
        lb = 0;
      nsymbols = ub - lb + 1;
    } else {
      nsymbols = 0;
    }

    // Different processing is required depending on whether frequency
    // hopping is enabled or not
    uint8_t dmrssymbolsetTemp[14];
    for (int i = 0; i < 14; ++i) {
      dmrssymbolsetTemp[i] = 0;
    }
    uint8_t  dmrssymbolsetTempCount = 0;
    uint8_t  pos1[2];
    uint16_t pos1_length;
    uint8_t  pos2[2];
    uint16_t pos2_length;
    if (nsymbols) {
      if (ftable.IntraSlotFreqHoppingFlag) {
        // Get the relevant single symbol hopping table
        if ((pdsch.DMRS.DMRSLength == 1) ||
            (pdsch.DMRS.DMRSAdditionalPosition <= 1)) { // No DM-RS for double symbols or addpos > 1 defined
          // Get the hop duration dependent symbol DM-RS position information
          uint8_t n1 = nsymbols / 2;  // Number of symbols in first hop
          uint8_t n2 = nsymbols - n1; // Number of symbols in second hop
          // First/second hop positions, defined relative to start of each hop

          if (n1) {
            if (ftable.MappingTypeB) {
              switch (n1) {
              case 0:
              case 1:
              case 2:
              case 3:
              case 4:
                if (pdsch.DMRS.DMRSAdditionalPosition == 0) {
                  pos1_length = 1;
                  pos1[0]     = 0;
                } else {
                  pos1_length = 1;
                  pos1[0]     = 0;
                }
                break;
              case 5:
              case 6:
              case 7:
                if (pdsch.DMRS.DMRSAdditionalPosition == 0) {
                  pos1_length = 1;
                  pos1[0]     = 0;
                } else {
                  pos1_length = 2;
                  pos1[0]     = 0;
                  pos1[1]     = 4;
                }
                break;
              default:
                pos1_length = 0;
                break;
              }
            } else {
              if (pdsch.DMRS.DMRSTypeAPosition == 2) {
                switch (n1) {
                case 4:
                case 5:
                case 6:
                  if (pdsch.DMRS.DMRSAdditionalPosition == 0) {
                    pos1_length = 1;
                    pos1[0]     = 2;
                  } else {
                    pos1_length = 1;
                    pos1[0]     = 2;
                  }
                  break;
                case 7:
                  if (pdsch.DMRS.DMRSAdditionalPosition == 0) {
                    pos1_length = 1;
                    pos1[0]     = 2;
                  } else {
                    pos1_length = 2;
                    pos1[0]     = 2;
                    pos1[1]     = 6;
                  }
                  break;
                default:
                  pos1_length = 0;
                  break;
                }
              } else if (pdsch.DMRS.DMRSTypeAPosition == 3) {
                switch (n1) {
                case 4:
                case 5:
                case 6:
                case 7:
                  if (pdsch.DMRS.DMRSAdditionalPosition == 0) {
                    pos1_length = 1;
                    pos1[0]     = 3;
                  } else {
                    pos1_length = 1;
                    pos1[0]     = 3;
                  }
                  break;
                default:
                  pos1_length = 0;
                  break;
                }
              }
            }
          } else {
            pos1_length = 0; // Degenerate case of no first hop
          }

          if (ftable.MappingTypeB) {
            switch (n2) {
            case 0:
            case 1:
            case 2:
            case 3:
            case 4:
              if (pdsch.DMRS.DMRSAdditionalPosition == 0) {
                pos2_length = 1;
                pos2[0]     = 0;
              } else {
                pos2_length = 1;
                pos2[0]     = 0;
              }
              break;
            case 5:
            case 6:
            case 7:
              if (pdsch.DMRS.DMRSAdditionalPosition == 0) {
                pos2_length = 1;
                pos2[0]     = 0;
              } else {
                pos2_length = 2;
                pos2[0]     = 0;
                pos2[1]     = 4;
              }
              break;
            default:
              pos2_length = 0;
              break;
            }
          } else {
            if (pdsch.DMRS.DMRSTypeAPosition == 2) {
              switch (n2) {
              case 4:
                if (pdsch.DMRS.DMRSAdditionalPosition == 0) {
                  pos2_length = 1;
                  pos2[0]     = 0;
                } else {
                  pos2_length = 1;
                  pos2[0]     = 0;
                }
                break;
              case 5:
              case 6:
              case 7:
                if (pdsch.DMRS.DMRSAdditionalPosition == 0) {
                  pos2_length = 1;
                  pos2[0]     = 0;
                } else {
                  pos2_length = 2;
                  pos2[0]     = 0;
                  pos2[1]     = 4;
                }
                break;
              default:
                pos2_length = 0;
                break;
              }
            } else if (pdsch.DMRS.DMRSTypeAPosition == 3) {
              switch (n2) {
              case 4:
                if (pdsch.DMRS.DMRSAdditionalPosition == 0) {
                  pos2_length = 1;
                  pos2[0]     = 0;
                } else {
                  pos2_length = 1;
                  pos2[0]     = 0;
                }
                break;
              case 5:
              case 6:
              case 7:
                if (pdsch.DMRS.DMRSAdditionalPosition == 0) {
                  pos2_length = 1;
                  pos2[0]     = 0;
                } else {
                  pos2_length = 2;
                  pos2[0]     = 0;
                  pos2[1]     = 4;
                }
                break;
              default:
                pos2_length = 0;
                break;
              }
            }
          }

          // Combine and adjust second position
          for (int i = 0; i < pos1_length; ++i) {
            dmrssymbolsetTemp[i] = pos1[i];
          }
          for (int i = 0; i < pos1_length; ++i) {
            dmrssymbolsetTemp[i] = n2 + pos2[i];
          }
          dmrssymbolsetTempCount = pos1_length + pos2_length;
        }
      } else {
        // Otherwise get the relevant non-hopping table
        // Get the duration dependent symbol DM-RS position information
        if (pdsch.DMRS.DMRSAdditionalPosition < 4) {
          uint8_t  pos[14];
          uint16_t pos_length;
          if (pdsch.DMRS.DMRSLength == 1) {
            if (ftable.MappingTypeB) {
              switch (nsymbols) {
              case 0:
              case 1:
              case 2:
              case 3:
              case 4:
                switch (pdsch.DMRS.DMRSAdditionalPosition) {
                case 0:
                case 1:
                case 2:
                case 3:
                  pos_length = 1;
                  pos[0]     = 0;
                  break;
                default:
                  pos_length = 0;
                  break;
                }
                break;
              case 5:
              case 6:
              case 7:
                switch (pdsch.DMRS.DMRSAdditionalPosition) {
                case 0:
                  pos_length = 1;
                  pos[0]     = 0;
                  break;
                case 1:
                case 2:
                case 3:
                  pos_length = 2;
                  pos[0]     = 0;
                  pos[1]     = 4;
                  break;
                default:
                  pos_length = 0;
                  break;
                }
                break;
              case 8:
              case 9:
                switch (pdsch.DMRS.DMRSAdditionalPosition) {
                case 0:
                  pos_length = 1;
                  pos[0]     = 0;
                  break;
                case 1:
                  pos_length = 2;
                  pos[0]     = 0;
                  pos[1]     = 6;
                  break;
                case 2:
                case 3:
                  pos_length = 3;
                  pos[0]     = 0;
                  pos[1]     = 3;
                  pos[2]     = 6;
                  break;
                default:
                  pos_length = 0;
                  break;
                }
                break;
              case 10:
              case 11:
                switch (pdsch.DMRS.DMRSAdditionalPosition) {
                case 0:
                  pos_length = 1;
                  pos[0]     = 0;
                  break;
                case 1:
                  pos_length = 2;
                  pos[0]     = 0;
                  pos[1]     = 8;
                  break;
                case 2:
                  pos_length = 3;
                  pos[0]     = 0;
                  pos[1]     = 4;
                  pos[2]     = 8;
                  break;
                case 3:
                  pos_length = 4;
                  pos[0]     = 0;
                  pos[1]     = 3;
                  pos[2]     = 6;
                  pos[3]     = 9;
                  break;
                default:
                  pos_length = 0;
                  break;
                }
                break;
              case 12:
              case 13:
              case 14:
                switch (pdsch.DMRS.DMRSAdditionalPosition) {
                case 0:
                  pos_length = 1;
                  pos[0]     = 0;
                  break;
                case 1:
                  pos_length = 2;
                  pos[0]     = 0;
                  pos[1]     = 10;
                  break;
                case 2:
                  pos_length = 3;
                  pos[0]     = 0;
                  pos[1]     = 5;
                  pos[2]     = 10;
                  break;
                case 3:
                  pos_length = 4;
                  pos[0]     = 0;
                  pos[1]     = 3;
                  pos[2]     = 6;
                  pos[3]     = 9;
                  break;
                default:
                  pos_length = 0;
                  break;
                }
                break;
              default:
                pos_length = 0;
                break;
              }
            } else {

              switch (nsymbols) {
              case 4:
              case 5:
              case 6:
              case 7:
                switch (pdsch.DMRS.DMRSAdditionalPosition) {
                case 0:
                case 1:
                case 2:
                case 3:
                  pos_length = 1;
                  pos[0]     = 0;
                  break;
                default:
                  pos_length = 0;
                  break;
                }
                break;
              case 8:
              case 9:
                switch (pdsch.DMRS.DMRSAdditionalPosition) {
                case 0:
                  pos_length = 1;
                  pos[0]     = 0;
                  break;
                case 1:
                case 2:
                case 3:
                  pos_length = 2;
                  pos[0]     = 0;
                  pos[1]     = 7;
                  break;
                default:
                  pos_length = 0;
                  break;
                }
                break;
              case 10:
              case 11:
                switch (pdsch.DMRS.DMRSAdditionalPosition) {
                case 0:
                  pos_length = 1;
                  pos[0]     = 0;
                  break;
                case 1:
                  pos_length = 2;
                  pos[0]     = 0;
                  pos[1]     = 9;
                  break;
                case 2:
                case 3:
                  pos_length = 3;
                  pos[0]     = 0;
                  pos[1]     = 6;
                  pos[2]     = 9;
                  break;
                default:
                  pos_length = 0;
                  break;
                }
                break;
              case 12:
                switch (pdsch.DMRS.DMRSAdditionalPosition) {
                case 0:
                  pos_length = 1;
                  pos[0]     = 0;
                  break;
                case 1:
                  pos_length = 2;
                  pos[0]     = 0;
                  pos[1]     = 9;
                  break;
                case 2:
                  pos_length = 3;
                  pos[0]     = 0;
                  pos[1]     = 6;
                  pos[2]     = 9;
                  break;
                case 3:
                  pos_length = 3;
                  pos[0]     = 0;
                  pos[1]     = 5;
                  pos[2]     = 8;
                  pos[3]     = 11;
                  break;
                default:
                  pos_length = 0;
                  break;
                }
                break;
              case 13:
              case 14:
                switch (pdsch.DMRS.DMRSAdditionalPosition) {
                case 0:
                  pos_length = 1;
                  pos[0]     = 0;
                  break;
                case 1:
                  pos_length = 2;
                  pos[0]     = 0;
                  pos[1]     = 11;
                  break;
                case 2:
                  pos_length = 3;
                  pos[0]     = 0;
                  pos[1]     = 7;
                  pos[2]     = 11;
                  break;
                case 3:
                  pos_length = 4;
                  pos[0]     = 0;
                  pos[1]     = 5;
                  pos[2]     = 8;
                  pos[3]     = 11;
                  break;
                default:
                  pos_length = 0;
                  break;
                }
                break;
              default:
                pos_length = 0;
                break;
              }
            }
          } else { // dmrsLength == 2
            if (ftable.MappingTypeB) {
              switch (nsymbols) {
              case 5:
              case 6:
              case 7:
                switch (pdsch.DMRS.DMRSAdditionalPosition) {
                case 0:
                case 1:
                  pos_length = 1;
                  pos[0]     = 0;
                  break;
                default:
                  pos_length = 0;
                  break;
                }
                break;
              case 8:
              case 9:
                switch (pdsch.DMRS.DMRSAdditionalPosition) {
                case 0:
                  pos_length = 1;
                  pos[0]     = 0;
                  break;
                case 1:
                  pos_length = 2;
                  pos[0]     = 0;
                  pos[1]     = 5;
                  break;
                default:
                  pos_length = 0;
                  break;
                }
                break;
              case 10:
              case 11:
                switch (pdsch.DMRS.DMRSAdditionalPosition) {
                case 0:
                  pos_length = 1;
                  pos[0]     = 0;
                  break;
                case 1:
                  pos_length = 2;
                  pos[0]     = 0;
                  pos[1]     = 7;
                  break;
                default:
                  pos_length = 0;
                  break;
                }
                break;
              case 12:
              case 13:
              case 14:
                switch (pdsch.DMRS.DMRSAdditionalPosition) {
                case 0:
                  pos_length = 1;
                  pos[0]     = 0;
                  break;
                case 1:
                  pos_length = 2;
                  pos[0]     = 0;
                  pos[1]     = 9;
                  break;
                default:
                  pos_length = 0;
                  break;
                }
                break;
              default:
                pos_length = 0;
                break;
              }
            } else {
              switch (nsymbols) {
              case 4:
              case 5:
              case 6:
              case 7:
              case 8:
              case 9:
                switch (pdsch.DMRS.DMRSAdditionalPosition) {
                case 0:
                case 1:
                  pos_length = 1;
                  pos[0]     = 0;
                  break;
                default:
                  pos_length = 0;
                  break;
                }
                break;
              case 10:
              case 11:
              case 12:
                switch (pdsch.DMRS.DMRSAdditionalPosition) {
                case 0:
                  pos_length = 1;
                  pos[0]     = 0;
                  break;
                case 1:
                  pos_length = 2;
                  pos[0]     = 0;
                  pos[1]     = 8;
                  break;
                default:
                  pos_length = 0;
                  break;
                }
                break;
              case 13:
              case 14:
                switch (pdsch.DMRS.DMRSAdditionalPosition) {
                case 0:
                  pos_length = 1;
                  pos[0]     = 0;
                  break;
                case 1:
                  pos_length = 2;
                  pos[0]     = 0;
                  pos[1]     = 10;
                  break;
                default:
                  pos_length = 0;
                  break;
                }
                break;
              default:
                pos_length = 0;
                break;
              }
            }
          }

          for (int i = 0; i < pos_length; ++i) {
            dmrssymbolsetTemp[i] = pos[i];
          }
          dmrssymbolsetTempCount = pos_length;
        }
        pos1_length = 1;
      }
    }
    // Adjust table information
    if (dmrssymbolsetTempCount) {
      // Adjust indices for the relative offset of the mapping type
      if (ftable.MappingTypeB) {
        for (int i = 0; i < dmrssymbolsetTempCount; ++i) {
          dmrssymbolsetTemp[i] += symbolset[0]; // If type B (non slot-wise)
        }
      } else {
        if (pos1_length) {
          dmrssymbolsetTemp[0] = pdsch.DMRS.DMRSTypeAPosition; // If type A (slot-wise) then 2 or 3
        }
      }
      // Adjust for double-symbol DM-RS
      // In the operational system, if RRC configured with max length 2
      // then the actual length is DCI signaled
      uint8_t ldashTemp[dmrssymbolsetTempCount * pdsch.DMRS.DMRSLength];
      for (int i = 0; i < dmrssymbolsetTempCount * pdsch.DMRS.DMRSLength; ++i) {
        ldashTemp[i] = 0;
      }
      if (pdsch.DMRS.DMRSLength == 2) {
        for (int i = dmrssymbolsetTempCount - 1; i >= 0; --i) {
          dmrssymbolsetTemp[2 * i + 1] = dmrssymbolsetTemp[i] + 1;
          dmrssymbolsetTemp[2 * i]     = dmrssymbolsetTemp[i];
        }
        dmrssymbolsetTempCount = 2 * dmrssymbolsetTempCount;
        for (int i = 1; i < dmrssymbolsetTempCount * pdsch.DMRS.DMRSLength; i += 2) {
          ldashTemp[i] = 1;
        }
      }
      // For non-standard set-ups, only return the DM-RS symbol indices
      // that overlap the actual allocation indices
      uint8_t ind[nPDSCHSym];
      uint8_t indCount = 0;
      for (int i = 0; i < nPDSCHSym; ++i) {
        for (int j = 0; j < dmrssymbolsetTempCount; ++j) {
          if (symbolset[i] == dmrssymbolsetTemp[j]) {
            ind[indCount++] = j;
          }
        }
      }
      // dmrssymbolset [indCount]
      dmrssymbolset_length = indCount;
      for (int i = 0; i < indCount; ++i) {
        dmrssymbolset[i] = dmrssymbolsetTemp[ind[i]];
      }
      //*ldash = creatMatrixUint8(1, indCount);
      ldash_length = indCount;
      for (int i = 0; i < indCount; ++i) {
        ldash[i] = ldashTemp[ind[i]];
      }
    } else {
      dmrssymbolset_length = 0;
      ldash_length         = 0;
    }
  }

  // Set of physical resource blocks
  uint8_t bwp[nSizeBWP];
  for (int i = 0; i < nSizeBWP; ++i) {
    bwp[i] = 0;
  }
  for (int i = 0; i < pdsch.PRBSet_length; ++i) {
    bwp[pdsch.PRBSet[i] % nSizeBWP] = 1;
  }
  uint8_t prbsetCount = 0;
  for (int i = 0; i < nSizeBWP; ++i) {
    if (bwp[i])
      prbsetCount++;
  }
  uint8_t  prbset[275];
  uint16_t prbset_length = prbsetCount;
  prbsetCount            = 0;
  for (int i = 0; i < nSizeBWP; ++i) {
    if (bwp[i])
      prbset[prbsetCount++] = i;
  }
  prbset[prbsetCount] = INF;

  uint8_t prbsetInterleave[275];
  if (pdsch.VRBToPRBInterleaving) {
    // Error!!!!!!!!还没写
    // Matrix_uint8 mapIndices;
    // vrbToPRBInterleaver(nSizeBWP, nStartBWP, pdsch.VRBBundleSize, &mapIndices);
  } else {
    for (int i = 0; i < prbsetCount; i++) {
      prbsetInterleave[i] = prbset[i];
    }
  }

  uint16_t prbcell[carrier.SymbolsPerSlot][prbset_length];
  for (int i = 0; i < carrier.SymbolsPerSlot; ++i) {
    for (int j = 0; j < prbset_length; ++j) {
      prbcell[i][j] = prbset[j];
    }
  }

  short flag = 0;

  for (int i = 0; i < pdsch.ReservedPRBSetLen; ++i) {
    // TODO: 暂时不考虑这个参数，占用内存太大
    // uint16_t reservedperiod = pdsch.ReservedPRBSet[i].Period;
    // uint16_t offset         = (carrier.NSlot % reservedperiod) * carrier.SymbolsPerSlot;
    // uint8_t  inter[48];
    // uint16_t interCNT     = 0;
    // uint16_t inter_length = 0;
    // if (nPDSCHSym != 0 && pdsch.ReservedPRBSet[i].SymbolSet_length != 0) {
    //   uint8_t  temp[48];
    //   uint16_t temp_length = carrier.SymbolsPerSlot;
    //   for (int j = 0; j < nPDSCHSym; ++j)
    //     temp[symbolset[j]] = 1;
    //   for (int j = 0; j < pdsch.ReservedPRBSet[i].SymbolSet_length; ++j) {
    //     uint16_t resSymbol = pdsch.ReservedPRBSet[i].SymbolSet[j] - offset;
    //     if ((resSymbol < carrier.SymbolsPerSlot) & ((int16_t)resSymbol >= 0))
    //       temp[resSymbol] += 1;
    //   }
    //   inter_length = temp_length;
    //   for (int j = 0; j < temp_length; ++j) {
    //     if (temp[j] == 2) {
    //       inter[interCNT] = j;
    //       interCNT++;
    //     }
    //   }
    // }
  }

  uint8_t nports          = pdsch.NumLayers;
  uint8_t cdmgroupsnodata = pdsch.DMRS.NumCDMGroupsWithoutData; // 1 2 3

  uint8_t dmrssc[6];
  uint8_t dmrssc_length;
  uint8_t dshiftsnodata[3];
  uint8_t dshiftsnodata_length;
  // DM-RS subcarrier (SC) locations in a resource block
  if (pdsch.DMRS.DMRSConfigurationType == 1) {
    // Type 1: 6 DM-RS SC per PRB per CDM (every other SC)
    dmrssc[0]     = 0; // RE indices in a PRB
    dmrssc[1]     = 2;
    dmrssc[2]     = 4;
    dmrssc[3]     = 6;
    dmrssc[4]     = 8;
    dmrssc[5]     = 10;
    dmrssc_length = 6;
    if (cdmgroupsnodata < 2) {
      dshiftsnodata_length = cdmgroupsnodata;
      dshiftsnodata[0]     = 0;
    } else {
      dshiftsnodata_length = 2;
      dshiftsnodata[0]     = 0;
      dshiftsnodata[1]     = 1;
    }
  } else {
    // Type 2: 4 DM-RS SC per PRB per CDM (2 groups of 2 SC)
    dmrssc[0]     = 0; // RE indices in a PRB
    dmrssc[1]     = 1;
    dmrssc[2]     = 6;
    dmrssc[3]     = 7;
    dmrssc_length = 4; // RE indices in a PRB
    if (cdmgroupsnodata == 1) {
      dshiftsnodata_length = 1;
      dshiftsnodata[0]     = 0;
    } else if (cdmgroupsnodata == 2) {
      dshiftsnodata_length = 2;
      dshiftsnodata[0]     = 0;
      dshiftsnodata[1]     = 2;
    } else {
      dshiftsnodata_length = 3;
      dshiftsnodata[0]     = 0;
      dshiftsnodata[1]     = 2;
      dshiftsnodata[2]     = 4;
    }
  }

  uint8_t dshifts = pdsch.DMRS.DeltaShifts[0];
  uint8_t fullprb[nRBSC];
  for (int i = 0; i < nRBSC; i++) {
    fullprb[i] = 1;
  }

  uint8_t dshiftsComp[7];
  uint8_t dshiftsComp_length = pdsch.DMRS.DeltaShifts_length + dshiftsnodata_length;

  for (int i = 0; i < pdsch.DMRS.DeltaShifts_length; ++i)
    dshiftsComp[i] = pdsch.DMRS.DeltaShifts[i];
  for (int i = 0; i < dshiftsnodata_length; ++i)
    dshiftsComp[i + pdsch.DMRS.DeltaShifts_length] = dshiftsnodata[i];

  uint8_t dmrsre[dmrssc_length][dshiftsComp_length];
  for (int i = 0; i < dmrssc_length; ++i)
    for (int j = 0; j < dshiftsComp_length; ++j)
      dmrsre[i][j] = dmrssc[i] + dshiftsComp[j];
  uint8_t pdschre[nRBSC];
  for (int i = 0; i < dmrssc_length; ++i) {
    for (int j = 0; j < dshiftsComp_length; ++j) {
      if (dmrsre[i][j] < nRBSC)
        fullprb[dmrsre[i][j]] = 0;
    }
  }

  int pdschreCNT = 0;
  for (int i = 0; i < nRBSC; ++i) {
    if (fullprb[i])
      pdschre[pdschreCNT++] = i;
  }

  uint8_t recell[symbperslot][14];
  uint8_t recell_n_length[symbperslot];
  for (int i = 0; i < symbperslot; ++i) {
    recell_n_length[i] = 0;
  }
  for (int i = 0; i < nPDSCHSym; ++i) {
    recell_n_length[symbolset[i]] = 12;
    for (int j = 0; j < 12; ++j) {
      recell[symbolset[i]][j] = j;
    }
  }

  uint16_t numDMRSSymbols = dmrssymbolset_length;
  for (int i = 0; i < numDMRSSymbols; ++i) {
    recell_n_length[dmrssymbolset[i]] = pdschreCNT;
    for (int j = 0; j < pdschreCNT; ++j) {
      recell[dmrssymbolset[i]][j] = pdschre[j];
    }
  }

  uint16_t slotindicesCount = 0;
  for (int i = 0; i < nPDSCHSym; ++i) {
    if (prbcell[symbolset[i]][0] != INF) {
      slotindicesCount += prbset_length * recell_n_length[symbolset[i]];
    }
  }
  uint16_t slotindices[slotindicesCount];
  slotindicesCount = 0;
  for (int i = 0; i < nPDSCHSym; ++i) {
    if (prbcell[symbolset[i]][0] != INF) {
      for (int j = 0; j < prbset_length; ++j) {
        for (int k = 0; k < recell_n_length[symbolset[i]]; ++k) {
          slotindices[slotindicesCount++] = recell[symbolset[i]][k] +
                                            nRBSC * (prbcell[symbolset[i]][j] + nStartBWP - nStartGrid) +
                                            nRBSC * nSizeGrid * symbolset[i];
        }
      }
    }
  }

  uint16_t slotIndices_M[slotindicesCount];
  for (int i = 0; i < slotindicesCount; ++i) {
    slotIndices_M[i] = slotindices[i];
  }

  uint16_t ind[slotindicesCount];
  for (int i = 0; i < slotindicesCount; ++i) {
    ind[i] = slotIndices_M[i] + (nRBSC * symbperslot * nSizeGrid) * (nports - 1);
  }
  uint16_t ncw       = (nports > 4) + 1;
  uint16_t NREPerPRB = nRBSC * (nPDSCHSym - numDMRSSymbols) + pdschreCNT * numDMRSSymbols;

  short_struct info_NREPerPRB;
  info_NREPerPRB.data = NREPerPRB;

  __v2048i16 pdsch_index_out;
  vclaim(pdsch_index_out);
  vbarrier();
  VSPM_OPEN();
  //  wn
  int pdsch_index_out_addr = vaddr(pdsch_index_out);
  for (int i = 0; i < slotindicesCount; i++) {
    *(volatile unsigned short *)(pdsch_index_out_addr + (i << 1)) = ind[i] - symbolset[0] * nSizeGrid * 12;
  }
  VSPM_CLOSE();
  pdsch_index_out = vsadd(pdsch_index_out, 0, MASKREAD_OFF, 2048);

  VSPM_OPEN();
  short_struct out_NREPerPRB;
  out_NREPerPRB.data = NREPerPRB;

  short_struct out_subcarrierLength;
  out_subcarrierLength.data = carrier.NSizeGrid * 12;

  short_struct out_pdsch_symbol_length;
  out_pdsch_symbol_length.data = pdsch.SymbolAllocation[1];

  short_struct pdsch_index_length_out;
  pdsch_index_length_out.data = slotindicesCount;
  
  VSPM_CLOSE();

  vreturn(pdsch_index_out, slotindicesCount * 2, &pdsch_index_length_out, sizeof(pdsch_index_length_out),
          &out_NREPerPRB, sizeof(out_NREPerPRB), &out_subcarrierLength, sizeof(out_subcarrierLength),
          &out_pdsch_symbol_length, sizeof(out_pdsch_symbol_length));
  // vreturn(pdsch_index_out, sizeof(pdsch_index_out), &pdsch_index_length_out, sizeof(pdsch_index_length_out),
  //         &out_NREPerPRB, sizeof(out_NREPerPRB), &out_subcarrierLength, sizeof(out_subcarrierLength),
  //         &out_pdsch_symbol_length, sizeof(out_pdsch_symbol_length));
}
