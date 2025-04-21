#include "venus_ext.h"
#include <algorithm>
#include <cassert>
#include <cstdlib>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <sys/stat.h> // 对于 UNIX 或 Linux 系统
#include <vector>

Venus_Emulator::~Venus_Emulator(){};

Venus_Emulator::Venus_Emulator() {
  for (int i = 0; i < this->VENUS_DSPM_ROW; i++)
    for (int j = 0; j < this->VENUS_DSPM_LANE; j++)
      this->VENUS_DSPM[i][j] = 0;
  for (int i = 0; i < this->VENUS_DSPM_ROW * this->VENUS_DSPM_LANE *
                          this->VENUS_DSPM_BANK_PER_LANE;
       i++)
    this->VENUS_MASK_DSPM[i] = false;
  vins_issue_idx = 0;
};
int Venus_Emulator::get_row_length(uint16_t avl) {
  if (this->MY_VENUS_INS_PARAM.vew == EW8)
    return ceil(avl / this->VENUS_DSPM_LANE / 2);
  else
    return ceil(avl / this->VENUS_DSPM_LANE);
}

// the elements in the tuple represent row bank and mask
tuple<uint16_t, uint16_t, uint16_t>
Venus_Emulator::get_abs_addr_and_mask(uint16_t idx, uint16_t start_row,
                                      bool forced_ew16 = false) {
  if (forced_ew16 == true)
    return make_tuple(floor(idx * 1.0 / this->VENUS_DSPM_LANE /
                            this->VENUS_DSPM_BANK_PER_LANE) +
                          start_row,
                      idx - (floor(idx * 1.0 / this->VENUS_DSPM_LANE /
                                   this->VENUS_DSPM_BANK_PER_LANE)) *
                                this->VENUS_DSPM_LANE *
                                this->VENUS_DSPM_BANK_PER_LANE,
                      0xffff);
  else {
    if (this->MY_VENUS_INS_PARAM.vew == EW8) {
      // std::cout <<std::fixed << std::setprecision(2) << (idx - (floor(idx*1.0
      // / 2 / this->VENUS_DSPM_LANE /
      // this->VENUS_DSPM_BANK_PER_LANE))*2*this->VENUS_DSPM_LANE*this->VENUS_DSPM_BANK_PER_LANE)/2
      // << endl;
      if (((int)(idx - (floor(idx * 1.0 / 2 / this->VENUS_DSPM_LANE /
                              this->VENUS_DSPM_BANK_PER_LANE)) *
                           2 * this->VENUS_DSPM_LANE *
                           this->VENUS_DSPM_BANK_PER_LANE) %
           2) == 0)
        return make_tuple(
            floor(idx * 1.0 / 2 / this->VENUS_DSPM_LANE /
                  this->VENUS_DSPM_BANK_PER_LANE) +
                start_row,
            floor((idx - (floor(idx * 1.0 / 2 / this->VENUS_DSPM_LANE /
                                this->VENUS_DSPM_BANK_PER_LANE)) *
                             2 * this->VENUS_DSPM_LANE *
                             this->VENUS_DSPM_BANK_PER_LANE) /
                  2),
            0xff);
      else
        return make_tuple(
            floor(idx * 1.0 / 2 / this->VENUS_DSPM_LANE /
                  this->VENUS_DSPM_BANK_PER_LANE) +
                start_row,
            floor((idx - (floor(idx * 1.0 / 2 / this->VENUS_DSPM_LANE /
                                this->VENUS_DSPM_BANK_PER_LANE)) *
                             2 * this->VENUS_DSPM_LANE *
                             this->VENUS_DSPM_BANK_PER_LANE) /
                  2),
            0xff00);
    } else
      return make_tuple(floor(idx * 1.0 / this->VENUS_DSPM_LANE /
                              this->VENUS_DSPM_BANK_PER_LANE) +
                            start_row,
                        idx - (floor(idx * 1.0 / this->VENUS_DSPM_LANE /
                                     this->VENUS_DSPM_BANK_PER_LANE)) *
                                  this->VENUS_DSPM_LANE *
                                  this->VENUS_DSPM_BANK_PER_LANE,
                        0xffff);
  }
}

// the elements in the tuple represent row bank and mask
tuple<uint16_t, uint16_t, uint16_t>
Venus_Emulator::get_abs_addr_and_mask(uint16_t abs_idx) {
  if (this->MY_VENUS_INS_PARAM.vew == EW8)
    return make_tuple(
        floor(abs_idx * 1.0 / 2 / this->VENUS_DSPM_LANE /
              this->VENUS_DSPM_BANK_PER_LANE),
        ceil((abs_idx - (floor(abs_idx * 1.0 / 2 / this->VENUS_DSPM_LANE /
                               this->VENUS_DSPM_BANK_PER_LANE)) *
                            2 * this->VENUS_DSPM_LANE *
                            this->VENUS_DSPM_BANK_PER_LANE) /
             2),
        0xff);
  else
    return make_tuple(floor(abs_idx * 1.0 / this->VENUS_DSPM_LANE /
                            this->VENUS_DSPM_BANK_PER_LANE),
                      abs_idx - (floor(abs_idx * 1.0 / this->VENUS_DSPM_LANE /
                                       this->VENUS_DSPM_BANK_PER_LANE)) *
                                    this->VENUS_DSPM_LANE *
                                    this->VENUS_DSPM_BANK_PER_LANE,
                      0xffff);
}

