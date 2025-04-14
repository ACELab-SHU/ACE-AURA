#include "RISCV.h"
using namespace CPU;
using namespace std;

Emulator::Emulator() {
  this->verbose = true;
  this->trap_cycle = 0;
  // irq
  this->irq_mask = 0xffffffff;
  this->irq = 0;
  this->pc = 0;
  this->irq_active = false;
  printf("Simulation Start!\n");
  // cout << "Simulation Start!\n" << endl;
  cout << endl;
  // initialize sram
  for (int i = 0; i < CPU::SRAMSIZE; ++i) {
    this->sram[i] = 0;
  }
}

Emulator::~Emulator() {}

void Emulator::init_emulator(char *firmware_name) {
  // initialize PC
  this->pc = this->PROGADDR_RESET;
  this->irq = 0;
  this->trap = false;
  this->ebreak = false;
  printf("The first PC address is %08x\n", this->pc);
  // get instructions
  FILE *fp;
  std::cout << firmware_name << endl;
  if ((fp = fopen(firmware_name, "r+")) == NULL) {
    printf("file open failed!\n");
  } else {
    printf("file opened successfully\n");
  }
  cout << endl;
  size_t arraySize = 0;
  char line[256]; // assuming each line does not exceed 256 characters
  printf("before text and data movement 0x210c0 val is: %d\n",
         this->sram[33840]);
  while (fgets(line, sizeof(line), fp) != NULL) {
    uint32_t value;

    // remove line breaks at the end of a line
    size_t length = strlen(line);
    if (length > 0 && line[length - 1] == '\n') {
      line[length - 1] = '\0';
    }

    // attempting to convert text to uint32 and store text section
    if ((sscanf(line, "%x", &value) == 1) &&
        (arraySize <= (Emulator::TEXT_SECTION_LENGTH / 4) - 1)) {
      this->sram[arraySize] = value;
      // if (value != 0)
      cout << "the " << arraySize << "th row value is "
          << std::hex << this->sram[arraySize] << std::endl;
      // printf("the %dth row value is %X\n", arraySize, this->sram[arraySize]);
      // printf("%d\n", arraySize);
    } else if (sscanf(line, "%x", &value) != 1) {
      printf("Failed to convert line to uint32: %s\n", line);
    }
    // attempting to convert text to uint32 and store text section
    if ((sscanf(line, "%x", &value) == 1) &&
        ((arraySize >= (Emulator::DATA_SECTION_OFFSET / 4)) &&
         (arraySize <=
          ((Emulator::DATA_SECTION_LENGTH + Emulator::DATA_SECTION_OFFSET) /
           4)))) {
      this->sram[(Emulator::BLOCK_DSPM_OFFSET / 4) +
                 (arraySize - (Emulator::DATA_SECTION_OFFSET / 4))] = value;
      // if (value != 0)
      //   printf("the %dth row value is %d\n", arraySize,
      //   this->sram[(Emulator::BLOCK_DSPM_OFFSET / 4) + (arraySize -
      //   (Emulator::DATA_SECTION_OFFSET / 4))]);
      // // printf("test array: %d.\n", this->sram[(BLOCK_DSPM_OFFSET / 4) +
      // (arraySize - Emulator::DATA_SECTION_OFFSET)]);
      // // printf("arraysize: %d\n", arraySize);
      // // printf("sram id: %d\n", (BLOCK_DSPM_OFFSET / 4) + (arraySize -
      // Emulator::DATA_SECTION_OFFSET)); if(value != 0)
      //   printf("%d\n",value);
    } else if ((sscanf(line, "%x", &value) != 1)) {
      printf("Failed to convert line to uint32: %s\n", line);
    }

    if (arraySize >= CPU::SRAMSIZE) {
      printf("Array is full, stopping reading.\n");
      break;
    }
    arraySize++;
  }
  printf("before text and data movement 51th sram val is: %d\n",
         this->sram[51]);
  printf("before text and data movement 0x210c0 val is: %d\n",
         this->sram[33840]);

  // while (fgets(line, sizeof(line), fp) != NULL)
  // {
  //   uint32_t value;

  //   // remove line breaks at the end of a line
  //   size_t length = strlen(line);
  //   if (length > 0 && line[length - 1] == '\n')
  //   {
  //     line[length - 1] = '\0';
  //   }

  //   // attempting to convert text to uint32
  //   if (sscanf(line, "%x", &value) == 1)
  //   {
  //     this->sram[arraySize++] = value;
  //     // if(value != 0)
  //     //   printf("%d\n",value);
  //   }
  //   else
  //   {
  //     printf("Failed to convert line to uint32: %s\n", line);
  //   }

  //   if (arraySize >= CPU::SRAMSIZE)
  //   {
  //     printf("Array is full, stopping reading.\n");
  //     break;
  //   }
  // }
  fclose(fp);
}

