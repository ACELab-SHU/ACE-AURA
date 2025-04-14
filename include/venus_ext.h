#include "RISCV.h"
#include <cmath>
#include <fstream>
#include <tuple>

using namespace std;

enum VEW { EW8, EW16 };
enum VMASK_READ { NORMAL_READ, READ_MASKED };
enum VMASK_WRITE { NORMAL_WRITE, WRITE_MASKED };
enum FUNC3 { IVV, IVX, IVI, MVV, MVX, MVI, nop, OPMISC };
enum OP_TYPE { // BitALU
  VAND,
  VOR,
  VXOR,
  VBRDCST,
  VSLL,
  VSRL,
  VSRA,
  VMNOT,
  VSEQ,
  VSNE,
  VSLTU,
  VSLT,
  VSLEU,
  VSLE,
  VSGTU,
  VSGT,
  // CAU
  VADD,
  VSADD,
  VSADDU,
  VRANGE,
  VRSUB,
  VSUB,
  VSSUB,
  VSSUBU,
  VMUL,
  VMULH,
  VMULHU,
  VMULHSU,
  VMULADD,
  VMULSUB,
  VADDMUL,
  VSUBMUL,
  VCMXMUL,
  VREDMIN,
  VREDSUM,
  VREDMAX,
  // SerDiv
  VDIV,
  VREM,
  VDIVU,
  VREMU,
  VSHUFFLE,
  VSHUFFLE_CLBMV,
  //
  VSETCSR,
  YIELD
};
class Venus_Emulator : public Emulator {
private:
  int test_cnt = 0;
#ifdef VENUSROW
  static const int VENUS_DSPM_ROW = VENUSROW;
#else
  static const int VENUS_DSPM_ROW = 2048;
#endif

#ifdef VENUSLANE
  static const int VENUS_DSPM_LANE = VENUSLANE;
#else
  static const int VENUS_DSPM_LANE = 32;
#endif

  static const int VENUS_DSPM_BANK_PER_LANE = 4;
  static const int VENUS_DSPM_BIT_PER_BANKROW = 16;
  int16_t VENUS_DSPM[VENUS_DSPM_ROW]
                    [VENUS_DSPM_LANE * VENUS_DSPM_BANK_PER_LANE];
  bool VENUS_MASK_DSPM[VENUS_DSPM_ROW * VENUS_DSPM_LANE *
                       VENUS_DSPM_BANK_PER_LANE];
  struct VENUS_INS_PARAM {
    uint16_t avl;
    VMASK_READ vmask_read;
    VMASK_WRITE vmask_write;
    VEW vew;
    FUNC3 function3;
    uint16_t op_code;
    uint16_t function5;
    uint16_t vd1_head;
    uint16_t vd2_head;
    uint16_t vs1_head;
    uint16_t vs2_head;
    int16_t scalar_op;
    OP_TYPE op;
    uint16_t vd1_msb;
    uint16_t vs1_msb;
    uint16_t vs2_msb;
    uint8_t vfu_shamt;
    uint8_t high_vd2_bits; // Current 3bits
    uint8_t high_vd1_bits; // Current 2bits
    uint8_t high_vs2_bits; // Current 2bits
    uint8_t high_vs1_bits; // Current 2bits
  } MY_VENUS_INS_PARAM;

public:
  int vins_issue_idx;
  ofstream dumpfile;
  struct TaskReturn {
    uint32_t length;
    uint32_t ptr;
    void *emu_ptr; // malloc'ed on emulator to put the calculated data
  } taskret[16][128];
  unsigned taskretnum[128];
  unsigned current_taskid;

  Venus_Emulator();
  ~Venus_Emulator();
  void decode_venus();
  void venus_execute();
  inline void decode_load();
  int32_t lw_from_vspm(int load_addr);
  int st_to_vspm(int store_addr, int store_value);
  int32_t lw_from_scalar_spm(int load_addr);
  int st_to_scalar_spm(int store_addr, int store_value);
  void decode_store();
  void emulate();
  void decode_venus_ext_msb();
  void decode_venus_ext_lsb();
  void dump_venus_ins();
  void dump_vins_result();
  int get_row_length(uint16_t avl);
  tuple<uint16_t, uint16_t, uint16_t>
  get_abs_addr_and_mask(uint16_t idx, uint16_t start_row, bool forced_ew16);
  tuple<uint16_t, uint16_t, uint16_t> get_abs_addr_and_mask(uint16_t abs_idx);
  char *venus_funct3_str(FUNC3 funct3);
  char *venus_vew_str(VEW vew);
  char *venus_optype_str(OP_TYPE op);
  void init_param() override {
    memset(&MY_VENUS_INS_PARAM, 0, sizeof(MY_VENUS_INS_PARAM));
  };
};