void Venus_Emulator::venus_execute() {
  uint16_t vs1_row, vs1_bank, vs1_mask;
  uint16_t vs2_row, vs2_bank, vs2_mask;
  uint16_t vd1_row, vd1_bank, vd1_mask;
  uint16_t vd2_row, vd2_bank, vd2_mask;
  int16_t temp_res;
  switch (this->MY_VENUS_INS_PARAM.op) {
  ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
  // BitALU
  ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
  case VSHUFFLE_CLBMV: {
    // this vins is for hardware, no action is needed in emulator
    this->instr_name = (char *)"VSHUFFLE_CLBMV";
    break;
  }
  case VAND: {
    this->instr_name = (char *)"VAND";
    for (int i = 0; i < this->MY_VENUS_INS_PARAM.avl; i++) {
      tie(vs1_row, vs1_bank, vs1_mask) =
          this->get_abs_addr_and_mask(i, this->MY_VENUS_INS_PARAM.vs1_head);
      tie(vs2_row, vs2_bank, vs2_mask) =
          this->get_abs_addr_and_mask(i, this->MY_VENUS_INS_PARAM.vs2_head);
      tie(vd1_row, vd1_bank, vd1_mask) =
          this->get_abs_addr_and_mask(i, this->MY_VENUS_INS_PARAM.vd1_head);
      if ((!this->MY_VENUS_INS_PARAM.vmask_read) ||
          (this->MY_VENUS_INS_PARAM.vmask_read && this->VENUS_MASK_DSPM[i])) {
        if (this->MY_VENUS_INS_PARAM.function3 == IVX)
          if (vd1_mask == 0xff00)
            temp_res =
                ((this->VENUS_DSPM[vs2_row][vs2_bank] & (vs2_mask)) >> 8) &
                this->MY_VENUS_INS_PARAM.scalar_op;
          else
            temp_res = (this->VENUS_DSPM[vs2_row][vs2_bank] & (vs2_mask)) &
                       this->MY_VENUS_INS_PARAM.scalar_op;
        else // IVV
            if (vd1_mask == 0xff00)
          temp_res = ((this->VENUS_DSPM[vs2_row][vs2_bank] & (vs2_mask)) >> 8) &
                     ((this->VENUS_DSPM[vs1_row][vs1_bank] & (vs1_mask)) >> 8);
        else
          temp_res = (this->VENUS_DSPM[vs2_row][vs2_bank] & (vs2_mask)) &
                     (this->VENUS_DSPM[vs1_row][vs1_bank] & (vs1_mask));
        // temp_res = (this->VENUS_DSPM[vs2_row][vs2_bank] & (vs2_mask)) &
        //            (this->VENUS_DSPM[vs1_row][vs1_bank] & (vs1_mask));
        if (vd1_mask == 0xff00)
          this->VENUS_DSPM[vd1_row][vd1_bank] =
              (this->VENUS_DSPM[vd1_row][vd1_bank] & (~vd1_mask)) +
              ((temp_res << 8) & vd1_mask);
        else
          this->VENUS_DSPM[vd1_row][vd1_bank] =
              (this->VENUS_DSPM[vd1_row][vd1_bank] & (~vd1_mask)) +
              (temp_res & vd1_mask);
      } else {
        this->VENUS_DSPM[vd1_row][vd1_bank] =
            (this->VENUS_DSPM[vs1_row][vs1_bank] & (vs1_mask)) +
            (this->VENUS_DSPM[vd1_row][vd1_bank] & (~vd1_mask));
      }
    }
    break;
  }
  case VOR: {
    this->instr_name = (char *)"VOR";
    for (int i = 0; i < this->MY_VENUS_INS_PARAM.avl; i++) {
      tie(vs1_row, vs1_bank, vs1_mask) =
          this->get_abs_addr_and_mask(i, this->MY_VENUS_INS_PARAM.vs1_head);
      tie(vs2_row, vs2_bank, vs2_mask) =
          this->get_abs_addr_and_mask(i, this->MY_VENUS_INS_PARAM.vs2_head);
      tie(vd1_row, vd1_bank, vd1_mask) =
          this->get_abs_addr_and_mask(i, this->MY_VENUS_INS_PARAM.vd1_head);
      if ((!this->MY_VENUS_INS_PARAM.vmask_read) ||
          (this->MY_VENUS_INS_PARAM.vmask_read && this->VENUS_MASK_DSPM[i])) {
        if (this->MY_VENUS_INS_PARAM.function3 == IVX) {
          if (vd1_mask == 0xff00)
            temp_res =
                ((this->VENUS_DSPM[vs2_row][vs2_bank] & (vs2_mask)) >> 8) |
                this->MY_VENUS_INS_PARAM.scalar_op;
          else
            temp_res = (this->VENUS_DSPM[vs2_row][vs2_bank] & (vs2_mask)) |
                       this->MY_VENUS_INS_PARAM.scalar_op;
        } else // IVV
            if (vd1_mask == 0xff00)
          temp_res = ((this->VENUS_DSPM[vs2_row][vs2_bank] & (vs2_mask)) >> 8) |
                     ((this->VENUS_DSPM[vs1_row][vs1_bank] & (vs1_mask)) >> 8);
        else
          temp_res = (this->VENUS_DSPM[vs2_row][vs2_bank] & (vs2_mask)) |
                     (this->VENUS_DSPM[vs1_row][vs1_bank] & (vs1_mask));
        if (vd1_mask == 0xff00)
          this->VENUS_DSPM[vd1_row][vd1_bank] =
              (this->VENUS_DSPM[vd1_row][vd1_bank] & (~vd1_mask)) +
              ((temp_res << 8) & vd1_mask);
        else
          this->VENUS_DSPM[vd1_row][vd1_bank] =
              (this->VENUS_DSPM[vd1_row][vd1_bank] & (~vd1_mask)) +
              (temp_res & vd1_mask);
      } else {
        this->VENUS_DSPM[vd1_row][vd1_bank] =
            (this->VENUS_DSPM[vs1_row][vs1_bank] & (vs1_mask)) +
            (this->VENUS_DSPM[vd1_row][vd1_bank] & (~vd1_mask));
      }
    }
    break;
  }
  case VBRDCST: {
    this->instr_name = (char *)"VBRDCST";
    for (int i = 0; i < this->MY_VENUS_INS_PARAM.avl; i++) {
      tie(vd1_row, vd1_bank, vd1_mask) =
          this->get_abs_addr_and_mask(i, this->MY_VENUS_INS_PARAM.vd1_head);
      if ((!this->MY_VENUS_INS_PARAM.vmask_read) ||
          (this->MY_VENUS_INS_PARAM.vmask_read && this->VENUS_MASK_DSPM[i]))
        if (vd1_mask == 0xff00)
          this->VENUS_DSPM[vd1_row][vd1_bank] =
              (this->VENUS_DSPM[vd1_row][vd1_bank] & (~vd1_mask)) +
              ((this->MY_VENUS_INS_PARAM.scalar_op << 8) & vd1_mask);
        else
          this->VENUS_DSPM[vd1_row][vd1_bank] =
              (this->VENUS_DSPM[vd1_row][vd1_bank] & (~vd1_mask)) +
              (this->MY_VENUS_INS_PARAM.scalar_op & vd1_mask);
      // this->VENUS_DSPM[vd1_row][vd1_bank] =
      // (this->VENUS_DSPM[vd1_row][vd1_bank] & (~vd1_mask)) +
      // (this->MY_VENUS_INS_PARAM.scalar_op & vd1_mask);
    }
    break;
  }
  case VSLL: {
    this->instr_name = (char *)"VSLL";
    for (int i = 0; i < this->MY_VENUS_INS_PARAM.avl; i++) {
      tie(vs1_row, vs1_bank, vs1_mask) =
          this->get_abs_addr_and_mask(i, this->MY_VENUS_INS_PARAM.vs1_head);
      tie(vs2_row, vs2_bank, vs2_mask) =
          this->get_abs_addr_and_mask(i, this->MY_VENUS_INS_PARAM.vs2_head);
      tie(vd1_row, vd1_bank, vd1_mask) =
          this->get_abs_addr_and_mask(i, this->MY_VENUS_INS_PARAM.vd1_head);
      if ((!this->MY_VENUS_INS_PARAM.vmask_read) ||
          (this->MY_VENUS_INS_PARAM.vmask_read && this->VENUS_MASK_DSPM[i])) {
        if (this->MY_VENUS_INS_PARAM.function3 == IVX) {
          if (vd1_mask == 0xff00)
            temp_res = ((this->VENUS_DSPM[vs2_row][vs2_bank] & (vs2_mask)) >> 8)
                       << this->MY_VENUS_INS_PARAM.scalar_op;
          else
            temp_res = (this->VENUS_DSPM[vs2_row][vs2_bank] & (vs2_mask))
                       << this->MY_VENUS_INS_PARAM.scalar_op;
        } else // IVV
            if (vd1_mask == 0xff00)
          temp_res =
              ((this->VENUS_DSPM[vs2_row][vs2_bank] & (vs2_mask)) >> 8)
              << ((this->VENUS_DSPM[vs1_row][vs1_bank] & (vs1_mask)) >> 8);
        else
          temp_res = (this->VENUS_DSPM[vs2_row][vs2_bank] & (vs2_mask))
                     << (this->VENUS_DSPM[vs1_row][vs1_bank] & (vs1_mask));
        // temp_res = (this->VENUS_DSPM[vs2_row][vs2_bank] & (vs2_mask)) <<
        // (this->VENUS_DSPM[vs1_row][vs1_bank] & (vs1_mask));
        if (vd1_mask == 0xff00)
          this->VENUS_DSPM[vd1_row][vd1_bank] =
              (this->VENUS_DSPM[vd1_row][vd1_bank] & (~vd1_mask)) +
              ((temp_res << 8) & vd1_mask);
        else
          this->VENUS_DSPM[vd1_row][vd1_bank] =
              (this->VENUS_DSPM[vd1_row][vd1_bank] & (~vd1_mask)) +
              (temp_res & vd1_mask);
      } else {
        this->VENUS_DSPM[vd1_row][vd1_bank] =
            (this->VENUS_DSPM[vs1_row][vs1_bank] & (vs1_mask)) +
            (this->VENUS_DSPM[vd1_row][vd1_bank] & (~vd1_mask));
      }
    }
    break;
  }
  case VSRL: {
    this->instr_name = (char *)"VSRL";
    for (int i = 0; i < this->MY_VENUS_INS_PARAM.avl; i++) {
      tie(vs1_row, vs1_bank, vs1_mask) =
          this->get_abs_addr_and_mask(i, this->MY_VENUS_INS_PARAM.vs1_head);
      tie(vs2_row, vs2_bank, vs2_mask) =
          this->get_abs_addr_and_mask(i, this->MY_VENUS_INS_PARAM.vs2_head);
      tie(vd1_row, vd1_bank, vd1_mask) =
          this->get_abs_addr_and_mask(i, this->MY_VENUS_INS_PARAM.vd1_head);
      if ((!this->MY_VENUS_INS_PARAM.vmask_read) ||
          (this->MY_VENUS_INS_PARAM.vmask_read && this->VENUS_MASK_DSPM[i])) {
        if (this->MY_VENUS_INS_PARAM.function3 == IVX) {
          if (vd1_mask == 0xff00)
            temp_res =
                ((this->VENUS_DSPM[vs2_row][vs2_bank] & (vs2_mask)) >> 8) >>
                this->MY_VENUS_INS_PARAM.scalar_op;
          else
            temp_res = (this->VENUS_DSPM[vs2_row][vs2_bank] & (vs2_mask)) >>
                       this->MY_VENUS_INS_PARAM.scalar_op;
        } else // IVV
            if (vd1_mask == 0xff00)
          temp_res =
              ((uint16_t)(this->VENUS_DSPM[vs2_row][vs2_bank] & (vs2_mask)) >>
               8) >>
              ((uint16_t)(this->VENUS_DSPM[vs1_row][vs1_bank] & (vs1_mask)) >>
               8);
        else
          temp_res =
              ((uint16_t)this->VENUS_DSPM[vs2_row][vs2_bank] & (vs2_mask)) >>
              (this->VENUS_DSPM[vs1_row][vs1_bank] & (vs1_mask));
        // temp_res = ((uint16_t)this->VENUS_DSPM[vs2_row][vs2_bank] &
        // (vs2_mask)) >>
        //            (this->VENUS_DSPM[vs1_row][vs1_bank] & (vs1_mask));
        if (vd1_mask == 0xff00)
          this->VENUS_DSPM[vd1_row][vd1_bank] =
              (this->VENUS_DSPM[vd1_row][vd1_bank] & (~vd1_mask)) +
              ((temp_res << 8) & vd1_mask);
        else
          this->VENUS_DSPM[vd1_row][vd1_bank] =
              (this->VENUS_DSPM[vd1_row][vd1_bank] & (~vd1_mask)) +
              (temp_res & vd1_mask);
      } else {
        this->VENUS_DSPM[vd1_row][vd1_bank] =
            (this->VENUS_DSPM[vs1_row][vs1_bank] & (vs1_mask)) +
            (this->VENUS_DSPM[vd1_row][vd1_bank] & (~vd1_mask));
      }
    }
    break;
  }
  case VSRA: {
    this->instr_name = (char *)"VSRA";
    for (int i = 0; i < this->MY_VENUS_INS_PARAM.avl; i++) {
      tie(vs1_row, vs1_bank, vs1_mask) =
          this->get_abs_addr_and_mask(i, this->MY_VENUS_INS_PARAM.vs1_head);
      tie(vs2_row, vs2_bank, vs2_mask) =
          this->get_abs_addr_and_mask(i, this->MY_VENUS_INS_PARAM.vs2_head);
      tie(vd1_row, vd1_bank, vd1_mask) =
          this->get_abs_addr_and_mask(i, this->MY_VENUS_INS_PARAM.vd1_head);
      if ((!this->MY_VENUS_INS_PARAM.vmask_read) ||
          (this->MY_VENUS_INS_PARAM.vmask_read && this->VENUS_MASK_DSPM[i])) {
        if (this->MY_VENUS_INS_PARAM.function3 == IVX)
          if (vd1_mask == 0xff00)
            temp_res =
                ((int16_t)(this->VENUS_DSPM[vs2_row][vs2_bank] & (vs2_mask)) >>
                 8) >>
                this->MY_VENUS_INS_PARAM.scalar_op;
          else if (vd1_mask == 0x00ff)
            temp_res =
                ((int16_t)((this->VENUS_DSPM[vs2_row][vs2_bank] & (vs2_mask))
                           << 8) >>
                 8) >>
                this->MY_VENUS_INS_PARAM.scalar_op;
          else
            temp_res =
                (int16_t)(this->VENUS_DSPM[vs2_row][vs2_bank] & (vs2_mask)) >>
                this->MY_VENUS_INS_PARAM.scalar_op;
        else // IVV
            if (vd1_mask == 0xff00)
          temp_res =
              ((int16_t)(this->VENUS_DSPM[vs2_row][vs2_bank] & (vs2_mask)) >>
               8) >>
              ((int16_t)(this->VENUS_DSPM[vs1_row][vs1_bank] & (vs1_mask)) >>
               8);
        else if (vd1_mask == 0x00ff)
          temp_res =
              ((int16_t)((this->VENUS_DSPM[vs2_row][vs2_bank] & (vs2_mask))
                         << 8) >>
               8) >>
              ((int16_t)((this->VENUS_DSPM[vs1_row][vs1_bank] & (vs1_mask))
                         << 8) >>
               8);
        else
          temp_res =
              (int16_t)(this->VENUS_DSPM[vs2_row][vs2_bank] & (vs2_mask)) >>
              (int16_t)(this->VENUS_DSPM[vs1_row][vs1_bank] & (vs1_mask));
        // temp_res = (this->VENUS_DSPM[vs2_row][vs2_bank] & (vs2_mask)) >>
        //            (this->VENUS_DSPM[vs1_row][vs1_bank] & (vs1_mask));
        if (vd1_mask == 0xff00)
          this->VENUS_DSPM[vd1_row][vd1_bank] =
              (this->VENUS_DSPM[vd1_row][vd1_bank] & (~vd1_mask)) +
              ((temp_res << 8) & vd1_mask);
        else
          this->VENUS_DSPM[vd1_row][vd1_bank] =
              (this->VENUS_DSPM[vd1_row][vd1_bank] & (~vd1_mask)) +
              (temp_res & vd1_mask);
      } else {
        this->VENUS_DSPM[vd1_row][vd1_bank] =
            (this->VENUS_DSPM[vs1_row][vs1_bank] & (vs1_mask)) +
            (this->VENUS_DSPM[vd1_row][vd1_bank] & (~vd1_mask));
      }
    }
    break;
  }
  case VXOR: {
    this->instr_name = (char *)"VXOR";
    for (int i = 0; i < this->MY_VENUS_INS_PARAM.avl; i++) {
      tie(vs1_row, vs1_bank, vs1_mask) =
          this->get_abs_addr_and_mask(i, this->MY_VENUS_INS_PARAM.vs1_head);
      tie(vs2_row, vs2_bank, vs2_mask) =
          this->get_abs_addr_and_mask(i, this->MY_VENUS_INS_PARAM.vs2_head);
      tie(vd1_row, vd1_bank, vd1_mask) =
          this->get_abs_addr_and_mask(i, this->MY_VENUS_INS_PARAM.vd1_head);
      if ((!this->MY_VENUS_INS_PARAM.vmask_read) ||
          (this->MY_VENUS_INS_PARAM.vmask_read && this->VENUS_MASK_DSPM[i])) {
        if (this->MY_VENUS_INS_PARAM.function3 == IVX)
          if (vd1_mask == 0xff00)
            temp_res =
                ((this->VENUS_DSPM[vs2_row][vs2_bank] & (vs2_mask)) >> 8) ^
                this->MY_VENUS_INS_PARAM.scalar_op;
          else
            temp_res = (this->VENUS_DSPM[vs2_row][vs2_bank] & (vs2_mask)) ^
                       this->MY_VENUS_INS_PARAM.scalar_op;
        else // IVV
            if (vd1_mask == 0xff00)
          temp_res = ((this->VENUS_DSPM[vs2_row][vs2_bank] & (vs2_mask)) >> 8) ^
                     ((this->VENUS_DSPM[vs1_row][vs1_bank] & (vs1_mask)) >> 8);
        else
          temp_res = (this->VENUS_DSPM[vs2_row][vs2_bank] & (vs2_mask)) ^
                     (this->VENUS_DSPM[vs1_row][vs1_bank] & (vs1_mask));
        // temp_res = (this->VENUS_DSPM[vs2_row][vs2_bank] & (vs2_mask)) ^
        //            (this->VENUS_DSPM[vs1_row][vs1_bank] & (vs1_mask));
        if (vd1_mask == 0xff00)
          this->VENUS_DSPM[vd1_row][vd1_bank] =
              (this->VENUS_DSPM[vd1_row][vd1_bank] & (~vd1_mask)) +
              ((temp_res << 8) & vd1_mask);
        else
          this->VENUS_DSPM[vd1_row][vd1_bank] =
              (this->VENUS_DSPM[vd1_row][vd1_bank] & (~vd1_mask)) +
              (temp_res & vd1_mask);
      } else {
        this->VENUS_DSPM[vd1_row][vd1_bank] =
            (this->VENUS_DSPM[vs1_row][vs1_bank] & (vs1_mask)) +
            (this->VENUS_DSPM[vd1_row][vd1_bank] & (~vd1_mask));
      }
    }
    break;
  }
  case VSEQ: {
    this->instr_name = (char *)"VSEQ";
    for (int i = 0; i < this->MY_VENUS_INS_PARAM.avl; i++) {
      tie(vs1_row, vs1_bank, vs1_mask) =
          this->get_abs_addr_and_mask(i, this->MY_VENUS_INS_PARAM.vs1_head);
      tie(vs2_row, vs2_bank, vs2_mask) =
          this->get_abs_addr_and_mask(i, this->MY_VENUS_INS_PARAM.vs2_head);
      tie(vd1_row, vd1_bank, vd1_mask) =
          this->get_abs_addr_and_mask(i, this->MY_VENUS_INS_PARAM.vd1_head);
      if ((!this->MY_VENUS_INS_PARAM.vmask_read) ||
          (this->MY_VENUS_INS_PARAM.vmask_read && this->VENUS_MASK_DSPM[i])) {
        switch (this->MY_VENUS_INS_PARAM.function3) {
        case IVV: {
          if (vd1_mask == 0xff00)
            temp_res =
                ((int16_t)(this->VENUS_DSPM[vs2_row][vs2_bank] & (vs2_mask)) >>
                 8) ==
                ((int16_t)(this->VENUS_DSPM[vs1_row][vs1_bank] & (vs1_mask)) >>
                 8);
          else
            temp_res =
                ((int16_t)(this->VENUS_DSPM[vs2_row][vs2_bank] & (vs2_mask))) ==
                ((int16_t)(this->VENUS_DSPM[vs1_row][vs1_bank] & (vs1_mask)));
          if (vd1_mask == 0xff00)
            this->VENUS_DSPM[vd1_row][vd1_bank] =
                (this->VENUS_DSPM[vd1_row][vd1_bank] & (~vd1_mask)) +
                ((temp_res << 8) & vd1_mask);
          else
            this->VENUS_DSPM[vd1_row][vd1_bank] =
                (this->VENUS_DSPM[vd1_row][vd1_bank] & (~vd1_mask)) +
                (temp_res & vd1_mask);
          // temp_res = (int16_t)(this->VENUS_DSPM[vs2_row][vs2_bank] &
          // (vs2_mask)) ==
          //            (int16_t)(this->VENUS_DSPM[vs1_row][vs1_bank] &
          //            (vs1_mask));
          // this->VENUS_DSPM[vd1_row][vd1_bank] =
          // (this->VENUS_DSPM[vd1_row][vd1_bank] & (~vd1_mask)) + (temp_res &
          // vd1_mask);
          break;
        }
        case IVX: {
          if (vd1_mask == 0xff00)
            temp_res =
                ((int16_t)(this->VENUS_DSPM[vs2_row][vs2_bank] & (vs2_mask)) >>
                 8) == this->MY_VENUS_INS_PARAM.scalar_op;
          else
            temp_res =
                ((int16_t)(this->VENUS_DSPM[vs2_row][vs2_bank] & (vs2_mask))) ==
                this->MY_VENUS_INS_PARAM.scalar_op;
          if (vd1_mask == 0xff00)
            this->VENUS_DSPM[vd1_row][vd1_bank] =
                (this->VENUS_DSPM[vd1_row][vd1_bank] & (~vd1_mask)) +
                ((temp_res << 8) & vd1_mask);
          else
            this->VENUS_DSPM[vd1_row][vd1_bank] =
                (this->VENUS_DSPM[vd1_row][vd1_bank] & (~vd1_mask)) +
                (temp_res & vd1_mask);
          break;
        }
        case MVV: {
          if (vd1_mask == 0xff00)
            temp_res =
                ((int16_t)(this->VENUS_DSPM[vs2_row][vs2_bank] & (vs2_mask)) >>
                 8) ==
                ((int16_t)(this->VENUS_DSPM[vs1_row][vs1_bank] & (vs1_mask)) >>
                 8);
          else
            temp_res =
                ((int16_t)(this->VENUS_DSPM[vs2_row][vs2_bank] & (vs2_mask))) ==
                ((int16_t)(this->VENUS_DSPM[vs1_row][vs1_bank] & (vs1_mask)));
          // temp_res = (int16_t)(this->VENUS_DSPM[vs2_row][vs2_bank] &
          // (vs2_mask)) ==
          //            (int16_t)(this->VENUS_DSPM[vs1_row][vs1_bank] &
          //            (vs1_mask));
          this->VENUS_MASK_DSPM[i] = (bool)temp_res;
          break;
        }
        case MVX: {
          if (vd1_mask == 0xff00)
            temp_res =
                ((int16_t)(this->VENUS_DSPM[vs2_row][vs2_bank] & (vs2_mask)) >>
                 8) == this->MY_VENUS_INS_PARAM.scalar_op;
          else
            temp_res =
                ((int16_t)(this->VENUS_DSPM[vs2_row][vs2_bank] & (vs2_mask))) ==
                this->MY_VENUS_INS_PARAM.scalar_op;
          this->VENUS_MASK_DSPM[i] = (bool)temp_res;
          break;
        }
        default: {
          // TODO: other cases
          std::cout << "TODO: VSEQ other cases" << endl;
          exit(EXIT_FAILURE);
          break;
        }
        }
      } else {
        this->VENUS_DSPM[vd1_row][vd1_bank] =
            (this->VENUS_DSPM[vs1_row][vs1_bank] & (vs1_mask)) +
            (this->VENUS_DSPM[vd1_row][vd1_bank] & (~vd1_mask));
      }
    }
    break;
  }
  case VSNE: {
    this->instr_name = (char *)"VSNE";
    for (int i = 0; i < this->MY_VENUS_INS_PARAM.avl; i++) {
      tie(vs1_row, vs1_bank, vs1_mask) =
          this->get_abs_addr_and_mask(i, this->MY_VENUS_INS_PARAM.vs1_head);
      tie(vs2_row, vs2_bank, vs2_mask) =
          this->get_abs_addr_and_mask(i, this->MY_VENUS_INS_PARAM.vs2_head);
      tie(vd1_row, vd1_bank, vd1_mask) =
          this->get_abs_addr_and_mask(i, this->MY_VENUS_INS_PARAM.vd1_head);
      if ((!this->MY_VENUS_INS_PARAM.vmask_read) ||
          (this->MY_VENUS_INS_PARAM.vmask_read && this->VENUS_MASK_DSPM[i])) {
        switch (this->MY_VENUS_INS_PARAM.function3) {
        case IVV: {
          if (vd1_mask == 0xff00)
            temp_res =
                ((int16_t)(this->VENUS_DSPM[vs2_row][vs2_bank] & (vs2_mask)) >>
                 8) !=
                ((int16_t)(this->VENUS_DSPM[vs1_row][vs1_bank] & (vs1_mask)) >>
                 8);
          else
            temp_res =
                ((int16_t)(this->VENUS_DSPM[vs2_row][vs2_bank] & (vs2_mask))) !=
                ((int16_t)(this->VENUS_DSPM[vs1_row][vs1_bank] & (vs1_mask)));
          if (vd1_mask == 0xff00)
            this->VENUS_DSPM[vd1_row][vd1_bank] =
                (this->VENUS_DSPM[vd1_row][vd1_bank] & (~vd1_mask)) +
                ((temp_res << 8) & vd1_mask);
          else
            this->VENUS_DSPM[vd1_row][vd1_bank] =
                (this->VENUS_DSPM[vd1_row][vd1_bank] & (~vd1_mask)) +
                (temp_res & vd1_mask);
          // temp_res = (int16_t)(this->VENUS_DSPM[vs2_row][vs2_bank] &
          // (vs2_mask)) !=
          //            (int16_t)(this->VENUS_DSPM[vs1_row][vs1_bank] &
          //            (vs1_mask));
          // this->VENUS_DSPM[vd1_row][vd1_bank] =
          // (this->VENUS_DSPM[vd1_row][vd1_bank] & (~vd1_mask)) + (temp_res &
          // vd1_mask);
          break;
        }
        case IVX: {
          if (vd1_mask == 0xff00)
            temp_res =
                ((int16_t)(this->VENUS_DSPM[vs2_row][vs2_bank] & (vs2_mask)) >>
                 8) != this->MY_VENUS_INS_PARAM.scalar_op;
          else
            temp_res =
                ((int16_t)(this->VENUS_DSPM[vs2_row][vs2_bank] & (vs2_mask))) !=
                this->MY_VENUS_INS_PARAM.scalar_op;
          if (vd1_mask == 0xff00)
            this->VENUS_DSPM[vd1_row][vd1_bank] =
                (this->VENUS_DSPM[vd1_row][vd1_bank] & (~vd1_mask)) +
                ((temp_res << 8) & vd1_mask);
          else
            this->VENUS_DSPM[vd1_row][vd1_bank] =
                (this->VENUS_DSPM[vd1_row][vd1_bank] & (~vd1_mask)) +
                (temp_res & vd1_mask);
          break;
        }
        case MVV: {
          if (vd1_mask == 0xff00)
            temp_res =
                ((int16_t)(this->VENUS_DSPM[vs2_row][vs2_bank] & (vs2_mask)) >>
                 8) !=
                ((int16_t)(this->VENUS_DSPM[vs1_row][vs1_bank] & (vs1_mask)) >>
                 8);
          else
            temp_res =
                ((int16_t)(this->VENUS_DSPM[vs2_row][vs2_bank] & (vs2_mask))) !=
                ((int16_t)(this->VENUS_DSPM[vs1_row][vs1_bank] & (vs1_mask)));
          // temp_res = (int16_t)(this->VENUS_DSPM[vs2_row][vs2_bank] &
          // (vs2_mask)) !=
          //            (int16_t)(this->VENUS_DSPM[vs1_row][vs1_bank] &
          //            (vs1_mask));
          this->VENUS_MASK_DSPM[i] = (bool)temp_res;
          break;
        }
        case MVX: {
          if (vd1_mask == 0xff00)
            temp_res =
                ((int16_t)(this->VENUS_DSPM[vs2_row][vs2_bank] & (vs2_mask)) >>
                 8) != this->MY_VENUS_INS_PARAM.scalar_op;
          else
            temp_res =
                ((int16_t)(this->VENUS_DSPM[vs2_row][vs2_bank] & (vs2_mask))) !=
                this->MY_VENUS_INS_PARAM.scalar_op;
          this->VENUS_MASK_DSPM[i] = (bool)temp_res;
          break;
        }
        default: {
          // TODO: other cases
          std::cout << "TODO: VSNE other cases" << endl;
          exit(EXIT_FAILURE);
          break;
        }
        }
      } else {
        this->VENUS_DSPM[vd1_row][vd1_bank] =
            (this->VENUS_DSPM[vs1_row][vs1_bank] & (vs1_mask)) +
            (this->VENUS_DSPM[vd1_row][vd1_bank] & (~vd1_mask));
      }
    }
    break;
  }
  case VSLTU: {
    this->instr_name = (char *)"VSLTU";
    for (int i = 0; i < this->MY_VENUS_INS_PARAM.avl; i++) {
      tie(vs1_row, vs1_bank, vs1_mask) =
          this->get_abs_addr_and_mask(i, this->MY_VENUS_INS_PARAM.vs1_head);
      tie(vs2_row, vs2_bank, vs2_mask) =
          this->get_abs_addr_and_mask(i, this->MY_VENUS_INS_PARAM.vs2_head);
      tie(vd1_row, vd1_bank, vd1_mask) =
          this->get_abs_addr_and_mask(i, this->MY_VENUS_INS_PARAM.vd1_head);
      if ((!this->MY_VENUS_INS_PARAM.vmask_read) ||
          (this->MY_VENUS_INS_PARAM.vmask_read && this->VENUS_MASK_DSPM[i])) {
        switch (this->MY_VENUS_INS_PARAM.function3) {
        case IVV: {
          temp_res =
              (uint16_t)(this->VENUS_DSPM[vs2_row][vs2_bank] & (vs2_mask)) >
              (uint16_t)(this->VENUS_DSPM[vs1_row][vs1_bank] & (vs1_mask));

          if (vd1_mask == 0xff00)
            this->VENUS_DSPM[vd1_row][vd1_bank] =
                (this->VENUS_DSPM[vd1_row][vd1_bank] & (~vd1_mask)) +
                ((temp_res << 8) & vd1_mask);
          else
            this->VENUS_DSPM[vd1_row][vd1_bank] =
                (this->VENUS_DSPM[vd1_row][vd1_bank] & (~vd1_mask)) +
                (temp_res & vd1_mask);

          // temp_res = (uint16_t)(this->VENUS_DSPM[vs2_row][vs2_bank] &
          // (vs2_mask)) >
          //            (uint16_t)(this->VENUS_DSPM[vs1_row][vs1_bank] &
          //            (vs1_mask));
          // this->VENUS_DSPM[vd1_row][vd1_bank] =
          // (this->VENUS_DSPM[vd1_row][vd1_bank] & (~vd1_mask)) + (temp_res &
          // vd1_mask);
          break;
        }
        case IVX: {
          if (vd1_mask == 0xff00)
            temp_res =
                ((uint16_t)(this->VENUS_DSPM[vs2_row][vs2_bank] & (vs2_mask)) >>
                 8) > (uint16_t)this->MY_VENUS_INS_PARAM.scalar_op;
          else
            temp_res =
                (uint16_t)((this->VENUS_DSPM[vs2_row][vs2_bank] & (vs2_mask))) >
                (uint16_t)this->MY_VENUS_INS_PARAM.scalar_op;
          if (vd1_mask == 0xff00)
            this->VENUS_DSPM[vd1_row][vd1_bank] =
                (this->VENUS_DSPM[vd1_row][vd1_bank] & (~vd1_mask)) +
                ((temp_res << 8) & vd1_mask);
          else
            this->VENUS_DSPM[vd1_row][vd1_bank] =
                (this->VENUS_DSPM[vd1_row][vd1_bank] & (~vd1_mask)) +
                (temp_res & vd1_mask);
          break;
        }
        case MVV: {
          temp_res =
              (uint16_t)(this->VENUS_DSPM[vs2_row][vs2_bank] & (vs2_mask)) >
              (uint16_t)(this->VENUS_DSPM[vs1_row][vs1_bank] & (vs1_mask));
          this->VENUS_MASK_DSPM[i] = (bool)temp_res;
          break;
        }
        case MVX: {
          if (vd1_mask == 0xff00)
            temp_res =
                ((uint16_t)(this->VENUS_DSPM[vs2_row][vs2_bank] & (vs2_mask)) >>
                 8) > (uint16_t)this->MY_VENUS_INS_PARAM.scalar_op;
          else
            temp_res =
                (uint16_t)((this->VENUS_DSPM[vs2_row][vs2_bank] & (vs2_mask))) >
                (uint16_t)this->MY_VENUS_INS_PARAM.scalar_op;
          this->VENUS_MASK_DSPM[i] = (bool)temp_res;
          break;
        }
        default: {
          // TODO: other cases
          std::cout << "TODO: VSLTU other cases" << endl;
          exit(EXIT_FAILURE);
          break;
        }
        }
      } else {
        this->VENUS_DSPM[vd1_row][vd1_bank] =
            (this->VENUS_DSPM[vs1_row][vs1_bank] & (vs1_mask)) +
            (this->VENUS_DSPM[vd1_row][vd1_bank] & (~vd1_mask));
      }
    }
    break;
  }
  case VSLT: {
    this->instr_name = (char *)"VSLT";
    for (int i = 0; i < this->MY_VENUS_INS_PARAM.avl; i++) {
      tie(vs1_row, vs1_bank, vs1_mask) =
          this->get_abs_addr_and_mask(i, this->MY_VENUS_INS_PARAM.vs1_head);
      tie(vs2_row, vs2_bank, vs2_mask) =
          this->get_abs_addr_and_mask(i, this->MY_VENUS_INS_PARAM.vs2_head);
      tie(vd1_row, vd1_bank, vd1_mask) =
          this->get_abs_addr_and_mask(i, this->MY_VENUS_INS_PARAM.vd1_head);
      if ((!this->MY_VENUS_INS_PARAM.vmask_read) ||
          (this->MY_VENUS_INS_PARAM.vmask_read && this->VENUS_MASK_DSPM[i])) {
        switch (this->MY_VENUS_INS_PARAM.function3) {
        case IVV: {
          if (vd1_mask == 0xff00)
            temp_res =
                ((int16_t)(this->VENUS_DSPM[vs2_row][vs2_bank] & (vs2_mask)) >>
                 8) >
                ((int16_t)(this->VENUS_DSPM[vs1_row][vs1_bank] & (vs1_mask)) >>
                 8);
          else if (vd1_mask == 0x00ff)
            temp_res =
                ((int16_t)((this->VENUS_DSPM[vs2_row][vs2_bank] & (vs2_mask))
                           << 8) >>
                 8) >
                ((int16_t)((this->VENUS_DSPM[vs1_row][vs1_bank] & (vs1_mask))
                           << 8) >>
                 8);
          else
            temp_res =
                (int16_t)(this->VENUS_DSPM[vs2_row][vs2_bank] & (vs2_mask)) >
                (int16_t)(this->VENUS_DSPM[vs1_row][vs1_bank] & (vs1_mask));
          if (vd1_mask == 0xff00)
            this->VENUS_DSPM[vd1_row][vd1_bank] =
                (this->VENUS_DSPM[vd1_row][vd1_bank] & (~vd1_mask)) +
                ((temp_res << 8) & vd1_mask);
          else
            this->VENUS_DSPM[vd1_row][vd1_bank] =
                (this->VENUS_DSPM[vd1_row][vd1_bank] & (~vd1_mask)) +
                (temp_res & vd1_mask);
          // temp_res = (int16_t)(this->VENUS_DSPM[vs2_row][vs2_bank] &
          // (vs2_mask)) >
          //            (int16_t)(this->VENUS_DSPM[vs1_row][vs1_bank] &
          //            (vs1_mask));
          // this->VENUS_DSPM[vd1_row][vd1_bank] =
          // (this->VENUS_DSPM[vd1_row][vd1_bank] & (~vd1_mask)) + (temp_res &
          // vd1_mask);
          break;
        }
        case IVX: {
          if (vd1_mask == 0xff00)
            temp_res =
                ((int16_t)(this->VENUS_DSPM[vs2_row][vs2_bank] & (vs2_mask)) >>
                 8) > this->MY_VENUS_INS_PARAM.scalar_op;
          else if (vd1_mask == 0x00ff)
            temp_res =
                ((int16_t)((this->VENUS_DSPM[vs2_row][vs2_bank] & (vs2_mask))
                           << 8) >>
                 8) > this->MY_VENUS_INS_PARAM.scalar_op;
          else
            temp_res =
                (int16_t)(this->VENUS_DSPM[vs2_row][vs2_bank] & (vs2_mask)) >
                this->MY_VENUS_INS_PARAM.scalar_op;
          if (vd1_mask == 0xff00)
            this->VENUS_DSPM[vd1_row][vd1_bank] =
                (this->VENUS_DSPM[vd1_row][vd1_bank] & (~vd1_mask)) +
                ((temp_res << 8) & vd1_mask);
          else
            this->VENUS_DSPM[vd1_row][vd1_bank] =
                (this->VENUS_DSPM[vd1_row][vd1_bank] & (~vd1_mask)) +
                (temp_res & vd1_mask);
          break;
        }
        case MVV: {
          if (vd1_mask == 0xff00)
            temp_res =
                ((int16_t)(this->VENUS_DSPM[vs2_row][vs2_bank] & (vs2_mask)) >>
                 8) >
                ((int16_t)(this->VENUS_DSPM[vs1_row][vs1_bank] & (vs1_mask)) >>
                 8);
          else if (vd1_mask == 0x00ff)
            temp_res =
                ((int16_t)((this->VENUS_DSPM[vs2_row][vs2_bank] & (vs2_mask))
                           << 8) >>
                 8) >
                ((int16_t)((this->VENUS_DSPM[vs1_row][vs1_bank] & (vs1_mask))
                           << 8) >>
                 8);
          else
            temp_res =
                (int16_t)(this->VENUS_DSPM[vs2_row][vs2_bank] & (vs2_mask)) >
                (int16_t)(this->VENUS_DSPM[vs1_row][vs1_bank] & (vs1_mask));

          // temp_res = (int16_t)(this->VENUS_DSPM[vs2_row][vs2_bank] &
          // (vs2_mask)) >
          //            (int16_t)(this->VENUS_DSPM[vs1_row][vs1_bank] &
          //            (vs1_mask));
          this->VENUS_MASK_DSPM[i] = (bool)temp_res;
          break;
        }
        case MVX: {
          if (vd1_mask == 0xff00)
            temp_res =
                ((int16_t)(this->VENUS_DSPM[vs2_row][vs2_bank] & (vs2_mask)) >>
                 8) > this->MY_VENUS_INS_PARAM.scalar_op;
          else if (vd1_mask == 0x00ff)
            temp_res =
                ((int16_t)((this->VENUS_DSPM[vs2_row][vs2_bank] & (vs2_mask))
                           << 8) >>
                 8) > this->MY_VENUS_INS_PARAM.scalar_op;
          else
            temp_res =
                (int16_t)(this->VENUS_DSPM[vs2_row][vs2_bank] & (vs2_mask)) >
                this->MY_VENUS_INS_PARAM.scalar_op;

          this->VENUS_MASK_DSPM[i] = (bool)temp_res;
          // if (vd1_mask == 0xff00)
          //   this->VENUS_MASK_DSPM[i] = (this->VENUS_MASK_DSPM[i] &
          //   (~vd1_mask)) + (((bool)temp_res) << 8);
          // else
          //   this->VENUS_MASK_DSPM[i] = (this->VENUS_MASK_DSPM[i] &
          //   (~vd1_mask)) + (bool)temp_res;
          break;
        }
        default: {
          // TODO: other cases
          std::cout << "TODO: VSLT other cases" << endl;
          exit(EXIT_FAILURE);
          break;
        }
        }
      } else {
        this->VENUS_DSPM[vd1_row][vd1_bank] =
            (this->VENUS_DSPM[vs1_row][vs1_bank] & (vs1_mask)) +
            (this->VENUS_DSPM[vd1_row][vd1_bank] & (~vd1_mask));
      }
    }
    break;
  }
  case VSLEU: {
    this->instr_name = (char *)"VSLEU";
    for (int i = 0; i < this->MY_VENUS_INS_PARAM.avl; i++) {
      tie(vs1_row, vs1_bank, vs1_mask) =
          this->get_abs_addr_and_mask(i, this->MY_VENUS_INS_PARAM.vs1_head);
      tie(vs2_row, vs2_bank, vs2_mask) =
          this->get_abs_addr_and_mask(i, this->MY_VENUS_INS_PARAM.vs2_head);
      tie(vd1_row, vd1_bank, vd1_mask) =
          this->get_abs_addr_and_mask(i, this->MY_VENUS_INS_PARAM.vd1_head);
      if ((!this->MY_VENUS_INS_PARAM.vmask_read) ||
          (this->MY_VENUS_INS_PARAM.vmask_read && this->VENUS_MASK_DSPM[i])) {
        switch (this->MY_VENUS_INS_PARAM.function3) {
        case IVV: {
          temp_res =
              (uint16_t)(this->VENUS_DSPM[vs2_row][vs2_bank] & (vs2_mask)) >=
              (uint16_t)(this->VENUS_DSPM[vs1_row][vs1_bank] & (vs1_mask));
          if (vd1_mask == 0xff00)
            this->VENUS_DSPM[vd1_row][vd1_bank] =
                (this->VENUS_DSPM[vd1_row][vd1_bank] & (~vd1_mask)) +
                ((temp_res << 8) & vd1_mask);
          else
            this->VENUS_DSPM[vd1_row][vd1_bank] =
                (this->VENUS_DSPM[vd1_row][vd1_bank] & (~vd1_mask)) +
                (temp_res & vd1_mask);
          break;
        }
        case IVX: {
          if (vd1_mask == 0xff00)
            temp_res =
                ((uint16_t)(this->VENUS_DSPM[vs2_row][vs2_bank] & (vs2_mask)) >>
                 8) >= (uint16_t)this->MY_VENUS_INS_PARAM.scalar_op;
          else
            temp_res = (uint16_t)((this->VENUS_DSPM[vs2_row][vs2_bank] &
                                   (vs2_mask))) >=
                       (uint16_t)this->MY_VENUS_INS_PARAM.scalar_op;
          if (vd1_mask == 0xff00)
            this->VENUS_DSPM[vd1_row][vd1_bank] =
                (this->VENUS_DSPM[vd1_row][vd1_bank] & (~vd1_mask)) +
                ((temp_res << 8) & vd1_mask);
          else
            this->VENUS_DSPM[vd1_row][vd1_bank] =
                (this->VENUS_DSPM[vd1_row][vd1_bank] & (~vd1_mask)) +
                (temp_res & vd1_mask);
          break;
        }
        case MVV: {
          temp_res =
              (uint16_t)(this->VENUS_DSPM[vs2_row][vs2_bank] & (vs2_mask)) >=
              (uint16_t)(this->VENUS_DSPM[vs1_row][vs1_bank] & (vs1_mask));
          this->VENUS_MASK_DSPM[i] = (bool)temp_res;
          break;
        }
        case MVX: {
          if (vd1_mask == 0xff00)
            temp_res =
                ((uint16_t)(this->VENUS_DSPM[vs2_row][vs2_bank] & (vs2_mask)) >>
                 8) >= (uint16_t)this->MY_VENUS_INS_PARAM.scalar_op;
          else
            temp_res = (uint16_t)((this->VENUS_DSPM[vs2_row][vs2_bank] &
                                   (vs2_mask))) >=
                       (uint16_t)this->MY_VENUS_INS_PARAM.scalar_op;
          this->VENUS_MASK_DSPM[i] = (bool)temp_res;
          break;
        }
        default: {
          // TODO: other cases
          std::cout << "TODO: VSLEU other cases" << endl;
          exit(EXIT_FAILURE);
          break;
        }
        }
      } else {
        this->VENUS_DSPM[vd1_row][vd1_bank] =
            (this->VENUS_DSPM[vs1_row][vs1_bank] & (vs1_mask)) +
            (this->VENUS_DSPM[vd1_row][vd1_bank] & (~vd1_mask));
      }
    }
    break;
  }
  case VSLE: {
    this->instr_name = (char *)"VSLE";
    for (int i = 0; i < this->MY_VENUS_INS_PARAM.avl; i++) {
      tie(vs1_row, vs1_bank, vs1_mask) =
          this->get_abs_addr_and_mask(i, this->MY_VENUS_INS_PARAM.vs1_head);
      tie(vs2_row, vs2_bank, vs2_mask) =
          this->get_abs_addr_and_mask(i, this->MY_VENUS_INS_PARAM.vs2_head);
      tie(vd1_row, vd1_bank, vd1_mask) =
          this->get_abs_addr_and_mask(i, this->MY_VENUS_INS_PARAM.vd1_head);
      if ((!this->MY_VENUS_INS_PARAM.vmask_read) ||
          (this->MY_VENUS_INS_PARAM.vmask_read && this->VENUS_MASK_DSPM[i])) {
        switch (this->MY_VENUS_INS_PARAM.function3) {
        case IVV: {
          if (vd1_mask == 0xff00)
            temp_res =
                ((int16_t)(this->VENUS_DSPM[vs2_row][vs2_bank] & (vs2_mask)) >>
                 8) >=
                ((int16_t)(this->VENUS_DSPM[vs1_row][vs1_bank] & (vs1_mask)) >>
                 8);
          else if (vd1_mask == 0x00ff)
            temp_res =
                ((int16_t)((this->VENUS_DSPM[vs2_row][vs2_bank] & (vs2_mask))
                           << 8) >>
                 8) >=
                ((int16_t)((this->VENUS_DSPM[vs1_row][vs1_bank] & (vs1_mask))
                           << 8) >>
                 8);
          else
            temp_res =
                (int16_t)(this->VENUS_DSPM[vs2_row][vs2_bank] & (vs2_mask)) >=
                (int16_t)(this->VENUS_DSPM[vs1_row][vs1_bank] & (vs1_mask));
          if (vd1_mask == 0xff00)
            this->VENUS_DSPM[vd1_row][vd1_bank] =
                (this->VENUS_DSPM[vd1_row][vd1_bank] & (~vd1_mask)) +
                ((temp_res << 8) & vd1_mask);
          else
            this->VENUS_DSPM[vd1_row][vd1_bank] =
                (this->VENUS_DSPM[vd1_row][vd1_bank] & (~vd1_mask)) +
                (temp_res & vd1_mask);

          // temp_res = (int16_t)(this->VENUS_DSPM[vs2_row][vs2_bank] &
          // (vs2_mask)) >=
          //            (int16_t)(this->VENUS_DSPM[vs1_row][vs1_bank] &
          //            (vs1_mask));
          // this->VENUS_DSPM[vd1_row][vd1_bank] =
          // (this->VENUS_DSPM[vd1_row][vd1_bank] & (~vd1_mask)) + (temp_res &
          // vd1_mask);
          break;
        }
        case IVX: {
          if (vd1_mask == 0xff00)
            temp_res =
                ((int16_t)(this->VENUS_DSPM[vs2_row][vs2_bank] & (vs2_mask)) >>
                 8) >= this->MY_VENUS_INS_PARAM.scalar_op;
          else if (vd1_mask == 0x00ff)
            temp_res =
                ((int16_t)((this->VENUS_DSPM[vs2_row][vs2_bank] & (vs2_mask))
                           << 8) >>
                 8) >= this->MY_VENUS_INS_PARAM.scalar_op;
          else
            temp_res =
                (int16_t)(this->VENUS_DSPM[vs2_row][vs2_bank] & (vs2_mask)) >=
                this->MY_VENUS_INS_PARAM.scalar_op;
          if (vd1_mask == 0xff00)
            this->VENUS_DSPM[vd1_row][vd1_bank] =
                (this->VENUS_DSPM[vd1_row][vd1_bank] & (~vd1_mask)) +
                ((temp_res << 8) & vd1_mask);
          else
            this->VENUS_DSPM[vd1_row][vd1_bank] =
                (this->VENUS_DSPM[vd1_row][vd1_bank] & (~vd1_mask)) +
                (temp_res & vd1_mask);
          break;
        }
        case MVV: {
          if (vd1_mask == 0xff00)
            temp_res =
                ((int16_t)(this->VENUS_DSPM[vs2_row][vs2_bank] & (vs2_mask)) >>
                 8) >=
                ((int16_t)(this->VENUS_DSPM[vs1_row][vs1_bank] & (vs1_mask)) >>
                 8);
          else if (vd1_mask == 0x00ff)
            temp_res =
                ((int16_t)((this->VENUS_DSPM[vs2_row][vs2_bank] & (vs2_mask))
                           << 8) >>
                 8) >=
                ((int16_t)((this->VENUS_DSPM[vs1_row][vs1_bank] & (vs1_mask))
                           << 8) >>
                 8);
          else
            temp_res =
                (int16_t)(this->VENUS_DSPM[vs2_row][vs2_bank] & (vs2_mask)) >=
                (int16_t)(this->VENUS_DSPM[vs1_row][vs1_bank] & (vs1_mask));
          // temp_res = (int16_t)(this->VENUS_DSPM[vs2_row][vs2_bank] &
          // (vs2_mask)) >=
          //            (int16_t)(this->VENUS_DSPM[vs1_row][vs1_bank] &
          //            (vs1_mask));
          this->VENUS_MASK_DSPM[i] = (bool)temp_res;
          break;
        }
        case MVX: {
          if (vd1_mask == 0xff00)
            temp_res =
                ((int16_t)(this->VENUS_DSPM[vs2_row][vs2_bank] & (vs2_mask)) >>
                 8) >= this->MY_VENUS_INS_PARAM.scalar_op;
          else if (vd1_mask == 0x00ff)
            temp_res =
                ((int16_t)((this->VENUS_DSPM[vs2_row][vs2_bank] & (vs2_mask))
                           << 8) >>
                 8) >= this->MY_VENUS_INS_PARAM.scalar_op;
          else
            temp_res =
                (int16_t)(this->VENUS_DSPM[vs2_row][vs2_bank] & (vs2_mask)) >=
                this->MY_VENUS_INS_PARAM.scalar_op;

          this->VENUS_MASK_DSPM[i] = (bool)temp_res;
          break;
        }
        default: {
          // TODO: other cases
          std::cout << "TODO: VSLE other cases" << endl;
          exit(EXIT_FAILURE);
          break;
        }
        }
      } else {
        this->VENUS_DSPM[vd1_row][vd1_bank] =
            (this->VENUS_DSPM[vs1_row][vs1_bank] & (vs1_mask)) +
            (this->VENUS_DSPM[vd1_row][vd1_bank] & (~vd1_mask));
      }
    }
    break;
  }
  case VSGTU: {
    this->instr_name = (char *)"VSGTU";
    for (int i = 0; i < this->MY_VENUS_INS_PARAM.avl; i++) {
      tie(vs1_row, vs1_bank, vs1_mask) =
          this->get_abs_addr_and_mask(i, this->MY_VENUS_INS_PARAM.vs1_head);
      tie(vs2_row, vs2_bank, vs2_mask) =
          this->get_abs_addr_and_mask(i, this->MY_VENUS_INS_PARAM.vs2_head);
      tie(vd1_row, vd1_bank, vd1_mask) =
          this->get_abs_addr_and_mask(i, this->MY_VENUS_INS_PARAM.vd1_head);
      if ((!this->MY_VENUS_INS_PARAM.vmask_read) ||
          (this->MY_VENUS_INS_PARAM.vmask_read && this->VENUS_MASK_DSPM[i])) {
        switch (this->MY_VENUS_INS_PARAM.function3) {
        case IVV: {
          temp_res =
              (uint16_t)(this->VENUS_DSPM[vs2_row][vs2_bank] & (vs2_mask)) <
              (uint16_t)(this->VENUS_DSPM[vs1_row][vs1_bank] & (vs1_mask));
          if (vd1_mask == 0xff00)
            this->VENUS_DSPM[vd1_row][vd1_bank] =
                (this->VENUS_DSPM[vd1_row][vd1_bank] & (~vd1_mask)) +
                ((temp_res << 8) & vd1_mask);
          else
            this->VENUS_DSPM[vd1_row][vd1_bank] =
                (this->VENUS_DSPM[vd1_row][vd1_bank] & (~vd1_mask)) +
                (temp_res & vd1_mask);
          break;
        }
        case IVX: {
          if (vd1_mask == 0xff00)
            temp_res =
                ((uint16_t)(this->VENUS_DSPM[vs2_row][vs2_bank] & (vs2_mask)) >>
                 8) < (uint16_t)this->MY_VENUS_INS_PARAM.scalar_op;
          else
            temp_res =
                (uint16_t)((this->VENUS_DSPM[vs2_row][vs2_bank] & (vs2_mask))) <
                (uint16_t)this->MY_VENUS_INS_PARAM.scalar_op;
          if (vd1_mask == 0xff00)
            this->VENUS_DSPM[vd1_row][vd1_bank] =
                (this->VENUS_DSPM[vd1_row][vd1_bank] & (~vd1_mask)) +
                ((temp_res << 8) & vd1_mask);
          else
            this->VENUS_DSPM[vd1_row][vd1_bank] =
                (this->VENUS_DSPM[vd1_row][vd1_bank] & (~vd1_mask)) +
                (temp_res & vd1_mask);
          break;
        }
        case MVV: {
          temp_res =
              (uint16_t)(this->VENUS_DSPM[vs2_row][vs2_bank] & (vs2_mask)) <
              (uint16_t)(this->VENUS_DSPM[vs1_row][vs1_bank] & (vs1_mask));
          this->VENUS_MASK_DSPM[i] = (bool)temp_res;
          break;
        }
        case MVX: {
          if (vd1_mask == 0xff00)
            temp_res =
                ((uint16_t)(this->VENUS_DSPM[vs2_row][vs2_bank] & (vs2_mask)) >>
                 8) < (uint16_t)this->MY_VENUS_INS_PARAM.scalar_op;
          else
            temp_res =
                (uint16_t)((this->VENUS_DSPM[vs2_row][vs2_bank] & (vs2_mask))) <
                (uint16_t)this->MY_VENUS_INS_PARAM.scalar_op;
          this->VENUS_MASK_DSPM[i] = (bool)temp_res;
          break;
        }
        default: {
          // TODO: other cases
          std::cout << "TODO: VSGTU other cases" << endl;
          exit(EXIT_FAILURE);
          break;
        }
        }
      } else {
        this->VENUS_DSPM[vd1_row][vd1_bank] =
            (this->VENUS_DSPM[vs1_row][vs1_bank] & (vs1_mask)) +
            (this->VENUS_DSPM[vd1_row][vd1_bank] & (~vd1_mask));
      }
    }
    break;
  }
  case VSGT: {
    this->instr_name = (char *)"VSGT";
    for (int i = 0; i < this->MY_VENUS_INS_PARAM.avl; i++) {
      tie(vs1_row, vs1_bank, vs1_mask) =
          this->get_abs_addr_and_mask(i, this->MY_VENUS_INS_PARAM.vs1_head);
      tie(vs2_row, vs2_bank, vs2_mask) =
          this->get_abs_addr_and_mask(i, this->MY_VENUS_INS_PARAM.vs2_head);
      tie(vd1_row, vd1_bank, vd1_mask) =
          this->get_abs_addr_and_mask(i, this->MY_VENUS_INS_PARAM.vd1_head);
      if ((!this->MY_VENUS_INS_PARAM.vmask_read) ||
          (this->MY_VENUS_INS_PARAM.vmask_read && this->VENUS_MASK_DSPM[i])) {
        switch (this->MY_VENUS_INS_PARAM.function3) {
        case IVV: {
          if (vd1_mask == 0xff00)
            temp_res =
                ((int16_t)(this->VENUS_DSPM[vs2_row][vs2_bank] & (vs2_mask)) >>
                 8) <
                ((int16_t)(this->VENUS_DSPM[vs1_row][vs1_bank] & (vs1_mask)) >>
                 8);
          else if (vd1_mask == 0x00ff)
            temp_res =
                ((int16_t)((this->VENUS_DSPM[vs2_row][vs2_bank] & (vs2_mask))
                           << 8) >>
                 8) <
                ((int16_t)((this->VENUS_DSPM[vs1_row][vs1_bank] & (vs1_mask))
                           << 8) >>
                 8);
          else
            temp_res =
                (int16_t)(this->VENUS_DSPM[vs2_row][vs2_bank] & (vs2_mask)) <
                (int16_t)(this->VENUS_DSPM[vs1_row][vs1_bank] & (vs1_mask));
          if (vd1_mask == 0xff00)
            this->VENUS_DSPM[vd1_row][vd1_bank] =
                (this->VENUS_DSPM[vd1_row][vd1_bank] & (~vd1_mask)) +
                ((temp_res << 8) & vd1_mask);
          else
            this->VENUS_DSPM[vd1_row][vd1_bank] =
                (this->VENUS_DSPM[vd1_row][vd1_bank] & (~vd1_mask)) +
                (temp_res & vd1_mask);
          // temp_res = (int16_t)(this->VENUS_DSPM[vs2_row][vs2_bank] &
          // (vs2_mask)) <
          //            (int16_t)(this->VENUS_DSPM[vs1_row][vs1_bank] &
          //            (vs1_mask));
          // this->VENUS_DSPM[vd1_row][vd1_bank] =
          // (this->VENUS_DSPM[vd1_row][vd1_bank] & (~vd1_mask)) + (temp_res &
          // vd1_mask);
          break;
        }
        case IVX: {
          if (vd1_mask == 0xff00)
            temp_res =
                ((int16_t)(this->VENUS_DSPM[vs2_row][vs2_bank] & (vs2_mask)) >>
                 8) < this->MY_VENUS_INS_PARAM.scalar_op;
          else if (vd1_mask == 0x00ff)
            temp_res =
                ((int16_t)((this->VENUS_DSPM[vs2_row][vs2_bank] & (vs2_mask))
                           << 8) >>
                 8) < this->MY_VENUS_INS_PARAM.scalar_op;
          else
            temp_res =
                (int16_t)(this->VENUS_DSPM[vs2_row][vs2_bank] & (vs2_mask)) <
                this->MY_VENUS_INS_PARAM.scalar_op;
          if (vd1_mask == 0xff00)
            this->VENUS_DSPM[vd1_row][vd1_bank] =
                (this->VENUS_DSPM[vd1_row][vd1_bank] & (~vd1_mask)) +
                ((temp_res << 8) & vd1_mask);
          else
            this->VENUS_DSPM[vd1_row][vd1_bank] =
                (this->VENUS_DSPM[vd1_row][vd1_bank] & (~vd1_mask)) +
                (temp_res & vd1_mask);
          break;
        }
        case MVV: {
          if (vd1_mask == 0xff00)
            temp_res =
                ((int16_t)(this->VENUS_DSPM[vs2_row][vs2_bank] & (vs2_mask)) >>
                 8) <
                ((int16_t)(this->VENUS_DSPM[vs1_row][vs1_bank] & (vs1_mask)) >>
                 8);
          else if (vd1_mask == 0x00ff)
            temp_res =
                ((int16_t)((this->VENUS_DSPM[vs2_row][vs2_bank] & (vs2_mask))
                           << 8) >>
                 8) <
                ((int16_t)((this->VENUS_DSPM[vs1_row][vs1_bank] & (vs1_mask))
                           << 8) >>
                 8);
          else
            temp_res =
                (int16_t)(this->VENUS_DSPM[vs2_row][vs2_bank] & (vs2_mask)) <
                (int16_t)(this->VENUS_DSPM[vs1_row][vs1_bank] & (vs1_mask));
          // temp_res = (int16_t)(this->VENUS_DSPM[vs2_row][vs2_bank] &
          // (vs2_mask)) <
          //            (int16_t)(this->VENUS_DSPM[vs1_row][vs1_bank] &
          //            (vs1_mask));
          this->VENUS_MASK_DSPM[i] = (bool)temp_res;
          break;
        }
        case MVX: {
          if (vd1_mask == 0xff00)
            temp_res =
                ((int16_t)(this->VENUS_DSPM[vs2_row][vs2_bank] & (vs2_mask)) >>
                 8) < this->MY_VENUS_INS_PARAM.scalar_op;
          else if (vd1_mask == 0x00ff)
            temp_res =
                ((int16_t)((this->VENUS_DSPM[vs2_row][vs2_bank] & (vs2_mask))
                           << 8) >>
                 8) < this->MY_VENUS_INS_PARAM.scalar_op;
          else
            temp_res =
                (int16_t)(this->VENUS_DSPM[vs2_row][vs2_bank] & (vs2_mask)) <
                this->MY_VENUS_INS_PARAM.scalar_op;
          // temp_res = (int16_t)(this->VENUS_DSPM[vs2_row][vs2_bank] &
          // (vs2_mask)) < this->MY_VENUS_INS_PARAM.scalar_op;
          this->VENUS_MASK_DSPM[i] = (bool)temp_res;
          break;
        }
        default: {
          // TODO: other cases
          std::cout << "TODO: VSGT other cases" << endl;
          exit(EXIT_FAILURE);
          break;
        }
        }
      } else {
        this->VENUS_DSPM[vd1_row][vd1_bank] =
            (this->VENUS_DSPM[vs1_row][vs1_bank] & (vs1_mask)) +
            (this->VENUS_DSPM[vd1_row][vd1_bank] & (~vd1_mask));
      }
    }
    break;
  }
  case VMNOT: {
    this->instr_name = (char *)"VMNOT";
    for (int i = 0; i < this->MY_VENUS_INS_PARAM.avl; i++)
      this->VENUS_MASK_DSPM[i] = !(this->VENUS_MASK_DSPM[i]);
    break;
  }
  ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
  // CAU
  ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
  case VADD: {
    this->instr_name = (char *)"VADD";
    for (int i = 0; i < this->MY_VENUS_INS_PARAM.avl; i++) {
      tie(vs1_row, vs1_bank, vs1_mask) =
          this->get_abs_addr_and_mask(i, this->MY_VENUS_INS_PARAM.vs1_head);
      tie(vs2_row, vs2_bank, vs2_mask) =
          this->get_abs_addr_and_mask(i, this->MY_VENUS_INS_PARAM.vs2_head);
      tie(vd1_row, vd1_bank, vd1_mask) =
          this->get_abs_addr_and_mask(i, this->MY_VENUS_INS_PARAM.vd1_head);
      // if((!this->MY_VENUS_INS_PARAM.vmask_read) ||
      // (this->MY_VENUS_INS_PARAM.vmask_read && this->VENUS_MASK_DSPM[i])) {
      //   if(this->MY_VENUS_INS_PARAM.function3 == IVX)
      //     temp_res = this->MY_VENUS_INS_PARAM.scalar_op +
      //               (this->VENUS_DSPM[vs2_row][vs2_bank] & (vs2_mask));
      //   else
      //     temp_res = (this->VENUS_DSPM[vs1_row][vs1_bank] & (vs1_mask)) +
      //                (this->VENUS_DSPM[vs2_row][vs2_bank] & (vs2_mask));
      //   this->VENUS_DSPM[vd1_row][vd1_bank] =
      //   (this->VENUS_DSPM[vd1_row][vd1_bank] & (~vd1_mask)) + (temp_res &
      //   vd1_mask);
      // }
      if ((!this->MY_VENUS_INS_PARAM.vmask_read) ||
          (this->MY_VENUS_INS_PARAM.vmask_read && this->VENUS_MASK_DSPM[i])) {
        if (this->MY_VENUS_INS_PARAM.function3 == IVX) {
          if (vd1_mask == 0xff00)
            temp_res =
                this->MY_VENUS_INS_PARAM.scalar_op +
                ((this->VENUS_DSPM[vs2_row][vs2_bank] & (vs2_mask)) >> 8);
          else
            temp_res = this->MY_VENUS_INS_PARAM.scalar_op +
                       (this->VENUS_DSPM[vs2_row][vs2_bank] & (vs2_mask));
        } else { // IVV
          if (vd1_mask == 0xff00)
            temp_res =
                ((this->VENUS_DSPM[vs1_row][vs1_bank] & (vs1_mask)) >> 8) +
                ((this->VENUS_DSPM[vs2_row][vs2_bank] & (vs2_mask)) >> 8);
          else
            temp_res = (this->VENUS_DSPM[vs1_row][vs1_bank] & (vs1_mask)) +
                       (this->VENUS_DSPM[vs2_row][vs2_bank] & (vs2_mask));
        }
        if (vd1_mask == 0xff00)
          this->VENUS_DSPM[vd1_row][vd1_bank] =
              (this->VENUS_DSPM[vd1_row][vd1_bank] & (~vd1_mask)) +
              ((temp_res << 8) & vd1_mask);
        else if (vd1_mask == 0xff)
          this->VENUS_DSPM[vd1_row][vd1_bank] =
              (this->VENUS_DSPM[vd1_row][vd1_bank] & (~vd1_mask)) +
              (temp_res & vd1_mask);
        else
          this->VENUS_DSPM[vd1_row][vd1_bank] =
              (this->VENUS_DSPM[vd1_row][vd1_bank] & (~vd1_mask)) +
              (temp_res & vd1_mask);
      } else {
        this->VENUS_DSPM[vd1_row][vd1_bank] =
            (this->VENUS_DSPM[vs1_row][vs1_bank] & (vs1_mask)) +
            (this->VENUS_DSPM[vd1_row][vd1_bank] & (~vd1_mask));
      }
    }
    break;
  }
  case VSADD: {
    this->instr_name = (char *)"VSADD";
    int add_res;
    for (int i = 0; i < this->MY_VENUS_INS_PARAM.avl; i++) {
      tie(vs1_row, vs1_bank, vs1_mask) =
          this->get_abs_addr_and_mask(i, this->MY_VENUS_INS_PARAM.vs1_head);
      tie(vs2_row, vs2_bank, vs2_mask) =
          this->get_abs_addr_and_mask(i, this->MY_VENUS_INS_PARAM.vs2_head);
      tie(vd1_row, vd1_bank, vd1_mask) =
          this->get_abs_addr_and_mask(i, this->MY_VENUS_INS_PARAM.vd1_head);
      if ((!this->MY_VENUS_INS_PARAM.vmask_read) ||
          (this->MY_VENUS_INS_PARAM.vmask_read && this->VENUS_MASK_DSPM[i])) {
        if (this->MY_VENUS_INS_PARAM.function3 == IVX) {
          if (vd1_mask == 0xff00)
            add_res =
                (signed int16_t)(this->MY_VENUS_INS_PARAM.scalar_op) +
                (signed int8_t)(
                    (this->VENUS_DSPM[vs2_row][vs2_bank] & (vs2_mask)) >> 8);
          else if (vd1_mask == 0x00ff)
            add_res =
                (signed int16_t)(this->MY_VENUS_INS_PARAM.scalar_op) +
                (signed int8_t)(
                    ((this->VENUS_DSPM[vs2_row][vs2_bank] & (vs2_mask)) << 8) >>
                    8);
          else
            add_res = (signed int16_t)(this->MY_VENUS_INS_PARAM.scalar_op) +
                      (signed int16_t)(this->VENUS_DSPM[vs2_row][vs2_bank] &
                                       (vs2_mask));
        } else // IVV
        {
          if (vd1_mask == 0xff00)
            add_res =
                (signed int16_t)(signed int8_t)(
                    (this->VENUS_DSPM[vs1_row][vs1_bank] & (vs1_mask)) >> 8) +
                (signed int8_t)(
                    (this->VENUS_DSPM[vs2_row][vs2_bank] & (vs2_mask)) >> 8);
          else if (vd1_mask == 0x00ff)
            add_res = (signed int16_t)(signed int8_t)(
                          (this->VENUS_DSPM[vs1_row][vs1_bank] & (vs1_mask))) +
                      (signed int8_t)(
                          (this->VENUS_DSPM[vs2_row][vs2_bank] & (vs2_mask)));
          else
            add_res = (signed int16_t)(this->VENUS_DSPM[vs1_row][vs1_bank] &
                                       (vs1_mask)) +
                      (signed int16_t)(this->VENUS_DSPM[vs2_row][vs2_bank] &
                                       (vs2_mask));
        }

        if ((signed int)add_res >
            (signed int)((1
                          << (((this->MY_VENUS_INS_PARAM.vew + 1) << 3) - 1)) -
                         1))
          temp_res = (1 << (((this->MY_VENUS_INS_PARAM.vew + 1) << 3) - 1)) - 1;
        else if ((signed int)add_res <
                 ((signed int)-1 *
                  (1 << (((this->MY_VENUS_INS_PARAM.vew + 1) << 3) - 1))))
          temp_res =
              -1 * (1 << (((this->MY_VENUS_INS_PARAM.vew + 1) << 3) - 1));
        else
          temp_res = add_res;

        if (vd1_mask == 0xff00)
          this->VENUS_DSPM[vd1_row][vd1_bank] =
              (this->VENUS_DSPM[vd1_row][vd1_bank] & (~vd1_mask)) +
              ((temp_res << 8) & vd1_mask);
        else if (vd1_mask == 0xff)
          this->VENUS_DSPM[vd1_row][vd1_bank] =
              (this->VENUS_DSPM[vd1_row][vd1_bank] & (~vd1_mask)) +
              (temp_res & vd1_mask);
        else
          this->VENUS_DSPM[vd1_row][vd1_bank] =
              (this->VENUS_DSPM[vd1_row][vd1_bank] & (~vd1_mask)) +
              (temp_res & vd1_mask);
      } else {
        this->VENUS_DSPM[vd1_row][vd1_bank] =
            (this->VENUS_DSPM[vs1_row][vs1_bank] & (vs1_mask)) +
            (this->VENUS_DSPM[vd1_row][vd1_bank] & (~vd1_mask));
      }
    }

    //   if ((!this->MY_VENUS_INS_PARAM.vmask_read) ||
    //   (this->MY_VENUS_INS_PARAM.vmask_read && this->VENUS_MASK_DSPM[i]))
    //   {
    //     if (this->MY_VENUS_INS_PARAM.function3 == IVX)
    //     {
    //       add_res = (signed int)(this->MY_VENUS_INS_PARAM.scalar_op) +
    //                 (signed int)(this->VENUS_DSPM[vs2_row][vs2_bank] &
    //                 (vs2_mask));
    //     }
    //     else
    //     {
    //       add_res = (signed int)(this->VENUS_DSPM[vs1_row][vs1_bank] &
    //       (vs1_mask)) +
    //                 (signed int)(this->VENUS_DSPM[vs2_row][vs2_bank] &
    //                 (vs2_mask));
    //     }
    //     if ((signed int)add_res > (signed int)((1 <<
    //     (((this->MY_VENUS_INS_PARAM.vew + 1) << 3) - 1)) - 1))
    //       temp_res = (1 << (((this->MY_VENUS_INS_PARAM.vew + 1) << 3) - 1)) -
    //       1;
    //     else if ((signed int)add_res < (signed int)(1 <<
    //     (((this->MY_VENUS_INS_PARAM.vew + 1) << 3) - 1)))
    //       temp_res = 1 << (((this->MY_VENUS_INS_PARAM.vew + 1) << 3) - 1);
    //     else
    //       temp_res = add_res;
    //     this->VENUS_DSPM[vd1_row][vd1_bank] =
    //     (this->VENUS_DSPM[vd1_row][vd1_bank] & (~vd1_mask)) + (temp_res &
    //     vd1_mask);
    //   }
    //   else
    //   {
    //     this->VENUS_DSPM[vd1_row][vd1_bank] =
    //     (this->VENUS_DSPM[vs2_row][vs2_bank] & (vs2_mask)) +
    //     (this->VENUS_DSPM[vd1_row][vd1_bank] & (~vd1_mask));
    //   }
    // }
    break;
  }
  case VSADDU: {
    this->instr_name = (char *)"VSADDU";
    int add_res;
    for (int i = 0; i < this->MY_VENUS_INS_PARAM.avl; i++) {
      tie(vs1_row, vs1_bank, vs1_mask) =
          this->get_abs_addr_and_mask(i, this->MY_VENUS_INS_PARAM.vs1_head);
      tie(vs2_row, vs2_bank, vs2_mask) =
          this->get_abs_addr_and_mask(i, this->MY_VENUS_INS_PARAM.vs2_head);
      tie(vd1_row, vd1_bank, vd1_mask) =
          this->get_abs_addr_and_mask(i, this->MY_VENUS_INS_PARAM.vd1_head);
      if ((!this->MY_VENUS_INS_PARAM.vmask_read) ||
          (this->MY_VENUS_INS_PARAM.vmask_read && this->VENUS_MASK_DSPM[i])) {
        if (this->MY_VENUS_INS_PARAM.function3 == IVX) {
          add_res =
              (unsigned int)(this->MY_VENUS_INS_PARAM.scalar_op) +
              (unsigned int)(this->VENUS_DSPM[vs2_row][vs2_bank] & (vs2_mask));
        } else {
          add_res =
              (unsigned int)(this->VENUS_DSPM[vs1_row][vs1_bank] & (vs1_mask)) +
              (unsigned int)(this->VENUS_DSPM[vs2_row][vs2_bank] & (vs2_mask));
        }
        if ((unsigned int)add_res >
            (unsigned int)((1 << ((this->MY_VENUS_INS_PARAM.vew + 1) << 3)) -
                           1))
          temp_res = (1 << ((this->MY_VENUS_INS_PARAM.vew + 1) << 3)) - 1;
        else
          temp_res = add_res;
        this->VENUS_DSPM[vd1_row][vd1_bank] =
            (this->VENUS_DSPM[vd1_row][vd1_bank] & (~vd1_mask)) +
            (temp_res & vd1_mask);
      } else {
        this->VENUS_DSPM[vd1_row][vd1_bank] =
            (this->VENUS_DSPM[vs1_row][vs1_bank] & (vs1_mask)) +
            (this->VENUS_DSPM[vd1_row][vd1_bank] & (~vd1_mask));
      }
    }
    break;
  }
  case VRANGE: {
    this->instr_name = (char *)"VRANGE";
    for (int i = 0; i < this->MY_VENUS_INS_PARAM.avl; i++) {
      tie(vd1_row, vd1_bank, vd1_mask) =
          this->get_abs_addr_and_mask(i, this->MY_VENUS_INS_PARAM.vd1_head);
      this->VENUS_DSPM[vd1_row][vd1_bank] =
          (this->VENUS_DSPM[vd1_row][vd1_bank] & (~vd1_mask)) +
          ((uint16_t)(i)&vd1_mask);
    }
    break;
  }
  case VRSUB: {
    this->instr_name = (char *)"VRSUB";
    for (int i = 0; i < this->MY_VENUS_INS_PARAM.avl; i++) {
      tie(vs1_row, vs1_bank, vs1_mask) =
          this->get_abs_addr_and_mask(i, this->MY_VENUS_INS_PARAM.vs1_head);
      tie(vs2_row, vs2_bank, vs2_mask) =
          this->get_abs_addr_and_mask(i, this->MY_VENUS_INS_PARAM.vs2_head);
      tie(vd1_row, vd1_bank, vd1_mask) =
          this->get_abs_addr_and_mask(i, this->MY_VENUS_INS_PARAM.vd1_head);
      if ((!this->MY_VENUS_INS_PARAM.vmask_read) ||
          (this->MY_VENUS_INS_PARAM.vmask_read && this->VENUS_MASK_DSPM[i])) {
        if (this->MY_VENUS_INS_PARAM.function3 == IVX) {
          if (vd1_mask == 0xff00)
            temp_res =
                ((this->VENUS_DSPM[vs2_row][vs2_bank] & (vs2_mask)) >> 8) -
                this->MY_VENUS_INS_PARAM.scalar_op;
          else
            temp_res = ((this->VENUS_DSPM[vs2_row][vs2_bank] & (vs2_mask))) -
                       this->MY_VENUS_INS_PARAM.scalar_op;
        } else { // IVV
          if (vd1_mask == 0xff00)
            temp_res =
                ((this->VENUS_DSPM[vs2_row][vs2_bank] & (vs2_mask)) >> 8) -
                ((this->VENUS_DSPM[vs1_row][vs1_bank] & (vs1_mask)) >> 8);
          else
            temp_res = ((this->VENUS_DSPM[vs2_row][vs2_bank] & (vs2_mask))) -
                       ((this->VENUS_DSPM[vs1_row][vs1_bank] & (vs1_mask)));
        }
        if (vd1_mask == 0xff00)
          this->VENUS_DSPM[vd1_row][vd1_bank] =
              (this->VENUS_DSPM[vd1_row][vd1_bank] & (~vd1_mask)) +
              ((temp_res << 8) & vd1_mask);
        else if (vd1_mask == 0xff)
          this->VENUS_DSPM[vd1_row][vd1_bank] =
              (this->VENUS_DSPM[vd1_row][vd1_bank] & (~vd1_mask)) +
              (temp_res & vd1_mask);
        else
          this->VENUS_DSPM[vd1_row][vd1_bank] =
              (this->VENUS_DSPM[vd1_row][vd1_bank] & (~vd1_mask)) +
              (temp_res & vd1_mask);

        // if (this->MY_VENUS_INS_PARAM.function3 == IVX)
        //   temp_res = (this->VENUS_DSPM[vs2_row][vs2_bank] & (vs2_mask)) -
        //   this->MY_VENUS_INS_PARAM.scalar_op;
        // else // IVV
        //   temp_res = (this->VENUS_DSPM[vs2_row][vs2_bank] & (vs2_mask)) -
        //              (this->VENUS_DSPM[vs1_row][vs1_bank] & (vs1_mask));
        // this->VENUS_DSPM[vd1_row][vd1_bank] =
        // (this->VENUS_DSPM[vd1_row][vd1_bank] & (~vd1_mask)) + (temp_res &
        // vd1_mask);
      } else {
        this->VENUS_DSPM[vd1_row][vd1_bank] =
            (this->VENUS_DSPM[vs1_row][vs1_bank] & (vs1_mask)) +
            (this->VENUS_DSPM[vd1_row][vd1_bank] & (~vd1_mask));
      }
    }
    break;
  }
  case VSUB: {
    this->instr_name = (char *)"VSUB";
    for (int i = 0; i < this->MY_VENUS_INS_PARAM.avl; i++) {
      tie(vs1_row, vs1_bank, vs1_mask) =
          this->get_abs_addr_and_mask(i, this->MY_VENUS_INS_PARAM.vs1_head);
      tie(vs2_row, vs2_bank, vs2_mask) =
          this->get_abs_addr_and_mask(i, this->MY_VENUS_INS_PARAM.vs2_head);
      tie(vd1_row, vd1_bank, vd1_mask) =
          this->get_abs_addr_and_mask(i, this->MY_VENUS_INS_PARAM.vd1_head);
      if ((!this->MY_VENUS_INS_PARAM.vmask_read) ||
          (this->MY_VENUS_INS_PARAM.vmask_read && this->VENUS_MASK_DSPM[i])) {
        if (this->MY_VENUS_INS_PARAM.function3 == IVX) {
          if (vd1_mask == 0xff00)
            temp_res =
                this->MY_VENUS_INS_PARAM.scalar_op -
                ((this->VENUS_DSPM[vs2_row][vs2_bank] & (vs2_mask)) >> 8);
          else
            temp_res = this->MY_VENUS_INS_PARAM.scalar_op -
                       ((this->VENUS_DSPM[vs2_row][vs2_bank] & (vs2_mask)));
        } else { // IVV
          if (vd1_mask == 0xff00)
            temp_res =
                ((this->VENUS_DSPM[vs1_row][vs1_bank] & (vs1_mask)) >> 8) -
                ((this->VENUS_DSPM[vs2_row][vs2_bank] & (vs2_mask)) >> 8);
          else
            temp_res = ((this->VENUS_DSPM[vs1_row][vs1_bank] & (vs1_mask))) -
                       ((this->VENUS_DSPM[vs2_row][vs2_bank] & (vs2_mask)));
        }
        if (vd1_mask == 0xff00)
          this->VENUS_DSPM[vd1_row][vd1_bank] =
              (this->VENUS_DSPM[vd1_row][vd1_bank] & (~vd1_mask)) +
              ((temp_res << 8) & vd1_mask);
        else if (vd1_mask == 0xff)
          this->VENUS_DSPM[vd1_row][vd1_bank] =
              (this->VENUS_DSPM[vd1_row][vd1_bank] & (~vd1_mask)) +
              (temp_res & vd1_mask);
        else
          this->VENUS_DSPM[vd1_row][vd1_bank] =
              (this->VENUS_DSPM[vd1_row][vd1_bank] & (~vd1_mask)) +
              (temp_res & vd1_mask);
      } else {
        this->VENUS_DSPM[vd1_row][vd1_bank] =
            (this->VENUS_DSPM[vs1_row][vs1_bank] & (vs1_mask)) +
            (this->VENUS_DSPM[vd1_row][vd1_bank] & (~vd1_mask));
      }
    }
    break;
  }
  case VSSUB: {
    this->instr_name = (char *)"VSSUB";
    int sub_res;
    for (int i = 0; i < this->MY_VENUS_INS_PARAM.avl; i++) {
      tie(vs1_row, vs1_bank, vs1_mask) =
          this->get_abs_addr_and_mask(i, this->MY_VENUS_INS_PARAM.vs1_head);
      tie(vs2_row, vs2_bank, vs2_mask) =
          this->get_abs_addr_and_mask(i, this->MY_VENUS_INS_PARAM.vs2_head);
      tie(vd1_row, vd1_bank, vd1_mask) =
          this->get_abs_addr_and_mask(i, this->MY_VENUS_INS_PARAM.vd1_head);
      if ((!this->MY_VENUS_INS_PARAM.vmask_read) ||
          (this->MY_VENUS_INS_PARAM.vmask_read && this->VENUS_MASK_DSPM[i])) {
        if (this->MY_VENUS_INS_PARAM.function3 == IVX)
          // sub_res = (signed int)(this->VENUS_DSPM[vs2_row][vs2_bank] &
          // (vs2_mask)) - (signed int)this->MY_VENUS_INS_PARAM.scalar_op;
          if (vd1_mask == 0xff00)
            sub_res =
                (signed int16_t)(this->MY_VENUS_INS_PARAM.scalar_op) -
                (signed int8_t)(
                    (this->VENUS_DSPM[vs2_row][vs2_bank] & (vs2_mask)) >> 8);
          else if (vd1_mask == 0x00ff)
            sub_res =
                (signed int16_t)(this->MY_VENUS_INS_PARAM.scalar_op) -
                (signed int8_t)(
                    ((this->VENUS_DSPM[vs2_row][vs2_bank] & (vs2_mask)) << 8) >>
                    8);
          else
            sub_res = (signed int16_t)(this->MY_VENUS_INS_PARAM.scalar_op) -
                      (signed int16_t)(this->VENUS_DSPM[vs2_row][vs2_bank] &
                                       (vs2_mask));

        else // IVV
            if (vd1_mask == 0xff00)
          sub_res =
              (signed int8_t)(
                  (this->VENUS_DSPM[vs1_row][vs1_bank] & (vs1_mask)) >> 8) -
              (signed int8_t)(
                  (this->VENUS_DSPM[vs2_row][vs2_bank] & (vs2_mask)) >> 8);
        else if (vd1_mask == 0x00ff)
          sub_res =
              (signed int8_t)(
                  ((this->VENUS_DSPM[vs1_row][vs1_bank] & (vs1_mask)) << 8) >>
                  8) -
              (signed int8_t)(
                  ((this->VENUS_DSPM[vs2_row][vs2_bank] & (vs2_mask)) << 8) >>
                  8);
        else
          sub_res = (signed int16_t)(this->VENUS_DSPM[vs1_row][vs1_bank] &
                                     (vs1_mask)) -
                    (signed int16_t)(this->VENUS_DSPM[vs2_row][vs2_bank] &
                                     (vs2_mask));
        // sub_res = (signed int)(this->VENUS_DSPM[vs1_row][vs1_bank] &
        // (vs1_mask)) -
        //           (signed int)(this->VENUS_DSPM[vs2_row][vs2_bank] &
        //           (vs2_mask));
        // if ((signed int)sub_res > (signed int)((1 <<
        // ((this->MY_VENUS_INS_PARAM.vew + 1) << 3 - 1)) - 1))
        //   temp_res = (1 << ((this->MY_VENUS_INS_PARAM.vew + 1) << 3 - 1)) -
        //   1;
        // else if ((signed int)sub_res < (signed int)(1 <<
        // ((this->MY_VENUS_INS_PARAM.vew + 1) << 3 - 1)))
        //   temp_res = 1 << ((this->MY_VENUS_INS_PARAM.vew + 1) << 3 - 1);
        // else
        //   temp_res = sub_res;
        // this->VENUS_DSPM[vd1_row][vd1_bank] =
        // (this->VENUS_DSPM[vd1_row][vd1_bank] & (~vd1_mask)) + (temp_res &
        // vd1_mask);
        if ((signed int)sub_res >
            (signed int)((1
                          << (((this->MY_VENUS_INS_PARAM.vew + 1) << 3) - 1)) -
                         1))
          temp_res = (1 << (((this->MY_VENUS_INS_PARAM.vew + 1) << 3) - 1)) - 1;
        else if ((signed int)sub_res <
                 (signed int)-1 *
                     (1 << (((this->MY_VENUS_INS_PARAM.vew + 1) << 3) - 1)))
          temp_res = -1 << (((this->MY_VENUS_INS_PARAM.vew + 1) << 3) - 1);
        else
          temp_res = sub_res;

        if (vd1_mask == 0xff00)
          this->VENUS_DSPM[vd1_row][vd1_bank] =
              (this->VENUS_DSPM[vd1_row][vd1_bank] & (~vd1_mask)) +
              ((temp_res << 8) & vd1_mask);
        else if (vd1_mask == 0xff)
          this->VENUS_DSPM[vd1_row][vd1_bank] =
              (this->VENUS_DSPM[vd1_row][vd1_bank] & (~vd1_mask)) +
              (temp_res & vd1_mask);
        else
          this->VENUS_DSPM[vd1_row][vd1_bank] =
              (this->VENUS_DSPM[vd1_row][vd1_bank] & (~vd1_mask)) +
              (temp_res & vd1_mask);
      } else {
        this->VENUS_DSPM[vd1_row][vd1_bank] =
            (this->VENUS_DSPM[vs1_row][vs1_bank] & (vs1_mask)) +
            (this->VENUS_DSPM[vd1_row][vd1_bank] & (~vd1_mask));
      }
    }
    break;
  }
  case VSSUBU: {
    this->instr_name = (char *)"VSSUBU";
    int sub_res;
    for (int i = 0; i < this->MY_VENUS_INS_PARAM.avl; i++) {
      tie(vs1_row, vs1_bank, vs1_mask) =
          this->get_abs_addr_and_mask(i, this->MY_VENUS_INS_PARAM.vs1_head);
      tie(vs2_row, vs2_bank, vs2_mask) =
          this->get_abs_addr_and_mask(i, this->MY_VENUS_INS_PARAM.vs2_head);
      tie(vd1_row, vd1_bank, vd1_mask) =
          this->get_abs_addr_and_mask(i, this->MY_VENUS_INS_PARAM.vd1_head);
      if ((!this->MY_VENUS_INS_PARAM.vmask_read) ||
          (this->MY_VENUS_INS_PARAM.vmask_read && this->VENUS_MASK_DSPM[i])) {
        if (this->MY_VENUS_INS_PARAM.function3 == IVX)
          sub_res =
              (unsigned int)(this->VENUS_DSPM[vs2_row][vs2_bank] & (vs2_mask)) -
              (unsigned int)this->MY_VENUS_INS_PARAM.scalar_op;
        else // IVV
          sub_res =
              (unsigned int)(this->VENUS_DSPM[vs1_row][vs1_bank] & (vs1_mask)) -
              (unsigned int)(this->VENUS_DSPM[vs2_row][vs2_bank] & (vs2_mask));
        if ((signed int)sub_res < 0)
          temp_res = 0;
        else
          temp_res = sub_res;
        this->VENUS_DSPM[vd1_row][vd1_bank] =
            (this->VENUS_DSPM[vd1_row][vd1_bank] & (~vd1_mask)) +
            (temp_res & vd1_mask);
      } else {
        this->VENUS_DSPM[vd1_row][vd1_bank] =
            (this->VENUS_DSPM[vs1_row][vs1_bank] & (vs1_mask)) +
            (this->VENUS_DSPM[vd1_row][vd1_bank] & (~vd1_mask));
      }
    }
    break;
  }
  case VMUL: {
    this->instr_name = (char *)"VMUL";
    for (int i = 0; i < this->MY_VENUS_INS_PARAM.avl; i++) {
      int mul_res;
      tie(vs1_row, vs1_bank, vs1_mask) =
          this->get_abs_addr_and_mask(i, this->MY_VENUS_INS_PARAM.vs1_head);
      tie(vs2_row, vs2_bank, vs2_mask) =
          this->get_abs_addr_and_mask(i, this->MY_VENUS_INS_PARAM.vs2_head);
      tie(vd1_row, vd1_bank, vd1_mask) =
          this->get_abs_addr_and_mask(i, this->MY_VENUS_INS_PARAM.vd1_head);
      // if (vd1_row == 24)
      // {
      //   printf("1");
      // }
      if ((!this->MY_VENUS_INS_PARAM.vmask_read) ||
          (this->MY_VENUS_INS_PARAM.vmask_read && this->VENUS_MASK_DSPM[i])) {
        if (this->MY_VENUS_INS_PARAM.function3 == IVX) {
          if (vd1_mask == 0xff00) {
            mul_res =
                (signed int8_t)(
                    (this->VENUS_DSPM[vs2_row][vs2_bank] & (vs2_mask)) >> 8) *
                (signed int8_t)(this->MY_VENUS_INS_PARAM.scalar_op);
            mul_res = mul_res >> this->MY_VENUS_INS_PARAM.vfu_shamt;
            mul_res = mul_res > INT8_MAX ? INT8_MAX : mul_res;
            mul_res = mul_res < INT8_MIN ? INT8_MIN : mul_res;
            temp_res = mul_res &
                       ((1 << ((this->MY_VENUS_INS_PARAM.vew + 1) << 3)) - 1);
          } else if (vd1_mask == 0xff) {
            mul_res = (signed int8_t)(this->VENUS_DSPM[vs2_row][vs2_bank] &
                                      (vs2_mask)) *
                      (signed int8_t)(this->MY_VENUS_INS_PARAM.scalar_op);
            mul_res = mul_res >> this->MY_VENUS_INS_PARAM.vfu_shamt;
            mul_res = mul_res > INT8_MAX ? INT8_MAX : mul_res;
            mul_res = mul_res < INT8_MIN ? INT8_MIN : mul_res;
            temp_res = mul_res &
                       ((1 << ((this->MY_VENUS_INS_PARAM.vew + 1) << 3)) - 1);
          } else {
            mul_res = (signed int16_t)(this->VENUS_DSPM[vs2_row][vs2_bank] &
                                       (vs2_mask)) *
                      (signed int16_t)(this->MY_VENUS_INS_PARAM.scalar_op);
            mul_res = mul_res >> this->MY_VENUS_INS_PARAM.vfu_shamt;
            mul_res = mul_res > INT16_MAX ? INT16_MAX : mul_res;
            mul_res = mul_res < INT16_MIN ? INT16_MIN : mul_res;
            temp_res = mul_res &
                       ((1 << ((this->MY_VENUS_INS_PARAM.vew + 1) << 3)) - 1);
          }
        } else { // IVV
          if (vd1_mask == 0xff00) {
            mul_res =
                (signed int8_t)(
                    (this->VENUS_DSPM[vs2_row][vs2_bank] & (vs2_mask)) >> 8) *
                (signed int8_t)(
                    (this->VENUS_DSPM[vs1_row][vs1_bank] & (vs1_mask)) >> 8);
            mul_res = mul_res >> this->MY_VENUS_INS_PARAM.vfu_shamt;
            mul_res = mul_res > INT8_MAX ? INT8_MAX : mul_res;
            mul_res = mul_res < INT8_MIN ? INT8_MIN : mul_res;
            temp_res = mul_res &
                       ((1 << ((this->MY_VENUS_INS_PARAM.vew + 1) << 3)) - 1);
          } else if (vd1_mask == 0xff) {
            mul_res = (signed int8_t)(
                          (this->VENUS_DSPM[vs2_row][vs2_bank] & (vs2_mask))) *
                      (signed int8_t)(
                          (this->VENUS_DSPM[vs1_row][vs1_bank] & (vs1_mask)));
            mul_res = mul_res >> this->MY_VENUS_INS_PARAM.vfu_shamt;
            mul_res = mul_res > INT8_MAX ? INT8_MAX : mul_res;
            mul_res = mul_res < INT8_MIN ? INT8_MIN : mul_res;
            temp_res = mul_res &
                       ((1 << ((this->MY_VENUS_INS_PARAM.vew + 1) << 3)) - 1);
          } else {
            mul_res = (signed int16_t)(
                          (this->VENUS_DSPM[vs2_row][vs2_bank] & (vs2_mask))) *
                      (signed int16_t)(
                          (this->VENUS_DSPM[vs1_row][vs1_bank] & (vs1_mask)));
            mul_res = mul_res >> this->MY_VENUS_INS_PARAM.vfu_shamt;
            mul_res = mul_res > INT16_MAX ? INT16_MAX : mul_res;
            mul_res = mul_res < INT16_MIN ? INT16_MIN : mul_res;
            temp_res = mul_res &
                       ((1 << ((this->MY_VENUS_INS_PARAM.vew + 1) << 3)) - 1);
          }
        }
        if (vd1_mask == 0xff00)
          this->VENUS_DSPM[vd1_row][vd1_bank] =
              (this->VENUS_DSPM[vd1_row][vd1_bank] & (~vd1_mask)) +
              ((temp_res << 8) & vd1_mask);
        else
          this->VENUS_DSPM[vd1_row][vd1_bank] =
              (this->VENUS_DSPM[vd1_row][vd1_bank] & (~vd1_mask)) +
              (temp_res & vd1_mask);
      } else {
        this->VENUS_DSPM[vd1_row][vd1_bank] =
            (this->VENUS_DSPM[vs1_row][vs1_bank] & (vs1_mask));
      }
    }
    break;
  }
  case VMULH: {
    int test111;
    this->instr_name = (char *)"VMULH";
    for (int i = 0; i < this->MY_VENUS_INS_PARAM.avl; i++) {
      int mul_res;
      tie(vs1_row, vs1_bank, vs1_mask) =
          this->get_abs_addr_and_mask(i, this->MY_VENUS_INS_PARAM.vs1_head);
      tie(vs2_row, vs2_bank, vs2_mask) =
          this->get_abs_addr_and_mask(i, this->MY_VENUS_INS_PARAM.vs2_head);
      tie(vd1_row, vd1_bank, vd1_mask) =
          this->get_abs_addr_and_mask(i, this->MY_VENUS_INS_PARAM.vd1_head);
      if ((!this->MY_VENUS_INS_PARAM.vmask_read) ||
          (this->MY_VENUS_INS_PARAM.vmask_read && this->VENUS_MASK_DSPM[i])) {
        if (this->MY_VENUS_INS_PARAM.function3 == IVX) {
          if (vd1_mask == 0xff00) {
            mul_res =
                (signed int8_t)(
                    (this->VENUS_DSPM[vs2_row][vs2_bank] & (vs2_mask)) >> 8) *
                (signed int8_t)(this->MY_VENUS_INS_PARAM.scalar_op);
            temp_res = (mul_res << this->MY_VENUS_INS_PARAM.vfu_shamt) >>
                       ((this->MY_VENUS_INS_PARAM.vew + 1) << 3);
          } else if (vd1_mask == 0xff) {
            mul_res = (signed int8_t)(
                          (this->VENUS_DSPM[vs2_row][vs2_bank] & (vs2_mask))) *
                      (signed int8_t)(this->MY_VENUS_INS_PARAM.scalar_op);
            temp_res = (mul_res << this->MY_VENUS_INS_PARAM.vfu_shamt) >>
                       ((this->MY_VENUS_INS_PARAM.vew + 1) << 3);
          } else {
            mul_res = (signed int16_t)(
                          (this->VENUS_DSPM[vs2_row][vs2_bank] & (vs2_mask))) *
                      (signed int16_t)(this->MY_VENUS_INS_PARAM.scalar_op);
            temp_res = (mul_res << this->MY_VENUS_INS_PARAM.vfu_shamt) >>
                       ((this->MY_VENUS_INS_PARAM.vew + 1) << 3);
          }
        } else { // IVV
          if (vd1_mask == 0xff00) {
            mul_res =
                (signed int8_t)(
                    (this->VENUS_DSPM[vs2_row][vs2_bank] & (vs2_mask)) >> 8) *
                (signed int8_t)(
                    (this->VENUS_DSPM[vs1_row][vs1_bank] & (vs1_mask)) >> 8);
            temp_res = ((mul_res << this->MY_VENUS_INS_PARAM.vfu_shamt)) >>
                       ((this->MY_VENUS_INS_PARAM.vew + 1) << 3);
          } else if (vd1_mask == 0xff) {
            mul_res = (signed int8_t)(this->VENUS_DSPM[vs2_row][vs2_bank] &
                                      (vs2_mask)) *
                      (signed int8_t)(this->VENUS_DSPM[vs1_row][vs1_bank] &
                                      (vs1_mask));
            temp_res = ((mul_res << this->MY_VENUS_INS_PARAM.vfu_shamt)) >>
                       ((this->MY_VENUS_INS_PARAM.vew + 1) << 3);
          } else {
            mul_res = (signed int16_t)(this->VENUS_DSPM[vs2_row][vs2_bank] &
                                       (vs2_mask)) *
                      (signed int16_t)(this->VENUS_DSPM[vs1_row][vs1_bank] &
                                       (vs1_mask));
            temp_res = ((mul_res << this->MY_VENUS_INS_PARAM.vfu_shamt)) >>
                       ((this->MY_VENUS_INS_PARAM.vew + 1) << 3);
          }
        }
        // if(i >= 40){
        //     test111 = 1;
        // }
        if (vd1_mask == 0xff00)
          this->VENUS_DSPM[vd1_row][vd1_bank] =
              (this->VENUS_DSPM[vd1_row][vd1_bank] & (~vd1_mask)) +
              ((temp_res << 8) & vd1_mask);
        else
          this->VENUS_DSPM[vd1_row][vd1_bank] =
              (this->VENUS_DSPM[vd1_row][vd1_bank] & (~vd1_mask)) +
              (temp_res & vd1_mask);
      } else {
        this->VENUS_DSPM[vd1_row][vd1_bank] =
            (this->VENUS_DSPM[vs1_row][vs1_bank] & (vs1_mask)) +
            (this->VENUS_DSPM[vd1_row][vd1_bank] & (~vd1_mask));
      }
    }
    break;
  }
  case VMULHU: {
    this->instr_name = (char *)"VMULHU";
    int mul_res;
    for (int i = 0; i < this->MY_VENUS_INS_PARAM.avl; i++) {
      tie(vs1_row, vs1_bank, vs1_mask) =
          this->get_abs_addr_and_mask(i, this->MY_VENUS_INS_PARAM.vs1_head);
      tie(vs2_row, vs2_bank, vs2_mask) =
          this->get_abs_addr_and_mask(i, this->MY_VENUS_INS_PARAM.vs2_head);
      tie(vd1_row, vd1_bank, vd1_mask) =
          this->get_abs_addr_and_mask(i, this->MY_VENUS_INS_PARAM.vd1_head);
      if ((!this->MY_VENUS_INS_PARAM.vmask_read) ||
          (this->MY_VENUS_INS_PARAM.vmask_read && this->VENUS_MASK_DSPM[i])) {
        if (this->MY_VENUS_INS_PARAM.function3 == IVX) {
          mul_res =
              (unsigned int)(this->VENUS_DSPM[vs2_row][vs2_bank] & (vs2_mask)) *
              (unsigned int)(this->MY_VENUS_INS_PARAM.scalar_op);
          temp_res = (mul_res << this->MY_VENUS_INS_PARAM.vfu_shamt) >>
                     ((this->MY_VENUS_INS_PARAM.vew + 1) << 3);
          // printf("%04x\t%x\n", (uint16_t)(this->VENUS_DSPM[vs2_row][vs2_bank]
          // & (vs2_mask)) *
          //            (uint16_t)this->MY_VENUS_INS_PARAM.scalar_op,
          //            this->MY_VENUS_INS_PARAM.scalar_op);
        } else { // IVV
          mul_res =
              (unsigned int)(this->VENUS_DSPM[vs2_row][vs2_bank] & (vs2_mask)) *
              (unsigned int)(this->VENUS_DSPM[vs1_row][vs1_bank] & (vs1_mask));
          temp_res = (mul_res << this->MY_VENUS_INS_PARAM.vfu_shamt) >>
                     ((this->MY_VENUS_INS_PARAM.vew + 1) << 3);
        }
        this->VENUS_DSPM[vd1_row][vd1_bank] =
            (this->VENUS_DSPM[vd1_row][vd1_bank] & (~vd1_mask)) +
            (temp_res & vd1_mask);
      } else {
        this->VENUS_DSPM[vd1_row][vd1_bank] =
            (this->VENUS_DSPM[vs1_row][vs1_bank] & (vs1_mask)) +
            (this->VENUS_DSPM[vd1_row][vd1_bank] & (~vd1_mask));
      }
    }
    break;
  }
  case VMULHSU: {
    this->instr_name = (char *)"VMULHSU";
    int mul_res;
    for (int i = 0; i < this->MY_VENUS_INS_PARAM.avl; i++) {
      tie(vs1_row, vs1_bank, vs1_mask) =
          this->get_abs_addr_and_mask(i, this->MY_VENUS_INS_PARAM.vs1_head);
      tie(vs2_row, vs2_bank, vs2_mask) =
          this->get_abs_addr_and_mask(i, this->MY_VENUS_INS_PARAM.vs2_head);
      tie(vd1_row, vd1_bank, vd1_mask) =
          this->get_abs_addr_and_mask(i, this->MY_VENUS_INS_PARAM.vd1_head);
      if ((!this->MY_VENUS_INS_PARAM.vmask_read) ||
          (this->MY_VENUS_INS_PARAM.vmask_read && this->VENUS_MASK_DSPM[i])) {
        if (this->MY_VENUS_INS_PARAM.function3 == IVX) {
          mul_res =
              (unsigned int)(this->VENUS_DSPM[vs2_row][vs2_bank] & (vs2_mask)) *
              (signed int16_t)(this->MY_VENUS_INS_PARAM.scalar_op);
          temp_res = (mul_res << this->MY_VENUS_INS_PARAM.vfu_shamt) >>
                     ((this->MY_VENUS_INS_PARAM.vew + 1) << 3);
        } else { // IVV
          mul_res =
              (unsigned int)(this->VENUS_DSPM[vs2_row][vs2_bank] & (vs2_mask)) *
              (signed int16_t)(this->VENUS_DSPM[vs1_row][vs1_bank] &
                               (vs1_mask));
          temp_res = (mul_res << this->MY_VENUS_INS_PARAM.vfu_shamt) >>
                     ((this->MY_VENUS_INS_PARAM.vew + 1) << 3);
        }
        this->VENUS_DSPM[vd1_row][vd1_bank] =
            (this->VENUS_DSPM[vd1_row][vd1_bank] & (~vd1_mask)) +
            (temp_res & vd1_mask);
      } else {
        this->VENUS_DSPM[vd1_row][vd1_bank] =
            (this->VENUS_DSPM[vs1_row][vs1_bank] & (vs1_mask)) +
            (this->VENUS_DSPM[vd1_row][vd1_bank] & (~vd1_mask));
      }
    }
    break;
  }
  case VMULADD: {
    this->instr_name = (char *)"VMULADD";
    int mul_res;
    for (int i = 0; i < this->MY_VENUS_INS_PARAM.avl; i++) {
      tie(vs1_row, vs1_bank, vs1_mask) =
          this->get_abs_addr_and_mask(i, this->MY_VENUS_INS_PARAM.vs1_head);
      tie(vs2_row, vs2_bank, vs2_mask) =
          this->get_abs_addr_and_mask(i, this->MY_VENUS_INS_PARAM.vs2_head);
      tie(vd1_row, vd1_bank, vd1_mask) =
          this->get_abs_addr_and_mask(i, this->MY_VENUS_INS_PARAM.vd1_head);
      tie(vd2_row, vd2_bank, vd2_mask) =
          this->get_abs_addr_and_mask(i, this->MY_VENUS_INS_PARAM.vd2_head);
      if ((!this->MY_VENUS_INS_PARAM.vmask_read) ||
          (this->MY_VENUS_INS_PARAM.vmask_read && this->VENUS_MASK_DSPM[i])) {
        if (this->MY_VENUS_INS_PARAM.function3 == IVX) {

          if (vd1_mask == 0xff00) {
            mul_res =
                (signed int8_t)(
                    (this->VENUS_DSPM[vs2_row][vs2_bank] & (vs2_mask)) >> 8) *
                (signed int8_t)(this->MY_VENUS_INS_PARAM.scalar_op);
            temp_res = (mul_res >> this->MY_VENUS_INS_PARAM.vfu_shamt) &
                       ((1 << ((this->MY_VENUS_INS_PARAM.vew + 1) << 3)) - 1);
            temp_res =
                temp_res +
                ((this->VENUS_DSPM[vd2_row][vd2_bank] & (vd2_mask)) >> 8);
          } else if (vd1_mask == 0xff) {
            mul_res = (signed int8_t)(this->VENUS_DSPM[vs2_row][vs2_bank] &
                                      (vs2_mask)) *
                      (signed int8_t)(this->MY_VENUS_INS_PARAM.scalar_op);
            temp_res = (mul_res >> this->MY_VENUS_INS_PARAM.vfu_shamt) &
                       ((1 << ((this->MY_VENUS_INS_PARAM.vew + 1) << 3)) - 1);
            temp_res =
                temp_res + (this->VENUS_DSPM[vd2_row][vd2_bank] & (vd2_mask));
          } else {
            mul_res = (signed int16_t)(this->VENUS_DSPM[vs2_row][vs2_bank] &
                                       (vs2_mask)) *
                      (signed int16_t)(this->MY_VENUS_INS_PARAM.scalar_op);
            temp_res = (mul_res >> this->MY_VENUS_INS_PARAM.vfu_shamt) &
                       ((1 << ((this->MY_VENUS_INS_PARAM.vew + 1) << 3)) - 1);
            temp_res =
                temp_res + (this->VENUS_DSPM[vd2_row][vd2_bank] & (vd2_mask));
          }
          // mul_res = (signed int16_t)(this->VENUS_DSPM[vs2_row][vs2_bank] &
          // (vs2_mask)) *
          //           (signed int16_t)(this->MY_VENUS_INS_PARAM.scalar_op);
          // temp_res = (mul_res >> this->MY_VENUS_INS_PARAM.vfu_shamt) & ((1 <<
          // ((this->MY_VENUS_INS_PARAM.vew + 1) << 3)) - 1); temp_res =
          // temp_res + this->VENUS_DSPM[vd2_row][vd2_bank] & (vd2_mask);
        } else { // IVV
          if (vd1_mask == 0xff00) {
            mul_res =
                (signed int8_t)(
                    (this->VENUS_DSPM[vs2_row][vs2_bank] & (vs2_mask)) >> 8) *
                (signed int8_t)(
                    (this->VENUS_DSPM[vs1_row][vs1_bank] & (vs1_mask)) >> 8);
            temp_res = (mul_res >> this->MY_VENUS_INS_PARAM.vfu_shamt) &
                       ((1 << ((this->MY_VENUS_INS_PARAM.vew + 1) << 3)) - 1);
            temp_res =
                temp_res +
                ((this->VENUS_DSPM[vd2_row][vd2_bank] & (vd2_mask)) >> 8);
          } else if (vd1_mask == 0xff) {
            mul_res = (signed int8_t)(
                          (this->VENUS_DSPM[vs2_row][vs2_bank] & (vs2_mask))) *
                      (signed int8_t)(
                          (this->VENUS_DSPM[vs1_row][vs1_bank] & (vs1_mask)));
            temp_res = (mul_res >> this->MY_VENUS_INS_PARAM.vfu_shamt) &
                       ((1 << ((this->MY_VENUS_INS_PARAM.vew + 1) << 3)) - 1);
            temp_res =
                temp_res + (this->VENUS_DSPM[vd2_row][vd2_bank] & (vd2_mask));
          } else {
            mul_res = (signed int16_t)(
                          (this->VENUS_DSPM[vs2_row][vs2_bank] & (vs2_mask))) *
                      (signed int16_t)(
                          (this->VENUS_DSPM[vs1_row][vs1_bank] & (vs1_mask)));
            temp_res = (mul_res >> this->MY_VENUS_INS_PARAM.vfu_shamt) &
                       ((1 << ((this->MY_VENUS_INS_PARAM.vew + 1) << 3)) - 1);
            temp_res =
                temp_res + (this->VENUS_DSPM[vd2_row][vd2_bank] & (vd2_mask));
          }
          // mul_res = (signed int16_t)(this->VENUS_DSPM[vs2_row][vs2_bank] &
          // (vs2_mask)) *
          //           (signed int16_t)(this->VENUS_DSPM[vs1_row][vs1_bank] &
          //           (vs1_mask));
          // temp_res = (mul_res >> this->MY_VENUS_INS_PARAM.vfu_shamt) & ((1 <<
          // ((this->MY_VENUS_INS_PARAM.vew + 1) << 3)) - 1); temp_res =
          // temp_res + this->VENUS_DSPM[vd2_row][vd2_bank] & (vd2_mask);
        }
        if (vd1_mask == 0xff00)
          this->VENUS_DSPM[vd1_row][vd1_bank] =
              (this->VENUS_DSPM[vd1_row][vd1_bank] & (~vd1_mask)) +
              ((temp_res << 8) & vd1_mask);
        else
          this->VENUS_DSPM[vd1_row][vd1_bank] =
              (this->VENUS_DSPM[vd1_row][vd1_bank] & (~vd1_mask)) +
              (temp_res & vd1_mask);
        // this->VENUS_DSPM[vd1_row][vd1_bank] =
        // (this->VENUS_DSPM[vd1_row][vd1_bank] & (~vd1_mask)) + (temp_res &
        // vd1_mask);
      } else {
        this->VENUS_DSPM[vd1_row][vd1_bank] =
            (this->VENUS_DSPM[vs1_row][vs1_bank] & (vs1_mask)) +
            (this->VENUS_DSPM[vd1_row][vd1_bank] & (~vd1_mask));
      }
    }
    break;
  }
  case VMULSUB: {
    this->instr_name = (char *)"VMULSUB";
    int mul_res;
    for (int i = 0; i < this->MY_VENUS_INS_PARAM.avl; i++) {
      tie(vs1_row, vs1_bank, vs1_mask) =
          this->get_abs_addr_and_mask(i, this->MY_VENUS_INS_PARAM.vs1_head);
      tie(vs2_row, vs2_bank, vs2_mask) =
          this->get_abs_addr_and_mask(i, this->MY_VENUS_INS_PARAM.vs2_head);
      tie(vd1_row, vd1_bank, vd1_mask) =
          this->get_abs_addr_and_mask(i, this->MY_VENUS_INS_PARAM.vd1_head);
      tie(vd2_row, vd2_bank, vd2_mask) =
          this->get_abs_addr_and_mask(i, this->MY_VENUS_INS_PARAM.vd2_head);
      if ((!this->MY_VENUS_INS_PARAM.vmask_read) ||
          (this->MY_VENUS_INS_PARAM.vmask_read && this->VENUS_MASK_DSPM[i])) {
        if (this->MY_VENUS_INS_PARAM.function3 == IVX) {
          if (vd1_mask == 0xff00) {
            mul_res =
                (signed int8_t)(
                    (this->VENUS_DSPM[vs2_row][vs2_bank] & (vs2_mask)) >> 8) *
                (signed int8_t)(this->MY_VENUS_INS_PARAM.scalar_op);
            temp_res = (mul_res >> this->MY_VENUS_INS_PARAM.vfu_shamt) &
                       ((1 << ((this->MY_VENUS_INS_PARAM.vew + 1) << 3)) - 1);
            temp_res =
                temp_res -
                ((this->VENUS_DSPM[vd2_row][vd2_bank] & (vd2_mask)) >> 8);
          } else if (vd1_mask == 0xff) {
            mul_res = (signed int8_t)(this->VENUS_DSPM[vs2_row][vs2_bank] &
                                      (vs2_mask)) *
                      (signed int8_t)(this->MY_VENUS_INS_PARAM.scalar_op);
            temp_res = (mul_res >> this->MY_VENUS_INS_PARAM.vfu_shamt) &
                       ((1 << ((this->MY_VENUS_INS_PARAM.vew + 1) << 3)) - 1);
            temp_res =
                temp_res - (this->VENUS_DSPM[vd2_row][vd2_bank] & (vd2_mask));
          } else {
            mul_res = (signed int16_t)(this->VENUS_DSPM[vs2_row][vs2_bank] &
                                       (vs2_mask)) *
                      (signed int16_t)(this->MY_VENUS_INS_PARAM.scalar_op);
            temp_res = (mul_res >> this->MY_VENUS_INS_PARAM.vfu_shamt) &
                       ((1 << ((this->MY_VENUS_INS_PARAM.vew + 1) << 3)) - 1);
            temp_res =
                temp_res - (this->VENUS_DSPM[vd2_row][vd2_bank] & (vd2_mask));
          }
        } else { // IVV
          if (vd1_mask == 0xff00) {
            mul_res =
                (signed int8_t)(
                    (this->VENUS_DSPM[vs2_row][vs2_bank] & (vs2_mask)) >> 8) *
                (signed int8_t)(
                    (this->VENUS_DSPM[vs1_row][vs1_bank] & (vs1_mask)) >> 8);
            temp_res = (mul_res >> this->MY_VENUS_INS_PARAM.vfu_shamt) &
                       ((1 << ((this->MY_VENUS_INS_PARAM.vew + 1) << 3)) - 1);
            temp_res =
                temp_res -
                ((this->VENUS_DSPM[vd2_row][vd2_bank] & (vd2_mask)) >> 8);
          } else if (vd1_mask == 0xff) {
            mul_res = (signed int8_t)(
                          (this->VENUS_DSPM[vs2_row][vs2_bank] & (vs2_mask))) *
                      (signed int8_t)(
                          (this->VENUS_DSPM[vs1_row][vs1_bank] & (vs1_mask)));
            temp_res = (mul_res >> this->MY_VENUS_INS_PARAM.vfu_shamt) &
                       ((1 << ((this->MY_VENUS_INS_PARAM.vew + 1) << 3)) - 1);
            temp_res =
                temp_res - (this->VENUS_DSPM[vd2_row][vd2_bank] & (vd2_mask));
          } else {
            mul_res = (signed int16_t)(
                          (this->VENUS_DSPM[vs2_row][vs2_bank] & (vs2_mask))) *
                      (signed int16_t)(
                          (this->VENUS_DSPM[vs1_row][vs1_bank] & (vs1_mask)));
            temp_res = (mul_res >> this->MY_VENUS_INS_PARAM.vfu_shamt) &
                       ((1 << ((this->MY_VENUS_INS_PARAM.vew + 1) << 3)) - 1);
            temp_res =
                temp_res - (this->VENUS_DSPM[vd2_row][vd2_bank] & (vd2_mask));
          }
        }
        if (vd1_mask == 0xff00)
          this->VENUS_DSPM[vd1_row][vd1_bank] =
              (this->VENUS_DSPM[vd1_row][vd1_bank] & (~vd1_mask)) +
              ((temp_res << 8) & vd1_mask);
        else
          this->VENUS_DSPM[vd1_row][vd1_bank] =
              (this->VENUS_DSPM[vd1_row][vd1_bank] & (~vd1_mask)) +
              (temp_res & vd1_mask);
      } else {
        this->VENUS_DSPM[vd1_row][vd1_bank] =
            (this->VENUS_DSPM[vs1_row][vs1_bank] & (vs1_mask)) +
            (this->VENUS_DSPM[vd1_row][vd1_bank] & (~vd1_mask));
      }
    }
    break;
  }
  case VADDMUL: {
    this->instr_name = (char *)"VADDMUL";
    int mul_res;
    for (int i = 0; i < this->MY_VENUS_INS_PARAM.avl; i++) {
      tie(vs1_row, vs1_bank, vs1_mask) =
          this->get_abs_addr_and_mask(i, this->MY_VENUS_INS_PARAM.vs1_head);
      tie(vs2_row, vs2_bank, vs2_mask) =
          this->get_abs_addr_and_mask(i, this->MY_VENUS_INS_PARAM.vs2_head);
      tie(vd1_row, vd1_bank, vd1_mask) =
          this->get_abs_addr_and_mask(i, this->MY_VENUS_INS_PARAM.vd1_head);
      tie(vd2_row, vd2_bank, vd2_mask) =
          this->get_abs_addr_and_mask(i, this->MY_VENUS_INS_PARAM.vd2_head);
      if ((!this->MY_VENUS_INS_PARAM.vmask_read) ||
          (this->MY_VENUS_INS_PARAM.vmask_read && this->VENUS_MASK_DSPM[i])) {
        if (this->MY_VENUS_INS_PARAM.function3 == IVX) {
          if (vd1_mask == 0xff00) {
            temp_res =
                this->MY_VENUS_INS_PARAM.scalar_op +
                ((this->VENUS_DSPM[vs2_row][vs2_bank] & (vs2_mask)) >> 8);
            mul_res =
                (signed int8_t)(
                    (this->VENUS_DSPM[vd2_row][vd2_bank] & (vd2_mask)) >> 8) *
                (signed int8_t)temp_res;
            temp_res = (mul_res >> this->MY_VENUS_INS_PARAM.vfu_shamt) &
                       ((1 << ((this->MY_VENUS_INS_PARAM.vew + 1) << 3)) - 1);
          } else if (vd1_mask == 0xff) {
            temp_res = this->MY_VENUS_INS_PARAM.scalar_op +
                       (this->VENUS_DSPM[vs2_row][vs2_bank] & (vs2_mask));
            mul_res = (signed int8_t)(this->VENUS_DSPM[vd2_row][vd2_bank] &
                                      (vd2_mask)) *
                      (signed int8_t)temp_res;
            temp_res = (mul_res >> this->MY_VENUS_INS_PARAM.vfu_shamt) &
                       ((1 << ((this->MY_VENUS_INS_PARAM.vew + 1) << 3)) - 1);
          } else {
            temp_res = this->MY_VENUS_INS_PARAM.scalar_op +
                       (this->VENUS_DSPM[vs2_row][vs2_bank] & (vs2_mask));
            mul_res = (signed int16_t)(this->VENUS_DSPM[vd2_row][vd2_bank] &
                                       (vd2_mask)) *
                      (signed int16_t)temp_res;
            temp_res = (mul_res >> this->MY_VENUS_INS_PARAM.vfu_shamt) &
                       ((1 << ((this->MY_VENUS_INS_PARAM.vew + 1) << 3)) - 1);
          }
          // temp_res = (this->VENUS_DSPM[vs2_row][vs2_bank] & (vs2_mask)) +
          //            (this->MY_VENUS_INS_PARAM.scalar_op);
          // mul_res = (signed int16_t)(temp_res) * (signed
          // int16_t)(this->VENUS_DSPM[vd2_row][vd2_bank] & (vd2_mask));
          // temp_res = (mul_res >> this->MY_VENUS_INS_PARAM.vfu_shamt) & ((1 <<
          // ((this->MY_VENUS_INS_PARAM.vew + 1) << 3)) - 1);
        } else { // IVV
          if (vd1_mask == 0xff00) {
            temp_res =
                ((this->VENUS_DSPM[vs1_row][vs1_bank] & (vs1_mask)) >> 8) +
                ((this->VENUS_DSPM[vs2_row][vs2_bank] & (vs2_mask)) >> 8);
            mul_res =
                (signed int8_t)(
                    (this->VENUS_DSPM[vd2_row][vd2_bank] & (vd2_mask)) >> 8) *
                (signed int8_t)temp_res;
            temp_res = (mul_res >> this->MY_VENUS_INS_PARAM.vfu_shamt) &
                       ((1 << ((this->MY_VENUS_INS_PARAM.vew + 1) << 3)) - 1);
          } else if (vd1_mask == 0xff) {
            temp_res = (this->VENUS_DSPM[vs1_row][vs1_bank] & (vs1_mask)) +
                       (this->VENUS_DSPM[vs2_row][vs2_bank] & (vs2_mask));
            mul_res = (signed int8_t)(this->VENUS_DSPM[vd2_row][vd2_bank] &
                                      (vd2_mask)) *
                      (signed int8_t)temp_res;
            temp_res = (mul_res >> this->MY_VENUS_INS_PARAM.vfu_shamt) &
                       ((1 << ((this->MY_VENUS_INS_PARAM.vew + 1) << 3)) - 1);
          } else {
            temp_res = (this->VENUS_DSPM[vs1_row][vs1_bank] & (vs1_mask)) +
                       (this->VENUS_DSPM[vs2_row][vs2_bank] & (vs2_mask));
            mul_res = (signed int16_t)(this->VENUS_DSPM[vd2_row][vd2_bank] &
                                       (vd2_mask)) *
                      (signed int16_t)temp_res;
            temp_res = (mul_res >> this->MY_VENUS_INS_PARAM.vfu_shamt) &
                       ((1 << ((this->MY_VENUS_INS_PARAM.vew + 1) << 3)) - 1);
          }
          // temp_res = (this->VENUS_DSPM[vs2_row][vs2_bank] & (vs2_mask)) +
          //            (this->VENUS_DSPM[vs1_row][vs1_bank] & (vs1_mask));
          // mul_res = (signed int16_t)(temp_res) * (signed
          // int16_t)(this->VENUS_DSPM[vd2_row][vd2_bank] & (vd2_mask));
          // temp_res = (mul_res >> this->MY_VENUS_INS_PARAM.vfu_shamt) & ((1 <<
          // ((this->MY_VENUS_INS_PARAM.vew + 1) << 3)) - 1);
        }
        if (vd1_mask == 0xff00)
          this->VENUS_DSPM[vd1_row][vd1_bank] =
              (this->VENUS_DSPM[vd1_row][vd1_bank] & (~vd1_mask)) +
              ((temp_res << 8) & vd1_mask);
        else
          this->VENUS_DSPM[vd1_row][vd1_bank] =
              (this->VENUS_DSPM[vd1_row][vd1_bank] & (~vd1_mask)) +
              (temp_res & vd1_mask);
        // this->VENUS_DSPM[vd1_row][vd1_bank] =
        // (this->VENUS_DSPM[vd1_row][vd1_bank] & (~vd1_mask)) + (temp_res &
        // vd1_mask);
      } else {
        this->VENUS_DSPM[vd1_row][vd1_bank] =
            (this->VENUS_DSPM[vs1_row][vs1_bank] & (vs1_mask)) +
            (this->VENUS_DSPM[vd1_row][vd1_bank] & (~vd1_mask));
      }
    }
    break;
  }
  case VSUBMUL: {
    this->instr_name = (char *)"VSUBMUL";
    int mul_res;
    for (int i = 0; i < this->MY_VENUS_INS_PARAM.avl; i++) {
      tie(vs1_row, vs1_bank, vs1_mask) =
          this->get_abs_addr_and_mask(i, this->MY_VENUS_INS_PARAM.vs1_head);
      tie(vs2_row, vs2_bank, vs2_mask) =
          this->get_abs_addr_and_mask(i, this->MY_VENUS_INS_PARAM.vs2_head);
      tie(vd1_row, vd1_bank, vd1_mask) =
          this->get_abs_addr_and_mask(i, this->MY_VENUS_INS_PARAM.vd1_head);
      tie(vd2_row, vd2_bank, vd2_mask) =
          this->get_abs_addr_and_mask(i, this->MY_VENUS_INS_PARAM.vd2_head);
      if ((!this->MY_VENUS_INS_PARAM.vmask_read) ||
          (this->MY_VENUS_INS_PARAM.vmask_read && this->VENUS_MASK_DSPM[i])) {
        if (this->MY_VENUS_INS_PARAM.function3 == IVX) {
          if (vd1_mask == 0xff00) {
            temp_res =
                this->MY_VENUS_INS_PARAM.scalar_op -
                ((this->VENUS_DSPM[vs2_row][vs2_bank] & (vs2_mask)) >> 8);
            mul_res =
                (signed int8_t)(
                    (this->VENUS_DSPM[vd2_row][vd2_bank] & (vd2_mask)) >> 8) *
                (signed int8_t)temp_res;
            temp_res = (mul_res >> this->MY_VENUS_INS_PARAM.vfu_shamt) &
                       ((1 << ((this->MY_VENUS_INS_PARAM.vew + 1) << 3)) - 1);
          } else if (vd1_mask == 0xff) {
            temp_res = this->MY_VENUS_INS_PARAM.scalar_op -
                       (this->VENUS_DSPM[vs2_row][vs2_bank] & (vs2_mask));
            mul_res = (signed int8_t)(this->VENUS_DSPM[vd2_row][vd2_bank] &
                                      (vd2_mask)) *
                      (signed int8_t)temp_res;
            temp_res = (mul_res >> this->MY_VENUS_INS_PARAM.vfu_shamt) &
                       ((1 << ((this->MY_VENUS_INS_PARAM.vew + 1) << 3)) - 1);
          } else {
            temp_res = this->MY_VENUS_INS_PARAM.scalar_op -
                       (this->VENUS_DSPM[vs2_row][vs2_bank] & (vs2_mask));
            mul_res = (signed int16_t)(this->VENUS_DSPM[vd2_row][vd2_bank] &
                                       (vd2_mask)) *
                      (signed int16_t)temp_res;
            temp_res = (mul_res >> this->MY_VENUS_INS_PARAM.vfu_shamt) &
                       ((1 << ((this->MY_VENUS_INS_PARAM.vew + 1) << 3)) - 1);
          }
          // temp_res = (this->MY_VENUS_INS_PARAM.scalar_op) -
          //            (this->VENUS_DSPM[vs2_row][vs2_bank] & (vs2_mask));
          // mul_res = (signed int16_t)(temp_res) * (signed
          // int16_t)(this->VENUS_DSPM[vd2_row][vd2_bank] & (vd2_mask));
          // temp_res = (mul_res >> this->MY_VENUS_INS_PARAM.vfu_shamt) & ((1 <<
          // ((this->MY_VENUS_INS_PARAM.vew + 1) << 3)) - 1);
        } else { // IVV
          if (vd1_mask == 0xff00) {
            temp_res =
                ((this->VENUS_DSPM[vs1_row][vs1_bank] & (vs1_mask)) >> 8) -
                ((this->VENUS_DSPM[vs2_row][vs2_bank] & (vs2_mask)) >> 8);
            mul_res =
                (signed int8_t)(
                    (this->VENUS_DSPM[vd2_row][vd2_bank] & (vd2_mask)) >> 8) *
                (signed int8_t)temp_res;
            temp_res = (mul_res >> this->MY_VENUS_INS_PARAM.vfu_shamt) &
                       ((1 << ((this->MY_VENUS_INS_PARAM.vew + 1) << 3)) - 1);
          } else if (vd1_mask == 0xff) {
            temp_res = (this->VENUS_DSPM[vs1_row][vs1_bank] & (vs1_mask)) -
                       (this->VENUS_DSPM[vs2_row][vs2_bank] & (vs2_mask));
            mul_res = (signed int8_t)(this->VENUS_DSPM[vd2_row][vd2_bank] &
                                      (vd2_mask)) *
                      (signed int8_t)temp_res;
            temp_res = (mul_res >> this->MY_VENUS_INS_PARAM.vfu_shamt) &
                       ((1 << ((this->MY_VENUS_INS_PARAM.vew + 1) << 3)) - 1);
          } else {
            temp_res = (this->VENUS_DSPM[vs1_row][vs1_bank] & (vs1_mask)) -
                       (this->VENUS_DSPM[vs2_row][vs2_bank] & (vs2_mask));
            mul_res = (signed int16_t)(this->VENUS_DSPM[vd2_row][vd2_bank] &
                                       (vd2_mask)) *
                      (signed int16_t)temp_res;
            temp_res = (mul_res >> this->MY_VENUS_INS_PARAM.vfu_shamt) &
                       ((1 << ((this->MY_VENUS_INS_PARAM.vew + 1) << 3)) - 1);
          }
          // temp_res = (this->VENUS_DSPM[vs1_row][vs1_bank] & (vs1_mask)) -
          //            (this->VENUS_DSPM[vs2_row][vs2_bank] & (vs2_mask));
          // mul_res = (signed int16_t)(temp_res) * (signed
          // int16_t)(this->VENUS_DSPM[vd2_row][vd2_bank] & (vd2_mask));
          // temp_res = (mul_res >> this->MY_VENUS_INS_PARAM.vfu_shamt) & ((1 <<
          // ((this->MY_VENUS_INS_PARAM.vew + 1) << 3)) - 1);
        }
        if (vd1_mask == 0xff00)
          this->VENUS_DSPM[vd1_row][vd1_bank] =
              (this->VENUS_DSPM[vd1_row][vd1_bank] & (~vd1_mask)) +
              ((temp_res << 8) & vd1_mask);
        else
          this->VENUS_DSPM[vd1_row][vd1_bank] =
              (this->VENUS_DSPM[vd1_row][vd1_bank] & (~vd1_mask)) +
              (temp_res & vd1_mask);
        // this->VENUS_DSPM[vd1_row][vd1_bank] =
        // (this->VENUS_DSPM[vd1_row][vd1_bank] & (~vd1_mask)) + (temp_res &
        // vd1_mask);
      } else {
        this->VENUS_DSPM[vd1_row][vd1_bank] =
            (this->VENUS_DSPM[vs1_row][vs1_bank] & (vs1_mask)) +
            (this->VENUS_DSPM[vd1_row][vd1_bank] & (~vd1_mask));
      }
    }
    break;
  }

    // case VCMXMUL:
    // {
    //   this->instr_name = (char *)"VCMXMUL";
    //   for (int i = 0; i < this->MY_VENUS_INS_PARAM.avl; i++)
    //   {
    //     tie(vs1_row, vs1_bank, vs1_mask) =
    //         this->get_abs_addr_and_mask(i, this->MY_VENUS_INS_PARAM.vs1_head
    //         + (this->MY_VENUS_INS_PARAM.high_vs1_bits << 8));
    //     tie(vs2_row, vs2_bank, vs2_mask) =
    //         this->get_abs_addr_and_mask(i, this->MY_VENUS_INS_PARAM.vs2_head
    //         + (this->MY_VENUS_INS_PARAM.high_vs2_bits << 8));
    //     tie(vd1_row, vd1_bank, vd1_mask) =
    //         this->get_abs_addr_and_mask(i, this->MY_VENUS_INS_PARAM.vd1_head
    //         + (this->MY_VENUS_INS_PARAM.high_vd1_bits << 8));
    //     tie(vd2_row, vd2_bank, vd2_mask) =
    //         this->get_abs_addr_and_mask(i, this->MY_VENUS_INS_PARAM.vd2_head
    //         + (this->MY_VENUS_INS_PARAM.high_vd2_bits << 8));

    //     if ((!this->MY_VENUS_INS_PARAM.vmask_read) ||
    //         (this->MY_VENUS_INS_PARAM.vmask_read &&
    //         this->VENUS_MASK_DSPM[i]))
    //     {
    //       int16_t vs1vd1 = ((this->VENUS_DSPM[vs1_row][vs1_bank] &
    //       (vs1_mask)) *
    //                         (this->VENUS_DSPM[vd1_row][vd1_bank] &
    //                         (vd1_mask))) &
    //                        ((1 << ((this->MY_VENUS_INS_PARAM.vew + 1) << 3))
    //                        - 1);
    //       int16_t vs2vd2 = ((this->VENUS_DSPM[vs2_row][vs2_bank] &
    //       (vs2_mask)) *
    //                         (this->VENUS_DSPM[vd2_row][vd2_bank] &
    //                         (vd2_mask))) &
    //                        ((1 << ((this->MY_VENUS_INS_PARAM.vew + 1) << 3))
    //                        - 1);
    //       int16_t vs1vd2 = ((this->VENUS_DSPM[vs1_row][vs1_bank] &
    //       (vs1_mask)) *
    //                         (this->VENUS_DSPM[vd2_row][vd2_bank] &
    //                         (vd2_mask))) &
    //                        ((1 << ((this->MY_VENUS_INS_PARAM.vew + 1) << 3))
    //                        - 1);
    //       int16_t vs2vd1 = ((this->VENUS_DSPM[vs2_row][vs2_bank] &
    //       (vs2_mask)) *
    //                         (this->VENUS_DSPM[vd1_row][vd1_bank] &
    //                         (vd1_mask))) &
    //                        ((1 << ((this->MY_VENUS_INS_PARAM.vew + 1) << 3))
    //                        - 1);
    //       int16_t sum_real = vs1vd2 - vs2vd1;
    //       int16_t sum_imag = vs1vd1 + vs2vd2;
    //       this->VENUS_DSPM[vd1_row][vd1_bank] =
    //           (this->VENUS_DSPM[vd1_row][vd1_bank] & (~vd1_mask)) +
    //           (sum_real & vd1_mask);
    //       this->VENUS_DSPM[vd2_row][vd2_bank] =
    //           (this->VENUS_DSPM[vd2_row][vd2_bank] & (~vd2_mask)) +
    //           (sum_imag & vd2_mask);
    //     }
    //     else
    //     {
    //       this->VENUS_DSPM[vd1_row][vd1_bank] =
    //           (this->VENUS_DSPM[vs1_row][vs1_bank] & (vs1_mask)) +
    //           (this->VENUS_DSPM[vd1_row][vd1_bank] & (~vd1_mask));
    //     }
    //   }
    //   break;
    // }

  case VCMXMUL: {
    this->instr_name = (char *)"VCMXMUL";
    int AsubB;
    int AaddB;
    int CsubD;
    int CmulAsubB;
    int DmulAaddB;
    int BmulCsubD;
    int temp_CmulAsubB;
    int temp_DmulAaddB;
    int temp_BmulCsubD;

    for (int i = 0; i < this->MY_VENUS_INS_PARAM.avl; i++) {
      tie(vs1_row, vs1_bank, vs1_mask) = this->get_abs_addr_and_mask(
          i, this->MY_VENUS_INS_PARAM.vs1_head +
                 (this->MY_VENUS_INS_PARAM.high_vs1_bits << 8));
      tie(vs2_row, vs2_bank, vs2_mask) = this->get_abs_addr_and_mask(
          i, this->MY_VENUS_INS_PARAM.vs2_head +
                 (this->MY_VENUS_INS_PARAM.high_vs2_bits << 8));
      tie(vd1_row, vd1_bank, vd1_mask) = this->get_abs_addr_and_mask(
          i, this->MY_VENUS_INS_PARAM.vd1_head +
                 (this->MY_VENUS_INS_PARAM.high_vd1_bits << 8));
      tie(vd2_row, vd2_bank, vd2_mask) = this->get_abs_addr_and_mask(
          i, this->MY_VENUS_INS_PARAM.vd2_head +
                 (this->MY_VENUS_INS_PARAM.high_vd2_bits << 8));

      if ((!this->MY_VENUS_INS_PARAM.vmask_read) ||
          (this->MY_VENUS_INS_PARAM.vmask_read && this->VENUS_MASK_DSPM[i])) {
        if (vd1_mask == 0xff00) {
          AsubB = (signed int8_t)(
                      (this->VENUS_DSPM[vs1_row][vs1_bank] & (vs1_mask)) >> 8) -
                  (signed int8_t)(
                      (this->VENUS_DSPM[vs2_row][vs2_bank] & (vs2_mask)) >> 8);
          AaddB = (signed int8_t)(
                      (this->VENUS_DSPM[vs1_row][vs1_bank] & (vs1_mask)) >> 8) +
                  (signed int8_t)(
                      (this->VENUS_DSPM[vs2_row][vs2_bank] & (vs2_mask)) >> 8);
          CsubD = (signed int8_t)(
                      (this->VENUS_DSPM[vd2_row][vd2_bank] & (vd2_mask)) >> 8) -
                  (signed int8_t)(
                      (this->VENUS_DSPM[vd1_row][vd1_bank] & (vd1_mask)) >> 8);

          // if ((signed int)AsubB >
          //     (signed int)((1
          //                   << (((this->MY_VENUS_INS_PARAM.vew + 1) << 3) -
          //                   1)) -
          //                  1))
          //   AsubB = (1 << (((this->MY_VENUS_INS_PARAM.vew + 1) << 3) - 1)) -
          //   1;
          // else if ((signed int)AsubB <
          //          (signed int)-1 *
          //              (1 << (((this->MY_VENUS_INS_PARAM.vew + 1) << 3) -
          //              1)))
          //   AsubB = 1 << (((this->MY_VENUS_INS_PARAM.vew + 1) << 3) - 1);
          // else
          //   AsubB = AsubB;

          // if ((signed int)AaddB >
          //     (signed int)((1
          //                   << (((this->MY_VENUS_INS_PARAM.vew + 1) << 3) -
          //                   1)) -
          //                  1))
          //   AaddB = (1 << (((this->MY_VENUS_INS_PARAM.vew + 1) << 3) - 1)) -
          //   1;
          // else if ((signed int)AaddB <
          //          (signed int)-1 *
          //              (1 << (((this->MY_VENUS_INS_PARAM.vew + 1) << 3) -
          //              1)))
          //   AaddB = 1 << (((this->MY_VENUS_INS_PARAM.vew + 1) << 3) - 1);
          // else
          //   AaddB = AaddB;

          // if ((signed int)CsubD >
          //     (signed int)((1
          //                   << (((this->MY_VENUS_INS_PARAM.vew + 1) << 3) -
          //                   1)) -
          //                  1))
          //   CsubD = (1 << (((this->MY_VENUS_INS_PARAM.vew + 1) << 3) - 1)) -
          //   1;
          // else if ((signed int)CsubD <
          //          (signed int)-1 *
          //              (1 << (((this->MY_VENUS_INS_PARAM.vew + 1) << 3) -
          //              1)))
          //   CsubD = 1 << (((this->MY_VENUS_INS_PARAM.vew + 1) << 3) - 1);
          // else
          //   CsubD = CsubD;

          CmulAsubB =
              (signed int8_t)(
                  (this->VENUS_DSPM[vd2_row][vd2_bank] & (vd2_mask)) >> 8) *
              (signed int16_t)AsubB;
          temp_CmulAsubB =
              (CmulAsubB >> this->MY_VENUS_INS_PARAM.vfu_shamt) &
              ((1 << ((this->MY_VENUS_INS_PARAM.vew + 1) << 3)) - 1);
          DmulAaddB =
              (signed int8_t)(
                  (this->VENUS_DSPM[vd1_row][vd1_bank] & (vd1_mask)) >> 8) *
              (signed int16_t)AaddB;
          temp_DmulAaddB =
              (DmulAaddB >> this->MY_VENUS_INS_PARAM.vfu_shamt) &
              ((1 << ((this->MY_VENUS_INS_PARAM.vew + 1) << 3)) - 1);
          BmulCsubD =
              (signed int8_t)(
                  (this->VENUS_DSPM[vs2_row][vs2_bank] & (vs2_mask)) >> 8) *
              (signed int16_t)CsubD;
          temp_BmulCsubD =
              (BmulCsubD >> this->MY_VENUS_INS_PARAM.vfu_shamt) &
              ((1 << ((this->MY_VENUS_INS_PARAM.vew + 1) << 3)) - 1);

          int sum_real =
              (signed int8_t)temp_CmulAsubB + (signed int8_t)temp_BmulCsubD;
          int sum_imag =
              (signed int8_t)temp_DmulAaddB + (signed int8_t)temp_BmulCsubD;

          if ((signed int)sum_real >
              (signed int)((1 << (((this->MY_VENUS_INS_PARAM.vew + 1) << 3) -
                                  1)) -
                           1))
            sum_real =
                (1 << (((this->MY_VENUS_INS_PARAM.vew + 1) << 3) - 1)) - 1;
          else if ((signed int)sum_real <
                   (signed int)-1 *
                       (1 << (((this->MY_VENUS_INS_PARAM.vew + 1) << 3) - 1)))
            sum_real = 1 << (((this->MY_VENUS_INS_PARAM.vew + 1) << 3) - 1);
          else
            sum_real = sum_real;

          if ((signed int)sum_imag >
              (signed int)((1 << (((this->MY_VENUS_INS_PARAM.vew + 1) << 3) -
                                  1)) -
                           1))
            sum_imag =
                (1 << (((this->MY_VENUS_INS_PARAM.vew + 1) << 3) - 1)) - 1;
          else if ((signed int)sum_imag <
                   (signed int)-1 *
                       (1 << (((this->MY_VENUS_INS_PARAM.vew + 1) << 3) - 1)))
            sum_imag = 1 << (((this->MY_VENUS_INS_PARAM.vew + 1) << 3) - 1);
          else
            sum_imag = sum_imag;

          this->VENUS_DSPM[vd1_row][vd1_bank] =
              (this->VENUS_DSPM[vd1_row][vd1_bank] & (~vd1_mask)) +
              ((sum_real << 8) & vd1_mask);
          this->VENUS_DSPM[vd2_row][vd2_bank] =
              (this->VENUS_DSPM[vd2_row][vd2_bank] & (~vd2_mask)) +
              ((sum_imag << 8) & vd2_mask);
        } else if (vd1_mask == 0xff) {

          AsubB =
              (signed int8_t)(
                  ((this->VENUS_DSPM[vs1_row][vs1_bank] & (vs1_mask)) << 8) >>
                  8) -
              (signed int8_t)(
                  ((this->VENUS_DSPM[vs2_row][vs2_bank] & (vs2_mask)) << 8) >>
                  8);
          AaddB =
              (signed int8_t)(
                  ((this->VENUS_DSPM[vs1_row][vs1_bank] & (vs1_mask)) << 8) >>
                  8) +
              (signed int8_t)(
                  ((this->VENUS_DSPM[vs2_row][vs2_bank] & (vs2_mask)) << 8) >>
                  8);
          CsubD =
              (signed int8_t)(
                  ((this->VENUS_DSPM[vd2_row][vd2_bank] & (vd2_mask)) << 8) >>
                  8) -
              (signed int8_t)(
                  ((this->VENUS_DSPM[vd1_row][vd1_bank] & (vd1_mask)) << 8) >>
                  8);

          // if ((signed int)AsubB >
          //     (signed int)((1
          //                   << (((this->MY_VENUS_INS_PARAM.vew + 1) << 3) -
          //                   1)) -
          //                  1))
          //   AsubB = (1 << (((this->MY_VENUS_INS_PARAM.vew + 1) << 3) - 1)) -
          //   1;
          // else if ((signed int)AsubB <
          //          (signed int)-1 *
          //              (1 << (((this->MY_VENUS_INS_PARAM.vew + 1) << 3) -
          //              1)))
          //   AsubB = 1 << (((this->MY_VENUS_INS_PARAM.vew + 1) << 3) - 1);
          // else
          //   AsubB = AsubB;

          // if ((signed int)AaddB >
          //     (signed int)((1
          //                   << (((this->MY_VENUS_INS_PARAM.vew + 1) << 3) -
          //                   1)) -
          //                  1))
          //   AaddB = (1 << (((this->MY_VENUS_INS_PARAM.vew + 1) << 3) - 1)) -
          //   1;
          // else if ((signed int)AaddB <
          //          (signed int)-1 *
          //              (1 << (((this->MY_VENUS_INS_PARAM.vew + 1) << 3) -
          //              1)))
          //   AaddB = 1 << (((this->MY_VENUS_INS_PARAM.vew + 1) << 3) - 1);
          // else
          //   AaddB = AaddB;

          // if ((signed int)CsubD >
          //     (signed int)((1
          //                   << (((this->MY_VENUS_INS_PARAM.vew + 1) << 3) -
          //                   1)) -
          //                  1))
          //   CsubD = (1 << (((this->MY_VENUS_INS_PARAM.vew + 1) << 3) - 1)) -
          //   1;
          // else if ((signed int)CsubD <
          //          (signed int)-1 *
          //              (1 << (((this->MY_VENUS_INS_PARAM.vew + 1) << 3) -
          //              1)))
          //   CsubD = 1 << (((this->MY_VENUS_INS_PARAM.vew + 1) << 3) - 1);
          // else
          //   CsubD = CsubD;

          CmulAsubB = (signed int8_t)(
                          (this->VENUS_DSPM[vd2_row][vd2_bank] & (vd2_mask))) *
                      (signed int16_t)(AsubB);
          temp_CmulAsubB =
              (CmulAsubB >> this->MY_VENUS_INS_PARAM.vfu_shamt) &
              ((1 << ((this->MY_VENUS_INS_PARAM.vew + 1) << 3)) - 1);
          DmulAaddB = (signed int8_t)(
                          (this->VENUS_DSPM[vd1_row][vd1_bank] & (vd1_mask))) *
                      (signed int16_t)(AaddB);
          temp_DmulAaddB =
              (DmulAaddB >> this->MY_VENUS_INS_PARAM.vfu_shamt) &
              ((1 << ((this->MY_VENUS_INS_PARAM.vew + 1) << 3)) - 1);
          BmulCsubD = (signed int8_t)(
                          (this->VENUS_DSPM[vs2_row][vs2_bank] & (vs2_mask))) *
                      (signed int16_t)(CsubD);
          temp_BmulCsubD =
              (BmulCsubD >> this->MY_VENUS_INS_PARAM.vfu_shamt) &
              ((1 << ((this->MY_VENUS_INS_PARAM.vew + 1) << 3)) - 1);

          int sum_real =
              (signed int8_t)temp_CmulAsubB + (signed int8_t)temp_BmulCsubD;
          int sum_imag =
              (signed int8_t)temp_DmulAaddB + (signed int8_t)temp_BmulCsubD;

          if ((signed int)sum_real >
              (signed int)((1 << (((this->MY_VENUS_INS_PARAM.vew + 1) << 3) -
                                  1)) -
                           1))
            sum_real =
                (1 << (((this->MY_VENUS_INS_PARAM.vew + 1) << 3) - 1)) - 1;
          else if ((signed int)sum_real <
                   (signed int)-1 *
                       (1 << (((this->MY_VENUS_INS_PARAM.vew + 1) << 3) - 1)))
            sum_real = 1 << (((this->MY_VENUS_INS_PARAM.vew + 1) << 3) - 1);
          else
            sum_real = sum_real;

          if ((signed int)sum_imag >
              (signed int)((1 << (((this->MY_VENUS_INS_PARAM.vew + 1) << 3) -
                                  1)) -
                           1))
            sum_imag =
                (1 << (((this->MY_VENUS_INS_PARAM.vew + 1) << 3) - 1)) - 1;
          else if ((signed int)sum_imag <
                   (signed int)-1 *
                       (1 << (((this->MY_VENUS_INS_PARAM.vew + 1) << 3) - 1)))
            sum_imag = 1 << (((this->MY_VENUS_INS_PARAM.vew + 1) << 3) - 1);
          else
            sum_imag = sum_imag;
          // vs1vd1 = (signed int8_t)((this->VENUS_DSPM[vs1_row][vs1_bank] &
          // (vs1_mask))) *
          //          (signed int8_t)((this->VENUS_DSPM[vd1_row][vd1_bank] &
          //          (vd1_mask)));
          // temp_vs1vd1 = (vs1vd1 >> this->MY_VENUS_INS_PARAM.vfu_shamt) &
          //               ((1 << ((this->MY_VENUS_INS_PARAM.vew + 1) << 3)) -
          //               1);
          // vs2vd2 = (signed int8_t)((this->VENUS_DSPM[vs2_row][vs2_bank] &
          // (vs2_mask))) *
          //          (signed int8_t)((this->VENUS_DSPM[vd2_row][vd2_bank] &
          //          (vd2_mask)));
          // temp_vs2vd2 = (vs2vd2 >> this->MY_VENUS_INS_PARAM.vfu_shamt) &
          //               ((1 << ((this->MY_VENUS_INS_PARAM.vew + 1) << 3)) -
          //               1);
          // vs1vd2 = (signed int8_t)((this->VENUS_DSPM[vs1_row][vs1_bank] &
          // (vs1_mask))) *
          //          (signed int8_t)((this->VENUS_DSPM[vd2_row][vd2_bank] &
          //          (vd2_mask)));
          // temp_vs1vd2 = (vs1vd2 >> this->MY_VENUS_INS_PARAM.vfu_shamt) &
          //               ((1 << ((this->MY_VENUS_INS_PARAM.vew + 1) << 3)) -
          //               1);
          // vs2vd1 = (signed int8_t)((this->VENUS_DSPM[vs2_row][vs2_bank] &
          // (vs2_mask))) *
          //          (signed int8_t)((this->VENUS_DSPM[vd1_row][vd1_bank] &
          //          (vd1_mask)));
          // temp_vs2vd1 = (vs2vd1 >> this->MY_VENUS_INS_PARAM.vfu_shamt) & ((1
          // << ((this->MY_VENUS_INS_PARAM.vew + 1) << 3)) - 1); int16_t
          // sum_real = temp_vs1vd2 - temp_vs2vd1; int16_t sum_imag =
          // temp_vs1vd1 + temp_vs2vd2; int16_t sum_real = temp_vs1vd1 -
          // temp_vs2vd2; int16_t sum_imag = temp_vs1vd2 + temp_vs2vd1;

          this->VENUS_DSPM[vd1_row][vd1_bank] =
              (this->VENUS_DSPM[vd1_row][vd1_bank] & (~vd1_mask)) +
              ((sum_real)&vd1_mask);
          this->VENUS_DSPM[vd2_row][vd2_bank] =
              (this->VENUS_DSPM[vd2_row][vd2_bank] & (~vd2_mask)) +
              ((sum_imag)&vd2_mask);
        } else {
          // vs1vd1 = ((this->VENUS_DSPM[vs1_row][vs1_bank] & (vs1_mask)) *
          //           (this->VENUS_DSPM[vd1_row][vd1_bank] & (vd1_mask))) &
          //          ((1 << ((this->MY_VENUS_INS_PARAM.vew + 1) << 3)) - 1);
          // vs2vd2 = ((this->VENUS_DSPM[vs2_row][vs2_bank] & (vs2_mask)) *
          //           (this->VENUS_DSPM[vd2_row][vd2_bank] & (vd2_mask))) &
          //          ((1 << ((this->MY_VENUS_INS_PARAM.vew + 1) << 3)) - 1);
          // vs1vd2 = ((this->VENUS_DSPM[vs1_row][vs1_bank] & (vs1_mask)) *
          //           (this->VENUS_DSPM[vd2_row][vd2_bank] & (vd2_mask))) &
          //          ((1 << ((this->MY_VENUS_INS_PARAM.vew + 1) << 3)) - 1);
          // vs2vd1 = ((this->VENUS_DSPM[vs2_row][vs2_bank] & (vs2_mask)) *
          //           (this->VENUS_DSPM[vd1_row][vd1_bank] & (vd1_mask))) &
          //          ((1 << ((this->MY_VENUS_INS_PARAM.vew + 1) << 3)) - 1);
          // // int16_t sum_real = vs1vd2 - vs2vd1;
          // // int16_t sum_imag = vs1vd1 + vs2vd2;
          // int16_t sum_real = temp_vs1vd2 - temp_vs2vd1;
          // int16_t sum_imag = temp_vs1vd1 + temp_vs2vd2;
          // // int16_t sum_real = temp_vs1vd1 - temp_vs2vd2;
          // // int16_t sum_imag = temp_vs2vd1 + temp_vs1vd2;

          // Tips/Warning: It is assumed that no overflow will occur at 16bit

          AsubB = ((this->VENUS_DSPM[vs1_row][vs1_bank] & (vs1_mask))) -
                  ((this->VENUS_DSPM[vs2_row][vs2_bank] & (vs2_mask)));
          AaddB = ((this->VENUS_DSPM[vs1_row][vs1_bank] & (vs1_mask))) +
                  ((this->VENUS_DSPM[vs2_row][vs2_bank] & (vs2_mask)));
          CsubD = ((this->VENUS_DSPM[vd2_row][vd2_bank] & (vd2_mask))) -
                  ((this->VENUS_DSPM[vd1_row][vd1_bank] & (vd1_mask)));

          if ((signed int)AsubB >
              (signed int)((1 << (((this->MY_VENUS_INS_PARAM.vew + 1) << 3) -
                                  1)) -
                           1))
            AsubB = (1 << (((this->MY_VENUS_INS_PARAM.vew + 1) << 3) - 1)) - 1;
          else if ((signed int)AsubB <
                   (signed int)-1 *
                       (1 << (((this->MY_VENUS_INS_PARAM.vew + 1) << 3) - 1)))
            AsubB = 1 << (((this->MY_VENUS_INS_PARAM.vew + 1) << 3) - 1);
          else
            AsubB = AsubB;

          if ((signed int)AaddB >
              (signed int)((1 << (((this->MY_VENUS_INS_PARAM.vew + 1) << 3) -
                                  1)) -
                           1))
            AaddB = (1 << (((this->MY_VENUS_INS_PARAM.vew + 1) << 3) - 1)) - 1;
          else if ((signed int)AaddB <
                   (signed int)-1 *
                       (1 << (((this->MY_VENUS_INS_PARAM.vew + 1) << 3) - 1)))
            AaddB = 1 << (((this->MY_VENUS_INS_PARAM.vew + 1) << 3) - 1);
          else
            AaddB = AaddB;

          if ((signed int)CsubD >
              (signed int)((1 << (((this->MY_VENUS_INS_PARAM.vew + 1) << 3) -
                                  1)) -
                           1))
            CsubD = (1 << (((this->MY_VENUS_INS_PARAM.vew + 1) << 3) - 1)) - 1;
          else if ((signed int)CsubD <
                   (signed int)-1 *
                       (1 << (((this->MY_VENUS_INS_PARAM.vew + 1) << 3) - 1)))
            CsubD = 1 << (((this->MY_VENUS_INS_PARAM.vew + 1) << 3) - 1);
          else
            CsubD = CsubD;

          CmulAsubB = ((this->VENUS_DSPM[vd2_row][vd2_bank] & (vd2_mask))) *
                      (AsubB & (vd2_mask));
          temp_CmulAsubB =
              (CmulAsubB >> this->MY_VENUS_INS_PARAM.vfu_shamt) &
              ((1 << ((this->MY_VENUS_INS_PARAM.vew + 1) << 3)) - 1);
          DmulAaddB = ((this->VENUS_DSPM[vd1_row][vd1_bank] & (vd1_mask))) *
                      (AaddB & (vd2_mask));
          temp_DmulAaddB =
              (DmulAaddB >> this->MY_VENUS_INS_PARAM.vfu_shamt) &
              ((1 << ((this->MY_VENUS_INS_PARAM.vew + 1) << 3)) - 1);
          BmulCsubD = ((this->VENUS_DSPM[vs2_row][vs2_bank] & (vs2_mask))) *
                      (CsubD & (vd2_mask));
          temp_BmulCsubD =
              (BmulCsubD >> this->MY_VENUS_INS_PARAM.vfu_shamt) &
              ((1 << ((this->MY_VENUS_INS_PARAM.vew + 1) << 3)) - 1);

          int16_t sum_real =
              (signed int16_t)temp_CmulAsubB + (signed int16_t)temp_BmulCsubD;
          int16_t sum_imag =
              (signed int16_t)temp_DmulAaddB + (signed int16_t)temp_BmulCsubD;

          if ((signed int)sum_real >
              (signed int)((1 << (((this->MY_VENUS_INS_PARAM.vew + 1) << 3) -
                                  1)) -
                           1))
            sum_real =
                (1 << (((this->MY_VENUS_INS_PARAM.vew + 1) << 3) - 1)) - 1;
          else if ((signed int)sum_real <
                   (signed int)-1 *
                       (1 << (((this->MY_VENUS_INS_PARAM.vew + 1) << 3) - 1)))
            sum_real = 1 << (((this->MY_VENUS_INS_PARAM.vew + 1) << 3) - 1);
          else
            sum_real = sum_real;

          if ((signed int)sum_imag >
              (signed int)((1 << (((this->MY_VENUS_INS_PARAM.vew + 1) << 3) -
                                  1)) -
                           1))
            sum_imag =
                (1 << (((this->MY_VENUS_INS_PARAM.vew + 1) << 3) - 1)) - 1;
          else if ((signed int)sum_imag <
                   (signed int)-1 *
                       (1 << (((this->MY_VENUS_INS_PARAM.vew + 1) << 3) - 1)))
            sum_imag = 1 << (((this->MY_VENUS_INS_PARAM.vew + 1) << 3) - 1);
          else
            sum_imag = sum_imag;

          this->VENUS_DSPM[vd1_row][vd1_bank] =
              (this->VENUS_DSPM[vd1_row][vd1_bank] & (~vd1_mask)) +
              (sum_real & vd1_mask);
          this->VENUS_DSPM[vd2_row][vd2_bank] =
              (this->VENUS_DSPM[vd2_row][vd2_bank] & (~vd2_mask)) +
              (sum_imag & vd2_mask);
        }
      }
    }
    break;
  }

  //   ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
  // SerDiv
  ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
  case VDIV: {
    this->instr_name = (char *)"VDIV";
    for (int i = 0; i < this->MY_VENUS_INS_PARAM.avl; i++) {
      int div_res;
      tie(vs1_row, vs1_bank, vs1_mask) =
          this->get_abs_addr_and_mask(i, this->MY_VENUS_INS_PARAM.vs1_head);
      tie(vs2_row, vs2_bank, vs2_mask) =
          this->get_abs_addr_and_mask(i, this->MY_VENUS_INS_PARAM.vs2_head);
      tie(vd1_row, vd1_bank, vd1_mask) =
          this->get_abs_addr_and_mask(i, this->MY_VENUS_INS_PARAM.vd1_head);
      if ((!this->MY_VENUS_INS_PARAM.vmask_read) ||
          (this->MY_VENUS_INS_PARAM.vmask_read && this->VENUS_MASK_DSPM[i])) {
        if (this->MY_VENUS_INS_PARAM.function3 == IVX) {
          if (vd1_mask == 0xff00) {
            if ((int8_t)(this->MY_VENUS_INS_PARAM.scalar_op) != 0) {
              div_res =
                  (int16_t)((int8_t)((this->VENUS_DSPM[vs1_row][vs1_bank] &
                                      (vs1_mask)) >>
                                     8)
                            << this->MY_VENUS_INS_PARAM.vfu_shamt) /
                  (int8_t)(this->MY_VENUS_INS_PARAM.scalar_op);
              div_res = div_res > INT8_MAX ? INT8_MAX : div_res;
              div_res = div_res < INT8_MIN ? INT8_MIN : div_res;
            } else {
              // div_res = -1;
              div_res = 1;
            }
            temp_res = (div_res) &
                       ((1 << ((this->MY_VENUS_INS_PARAM.vew + 1) << 3)) - 1);
          } else if (vd1_mask == 0xff) {
            if ((int8_t)(this->MY_VENUS_INS_PARAM.scalar_op) != 0) {
              div_res =
                  (int16_t)(
                      (int8_t)(this->VENUS_DSPM[vs1_row][vs1_bank] & (vs1_mask))
                      << this->MY_VENUS_INS_PARAM.vfu_shamt) /
                  (int8_t)(this->MY_VENUS_INS_PARAM.scalar_op);
              div_res = div_res > INT8_MAX ? INT8_MAX : div_res;
              div_res = div_res < INT8_MIN ? INT8_MIN : div_res;
            } else {
              // div_res = -1;
              div_res = 1;
            }
            temp_res = (div_res) &
                       ((1 << ((this->MY_VENUS_INS_PARAM.vew + 1) << 3)) - 1);
          } else {
            if ((int16_t)(this->MY_VENUS_INS_PARAM.scalar_op) != 0) {
              div_res =
                  ((int16_t)(this->VENUS_DSPM[vs1_row][vs1_bank] & (vs1_mask))
                   << this->MY_VENUS_INS_PARAM.vfu_shamt) /
                  (int16_t)(this->MY_VENUS_INS_PARAM.scalar_op);
              div_res = div_res > INT16_MAX ? INT16_MAX : div_res;
              div_res = div_res < INT16_MIN ? INT16_MIN : div_res;
            } else {
              // div_res = -1;
              div_res = 1;
            }
            temp_res = (div_res) &
                       ((1 << ((this->MY_VENUS_INS_PARAM.vew + 1) << 3)) - 1);
          }
        } else { // IVV
          if (vd1_mask == 0xff00) {
            if ((int8_t)((this->VENUS_DSPM[vs2_row][vs2_bank] & (vs2_mask)) >>
                         8) != 0) {
              div_res =
                  (int16_t)((int8_t)((this->VENUS_DSPM[vs1_row][vs1_bank] &
                                      (vs1_mask)) >>
                                     8)
                            << this->MY_VENUS_INS_PARAM.vfu_shamt) /
                  (int8_t)((this->VENUS_DSPM[vs2_row][vs2_bank] & (vs2_mask)) >>
                           8);
              div_res = div_res > INT8_MAX ? INT8_MAX : div_res;
              div_res = div_res < INT8_MIN ? INT8_MIN : div_res;
            } else {
              // div_res = -1;
              div_res = 1;
            }
            temp_res = (div_res) &
                       ((1 << ((this->MY_VENUS_INS_PARAM.vew + 1) << 3)) - 1);
          } else if (vd1_mask == 0xff) {
            if ((int8_t)((this->VENUS_DSPM[vs2_row][vs2_bank] & (vs2_mask))) !=
                0) {
              div_res =
                  (int16_t)(
                      (int8_t)(this->VENUS_DSPM[vs1_row][vs1_bank] & (vs1_mask))
                      << this->MY_VENUS_INS_PARAM.vfu_shamt) /
                  (int8_t)((this->VENUS_DSPM[vs2_row][vs2_bank] & (vs2_mask)));
              div_res = div_res > INT8_MAX ? INT8_MAX : div_res;
              div_res = div_res < INT8_MIN ? INT8_MIN : div_res;
            } else {
              // div_res = -1;
              div_res = 1;
            }
            temp_res = (div_res) &
                       ((1 << ((this->MY_VENUS_INS_PARAM.vew + 1) << 3)) - 1);
          } else {
            if ((int16_t)((this->VENUS_DSPM[vs2_row][vs2_bank] & (vs2_mask))) !=
                0) {
              div_res =
                  ((int16_t)((this->VENUS_DSPM[vs1_row][vs1_bank] & (vs1_mask)))
                   << this->MY_VENUS_INS_PARAM.vfu_shamt) /
                  (int16_t)((this->VENUS_DSPM[vs2_row][vs2_bank] & (vs2_mask)));
              div_res = div_res > INT16_MAX ? INT16_MAX : div_res;
              div_res = div_res < INT16_MIN ? INT16_MIN : div_res;
            } else {
              // div_res = -1;
              div_res = 1;
            }
            temp_res = (div_res) &
                       ((1 << ((this->MY_VENUS_INS_PARAM.vew + 1) << 3)) - 1);
          }
        }
        if (vd1_mask == 0xff00)
          this->VENUS_DSPM[vd1_row][vd1_bank] =
              (this->VENUS_DSPM[vd1_row][vd1_bank] & (~vd1_mask)) +
              ((temp_res << 8) & vd1_mask);
        else
          this->VENUS_DSPM[vd1_row][vd1_bank] =
              (this->VENUS_DSPM[vd1_row][vd1_bank] & (~vd1_mask)) +
              (temp_res & vd1_mask);
      } else {
        this->VENUS_DSPM[vd1_row][vd1_bank] =
            (this->VENUS_DSPM[vs1_row][vs1_bank] & (vs1_mask)) +
            (this->VENUS_DSPM[vd1_row][vd1_bank] & (~vd1_mask));
      }
      // if ((!this->MY_VENUS_INS_PARAM.vmask_read) ||
      // (this->MY_VENUS_INS_PARAM.vmask_read && this->VENUS_MASK_DSPM[i]))
      // {
      //   if (this->MY_VENUS_INS_PARAM.function3 == IVX)
      //     temp_res = this->MY_VENUS_INS_PARAM.scalar_op /
      //     (this->VENUS_DSPM[vs2_row][vs2_bank] & (vs2_mask));
      //   else // IVV
      //     temp_res = (this->VENUS_DSPM[vs1_row][vs1_bank] & (vs1_mask)) /
      //                (this->VENUS_DSPM[vs2_row][vs2_bank] & (vs2_mask));
      //   this->VENUS_DSPM[vd1_row][vd1_bank] =
      //   (this->VENUS_DSPM[vd1_row][vd1_bank] & (~vd1_mask)) + (temp_res &
      //   vd1_mask);
      // }
      // else
      // {
      //   this->VENUS_DSPM[vd1_row][vd1_bank] =
      //   (this->VENUS_DSPM[vs2_row][vs2_bank] & (vs2_mask)) +
      //   (this->VENUS_DSPM[vd1_row][vd1_bank] & (~vd1_mask));
      // }
    }
    break;
  }
  case VREM: {
    this->instr_name = (char *)"VREM";
    this->test_cnt++;
    for (int i = 0; i < this->MY_VENUS_INS_PARAM.avl; i++) {
      tie(vs1_row, vs1_bank, vs1_mask) =
          this->get_abs_addr_and_mask(i, this->MY_VENUS_INS_PARAM.vs1_head);
      tie(vs2_row, vs2_bank, vs2_mask) =
          this->get_abs_addr_and_mask(i, this->MY_VENUS_INS_PARAM.vs2_head);
      tie(vd1_row, vd1_bank, vd1_mask) =
          this->get_abs_addr_and_mask(i, this->MY_VENUS_INS_PARAM.vd1_head);
      if ((!this->MY_VENUS_INS_PARAM.vmask_read) ||
          (this->MY_VENUS_INS_PARAM.vmask_read && this->VENUS_MASK_DSPM[i])) {
        if (this->MY_VENUS_INS_PARAM.function3 == IVX) {
          if (vd1_mask == 0xff00)
            temp_res =
                this->MY_VENUS_INS_PARAM.scalar_op %
                (signed int8_t)(
                    (this->VENUS_DSPM[vs2_row][vs2_bank] & (vs2_mask)) >> 8);
          else if (vd1_mask == 0x00ff)
            temp_res = this->MY_VENUS_INS_PARAM.scalar_op %
                       (signed int8_t)(this->VENUS_DSPM[vs2_row][vs2_bank] &
                                       (vs2_mask));
          else
            temp_res = this->MY_VENUS_INS_PARAM.scalar_op %
                       (signed int16_t)(this->VENUS_DSPM[vs2_row][vs2_bank] &
                                        (vs2_mask));
        } else { // IVV
          if (vd1_mask == 0xff00)
            temp_res =
                (signed int8_t)(
                    (this->VENUS_DSPM[vs1_row][vs1_bank] & (vs1_mask)) >> 8) %
                (signed int8_t)(
                    (this->VENUS_DSPM[vs2_row][vs2_bank] & (vs2_mask)) >> 8);
          else if (vd1_mask == 0x00ff)
            temp_res = (signed int8_t)(this->VENUS_DSPM[vs1_row][vs1_bank] &
                                       (vs1_mask)) %
                       (signed int8_t)(this->VENUS_DSPM[vs2_row][vs2_bank] &
                                       (vs2_mask));
          else
            temp_res = (signed int16_t)(this->VENUS_DSPM[vs1_row][vs1_bank] &
                                        (vs1_mask)) %
                       (signed int16_t)(this->VENUS_DSPM[vs2_row][vs2_bank] &
                                        (vs2_mask));
        }
        if (vd1_mask == 0xff00)
          this->VENUS_DSPM[vd1_row][vd1_bank] =
              (this->VENUS_DSPM[vd1_row][vd1_bank] & (~vd1_mask)) +
              ((temp_res << 8) & vd1_mask);
        else if (vd1_mask == 0xff)
          this->VENUS_DSPM[vd1_row][vd1_bank] =
              (this->VENUS_DSPM[vd1_row][vd1_bank] & (~vd1_mask)) +
              (temp_res & vd1_mask);
        else
          this->VENUS_DSPM[vd1_row][vd1_bank] =
              (this->VENUS_DSPM[vd1_row][vd1_bank] & (~vd1_mask)) +
              (temp_res & vd1_mask);
        // if (this->MY_VENUS_INS_PARAM.function3 == IVX)
        //   temp_res = (signed int16_t)(this->VENUS_DSPM[vs2_row][vs2_bank] &
        //   (vs2_mask)) % this->MY_VENUS_INS_PARAM.scalar_op;
        // else // IVV
        //   temp_res = (signed int16_t)(this->VENUS_DSPM[vs1_row][vs1_bank] &
        //   (vs1_mask)) %
        //              (signed int16_t)(this->VENUS_DSPM[vs2_row][vs2_bank] &
        //              (vs2_mask));
        // this->VENUS_DSPM[vd1_row][vd1_bank] =
        // (this->VENUS_DSPM[vd1_row][vd1_bank] & (~vd1_mask)) + (temp_res &
        // vd1_mask);
      } else {
        this->VENUS_DSPM[vd1_row][vd1_bank] =
            (this->VENUS_DSPM[vs1_row][vs1_bank] & (vs1_mask)) +
            (this->VENUS_DSPM[vd1_row][vd1_bank] & (~vd1_mask));
      }
    }
    break;
  }
  case VDIVU: {
    this->instr_name = (char *)"VDIVU";
    for (int i = 0; i < this->MY_VENUS_INS_PARAM.avl; i++) {
      tie(vs1_row, vs1_bank, vs1_mask) =
          this->get_abs_addr_and_mask(i, this->MY_VENUS_INS_PARAM.vs1_head);
      tie(vs2_row, vs2_bank, vs2_mask) =
          this->get_abs_addr_and_mask(i, this->MY_VENUS_INS_PARAM.vs2_head);
      tie(vd1_row, vd1_bank, vd1_mask) =
          this->get_abs_addr_and_mask(i, this->MY_VENUS_INS_PARAM.vd1_head);
      if ((!this->MY_VENUS_INS_PARAM.vmask_read) ||
          (this->MY_VENUS_INS_PARAM.vmask_read && this->VENUS_MASK_DSPM[i])) {
        if (this->MY_VENUS_INS_PARAM.function3 == IVX)
          temp_res =
              (uint16_t)this->MY_VENUS_INS_PARAM.scalar_op /
              ((uint16_t)this->VENUS_DSPM[vs2_row][vs2_bank] & (vs2_mask));
        else // IVV
          temp_res =
              ((uint16_t)this->VENUS_DSPM[vs1_row][vs1_bank] & (vs1_mask)) /
              ((uint16_t)this->VENUS_DSPM[vs2_row][vs2_bank] & (vs2_mask));
        this->VENUS_DSPM[vd1_row][vd1_bank] =
            (this->VENUS_DSPM[vd1_row][vd1_bank] & (~vd1_mask)) +
            (temp_res & vd1_mask);
      } else {
        this->VENUS_DSPM[vd1_row][vd1_bank] =
            (this->VENUS_DSPM[vs1_row][vs1_bank] & (vs1_mask)) +
            (this->VENUS_DSPM[vd1_row][vd1_bank] & (~vd1_mask));
      }
    }
    break;
  }
  case VREMU: {
    this->instr_name = (char *)"VREMU";
    for (int i = 0; i < this->MY_VENUS_INS_PARAM.avl; i++) {
      tie(vs1_row, vs1_bank, vs1_mask) =
          this->get_abs_addr_and_mask(i, this->MY_VENUS_INS_PARAM.vs1_head);
      tie(vs2_row, vs2_bank, vs2_mask) =
          this->get_abs_addr_and_mask(i, this->MY_VENUS_INS_PARAM.vs2_head);
      tie(vd1_row, vd1_bank, vd1_mask) =
          this->get_abs_addr_and_mask(i, this->MY_VENUS_INS_PARAM.vd1_head);
      if ((!this->MY_VENUS_INS_PARAM.vmask_read) ||
          (this->MY_VENUS_INS_PARAM.vmask_read && this->VENUS_MASK_DSPM[i])) {
        if (this->MY_VENUS_INS_PARAM.function3 == IVX)
          temp_res =
              (uint16_t)this->MY_VENUS_INS_PARAM.scalar_op %
              ((uint16_t)this->VENUS_DSPM[vs2_row][vs2_bank] & (vs2_mask));
        else // IVV
          temp_res =
              ((uint16_t)this->VENUS_DSPM[vs1_row][vs1_bank] & (vs1_mask)) %
              ((uint16_t)this->VENUS_DSPM[vs2_row][vs2_bank] & (vs2_mask));
        this->VENUS_DSPM[vd1_row][vd1_bank] =
            (this->VENUS_DSPM[vd1_row][vd1_bank] & (~vd1_mask)) +
            (temp_res & vd1_mask);
      } else {
        this->VENUS_DSPM[vd1_row][vd1_bank] =
            (this->VENUS_DSPM[vs1_row][vs1_bank] & (vs1_mask)) +
            (this->VENUS_DSPM[vd1_row][vd1_bank] & (~vd1_mask));
      }
    }
    break;
  }
  case VSHUFFLE: {

    vector<int> index_arr;
    vector<int> number_arr;
    vector<int> shuffled_arr;
    // In IVV shuffle has two mode gather and scatter, differentiated by mask
    // read bit
    if (this->MY_VENUS_INS_PARAM.function3 == IVV) {
      switch (this->MY_VENUS_INS_PARAM.vmask_read) {
      case READ_MASKED: { // gather
        this->instr_name = (char *)"GATHER";
        for (int i = 0; i < this->MY_VENUS_INS_PARAM.avl; i++) {
          tie(vs2_row, vs2_bank, vs2_mask) = this->get_abs_addr_and_mask(
              i, this->MY_VENUS_INS_PARAM.vs2_head, 1);
          // std::cout << vs1_row << vs1_bank << vs1_mask << endl;
          index_arr.push_back(this->VENUS_DSPM[vs2_row][vs2_bank] & (vs2_mask));
        }
        for (int i = 0; i < this->MY_VENUS_INS_PARAM.avl; i++) {
          tie(vs1_row, vs1_bank, vs1_mask) = this->get_abs_addr_and_mask(
              index_arr[i], this->MY_VENUS_INS_PARAM.vs1_head);
          // tie(vs2_row, vs2_bank, vs2_mask) = this->get_abs_addr_and_mask(i,
          // this->MY_VENUS_INS_PARAM.vs2_head);
          if (vs1_mask == 0xff00)
            shuffled_arr.push_back(
                (this->VENUS_DSPM[vs1_row][vs1_bank] & (vs1_mask)) >> 8);
          else
            shuffled_arr.push_back(
                (this->VENUS_DSPM[vs1_row][vs1_bank] & (vs1_mask)));
          // number_arr.push_back(this->VENUS_DSPM[vs2_row][vs2_bank] &
          // (vs2_mask));
        }
        for (int i = 0; i < this->MY_VENUS_INS_PARAM.avl; i++) {
          tie(vd1_row, vd1_bank, vd1_mask) =
              this->get_abs_addr_and_mask(i, this->MY_VENUS_INS_PARAM.vd1_head);
          if (vd1_mask == 0xff00)
            this->VENUS_DSPM[vd1_row][vd1_bank] =
                (this->VENUS_DSPM[vd1_row][vd1_bank] & (~vd1_mask)) +
                ((shuffled_arr[i] << 8) & vd1_mask);
          else
            this->VENUS_DSPM[vd1_row][vd1_bank] =
                (this->VENUS_DSPM[vd1_row][vd1_bank] & (~vd1_mask)) +
                (shuffled_arr[i] & vd1_mask);
          // this->VENUS_DSPM[vd1_row][vd1_bank] =
          // (this->VENUS_DSPM[vd1_row][vd1_bank] & (~vd1_mask)) +
          // (shuffled_arr[i] & vd1_mask);
        }
        break;
      }
      case NORMAL_READ: { // scatter
        this->instr_name = (char *)"SCATTER";
        for (int i = 0; i < this->MY_VENUS_INS_PARAM.avl; i++) {
          tie(vs2_row, vs2_bank, vs2_mask) = this->get_abs_addr_and_mask(
              i, this->MY_VENUS_INS_PARAM.vs2_head, 1);
          // std::cout << vs1_row << vs1_bank << vs1_mask << endl;
          index_arr.push_back(this->VENUS_DSPM[vs2_row][vs2_bank] & (vs2_mask));
        }
        for (int i = 0; i < this->MY_VENUS_INS_PARAM.avl; i++) {
          tie(vs1_row, vs1_bank, vs1_mask) =
              this->get_abs_addr_and_mask(i, this->MY_VENUS_INS_PARAM.vs1_head);
          // tie(vs2_row, vs2_bank, vs2_mask) = this->get_abs_addr_and_mask(i,
          // this->MY_VENUS_INS_PARAM.vs2_head);
          if (vs1_mask == 0xff00)
            shuffled_arr.push_back(
                (this->VENUS_DSPM[vs1_row][vs1_bank] & (vs1_mask)) >> 8);
          else
            shuffled_arr.push_back(
                (this->VENUS_DSPM[vs1_row][vs1_bank] & (vs1_mask)));
          // number_arr.push_back(this->VENUS_DSPM[vs2_row][vs2_bank] &
          // (vs2_mask));
        }
        for (int i = 0; i < this->MY_VENUS_INS_PARAM.avl; i++) {
          tie(vd1_row, vd1_bank, vd1_mask) = this->get_abs_addr_and_mask(
              index_arr[i], this->MY_VENUS_INS_PARAM.vd1_head);
          if (vd1_mask == 0xff00)
            this->VENUS_DSPM[vd1_row][vd1_bank] =
                (this->VENUS_DSPM[vd1_row][vd1_bank] & (~vd1_mask)) +
                ((shuffled_arr[i] << 8) & vd1_mask);
          else
            this->VENUS_DSPM[vd1_row][vd1_bank] =
                (this->VENUS_DSPM[vd1_row][vd1_bank] & (~vd1_mask)) +
                (shuffled_arr[i] & vd1_mask);
        }
        break;
      }
      }
    } else // function3 else
    {
      for (int i = 0; i < this->MY_VENUS_INS_PARAM.avl; i++) {
        tie(vs1_row, vs1_bank, vs1_mask) =
            this->get_abs_addr_and_mask(i, this->MY_VENUS_INS_PARAM.vs1_head);
        // tie(vs2_row, vs2_bank, vs2_mask) = this->get_abs_addr_and_mask(i,
        // this->MY_VENUS_INS_PARAM.vs2_head);
        shuffled_arr.push_back(this->VENUS_DSPM[vs1_row][vs1_bank] &
                               (vs1_mask));
        // number_arr.push_back(this->VENUS_DSPM[vs2_row][vs2_bank] &
        // (vs2_mask));
      }
      // in IVX vshuffle is cyclic shift
      for (int i = 0; i < this->MY_VENUS_INS_PARAM.avl; i++) {
        if (i + this->MY_VENUS_INS_PARAM.scalar_op >=
            this->MY_VENUS_INS_PARAM.avl)
          tie(vd1_row, vd1_bank, vd1_mask) = this->get_abs_addr_and_mask(
              i + this->MY_VENUS_INS_PARAM.scalar_op -
                  this->MY_VENUS_INS_PARAM.avl,
              this->MY_VENUS_INS_PARAM.vd1_head);
        else
          tie(vd1_row, vd1_bank, vd1_mask) = this->get_abs_addr_and_mask(
              i + this->MY_VENUS_INS_PARAM.scalar_op,
              this->MY_VENUS_INS_PARAM.vd1_head);
        this->VENUS_DSPM[vd1_row][vd1_bank] =
            (this->VENUS_DSPM[vd1_row][vd1_bank] & (~vd1_mask)) +
            (shuffled_arr[i] & vd1_mask);
      }
    }
    break;
  }
  case VREDMIN: {
    // int8_t min_value = 0x7f;
    int8_t min_value;
    int16_t min_value_1;
    this->instr_name = (char *)"VREDMIN";
    vector<int8_t> number_ew8_arr;
    vector<int16_t> number_ew16_arr;
    if (this->MY_VENUS_INS_PARAM.vew == EW8) {
      for (int i = 0; i < this->MY_VENUS_INS_PARAM.avl; i++) {
        tie(vs2_row, vs2_bank, vs2_mask) =
            this->get_abs_addr_and_mask(i, this->MY_VENUS_INS_PARAM.vs2_head);
        if ((!this->MY_VENUS_INS_PARAM.vmask_read) ||
            (this->MY_VENUS_INS_PARAM.vmask_read && this->VENUS_MASK_DSPM[i])) {
          if ((i % 2) == 1)
            number_ew8_arr.push_back((int8_t)(
                (this->VENUS_DSPM[vs2_row][vs2_bank] & (vs2_mask)) >> 8));
          else
            number_ew8_arr.push_back(
                (int8_t)(this->VENUS_DSPM[vs2_row][vs2_bank] & (vs2_mask)));
        }
      }
      min_value =
          *std::min_element(number_ew8_arr.begin(), number_ew8_arr.end());
    } else {
      for (int i = 0; i < this->MY_VENUS_INS_PARAM.avl; i++) {
        tie(vs2_row, vs2_bank, vs2_mask) =
            this->get_abs_addr_and_mask(i, this->MY_VENUS_INS_PARAM.vs2_head);
        if ((!this->MY_VENUS_INS_PARAM.vmask_read) ||
            (this->MY_VENUS_INS_PARAM.vmask_read && this->VENUS_MASK_DSPM[i])) {
          number_ew16_arr.push_back(
              (int16_t)(this->VENUS_DSPM[vs2_row][vs2_bank] & (vs2_mask)));
        }
      }
      min_value_1 =
          *std::min_element(number_ew16_arr.begin(), number_ew16_arr.end());
    }
    // for (int i = 0; i < number_ew8_arr.size(); i++) {
    //   if ((int8_t)number_ew8_arr[i] < (int8_t)min_value) {
    //       min_value = number_ew8_arr[i];
    //   }
    // }

    tie(vd1_row, vd1_bank, vd1_mask) =
        this->get_abs_addr_and_mask(0, this->MY_VENUS_INS_PARAM.vd1_head);
    if (vd1_mask == 0xff00)
      this->VENUS_DSPM[vd1_row][vd1_bank] =
          (this->VENUS_DSPM[vd1_row][vd1_bank] & (~vd1_mask)) +
          ((min_value << 8) & vd1_mask);
    else if (vd1_mask == 0xff)
      this->VENUS_DSPM[vd1_row][vd1_bank] =
          (this->VENUS_DSPM[vd1_row][vd1_bank] & (~vd1_mask)) +
          (min_value & vd1_mask);
    else
      this->VENUS_DSPM[vd1_row][vd1_bank] =
          (this->VENUS_DSPM[vd1_row][vd1_bank] & (~vd1_mask)) +
          (min_value_1 & vd1_mask);
    break;
  }

  case VREDMAX: {
    // int8_t min_value = 0x7f;
    int8_t max_value;
    int16_t max_value_1;
    this->instr_name = (char *)"VREDMAX";
    vector<int8_t> number_ew8_arr;
    vector<int16_t> number_ew16_arr;
    if (this->MY_VENUS_INS_PARAM.vew == EW8) {
      for (int i = 0; i < this->MY_VENUS_INS_PARAM.avl; i++) {
        tie(vs2_row, vs2_bank, vs2_mask) =
            this->get_abs_addr_and_mask(i, this->MY_VENUS_INS_PARAM.vs2_head);
        if ((!this->MY_VENUS_INS_PARAM.vmask_read) ||
            (this->MY_VENUS_INS_PARAM.vmask_read && this->VENUS_MASK_DSPM[i])) {
          if ((i % 2) == 1)
            number_ew8_arr.push_back((int8_t)(
                (this->VENUS_DSPM[vs2_row][vs2_bank] & (vs2_mask)) >> 8));
          else
            number_ew8_arr.push_back(
                (int8_t)(this->VENUS_DSPM[vs2_row][vs2_bank] & (vs2_mask)));
        }
      }
      max_value =
          *std::max_element(number_ew8_arr.begin(), number_ew8_arr.end());
    } else {
      for (int i = 0; i < this->MY_VENUS_INS_PARAM.avl; i++) {
        tie(vs2_row, vs2_bank, vs2_mask) =
            this->get_abs_addr_and_mask(i, this->MY_VENUS_INS_PARAM.vs2_head);
        if ((!this->MY_VENUS_INS_PARAM.vmask_read) ||
            (this->MY_VENUS_INS_PARAM.vmask_read && this->VENUS_MASK_DSPM[i])) {
          number_ew16_arr.push_back(
              (int16_t)(this->VENUS_DSPM[vs2_row][vs2_bank] & (vs2_mask)));
        }
      }
      max_value_1 =
          *std::max_element(number_ew16_arr.begin(), number_ew16_arr.end());
    }
    // for (int i = 0; i < number_ew8_arr.size(); i++) {
    //   if ((int8_t)number_ew8_arr[i] < (int8_t)min_value) {
    //       min_value = number_ew8_arr[i];
    //   }
    // }

    tie(vd1_row, vd1_bank, vd1_mask) =
        this->get_abs_addr_and_mask(0, this->MY_VENUS_INS_PARAM.vd1_head);
    if (vd1_mask == 0xff00)
      this->VENUS_DSPM[vd1_row][vd1_bank] =
          (this->VENUS_DSPM[vd1_row][vd1_bank] & (~vd1_mask)) +
          ((max_value << 8) & vd1_mask);
    else if (vd1_mask == 0xff)
      this->VENUS_DSPM[vd1_row][vd1_bank] =
          (this->VENUS_DSPM[vd1_row][vd1_bank] & (~vd1_mask)) +
          (max_value & vd1_mask);
    else
      this->VENUS_DSPM[vd1_row][vd1_bank] =
          (this->VENUS_DSPM[vd1_row][vd1_bank] & (~vd1_mask)) +
          (max_value_1 & vd1_mask);
    break;
  }

  case VREDSUM: {
    // int8_t min_value = 0x7f;
    int32_t sum_result = 0;
    this->instr_name = (char *)"VREDSUM";
    if (this->MY_VENUS_INS_PARAM.vew == EW8) {
      for (int i = 0; i < this->MY_VENUS_INS_PARAM.avl; i++) {
        tie(vs2_row, vs2_bank, vs2_mask) =
            this->get_abs_addr_and_mask(i, this->MY_VENUS_INS_PARAM.vs2_head);
        if ((!this->MY_VENUS_INS_PARAM.vmask_read) ||
            (this->MY_VENUS_INS_PARAM.vmask_read && this->VENUS_MASK_DSPM[i])) {
          if ((i % 2) == 1)
            sum_result =
                sum_result +
                (int8_t)((this->VENUS_DSPM[vs2_row][vs2_bank] & (vs2_mask)) >>
                         8);
          else
            sum_result =
                sum_result +
                (int8_t)(this->VENUS_DSPM[vs2_row][vs2_bank] & (vs2_mask));
        }
      }
    } else {
      for (int i = 0; i < this->MY_VENUS_INS_PARAM.avl; i++) {
        tie(vs2_row, vs2_bank, vs2_mask) =
            this->get_abs_addr_and_mask(i, this->MY_VENUS_INS_PARAM.vs2_head);
        if ((!this->MY_VENUS_INS_PARAM.vmask_read) ||
            (this->MY_VENUS_INS_PARAM.vmask_read && this->VENUS_MASK_DSPM[i])) {
          sum_result =
              sum_result +
              (int16_t)(this->VENUS_DSPM[vs2_row][vs2_bank] & (vs2_mask));
        }
      }
    }

    // for (int i = 0; i < number_ew8_arr.size(); i++) {
    //   if ((int8_t)number_ew8_arr[i] < (int8_t)min_value) {
    //       min_value = number_ew8_arr[i];
    //   }
    // }

    tie(vd1_row, vd1_bank, vd1_mask) =
        this->get_abs_addr_and_mask(0, this->MY_VENUS_INS_PARAM.vd1_head, 1);
    this->VENUS_DSPM[vd1_row][vd1_bank] = ((sum_result)&0xffff);
    tie(vd1_row, vd1_bank, vd1_mask) =
        this->get_abs_addr_and_mask(1, this->MY_VENUS_INS_PARAM.vd1_head, 1);
    this->VENUS_DSPM[vd1_row][vd1_bank] = ((sum_result >> 16) & 0xffff);
    break;
  }

  // }
  default: {
    printf("Encountered illegal venus instructions!\n");
    printf("LSB PC is %08x\n", this->pc);
    printf("MSB PC is %08x\n", this->pc - 4);
    uint32_t MSBinstr = this->sram[(this->pc - 4) >> 2];
    std::cout << "This illegal instruction is " << hex << MSBinstr << ", "
              << bitset<32>(MSBinstr) << endl;
    std::cout << "AVL:\t" << std::dec << this->MY_VENUS_INS_PARAM.avl << "\n"
              << "vmask_read:\t" << this->MY_VENUS_INS_PARAM.vmask_read << "\n"
              << "vew:\t" << venus_vew_str(this->MY_VENUS_INS_PARAM.vew) << "\n"
              << "funct3:\t"
              << venus_funct3_str(this->MY_VENUS_INS_PARAM.function3) << "\n"
              << "scalar_op:\t" << this->MY_VENUS_INS_PARAM.scalar_op << "\n"
              << "funct5:\t" << this->MY_VENUS_INS_PARAM.function5 << endl;
    exit(EXIT_FAILURE);
  }
  }
};