void Emulator::emulate() {
  bool instruction_valid = false;
  this->instr = this->sram[this->pc >> 2];
  this->rd = (this->instr >> 7) & 0x1F;
  this->rs1 = (this->instr >> 15) & 0x1F;
  this->rs2 = (this->instr >> 20) & 0x1F;

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
  // == 0xc02024f3 ||
  //     this->instr == 0x0800400b|| this->instr == 0x0600600b  || this->instr
  //     == 0xc0202573) { // irq instruction, for test
  //   instruction_valid = true;
  //   this->next_pc = this->pc + 4; // irq instruction, for test
  // }
  if (this->instr == 0x00100073) {
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
    case 0b1011011: {
      instruction_valid = true;
      break;
    }
    }
  }
  if (((this->instr & 0x7F) == 0b1110011 &&
       ((this->instr & 0xfffff000) == 0b11000000000000000010000000000000)) ||
      (((this->instr & 0x7F) == 0b1110011) &&
       ((this->instr & 0xfffff000) ==
        0b11000000000100000010000000000000))) { // instr_rdcycle
    instruction_valid = true;
    this->cpuregs[this->rd] = this->counter.cycle_count & 0xFFFFFFFF;
    this->instr_name = (char *)"rdcycle";
    this->next_pc = this->pc + 4;
    // printf("this->counter.cycle_count: %08x\n", this->counter.cycle_count);
  } else if (((((this->instr & 0x7F) == 0b1110011) &&
               ((this->instr & 0xfffff000) ==
                0b11000000000000000010000000000000)) ||
              (((this->instr & 0x7F) == 0b1110011) &&
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
  } else if (((this->instr & 0x7F) == 0b1110011) &&
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
  if ((this->next_pc & 0b11) != 0) {
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
  // update pc
  this->pc = this->next_pc;
}

inline void Emulator::decode_lui() {
  if (this->rd != 0)
    this->cpuregs[this->rd] = this->instr & 0xFFFFF000;
  this->next_pc = this->pc + 4;
  this->instr_name = (char *)"lui";
}

inline void Emulator::decode_auipc() {
  if (this->rd != 0)
    this->cpuregs[this->rd] = (this->instr & 0xFFFFF000) + this->pc;
  this->next_pc = this->pc + 4;
  this->instr_name = (char *)"auipc";
}

inline void Emulator::decode_jal() {
  // this->jump_offset = (((this->instr >> 12) & 0xFF) << 12) | (((this->instr
  // >> 20) & 0b1) << 11) | (((this->instr >> 21) & 0x3FF) << 1) |
  // (((this->instr >> 31) & 0b1) << 20);
  this->jump_offset = (((this->instr >> 31) & 0b1) << 20) |
                      (((this->instr >> 21) & 0x3FF) << 1) |
                      (((this->instr >> 20) & 0b1) << 11) |
                      (((this->instr >> 12) & 0xFF) << 12);
  this->jump_offset = ((this->instr >> 31) & 0b1) == 0
                          ? this->jump_offset
                          : this->jump_offset | 0xFFE00000;
  this->next_pc = this->pc + this->jump_offset;
  if (this->rd != 0)
    this->cpuregs[this->rd] = this->pc + 4;
  this->instr_name = (char *)"jal";
}

inline void Emulator::decode_jalr() {
  this->jump_offset = (this->instr >> 31) == 0
                          ? (this->instr >> 20)
                          : ((this->instr >> 20) | 0xFFFFF000);
  this->next_pc =
      this->cpuregs[this->rs1] + this->signed_sim(this->jump_offset);
  if (this->rd != 0)
    this->cpuregs[this->rd] = this->pc + 4;
  this->instr_name = (char *)"jalr";
}

inline void Emulator::decode_branch() {
  uint32_t op1 = this->cpuregs[this->rs1];
  uint32_t op2 = this->cpuregs[this->rs2];
  int offset = this->signed_sim(this->branch_offset);
  switch ((this->instr >> 12) & 0b111) {
  case 0b0: {
    if (this->signed_sim(op1) == this->signed_sim(op2))
      this->next_pc = this->pc + offset;
    else
      this->next_pc = this->pc + 4;
    this->instr_name = (char *)"beq";
    // printf("beq rs1: %d, rs2: %d\n", this->signed_sim(op1),
    // this->signed_sim(op2));
    break;
  }
  case 0b1: {
    if (this->signed_sim(op1) == this->signed_sim(op2))
      this->next_pc = this->pc + 4;
    else
      this->next_pc = this->pc + offset;
    this->instr_name = (char *)"bne";
    break;
  }
  case 0b100: {
    if (this->signed_sim(op1) < this->signed_sim(op2))
      this->next_pc = this->pc + offset;
    else
      this->next_pc = this->pc + 4;
    this->instr_name = (char *)"blt";
    break;
  }
  case 0b101: {
    if (this->signed_sim(op1) >= this->signed_sim(op2))
      this->next_pc = this->pc + offset;
    else
      this->next_pc = this->pc + 4;
    this->instr_name = (char *)"bge";
    break;
  }
  case 0b110: {
    if ((op1) < (op2))
      this->next_pc = this->pc + offset;
    else
      this->next_pc = this->pc + 4;
    this->instr_name = (char *)"bltu";
    break;
  }
  case 0b111: {
    if ((op1) >= (op2))
      this->next_pc = this->pc + offset;
    else
      this->next_pc = this->pc + 4;
    this->instr_name = (char *)"bgeu";
    break;
  }
  }
}

inline void Emulator::decode_load() {
  this->load_store_addr =
      this->cpuregs[this->rs1] +
      this->signed_sim((this->instr >> 31) == 0
                           ? (this->instr >> 20)
                           : ((this->instr >> 20) | 0xFFFFF000));
  if (this->load_store_addr > 0x80000000)
    this->load_store_addr = this->load_store_addr - 0x80000000;
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
  this->next_pc = this->pc + 4;
}

inline void Emulator::decode_store() {
  this->load_store_addr =
      this->cpuregs[this->rs1] +
      ((((this->instr >> 31) == 0) ? 0 : 0xFFFFF000) |
       (((this->instr >> 7) & 0x1F) + (((this->instr >> 25) & 0x7F) << 5)));
  if (this->load_store_addr > 0x80000000)
    this->load_store_addr = this->load_store_addr - 0x80000000;
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
    // printf("scalar printf: %c", store_data);
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
  this->next_pc = this->pc + 4;
}

inline void Emulator::decode_arthimetic_imm() {
  if (this->rd != 0) {
    switch ((this->instr >> 12) & 0b111) {
    case 0b0: {
      this->cpuregs[this->rd] =
          (this->cpuregs[this->rs1]) + (((this->instr >> 31) == 0)
                                            ? this->instr >> 20
                                            : (this->instr >> 20) | 0xFFFFF000);
      this->instr_name = (char *)"addi";
      break;
    }
    case 0b10: {
      this->cpuregs[this->rd] =
          this->signed_sim(this->cpuregs[this->rs1]) <
          this->signed_sim(((this->instr >> 31) == 0)
                               ? this->instr >> 20
                               : (this->instr >> 20) | 0xFFFFF000);
      this->instr_name = (char *)"slti";
      break;
    }
    case 0b11: {
      this->cpuregs[this->rd] =
          (this->cpuregs[this->rs1]) < ((this->instr >> 31) == 0
                                            ? this->instr >> 20
                                            : (this->instr >> 20) | 0xFFFFF000);
      this->instr_name = (char *)"sltiu";
      break;
    }
    case 0b100: {
      this->cpuregs[this->rd] =
          ((this->cpuregs[this->rs1]) ^
           (((this->instr >> 31) == 0) ? this->instr >> 20
                                       : (this->instr >> 20) | 0xFFFFF000));
      this->instr_name = (char *)"xori";
      break;
    }
    case 0b110: {
      this->cpuregs[this->rd] =
          (this->cpuregs[this->rs1]) |
          (((this->instr >> 31) == 0) ? this->instr >> 20
                                      : (this->instr >> 20) | 0xFFFFF000);
      this->instr_name = (char *)"ori";
      break;
    }
    case 0b111: {
      this->cpuregs[this->rd] =
          (this->cpuregs[this->rs1]) &
          (((this->instr >> 31) == 0) ? this->instr >> 20
                                      : (this->instr >> 20) | 0xFFFFF000);
      this->instr_name = (char *)"andi";
      break;
    }
    case 0b001: {
      this->cpuregs[this->rd] = this->signed_sim(this->cpuregs[this->rs1])
                                << ((this->instr >> 20) & 0x1F);
      this->instr_name = (char *)"slli";
      break;
    }
    case 0b101: {
      if (((this->instr >> 25) & 0x7F) == 0b0) {
        this->cpuregs[this->rd] =
            this->cpuregs[this->rs1] >> ((this->instr >> 20) & 0x1F);
        this->instr_name = (char *)"srli";
      } else if (((this->instr >> 25) & 0x7F) == 0b100000) {
        int srai_temp = this->signed_sim(this->cpuregs[this->rs1]);
        this->cpuregs[this->rd] = srai_temp >> ((this->instr >> 20) & 0x1F);
        this->instr_name = (char *)"srai";
      }
      break;
    }
    }
  }
  this->next_pc = this->pc + 4;
}

inline void Emulator::decode_arthimetic_reg() {
  if (this->rd != 0) {
    switch ((this->instr >> 12) & 0b111) {
    case 0b0: {
      if (((this->instr >> 25) & 0x7F) == 0b0) {
        this->cpuregs[this->rd] = this->signed_sim(this->cpuregs[this->rs1]) +
                                  this->signed_sim(this->cpuregs[this->rs2]);
        this->instr_name = (char *)"add";
      } else if (((this->instr >> 25) & 0x7F) == 0b0100000) {
        this->cpuregs[this->rd] = this->signed_sim(this->cpuregs[this->rs1]) -
                                  this->signed_sim(this->cpuregs[this->rs2]);
        this->instr_name = (char *)"sub";
      }
      break;
    }
    case 0b1: {
      this->cpuregs[this->rd] = this->signed_sim(this->cpuregs[this->rs1])
                                << (this->cpuregs[this->rs2] & 0x1F);
      this->instr_name = (char *)"sll";
      break;
    }
    case 0b10: {
      this->cpuregs[this->rd] = this->signed_sim(this->cpuregs[this->rs1]) <
                                this->signed_sim(this->cpuregs[this->rs2]);
      this->instr_name = (char *)"slt";
      break;
    }
    case 0b11: {
      this->cpuregs[this->rd] =
          (this->cpuregs[this->rs1]) < (this->cpuregs[this->rs2]);
      this->instr_name = (char *)"sltu";
      break;
    }
    case 0b100: {
      this->cpuregs[this->rd] =
          this->cpuregs[this->rs1] ^ this->cpuregs[this->rs2];
      this->instr_name = (char *)"xor";
      break;
    }
    case 0b110: {
      this->cpuregs[this->rd] =
          this->cpuregs[this->rs1] | this->cpuregs[this->rs2];
      this->instr_name = (char *)"or";
      break;
    }
    case 0b111: {
      this->cpuregs[this->rd] =
          this->cpuregs[this->rs1] & this->cpuregs[this->rs2];
      this->instr_name = (char *)"and";
      break;
    }
    case 0b101: {
      if (((this->instr >> 25) & 0x7F) == 0b0) {
        this->cpuregs[this->rd] =
            this->cpuregs[this->rs1] >> (this->cpuregs[this->rs2] & 0x1F);
        this->instr_name = (char *)"srl";
      } else if (((this->instr >> 25) & 0x7F) == 0b0100000) {
        int sra_temp = this->signed_sim(this->cpuregs[this->rs1]);
        this->cpuregs[this->rd] =
            sra_temp >> ((this->cpuregs[this->rs2] & 0x1F));
        this->instr_name = (char *)"sra";
      }
      break;
    }
    }
  }
  this->next_pc = this->pc + 4;
}

inline void Emulator::decode_RV32M() {
  if (this->rd != 0) {
    uint64_t reg_op1, reg_op2;
    switch ((this->instr >> 12) & 0b111) {
    case 0b0: {
      reg_op1 = this->signed_sim(this->cpuregs[this->rs1]);
      reg_op2 = this->signed_sim(this->cpuregs[this->rs2]);
      this->cpuregs[this->rd] = (reg_op1 * reg_op2) & 0xFFFFFFFF;
      this->instr_name = (char *)"mul";
      break;
    }
    case 0b1: {
      reg_op1 = this->signed_sim(this->cpuregs[this->rs1]);
      reg_op2 = this->signed_sim(this->cpuregs[this->rs2]);
      this->cpuregs[this->rd] = (((reg_op1 * reg_op2) >> 32) & 0xFFFFFFFF);
      this->instr_name = (char *)"mulh";
      break;
    }
    case 0b10: {
      reg_op1 = this->signed_sim(this->cpuregs[this->rs1]);
      reg_op2 = (this->cpuregs[this->rs2]);
      this->cpuregs[this->rd] = (((reg_op1 * reg_op2) >> 32) & 0xFFFFFFFF);
      this->instr_name = (char *)"mulhsu";
      break;
    }
    case 0b11: {
      reg_op1 = (this->cpuregs[this->rs1]);
      reg_op2 = (this->cpuregs[this->rs2]);
      this->cpuregs[this->rd] = (((reg_op1 * reg_op2) >> 32) & 0xFFFFFFFF);
      this->instr_name = (char *)"mulhu";
      break;
    }
    case 0b100: {
      int div_reg_op1 = this->signed_sim(this->cpuregs[this->rs1]);
      int div_reg_op2 = this->signed_sim(this->cpuregs[this->rs2]);
      if (div_reg_op2 == 0) {
        this->cpuregs[this->rd] = 0xFFFFFFFF;
      } else if (div_reg_op1 == (signed)(-0x80000000) && div_reg_op2 == -1) {
        // } else if (div_reg_op1 == -0x80000000 && div_reg_op2 == -1) {
        this->cpuregs[this->rd] = -0x80000000;
      } else {
        this->cpuregs[this->rd] = div_reg_op1 / div_reg_op2;
      }
      this->instr_name = (char *)"div";
      break;
    }
    case 0b101: {
      reg_op1 = (this->cpuregs[this->rs1]);
      reg_op2 = (this->cpuregs[this->rs2]);
      if (reg_op2 == 0) {
        this->cpuregs[this->rd] = 0xFFFFFFFF;
      } else {
        this->cpuregs[this->rd] = reg_op1 / reg_op2;
      }
      this->instr_name = (char *)"divu";
      break;
    }
    case 0b110: {
      int rem_op1 = this->signed_sim(this->cpuregs[this->rs1]);
      int rem_op2 = this->signed_sim(this->cpuregs[this->rs2]);
      if (rem_op2 == 0)
        this->cpuregs[this->rd] = rem_op1;
      // else if (rem_op1 == -0x80000000 && rem_op2 == -1)
      else if (rem_op1 == (signed)(-0x80000000) && rem_op2 == -1)
        this->cpuregs[this->rd] = 0;
      else
        this->cpuregs[this->rd] = rem_op1 % rem_op2;
      this->instr_name = (char *)"rem";
      break;
    }
    case 0b111: {
      reg_op1 = (this->cpuregs[this->rs1]);
      reg_op2 = (this->cpuregs[this->rs2]);
      if (reg_op2 == 0)
        this->cpuregs[this->rd] = (reg_op1);
      else
        this->cpuregs[this->rd] = reg_op1 % reg_op2;
      this->instr_name = (char *)"remu";
      break;
    }
    }
  }
  this->next_pc = this->pc + 4;
}

inline void Emulator::decode_IRQ() {
  switch ((this->instr >> 25) & 0x7F) {
  case 0b0000010: {
    this->next_pc = this->cpuregs[32];
    this->instr_name = (char *)"retirq";
    this->irq_active = false;
    break;
  }
  case 0b0000100: {
    this->next_pc = this->pc;
    this->instr_name = (char *)"waitirq";
    break;
  }
  case 0b0000000: {
    this->cpuregs[this->rd] = this->cpuregs[this->rs1];
    this->instr_name = (char *)"getq";
    break;
  }
  case 0b0000001: {
    this->rd = this->rd + 32;
    this->cpuregs[this->rd] = this->cpuregs[this->rs1];
    this->instr_name = (char *)"setq";
    break;
  }
  case 0b0000011: {
    this->cpuregs[this->rd] = this->irq_mask;
    this->irq_mask = this->cpuregs[this->rs1];
    this->instr_name = (char *)"maskirq";
    break;
  }
  case 0b0000101: {
    this->cpuregs[this->rd] = this->timer;
    this->timer = this->cpuregs[this->rs1];
    this->instr_name = (char *)"timer";
    break;
  }
  }
  this->next_pc = this->pc + 4;
}

// 32bits signed
int Emulator::signed_sim(uint32_t reg_range) {
  if (((reg_range >> 31) & 0b1) == 0) {
    return reg_range;
  } else {
    return -((reg_range ^ 0xFFFFFFFF) + 1);
  }
}

int Emulator::integerDivision(int dividend, int divisor) {
  if (divisor == 0) {
    printf("Division by zero.\n");
    return 0;
  }

  // determine the positivity and negativity of the result
  int sign = (dividend < 0) ^ (divisor < 0) ? -1 : 1;

  // calculate by turning both the dividend and the dividend into positive
  // numbers
  int udividend = abs(dividend);
  int udivisor = abs(divisor);

  int quotient = 0;
  while (udividend >= udivisor) {
    udividend -= udivisor;
    quotient++;
  }
  return sign * quotient;
}

uint32_t Emulator::unsignedDivision(uint32_t dividend, uint32_t divisor) {
  if (divisor == 0) {
    printf("Division by zero.\n");
    return 0;
  }

  uint32_t quotient = 0;
  while (dividend >= divisor) {
    dividend -= divisor;
    quotient++;
  }

  return quotient;
}

int Emulator::remainderDivision(int dividend, int divisor) {
  if (divisor == 0) {
    printf("Division by zero.\n");
    return 0;
  }

  // determine the positivity and negativity of the result
  int sign = dividend < 0 ? -1 : 1;

  // convert the dividend to a positive number
  int udividend = abs(dividend);
  int udivisor = abs(divisor);

  while (udividend >= udivisor) {
    udividend -= udivisor;
  }

  return sign * udividend;
}

char *Emulator::getRISCVRegABI(int id) { return regname[id]; }