/*
 * @Author: YihaoShen
 * @Date: 2024-07-20 23:48:03
 * @LastEditors: YihaoShen
 * @LastEditTime: 2024-07-29 13:01:51
 * @Description: PDSCH DMRS index generation
 *
 * Copyright (c) 2024 by ACE_Lab, All Rights Reserved.
 */

#include "data_type.h"
#include "riscv_printf.h"
#include "venus.h"
#include <stdint.h>

typedef short __v2048i16 __attribute__((ext_vector_type(2048)));
typedef char  __v4096i8 __attribute__((ext_vector_type(4096)));

typedef struct fTABLE {
  uint8_t MappingTypeB; // Flag to indicate if mapping type is set to 'B' or not (0 or 1). Default is the indication of
                        // mapping type B for PDSCH
  uint8_t IntraSlotFreqHoppingFlag; // Flag to indicate the presence of intra-slot frequency hopping (Default 0)
} __attribute__((aligned(64))) fTABLE;

// // input Parameter for test
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
 * @Task_name: Task_pdschDmrsIndices
 * @input:
 *  1. pdsch —— nrPDSCHConfig
 *  2. carrier —— nrCarrierConfig
 * @output:
 *  1. index_out —— pdsch dmrs index
 * */

int Task_nrPDSCHDMRSIndices(nrPDSCHConfig pdsch, nrCarrierConfig carrier) {
  uint16_t pdsch_NSizeBWP  = pdsch.NSizeBWP;
  uint16_t pdsch_NStartBWP = pdsch.NStartBWP;
  uint16_t nSizeGrid       = carrier.NSizeGrid;
  uint16_t nStartGrid      = carrier.NStartGrid;
  uint16_t nSizeBWP        = (pdsch_NSizeBWP == 0) ? nSizeGrid : pdsch_NSizeBWP;
  uint16_t nStartBWP       = (pdsch_NStartBWP == 0) ? nStartGrid : pdsch_NStartBWP;
  uint16_t symbperslot     = carrier.SymbolsPerSlot;
  uint16_t nRBSC           = 12;
  uint16_t nports          = pdsch.NumLayers;

  uint8_t symbolset[14]; // nPDSCHSym
  uint8_t nPDSCHSym;
  uint8_t nPDSCHStart;

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

  // //

  uint16_t index_dmrs[10000];
  uint16_t index_dmrs_length;
  uint16_t prbcell[carrier.SymbolsPerSlot][prbset_length];

  uint8_t flag;

  if (dmrssymbolset_length) {

    // out = prbcell
    for (int i = 0; i < carrier.SymbolsPerSlot; ++i) {
      for (int j = 0; j < prbset_length; ++j) {
        prbcell[i][j] = prbset[j];
      }
    }
    flag = 0;

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

    uint16_t ndmrssc  = pdsch.DMRS.DMRSSubcarrierLocations_length;
    uint32_t nTotalRE = nRBSC * symbperslot * nSizeGrid;
    if (flag) {
      for (int i = 0; i < dmrssymbolset_length; ++i) {
        ;
      }
    } else {
      uint32_t len      = ndmrssc * prbset_length;
      index_dmrs_length = dmrssymbolset_length * len;
      for (int i = 0; i < dmrssymbolset_length; ++i) {
        uint16_t prbActive[len];
        uint16_t prbActive_length = len;
        for (int j = 0; j < prbset_length; ++j) {
          for (int k = 0; k < ndmrssc; ++k) {
            prbActive[j * ndmrssc + k] = nRBSC * prbcell[dmrssymbolset[i]][j] + nStartBWP - nStartGrid;
          }
        }
        for (int j = 0; j < prbset_length; ++j)
          for (int k = 0; k < pdsch.DMRS.DMRSSubcarrierLocations_length; ++k)
            index_dmrs[i * len + j * pdsch.DMRS.DMRSSubcarrierLocations_length + k] =
                pdsch.DMRS.DMRSSubcarrierLocations[k];
        for (int j = 0; j < prbActive_length; ++j)
          index_dmrs[i * len + j] += (prbActive[j] + nRBSC * nSizeGrid * dmrssymbolset[i]);
        for (int j = 0; j < nports; ++j) {
          for (int k = 0; k < prbActive_length; ++k) {
            index_dmrs[i * len + k] += nTotalRE * j;
          }
        }
      }
    }
  }

  __v2048i16 index_out;
  vclaim(index_out);
  vbarrier();
  VSPM_OPEN();
  int index_out_addr = vaddr(index_out);
  for (int i = 0; i < index_dmrs_length; i++) {
    *(volatile unsigned short *)(index_out_addr + (i << 1)) = index_dmrs[i] - symbolset[0] * nSizeGrid * 12;
  }
  VSPM_CLOSE();
  index_out = vadd(index_out, 0, MASKREAD_OFF, 2048);

  __v2048i16 dmrs_symbol_location;
  vclaim(dmrs_symbol_location);
  vbarrier();
  VSPM_OPEN();
  int dmrs_symbol_location_addr = vaddr(dmrs_symbol_location);
  for (int i = 0; i < dmrssymbolset_length; i++) {
    *(volatile unsigned short *)(dmrs_symbol_location_addr + (i << 1)) = dmrssymbolset[i] - symbolset[0];
  }
  VSPM_CLOSE();

  short_struct dmrs_symbol_length;
  dmrs_symbol_length.data = dmrssymbolset_length;

  short_struct dmrs_length_one_symbol;
  dmrs_length_one_symbol.data = index_dmrs_length / dmrssymbolset_length;

  short_struct dmrs_interval;
  dmrs_interval.data = 2;

  vreturn(index_out, sizeof(index_out), dmrs_symbol_location, sizeof(dmrs_symbol_location), &dmrs_symbol_length,
          sizeof(dmrs_symbol_length), &dmrs_length_one_symbol, sizeof(dmrs_length_one_symbol), &dmrs_interval,
          sizeof(dmrs_interval));
}