void Venus_Emulator::emulate() {
emulate_start:
  bool instruction_valid = false;
  this->pc = (this->pc > 0x80000000) ? this->pc - 0x80000000 : this->pc;
  this->instr = this->sram[this->pc >> 2];
  this->rd = (this->instr >> 7) & 0x1F;
  this->rs1 = (this->instr >> 15) & 0x1F;
  this->rs2 = (this->instr >> 20) & 0x1F;
  // if((this->pc > 21820 && this-> VENUS_DSPM[64][0] != -16))
  // {
  //   printf("venus spm is illegally changed!\n");
  // }/
  // if ((this->sram[546]) != 2112573027)
  // {
  //   printf("venus spm is changed!\n");
  // }
  if ((this->pc == 1052)) {
    printf("venus spm is illegally changed!\n");
  }
  if ((this->instr & 0x7F) == 0b0001011 &&
      (((this->instr >> 25) & 0x7F) == 0b0)) {
    this->rs1 = this->rs1 + 32;
  } else if ((this->instr & 0x7F) == 0b0001011 &&
             (((this->instr >> 25) & 0x7F) == 0b10)) {
    this->rs1 = 32;
  }
  uint32_t branch_offset_temp = (((this->instr >> 31) & 0x1) << 12) +
                                (((this->instr >> 25) & 0x3F) << 5) +
                                (((this->instr >> 8) & 0xF) << 1) +
                                (((this->instr >> 7) & 0x1) << 11);
  this->branch_offset = (this->instr >> 31) == 0
                            ? branch_offset_temp
                            : branch_offset_temp | 0xFFFFE000;
  // decoding and execution
  // if (this->instr == 0x0a00e00b || this->instr == 0xc0002473 || this->instr
  // == 0xc02024f3 || \
  //     this->instr == 0x0800400b|| this->instr == 0x0600600b  || this->instr
  //     == 0xc0202573) { // irq instruction, for test
  //   instruction_valid = true;
  //   this->next_pc = this->pc + 4; // irq instruction, for test
  // }
  // encounter barrier signal
  if (this->instr == 0x800705b) {
    // printf("barrier!\n");
    this->next_pc = this->pc + 4;
    goto emulate_exit;
  }
  if (this->instr == 0x00100073) {
    instruction_valid = true;
    this->trap = true;
    this->ebreak = true;
    this->irq = this->irq | 0x00000002;
    this->instr_name = (char *)"ebreak";
    printf("The emulator is already stoped!\n");
  } else
    this->trap = false;
  if (this->trap == true) {
    printf("TRAP!\n");
  } else {
    switch (this->instr & 0x7F) {
    case 0b0110111: {
      instruction_valid = true;
      this->decode_lui();
      break;
    }
    case 0b0010111: {
      instruction_valid = true;
      this->decode_auipc();
      break;
    }
    case 0b1101111: {
      instruction_valid = true;
      this->decode_jal();
      break;
    }
    case 0b1100111: {
      instruction_valid = true;
      this->decode_jalr();
      break;
    }
    case 0b1100011: {
      instruction_valid = true;
      this->decode_branch();
      break;
    } // 0x1100111
    case 0b0000011: {
      instruction_valid = true;
      this->decode_load();
      break;
    }
    case 0b0100011: {
      instruction_valid = true;
      this->decode_store();
      break;
    } // store
    case 0b0010011: {
      instruction_valid = true;
      this->decode_arthimetic_imm();
      break;
    }
    case 0b0110011: {
      instruction_valid = true;
      if ((((this->instr >> 25) & 0x7F) == 0b0) ||
          (((this->instr >> 25) & 0x7F) == 0b0100000))
        this->decode_arthimetic_reg();
      else if ((((this->instr >> 25) & 0x7F) == 0b0000001))
        this->decode_RV32M();
      break;
    }
    case 0b0001011: {
      instruction_valid = true;
      this->decode_IRQ();
      break;
    }

    // venus extension
    case 0b0101011:
    case 0b1011011: {
      instruction_valid = true;

      // Bypass VSETCSR instruction Here
      if ((((this->instr >> 12) & 0x7) == 0b111) &&
          (((this->instr >> 27) & 0x1F) == 0b00100)) {
        uint16_t imm12 = ((this->instr >> 7) & 0x1F);
        switch ((this->instr >> 15) & 0x1F) {
        case 0: {
          this->MY_VENUS_INS_PARAM.vfu_shamt = this->cpuregs[imm12];
          printf("Write Shift Amount 0x%x to CSR%d\n", this->cpuregs[imm12], 0);
          goto venus_anchor_set_csr;
          // break;
        }
        }
      }

      // Bypass VSETVSRIMM instruction Here
      if ((((this->instr >> 12) & 0x7) == 0b111) &&
          (((this->instr >> 27) & 0x1F) == 0b00101)) {
        uint8_t csr_addr_imm = ((this->instr >> 7) & 0x1F);
        uint16_t csr_data_imm = ((this->instr >> 15) & 0xFFF);
        switch (csr_addr_imm) {
        case 0: {
          this->MY_VENUS_INS_PARAM.vfu_shamt = csr_data_imm;
          printf("Write Shift Amount 0x%x to CSR%d\n", csr_data_imm, 0);
          goto venus_anchor_set_csr;
        }
        case 1: {
          this->MY_VENUS_INS_PARAM.high_vd2_bits = (csr_data_imm >> 6) & 0x7;
          this->MY_VENUS_INS_PARAM.high_vd1_bits = (csr_data_imm >> 4) & 0x3;
          this->MY_VENUS_INS_PARAM.high_vs2_bits = (csr_data_imm >> 2) & 0x3;
          this->MY_VENUS_INS_PARAM.high_vs1_bits = (csr_data_imm)&0x3;
          printf("Write High Bit 0x%x for vd2 to CSR%d\n",
                 this->MY_VENUS_INS_PARAM.high_vd2_bits, 1);
          printf("Write High Bit 0x%x for vd1 to CSR%d\n",
                 this->MY_VENUS_INS_PARAM.high_vd1_bits, 1);
          printf("Write High Bit 0x%x for vs2 to CSR%d\n",
                 this->MY_VENUS_INS_PARAM.high_vs2_bits, 1);
          printf("Write High Bit 0x%x for vs1 to CSR%d\n",
                 this->MY_VENUS_INS_PARAM.high_vs1_bits, 1);
          goto venus_anchor_set_csr;
        }
        }
      }

      this->decode_venus_ext_msb();
      goto venus_anchor; // jump to fetch next vins(LSB)
      break;
    }
    }
  }
  if (((this->instr & 0x7F) == 0b1110011 &&
       ((this->instr & 0xfffff000) == 0b11000000000000000010000000000000)) ||
      ((this->instr & 0x7F == 0b1110011) &&
       ((this->instr & 0xfffff000) ==
        0b11000000000100000010000000000000))) { // instr_rdcycle
    instruction_valid = true;
    this->cpuregs[this->rd] = this->counter.cycle_count & 0xFFFFFFFF;
    this->instr_name = (char *)"rdcycle";
    this->next_pc = this->pc + 4;
    // printf("this->counter.cycle_count: %08x\n", this->counter.cycle_count);
  } else if ((((this->instr & 0x7F == 0b1110011) &&
               ((this->instr & 0xfffff000) ==
                0b11000000000000000010000000000000)) ||
              ((this->instr & 0x7F == 0b1110011) &&
               ((this->instr & 0xfffff000) ==
                0b11000000000100000010000000000000)))) { // instr_rdcycleh
    instruction_valid = true;
    this->cpuregs[this->rd] = (this->counter.cycle_count >> 32) & 0xFFFFFFFF;
    this->instr_name = (char *)"rdcycleh";
    this->next_pc = this->pc + 4;
  } else if (((this->instr & 0x7F) == 0b1110011) &&
             ((this->instr & 0xfffff000) ==
              0b11000000001000000010000000000000)) { // instr_rdinstr
    instruction_valid = true;
    this->cpuregs[this->rd] = this->counter.instr_count & 0xFFFFFFFF;
    this->instr_name = (char *)"rdinstr";
    this->next_pc = this->pc + 4;
  } else if ((this->instr & 0x7F == 0b1110011) &&
             ((this->instr & 0xfffff000) ==
              0b11001000001000000010000000000000)) { // instr_rdinstrh
    instruction_valid = true;
    this->cpuregs[this->rd] = (this->counter.instr_count >> 32) & 0xFFFFFFFF;
    this->instr_name = (char *)"rdinstrh";
    this->next_pc = this->pc + 4;
  }
  this->counter.cycle_count++;
  this->counter.instr_count++;
  if (timer == 1) {
    timer--;
    this->irq = this->irq | 0x00000001;
  }
  if (timer != 0) {
    timer--;
  }
  if (this->next_pc & 0b11 != 0) {
    this->irq = this->irq | 0x00000004;
  }
  if (this->irq & (~(this->irq_mask))) {
    if (!this->irq_active) { // user status
      this->next_pc = this->PROGADDR_IRQ;
      this->cpuregs[32] = this->pc;
      this->cpuregs[33] = this->irq & (~(this->irq_mask));
      this->irq_active = true;
      this->irq = 0;
    } else if (this->irq_active && (this->irq & 0b10)) { // irq status
      this->trap = true;
    }
  } else if (irq & 0b10) {
    this->trap = true;
  } else {
    this->irq = 0;
  }
  if (verbose) {
    printf("-------------------------------------------------current cpu "
           "state-------------------------------------------------\n");
    printf("It has been running for %ld cycles\n", this->counter.cycle_count);
    printf("The current PC is %08x\n", this->pc);
    printf("The instruction is %08x %s\n", this->sram[this->pc >> 2],
           this->instr_name);
    printf("rs1: %d, rs2: %d, rd: %d\n", this->rs1, this->rs2, this->rd);
    if (this->instr_name == "jal" || this->instr_name == "jalr")
      printf("jump_offset: %08x\n", this->jump_offset);
    if (this->instr_name == "lw" || this->instr_name == "lb" ||
        this->instr_name == "lh" || this->instr_name == "lbu" ||
        this->instr_name == "lhu" || this->instr_name == "sw" ||
        this->instr_name == "sb" || this->instr_name == "sh") {
      printf("load_store_addr: %08x, is index %d\n", this->load_store_addr,
             this->load_store_addr / 4);
    }
    printf("The next PC is %08x\n", this->next_pc);
    for (int i = 0; i < RISCV::REGNUM; ++i) {
      printf("cpureg[%d](%s): %08x\n", i, getRISCVRegABI(i), this->cpuregs[i]);
    }
    printf("-------------------------------------------------------end---------"
           "------------------------------------------------\n");
    cout << endl;
    // printf("+%08x++%d++%08x++%08x++%s+\n", this->pc, this->rd,
    // this->cpuregs[this->rd], this->instr, this->instr_name); // compare with
    // picorv32
  }

  if (instruction_valid == false) {
    printf("%s\n", this->instr_name);
    printf("Encountered illegal instructions!\n");
    printf("Current PC is %08x\n", this->pc);
    std::cout << "This illegal instruction is " << hex << this->instr << ", "
              << bitset<32>(this->instr) << endl;
    exit(EXIT_FAILURE);
  }
  goto emulate_exit;
  // update pc
venus_anchor:
  this->next_pc = this->pc + 4;
  this->pc = this->next_pc;
  this->instr = this->sram[this->pc >> 2];
  Venus_Emulator::decode_venus_ext_lsb();
  // Venus_Emulator::dump_venus_ins();
  Venus_Emulator::venus_execute();
  Venus_Emulator::dump_venus_ins();
  Venus_Emulator::dump_vins_result();
venus_anchor_set_csr:
  this->next_pc = this->pc + 4;
  if (venus_verbose) {
    // printf("-------------------------------------------------venus
    // state-------------------------------------------------\n");
    // printf("Instruction Name: %s\n", this->instr_name);
    // printf("MSB PC is %08x\n", this->pc - 4);
    uint32_t MSBinstr = this->sram[(this->pc - 4) >> 2];
    // std::cout << "This MSB instruction is " << hex << MSBinstr << ", " <<
    // bitset<32>(MSBinstr) << endl; printf("LSB PC is %08x\n", this->pc);
    uint32_t LSBinstr = this->sram[(this->pc) >> 2];
    // std::cout << "This LSB instruction is " << hex << LSBinstr << ", " <<
    // bitset<32>(LSBinstr) << endl; std::cout << "AVL:\t" << std::dec <<
    // this->MY_VENUS_INS_PARAM.avl << "\n"
    //           << "vmask_read:\t" << this->MY_VENUS_INS_PARAM.vmask_read <<
    //           "\n"
    //           << "vew:\t" << venus_vew_str(this->MY_VENUS_INS_PARAM.vew) <<
    //           "\n"
    //           << "funct3:\t" <<
    //           venus_funct3_str(this->MY_VENUS_INS_PARAM.function3) << "\n"
    //           << "scalar_op:\t" << this->MY_VENUS_INS_PARAM.scalar_op << "\n"
    //           << "funct5:\t" << this->MY_VENUS_INS_PARAM.function5 << endl;
    for (int i = 0; i < this->MY_VENUS_INS_PARAM.avl; ++i) {
      uint16_t vd1_row, vd1_bank, vd1_mask;
      tie(vd1_row, vd1_bank, vd1_mask) =
          this->get_abs_addr_and_mask(i, this->MY_VENUS_INS_PARAM.vd1_head);
      // printf("venusreg[%d][%d]: %04x\t\tvenusmask[%d]: %d\n",
      //        vd1_row, vd1_bank, this->VENUS_DSPM[vd1_row][vd1_bank],
      //        i, this->VENUS_MASK_DSPM[i]);
    }
    //   printf("-------------------------------------------------------end---------------------------------------------------------\n");
  }
emulate_exit:
  this->pc = this->next_pc;
};

void Venus_Emulator::dump_venus_ins() {
  this->dumpfile << "op: " << this->instr_name
                 << " avl: " << this->MY_VENUS_INS_PARAM.avl
                 << " scalar_op: " << this->MY_VENUS_INS_PARAM.scalar_op
                 << " vd1_head: " << this->MY_VENUS_INS_PARAM.vd1_head
                 << " vd2_head: " << this->MY_VENUS_INS_PARAM.vd2_head
                 << " vs1_head: " << this->MY_VENUS_INS_PARAM.vs1_head
                 << " vs2_head: " << this->MY_VENUS_INS_PARAM.vs2_head
                 << " vew: "
                 << this->venus_vew_str(this->MY_VENUS_INS_PARAM.vew)
                 << " OperationType: "
                 << this->venus_funct3_str(this->MY_VENUS_INS_PARAM.function3)
                 << std::endl;
  // this->dumpfile << "avl: " << this->MY_VENUS_INS_PARAM.avl << std::endl;
  // this->dumpfile << "scalar_op: " <<  this->MY_VENUS_INS_PARAM.scalar_op <<
  // std::endl; this->dumpfile << "vd1_head: " <<
  // this->MY_VENUS_INS_PARAM.vd1_head << std::endl; this->dumpfile <<
  // "vd2_head: " << this->MY_VENUS_INS_PARAM.vd2_head << std::endl;
  // this->dumpfile << "vs1_head: " << this->MY_VENUS_INS_PARAM.vs1_head <<
  // std::endl; this->dumpfile << "vs2_head: " <<
  // this->MY_VENUS_INS_PARAM.vs2_head << std::endl; this->dumpfile << "vew: "
  // << this->venus_vew_str(this->MY_VENUS_INS_PARAM.vew) << std::endl;
  // // outFile << "vmask_write: " << static_cast<int>(vmask_write) <<
  // std::endl;
  // // outFile << "vmask_read: " << static_cast<int>(vew) << std::endl;
  // this->dumpfile << "\n"<< std::endl;
};

bool isFileExists_stat(string &name) {
  struct stat buffer;
  return (stat(name.c_str(), &buffer) == 0);
}

void Venus_Emulator::dump_vins_result() {
  ofstream dumpfile;
  uint16_t vd1_row;
  uint16_t vd1_bank;
  uint16_t vd1_mask;
  uint16_t vd2_row;
  uint16_t vd2_bank;
  uint16_t vd2_mask;
  if (this->instr_name == "VSHUFFLE_CLBMV") {
    this->vins_issue_idx = this->vins_issue_idx + 1;
    return;
  }

  // First check whether `emulator_vins_result` exists
  std::string emuInstLogParentDir = "emulator_vins_result";
  bool ret = isFileExists_stat(emuInstLogParentDir);
  if (!ret) {
    if (!mkdir(emuInstLogParentDir.c_str(), 0755))
      std::cout << "Directory created successfully." << std::endl;
    else {
      std::cerr << "Failed to create directory." << std::endl;
      throw std::runtime_error("Failed to create directory.");
    }
  }

  std::string temp = this->instr_name;
  std::string new_file_path = "./" + emuInstLogParentDir + "/task" +
                              std::to_string(this->current_taskid);

  ret = isFileExists_stat(new_file_path);
  if (!ret) {
    if (!mkdir(new_file_path.c_str(), 0755))
      std::cout << "Directory created successfully." << std::endl;
    else {
      std::cerr << "Failed to create directory." << std::endl;
      throw std::runtime_error("Failed to create directory.");
    }
  }

  // std::string filename = "./emulator_vins_result/" + temp + "_" +
  // std::to_string(this->MY_VENUS_INS_PARAM.vs1_head) + "_" +
  //                        std::to_string(this->MY_VENUS_INS_PARAM.vs2_head) +
  //                        "_" +
  //                        std::to_string(this->MY_VENUS_INS_PARAM.vd1_head) +
  //                        "_" + std::to_string(this->vins_issue_idx) + ".txt";
  std::string filename = "./" + emuInstLogParentDir + "/task" +
                         std::to_string(this->current_taskid) + "/" + temp +
                         "_" + std::to_string(this->vins_issue_idx) + ".txt";

  if (this->MY_VENUS_INS_PARAM.op == VSHUFFLE)

  {
    dumpfile.open(filename);
    for (int i = 0; i < 4096; i++)

    {
      tie(vd1_row, vd1_bank, vd1_mask) =
          this->get_abs_addr_and_mask(i, this->MY_VENUS_INS_PARAM.vd1_head);
      if (vd1_mask == 0xff00)
        dumpfile << ((this->VENUS_DSPM[vd1_row][vd1_bank] & (vd1_mask)) >> 8)
                 << std::endl;
      else
        dumpfile << ((this->VENUS_DSPM[vd1_row][vd1_bank] & (vd1_mask)))
                 << std::endl;
    }
  } else if (this->MY_VENUS_INS_PARAM.op == VCMXMUL) {
    std::string filename = "./" + emuInstLogParentDir + "/task" +
                           std::to_string(this->current_taskid) + "/" + temp +
                           "_" + std::to_string(this->vins_issue_idx) + "_vd1" +
                           ".txt";
    dumpfile.open(filename);
    for (int i = 0; i < this->MY_VENUS_INS_PARAM.avl; i++) {
      tie(vd1_row, vd1_bank, vd1_mask) =
          this->get_abs_addr_and_mask(i, this->MY_VENUS_INS_PARAM.vd1_head);
      if (vd1_mask == 0xff00)
        dumpfile << ((this->VENUS_DSPM[vd1_row][vd1_bank] & (vd1_mask)) >> 8)
                 << std::endl;
      else
        dumpfile << ((this->VENUS_DSPM[vd1_row][vd1_bank] & (vd1_mask)))
                 << std::endl;
    }
    std::string filename1 = "./" + emuInstLogParentDir + "/task" +
                            std::to_string(this->current_taskid) + "/" + temp +
                            "_" + std::to_string(this->vins_issue_idx) +
                            "_vd2" + ".txt";
    dumpfile.close();
    dumpfile.open(filename1);
    for (int i = 0; i < this->MY_VENUS_INS_PARAM.avl; i++) {
      tie(vd2_row, vd2_bank, vd2_mask) =
          this->get_abs_addr_and_mask(i, this->MY_VENUS_INS_PARAM.vd2_head);
      if (vd2_mask == 0xff00)
        dumpfile << ((this->VENUS_DSPM[vd2_row][vd2_bank] & (vd2_mask)) >> 8)
                 << std::endl;
      else
        dumpfile << ((this->VENUS_DSPM[vd2_row][vd2_bank] & (vd2_mask)))
                 << std::endl;
    }
  } else {
    dumpfile.open(filename);
    for (int i = 0; i < this->MY_VENUS_INS_PARAM.avl; i++)

    {
      tie(vd1_row, vd1_bank, vd1_mask) =
          this->get_abs_addr_and_mask(i, this->MY_VENUS_INS_PARAM.vd1_head);
      if (vd1_mask == 0xff00)
        dumpfile << ((this->VENUS_DSPM[vd1_row][vd1_bank] & (vd1_mask)) >> 8)
                 << std::endl;
      else
        dumpfile << ((this->VENUS_DSPM[vd1_row][vd1_bank] & (vd1_mask)))
                 << std::endl;
    }
  }
  dumpfile.close();

  //////////////////////////////////////////////////////////////////////////////////////////////////
  // VInst-level Compare
  //////////////////////////////////////////////////////////////////////////////////////////////////
  // Check `diff --help` in linux shell
  // int headcheck = 24;
  // std::string vcs_log_name = "../vins_result/vins_result_tile0_task" +
  //                            std::to_string(this->current_taskid + 1) + "/" +
  //                            temp + "_" +
  //                            std::to_string(this->vins_issue_idx) +
  //                            ".txt";
  // ret = isFileExists_stat(vcs_log_name);
  // if (!ret) {
  //   std::cerr << "Failed to find logfile: " << vcs_log_name << std::endl;
  //   // throw std::runtime_error("Failed to find logfile.");
  // }
  // std::string cmd = "diff -w <(head -n " + std::to_string(headcheck) + " " +
  //                   filename + ") <(head -n " + std::to_string(headcheck) +
  //                   " " + vcs_log_name + ") > /dev/null 2>&1";
  // ret = system(("bash -c \"" + cmd + "\"").c_str());
  // if (ret) {
  //   std::cout << "Compare Failed! "
  //             << "Task ID: " << this->current_taskid
  //             << ", Inst Name: " << this->instr_name << "_"
  //             << std::to_string(this->vins_issue_idx) << std::endl;
  //   // throw std::runtime_error("Compare Failed.");
  // }
  this->vins_issue_idx = this->vins_issue_idx + 1;
};

void Venus_Emulator::decode_venus_ext_lsb() {
  if (this->MY_VENUS_INS_PARAM.op >= VMULADD &&
      this->MY_VENUS_INS_PARAM.op <= VCMXMUL) {
    if (this->MY_VENUS_INS_PARAM.function3 == IVX ||
        this->MY_VENUS_INS_PARAM.function3 == MVX)
      this->MY_VENUS_INS_PARAM.scalar_op = this->cpuregs[(this->instr) & 0x1f];
    else
      this->MY_VENUS_INS_PARAM.vs1_head =
          ((this->instr) & 0xff) + (this->MY_VENUS_INS_PARAM.vs1_msb << 10);
    this->MY_VENUS_INS_PARAM.vs2_head =
        ((this->instr >> 8) & 0xff) + (this->MY_VENUS_INS_PARAM.vs2_msb << 10);
    this->MY_VENUS_INS_PARAM.vd1_head =
        ((this->instr >> 16) & 0xff) + (this->MY_VENUS_INS_PARAM.vd1_msb << 10);
    this->MY_VENUS_INS_PARAM.vd2_head = (this->instr >> 24) & 0xff;
  } else {
    if (this->MY_VENUS_INS_PARAM.function3 == IVX ||
        this->MY_VENUS_INS_PARAM.function3 == MVX)
      this->MY_VENUS_INS_PARAM.scalar_op = this->cpuregs[(this->instr) & 0x1f];
    else
      this->MY_VENUS_INS_PARAM.vs1_head =
          ((this->instr) & 0x3ff) + (this->MY_VENUS_INS_PARAM.vs1_msb << 10);
    this->MY_VENUS_INS_PARAM.vs2_head =
        ((this->instr >> 10) & 0x3ff) +
        (this->MY_VENUS_INS_PARAM.vs2_msb << 10);
    this->MY_VENUS_INS_PARAM.vd1_head =
        ((this->instr >> 20) & 0x3ff) +
        (this->MY_VENUS_INS_PARAM.vd1_msb << 10);
  }
};

void Venus_Emulator::decode_venus_ext_msb() {
  this->MY_VENUS_INS_PARAM.avl = this->cpuregs[((this->instr >> 7) & 0x1f)];
  this->MY_VENUS_INS_PARAM.vmask_read =
      static_cast<VMASK_READ>((this->instr >> 25) & 0x1);
  this->MY_VENUS_INS_PARAM.vew = static_cast<VEW>((this->instr >> 26) & 0x1);
  this->MY_VENUS_INS_PARAM.function3 =
      static_cast<FUNC3>((this->instr >> 12) & 0x7);
  this->MY_VENUS_INS_PARAM.op_code = (this->instr) & 0x7f;
  this->MY_VENUS_INS_PARAM.function5 = (this->instr >> 27) & 0x1f;
  this->MY_VENUS_INS_PARAM.vs2_msb = (this->instr >> 16) & 0x01;
  this->MY_VENUS_INS_PARAM.vs1_msb = (this->instr >> 15) & 0x01;
  this->MY_VENUS_INS_PARAM.vd1_msb = (this->instr >> 17) & 0x01;
  switch (this->MY_VENUS_INS_PARAM.op_code) {
  case 0b1011011: // 1011011
    switch (this->MY_VENUS_INS_PARAM.function5) {
    case 0b00000:
      if (MY_VENUS_INS_PARAM.function3 == OPMISC)
        this->MY_VENUS_INS_PARAM.op = VMNOT;
      else
        this->MY_VENUS_INS_PARAM.op = VAND;
      break;
    case 0b00001:
      if (MY_VENUS_INS_PARAM.function3 == OPMISC)
        this->MY_VENUS_INS_PARAM.op = VRANGE;
      else
        this->MY_VENUS_INS_PARAM.op = VOR;
      break;
    case 0b00010:
      if (MY_VENUS_INS_PARAM.function3 == OPMISC)
        this->MY_VENUS_INS_PARAM.op = VSHUFFLE_CLBMV;
      else
        this->MY_VENUS_INS_PARAM.op = VBRDCST;
      break;
    case 0b01000:
      this->MY_VENUS_INS_PARAM.op = VSHUFFLE;
      break;
    case 0b11111:
      this->MY_VENUS_INS_PARAM.op = VSHUFFLE;
      break;
    case 0b00011:
      this->MY_VENUS_INS_PARAM.op = VSLL;
      break;
    case 0b00100:
      this->MY_VENUS_INS_PARAM.op = VSRL;
      break;
    case 0b00101:
      this->MY_VENUS_INS_PARAM.op = VSRA;
      break;
    case 0b00110:
      this->MY_VENUS_INS_PARAM.op = VXOR;
      break;
    case 0b10000:
      this->MY_VENUS_INS_PARAM.op = VSEQ;
      break;
    case 0b10001:
      this->MY_VENUS_INS_PARAM.op = VSNE;
      break;
    case 0b10010:
      this->MY_VENUS_INS_PARAM.op = VSLTU;
      break;
    case 0b10011:
      this->MY_VENUS_INS_PARAM.op = VSLT;
      break;
    case 0b10100:
      this->MY_VENUS_INS_PARAM.op = VSLEU;
      break;
    case 0b10101:
      this->MY_VENUS_INS_PARAM.op = VSLE;
      break;
    case 0b10110:
      this->MY_VENUS_INS_PARAM.op = VSGTU;
      break;
    case 0b10111:
      this->MY_VENUS_INS_PARAM.op = VSGT;
      break;
    // case 0b11111:
    //   this->MY_VENUS_INS_PARAM.op = YIELD;
    //   break;
    default:
      std::cout << "Unknown funct5 in 0b1011011" << endl;
      exit(EXIT_FAILURE);
      break;
    }
    break;
  case 0b0101011:
    switch (this->MY_VENUS_INS_PARAM.function5) {
    case 0b00000:
      this->MY_VENUS_INS_PARAM.op = VADD;
      break;
    case 0b00001:
      this->MY_VENUS_INS_PARAM.op = VRSUB;
      break;
    case 0b00010:
      this->MY_VENUS_INS_PARAM.op = VSUB;
      break;
    case 0b00011:
      this->MY_VENUS_INS_PARAM.op = VMUL;
      break;
    case 0b00100:
      this->MY_VENUS_INS_PARAM.op = VMULH;
      break;
    case 0b00101:
      this->MY_VENUS_INS_PARAM.op = VMULHU;
      break;
    case 0b00110:
      this->MY_VENUS_INS_PARAM.op = VMULHSU;
      break;
    case 0b00111:
      this->MY_VENUS_INS_PARAM.op = VMULADD;
      break;
    case 0b01000:
      this->MY_VENUS_INS_PARAM.op = VMULSUB;
      break;
    case 0b01001:
      this->MY_VENUS_INS_PARAM.op = VADDMUL;
      break;
    case 0b01010:
      this->MY_VENUS_INS_PARAM.op = VSUBMUL;
      break;
    case 0b01011:
      this->MY_VENUS_INS_PARAM.op = VCMXMUL;
      break;
    case 0b01100:
      this->MY_VENUS_INS_PARAM.op = VDIV;
      break;
    case 0b01101:
      this->MY_VENUS_INS_PARAM.op = VREM;
      break;
    case 0b01110:
      this->MY_VENUS_INS_PARAM.op = VDIVU;
      break;
    case 0b01111:
      this->MY_VENUS_INS_PARAM.op = VREMU;
      break;
    case 0b10000:
      this->MY_VENUS_INS_PARAM.op = VSADD;
      break;
    case 0b10001:
      this->MY_VENUS_INS_PARAM.op = VSADDU;
      break;
    case 0b10010:
      this->MY_VENUS_INS_PARAM.op = VSSUB;
      break;
    case 0b10011:
      this->MY_VENUS_INS_PARAM.op = VSSUBU;
      break;
    case 0b11101:
      this->MY_VENUS_INS_PARAM.op = VREDMIN;
      break;
    case 0b11111:
      this->MY_VENUS_INS_PARAM.op = VREDSUM;
      break;
    default:
      std::cout << "Unknown funct5 in 0b0101011" << endl;
      exit(EXIT_FAILURE);
      break;
    }
    break;
  default:
    std::cout << "Unknown funct7 in Venus Extension" << endl;
    exit(EXIT_FAILURE);
    break;
  }
};
int32_t Venus_Emulator::lw_from_vspm(int load_addr) {
  // assert((load_addr & 0b11) == 0);
  int num_word = load_addr;
  int row =
      num_word / (this->VENUS_DSPM_LANE * this->VENUS_DSPM_BANK_PER_LANE / 2);
  int bank_1st =
      num_word % ((this->VENUS_DSPM_LANE * this->VENUS_DSPM_BANK_PER_LANE / 2));
  // printf("%x\t%d\t%d\t%d\t%d\n", load_addr, row,
  // bank_1st,this->VENUS_DSPM[row][bank_1st*2],this->VENUS_DSPM[row][bank_1st*2
  // + 1]);
  return ((this->VENUS_DSPM[row][bank_1st * 2]) & 0xffff) +
         (unsigned)(this->VENUS_DSPM[row][bank_1st * 2 + 1] << 16);
}

int32_t Venus_Emulator::st_to_vspm(int store_addr, int store_value) {
  // assert((store_addr & 0b11) == 0);
  int num_word = store_addr;
  int row =
      num_word / (this->VENUS_DSPM_LANE * this->VENUS_DSPM_BANK_PER_LANE / 2);
  int bank_1st =
      num_word % ((this->VENUS_DSPM_LANE * this->VENUS_DSPM_BANK_PER_LANE / 2));
  // std::cout << num_word << endl;
  // std::cout << this->VENUS_DSPM[row][bank_1st*2 + 1] << 16 << endl;
  // std::cout << row << endl;
  // std::cout << bank_1st << endl;
  this->VENUS_DSPM[row][bank_1st * 2] = store_value & 0xffff;
  this->VENUS_DSPM[row][bank_1st * 2 + 1] = store_value >> 16;
  return 0;
}

int32_t Venus_Emulator::lw_from_scalar_spm(int load_addr) {
  return this->sram[load_addr];
}

int32_t Venus_Emulator::st_to_scalar_spm(int store_addr, int store_value) {
  this->sram[store_addr] = store_value;
  return 0;
}

inline void Venus_Emulator::decode_store() {
  int temp;
  this->load_store_addr =
      this->cpuregs[this->rs1] +
      ((((this->instr >> 31) == 0) ? 0 : 0xFFFFF000) |
       (((this->instr >> 7) & 0x1F) + (((this->instr >> 25) & 0x7F) << 5)));
  if (this->load_store_addr > this->BLOCK_PERSPECTIVE_BASEADDR) {
    this->load_store_addr =
        this->load_store_addr - this->BLOCK_PERSPECTIVE_BASEADDR;
    // tile control register no action here
    if (this->load_store_addr >= this->BLOCK_CSR_OFFSET) {
      unsigned offset_addr = this->load_store_addr - this->BLOCK_CSR_OFFSET -
                             this->BLOCK_CSR_NUMRET_OFFSET;
      if (offset_addr == 0) {
        printf("This task returns %d values\n", this->cpuregs[this->rs2]);
        this->taskretnum[this->current_taskid] = this->cpuregs[this->rs2];
      }
      offset_addr = this->load_store_addr - this->BLOCK_CSR_OFFSET -
                    this->BLOCK_CSR_RET0ADDR_OFFSET;
      if (offset_addr >= 0 && offset_addr <= 0x7C) {
        if ((offset_addr >> 2) & 0x1) {
          printf("Return %d length: %d\n", offset_addr >> 3,
                 this->cpuregs[this->rs2]);
          this->taskret[offset_addr >> 3][this->current_taskid].length =
              this->cpuregs[this->rs2];
        } else {
          printf("Return %d address: %x\n", offset_addr >> 3,
                 this->cpuregs[this->rs2]);
          this->taskret[offset_addr >> 3][this->current_taskid].ptr =
              this->cpuregs[this->rs2];
        }
      }
      goto out;
    }
    // venus vector spm load only load word
    if (this->load_store_addr >= this->BLOCK_VRF_OFFSET) {
      // this->cpuregs[this->rd] = this->lw_from_vspm(this->load_store_addr -
      // 0x100000);
      switch ((this->instr >> 12) & 0b111) {
      case 0b0: {
        switch ((this->load_store_addr - this->BLOCK_VRF_OFFSET) & 0b11) {
        case 0b0: {
          temp = ((this->lw_from_vspm(
                      (this->load_store_addr - this->BLOCK_VRF_OFFSET) >> 2)) &
                  0xFFFFFF00) |
                 (this->cpuregs[this->rs2] & 0xFF);
          this->st_to_vspm(
              (this->load_store_addr - this->BLOCK_VRF_OFFSET) >> 2, temp);
          break;
        }
        case 0b1: {
          temp = ((this->lw_from_vspm(
                      (this->load_store_addr - this->BLOCK_VRF_OFFSET) >> 2)) &
                  0xFFFF00FF) |
                 ((this->cpuregs[this->rs2] & 0xFF) << 8);
          this->st_to_vspm(
              (this->load_store_addr - this->BLOCK_VRF_OFFSET) >> 2, temp);
          break;
        }
        case 0b10: {
          temp = ((this->lw_from_vspm(
                      (this->load_store_addr - this->BLOCK_VRF_OFFSET) >> 2)) &
                  0xFF00FFFF) |
                 ((this->cpuregs[this->rs2] & 0xFF) << 16);
          this->st_to_vspm(
              (this->load_store_addr - this->BLOCK_VRF_OFFSET) >> 2, temp);
          break;
        }
        case 0b11: {
          temp = ((this->lw_from_vspm(
                      (this->load_store_addr - this->BLOCK_VRF_OFFSET) >> 2)) &
                  0x00FFFFFF) |
                 ((this->cpuregs[this->rs2] & 0xFF) << 24);
          this->st_to_vspm(
              (this->load_store_addr - this->BLOCK_VRF_OFFSET) >> 2, temp);
          break;
        }
        }
        this->instr_name = (char *)"sb";
        break;
      }
      case 0b1: {
        switch ((this->load_store_addr - this->BLOCK_VRF_OFFSET) & 0b11) {
        case 0b0: {
          temp = ((this->lw_from_vspm(
                      (this->load_store_addr - this->BLOCK_VRF_OFFSET) >> 2)) &
                  0xFFFF0000) |
                 (this->cpuregs[this->rs2] & 0xFFFF);
          this->st_to_vspm(
              (this->load_store_addr - this->BLOCK_VRF_OFFSET) >> 2, temp);
          break;
        }
        case 0b10: {
          temp = ((this->lw_from_vspm(
                      (this->load_store_addr - this->BLOCK_VRF_OFFSET) >> 2)) &
                  0x0000FFFF) |
                 ((this->cpuregs[this->rs2] & 0xFFFF) << 16);
          this->st_to_vspm(
              (this->load_store_addr - this->BLOCK_VRF_OFFSET) >> 2, temp);
          break;
        }
        }
        this->instr_name = (char *)"sh";
        break;
      }
      case 0b10: {
        this->st_to_vspm((this->load_store_addr - this->BLOCK_VRF_OFFSET) >> 2,
                         this->cpuregs[this->rs2]);
        this->instr_name = (char *)"sw";
        break;
      }
      }
      goto out;
    }
  }
  if ((this->load_store_addr >> 2) < CPU::SRAMSIZE &&
      (this->load_store_addr) != PRINTF_ADDR) {
    switch ((this->instr >> 12) & 0b111) {
    case 0b0: {
      switch (this->load_store_addr & 0b11) {
      case 0b0: {
        (this->sram[this->load_store_addr >> 2]) =
            ((this->sram[this->load_store_addr >> 2]) & 0xFFFFFF00) |
            (this->cpuregs[this->rs2] & 0xFF);
        break;
      }
      case 0b1: {
        (this->sram[this->load_store_addr >> 2]) =
            ((this->sram[this->load_store_addr >> 2]) & 0xFFFF00FF) |
            ((this->cpuregs[this->rs2] & 0xFF) << 8);
        break;
      }
      case 0b10: {
        (this->sram[this->load_store_addr >> 2]) =
            ((this->sram[this->load_store_addr >> 2]) & 0xFF00FFFF) |
            ((this->cpuregs[this->rs2] & 0xFF) << 16);
        break;
      }
      case 0b11: {
        (this->sram[this->load_store_addr >> 2]) =
            ((this->sram[this->load_store_addr >> 2]) & 0x00FFFFFF) |
            ((this->cpuregs[this->rs2] & 0xFF) << 24);
        break;
      }
      }
      this->instr_name = (char *)"sb";
      break;
    }
    case 0b1: {
      switch (this->load_store_addr & 0b11) {
      case 0b0: {
        (this->sram[this->load_store_addr >> 2]) =
            ((this->sram[this->load_store_addr >> 2]) & 0xFFFF0000) |
            (this->cpuregs[this->rs2] & 0xFFFF);
        break;
      }
      case 0b10: {
        (this->sram[this->load_store_addr >> 2]) =
            ((this->sram[this->load_store_addr >> 2]) & 0x0000FFFF) |
            ((this->cpuregs[this->rs2] & 0xFFFF) << 16);
        break;
      }
      }
      this->instr_name = (char *)"sh";
      break;
    }
    case 0b10: {
      (this->sram[this->load_store_addr >> 2]) = (this->cpuregs[this->rs2]);
      this->instr_name = (char *)"sw";
      break;
    }
    }
  } else if (this->load_store_addr == this->PRINTF_ADDR) {
    uint32_t store_data;
    switch ((this->instr >> 12) & 0b111) {
    case 0b0:
      store_data = this->cpuregs[this->rs2] & 0xFF;
      break;
    case 0b1:
      store_data = this->cpuregs[this->rs2] & 0xFFFF;
      break;
    case 0b10:
      store_data = (this->cpuregs[this->rs2]);
      break;
    }
    // printf("tile printf: %c\n", store_data);
    fprintf(stderr, "%c", store_data);
    this->instr_name = (char *)"sw";
  } else if (this->load_store_addr == this->TEST_PRINTF_ADDR) {
    uint32_t store_data;
    switch ((this->instr >> 12) & 0b111) {
    case 0b0:
      store_data = this->cpuregs[this->rs2] & 0xFF;
      break;
    case 0b1:
      store_data = this->cpuregs[this->rs2] & 0xFFFF;
      break;
    case 0b10:
      store_data = (this->cpuregs[this->rs2]);
      break;
    }
    if (store_data == 123456789)
      printf("ALL TESTS PASSED.\n");
    else
      printf("ERROR!\n");
    this->instr_name = (char *)"sw";
  }
out:
  this->next_pc = this->pc + 4;
}

inline void Venus_Emulator::decode_load() {
  this->load_store_addr =
      this->cpuregs[this->rs1] +
      this->signed_sim((this->instr >> 31) == 0
                           ? (this->instr >> 20)
                           : ((this->instr >> 20) | 0xFFFFF000));
  if (this->load_store_addr > this->BLOCK_PERSPECTIVE_BASEADDR) {
    this->load_store_addr =
        this->load_store_addr - this->BLOCK_PERSPECTIVE_BASEADDR;
    // tile control register no action here
    if (this->load_store_addr >= this->BLOCK_CSR_OFFSET) {
      goto out;
    }
    // venus vector spm lw lb lh
    if (this->load_store_addr >= this->BLOCK_VRF_OFFSET) {
      // this->cpuregs[this->rd] = this->lw_from_vspm(this->load_store_addr -
      // 0x100000);
      switch ((this->instr >> 12) & 0b111) {
      case 0b0: {
        switch ((this->load_store_addr - this->BLOCK_VRF_OFFSET) & 0b11) {
        case 0b0: {
          this->cpuregs[this->rd] =
              (((this->lw_from_vspm(
                    (this->load_store_addr - this->BLOCK_VRF_OFFSET) >> 2)) >>
                7) &
               0b1)
                  ? (0xFFFFFF00 |
                     ((this->lw_from_vspm(
                          (this->load_store_addr - this->BLOCK_VRF_OFFSET) >>
                          2)) &
                      0xFF))
                  : ((this->lw_from_vspm(
                         (this->load_store_addr - this->BLOCK_VRF_OFFSET) >>
                         2)) &
                     0xFF);
          break;
        }
        case 0b1: {
          this->cpuregs[this->rd] =
              (((this->lw_from_vspm(
                    (this->load_store_addr - this->BLOCK_VRF_OFFSET) >> 2)) >>
                15) &
               0b1)
                  ? (0xFFFFFF00 |
                     (((this->lw_from_vspm(
                           (this->load_store_addr - this->BLOCK_VRF_OFFSET) >>
                           2)) >>
                       8) &
                      0xFF))
                  : (((this->lw_from_vspm(
                          (this->load_store_addr - this->BLOCK_VRF_OFFSET) >>
                          2)) >>
                      8) &
                     0xFF);
          break;
        }
        case 0b10: {
          this->cpuregs[this->rd] =
              (((this->lw_from_vspm(
                    (this->load_store_addr - this->BLOCK_VRF_OFFSET) >> 2)) >>
                23) &
               0b1)
                  ? (0xFFFFFF00 |
                     (((this->lw_from_vspm(
                           (this->load_store_addr - this->BLOCK_VRF_OFFSET) >>
                           2)) >>
                       16) &
                      0xFF))
                  : (((this->lw_from_vspm(
                          (this->load_store_addr - this->BLOCK_VRF_OFFSET) >>
                          2)) >>
                      16) &
                     0xFF);
          break;
        }
        case 0b11: {
          this->cpuregs[this->rd] =
              (((this->lw_from_vspm(
                    (this->load_store_addr - this->BLOCK_VRF_OFFSET) >> 2)) >>
                31) &
               0b1)
                  ? (0xFFFFFF00 |
                     (((this->lw_from_vspm(
                           (this->load_store_addr - this->BLOCK_VRF_OFFSET) >>
                           2)) >>
                       24) &
                      0xFF))
                  : (((this->lw_from_vspm(
                          (this->load_store_addr - this->BLOCK_VRF_OFFSET) >>
                          2)) >>
                      24) &
                     0xFF);
          break;
        }
        }
        this->instr_name = (char *)"lb";
        break;
      }
      case 0b1: {
        switch ((this->load_store_addr - this->BLOCK_VRF_OFFSET) & 0b11) {
        case 0b0: {
          // printf("%d",(this->load_store_addr - 0x100000) >> 2);
          this->cpuregs[this->rd] =
              (((this->lw_from_vspm(
                    (this->load_store_addr - this->BLOCK_VRF_OFFSET) >> 2)) >>
                15) &
               0b1)
                  ? (0xFFFF0000 |
                     (this->lw_from_vspm(
                          (this->load_store_addr - this->BLOCK_VRF_OFFSET) >>
                          2) &
                      0xFFFF))
                  : (this->lw_from_vspm(
                         (this->load_store_addr - this->BLOCK_VRF_OFFSET) >>
                         2) &
                     0xFFFF);
          break;
        }
        case 0b10: {
          this->cpuregs[this->rd] =
              (((this->lw_from_vspm(
                    (this->load_store_addr - this->BLOCK_VRF_OFFSET) >> 2)) >>
                31) &
               0b1)
                  ? (0xFFFF0000 |
                     ((this->lw_from_vspm(
                           (this->load_store_addr - this->BLOCK_VRF_OFFSET) >>
                           2) >>
                       16) &
                      0xFFFF))
                  : (((this->lw_from_vspm(
                          (this->load_store_addr - this->BLOCK_VRF_OFFSET) >>
                          2)) >>
                      16) &
                     0xFFFF);
          break;
        }
        }
        this->instr_name = (char *)"lh";
        break;
      }
      case 0b10: {
        this->cpuregs[this->rd] = ((this->lw_from_vspm(
            (this->load_store_addr - this->BLOCK_VRF_OFFSET) >> 2)));
        this->instr_name = (char *)"lw";
        break;
      }
      case 0b100: {
        switch (this->load_store_addr & 0b11) {
        case 0b0: {
          this->cpuregs[this->rd] =
              ((this->lw_from_vspm(
                   (this->load_store_addr - this->BLOCK_VRF_OFFSET) >> 2)) &
               0xFF);
          break;
        }
        case 0b1: {
          this->cpuregs[this->rd] =
              (((this->lw_from_vspm(
                    (this->load_store_addr - this->BLOCK_VRF_OFFSET) >> 2)) >>
                8) &
               0xFF);
          break;
        }
        case 0b10: {
          this->cpuregs[this->rd] =
              (((this->lw_from_vspm(
                    (this->load_store_addr - this->BLOCK_VRF_OFFSET) >> 2)) >>
                16) &
               0xFF);
          break;
        }
        case 0b11: {
          this->cpuregs[this->rd] =
              (((this->lw_from_vspm(
                    (this->load_store_addr - this->BLOCK_VRF_OFFSET) >> 2)) >>
                24) &
               0xFF);
          break;
        }
        }
        this->instr_name = (char *)"lbu";
        break;
      }
      case 0b101: {
        switch (this->load_store_addr & 0b11) {
        case 0b0: {
          this->cpuregs[this->rd] =
              ((this->lw_from_vspm(
                   (this->load_store_addr - this->BLOCK_VRF_OFFSET) >> 2)) &
               0xFFFF);
          break;
        }
        case 0b10: {
          this->cpuregs[this->rd] =
              (((this->lw_from_vspm(
                    (this->load_store_addr - this->BLOCK_VRF_OFFSET) >> 2)) >>
                16) &
               0xFFFF);
          break;
        }
        }
        this->instr_name = (char *)"lhu";
        break;
      }
      }
      goto out;
    }
  }
  if (this->rd != 0) {
    switch ((this->instr >> 12) & 0b111) {
    case 0b0: {
      switch (this->load_store_addr & 0b11) {
      case 0b0: {
        this->cpuregs[this->rd] =
            (((this->sram[this->load_store_addr >> 2]) >> 7) & 0b1)
                ? (0xFFFFFF00 |
                   ((this->sram[this->load_store_addr >> 2]) & 0xFF))
                : ((this->sram[this->load_store_addr >> 2]) & 0xFF);
        break;
      }
      case 0b1: {
        this->cpuregs[this->rd] =
            (((this->sram[this->load_store_addr >> 2]) >> 15) & 0b1)
                ? (0xFFFFFF00 |
                   (((this->sram[this->load_store_addr >> 2]) >> 8) & 0xFF))
                : (((this->sram[this->load_store_addr >> 2]) >> 8) & 0xFF);
        break;
      }
      case 0b10: {
        this->cpuregs[this->rd] =
            (((this->sram[this->load_store_addr >> 2]) >> 23) & 0b1)
                ? (0xFFFFFF00 |
                   (((this->sram[this->load_store_addr >> 2]) >> 16) & 0xFF))
                : (((this->sram[this->load_store_addr >> 2]) >> 16) & 0xFF);
        break;
      }
      case 0b11: {
        this->cpuregs[this->rd] =
            (((this->sram[this->load_store_addr >> 2]) >> 31) & 0b1)
                ? (0xFFFFFF00 |
                   (((this->sram[this->load_store_addr >> 2]) >> 24) & 0xFF))
                : (((this->sram[this->load_store_addr >> 2]) >> 24) & 0xFF);
        break;
      }
      }
      this->instr_name = (char *)"lb";
      break;
    }
    case 0b1: {
      switch (this->load_store_addr & 0b11) {
      case 0b0: {
        this->cpuregs[this->rd] =
            (((this->sram[this->load_store_addr >> 2]) >> 15) & 0b1)
                ? (0xFFFF0000 |
                   ((this->sram[this->load_store_addr >> 2]) & 0xFFFF))
                : ((this->sram[this->load_store_addr >> 2]) & 0xFFFF);
        break;
      }
      case 0b10: {
        this->cpuregs[this->rd] =
            (((this->sram[this->load_store_addr >> 2]) >> 31) & 0b1)
                ? (0xFFFF0000 |
                   (((this->sram[this->load_store_addr >> 2]) >> 16) & 0xFFFF))
                : (((this->sram[this->load_store_addr >> 2]) >> 16) & 0xFFFF);
        break;
      }
      }
      this->instr_name = (char *)"lh";
      break;
    }
    case 0b10: {
      this->cpuregs[this->rd] = ((this->sram[this->load_store_addr >> 2]));
      this->instr_name = (char *)"lw";
      break;
    }
    case 0b100: {
      switch (this->load_store_addr & 0b11) {
      case 0b0: {
        this->cpuregs[this->rd] =
            ((this->sram[this->load_store_addr >> 2]) & 0xFF);
        break;
      }
      case 0b1: {
        this->cpuregs[this->rd] =
            (((this->sram[this->load_store_addr >> 2]) >> 8) & 0xFF);
        break;
      }
      case 0b10: {
        this->cpuregs[this->rd] =
            (((this->sram[this->load_store_addr >> 2]) >> 16) & 0xFF);
        break;
      }
      case 0b11: {
        this->cpuregs[this->rd] =
            (((this->sram[this->load_store_addr >> 2]) >> 24) & 0xFF);
        break;
      }
      }
      this->instr_name = (char *)"lbu";
      break;
    }
    case 0b101: {
      switch (this->load_store_addr & 0b11) {
      case 0b0: {
        this->cpuregs[this->rd] =
            ((this->sram[this->load_store_addr >> 2]) & 0xFFFF);
        break;
      }
      case 0b10: {
        this->cpuregs[this->rd] =
            (((this->sram[this->load_store_addr >> 2]) >> 16) & 0xFFFF);
        break;
      }
      }
      this->instr_name = (char *)"lhu";
      break;
    }
    }
  }
out:
  this->next_pc = this->pc + 4;
}

char *Venus_Emulator::venus_funct3_str(FUNC3 funct3) {
  switch (funct3) {
  case IVV:
    return (char *)"IVV";
  case IVX:
    return (char *)"IVX";
  case IVI:
    return (char *)"IVI";
  case MVV:
    return (char *)"MVV";
  case MVI:
    return (char *)"MVI";
  case nop:
    return (char *)"nop";
  case OPMISC:
    return (char *)"OPMISC";
  }
}

char *Venus_Emulator::venus_vew_str(VEW vew) {
  switch (vew) {
  case EW8:
    return (char *)"EW8";
  case EW16:
    return (char *)"EW16";
  }
}
