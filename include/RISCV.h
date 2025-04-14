#ifndef __RISCV_H__
#define __RISCV_H__

#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdarg.h>
#include <climits>
#include <iostream>
#include <bitset>

namespace RISCV
{
    const int REGNUM = 36;
    extern const char *REGNAME[32];
} // namespace RISCV

namespace CPU
{
    const int SRAMSIZE = 0x100000;
}

class Emulator
{
public:
    Emulator();
    ~Emulator();
    uint32_t PROGADDR_RESET = 0x00000000;
    uint32_t PROGADDR_IRQ = 0x00000010;
    uint32_t PRINTF_ADDR = 0x10000000;
    uint32_t TEST_PRINTF_ADDR = 0x20000000;

    unsigned BUS_WIDTH = 64; // In bytes

    uint32_t BLOCK_PERSPECTIVE_BASEADDR = 0x80000000;
    uint32_t TASK_STRUCT_OFFSET = 0xA000;
    uint32_t BLOCK_DSPM_OFFSET = 0x20000;
    uint32_t TEXT_SECTION_LENGTH = 3968;
    uint32_t DATA_SECTION_OFFSET = 3968;
    uint32_t DATA_SECTION_LENGTH = 5952;
    uint32_t BLOCK_VRF_OFFSET = 0x100000;
    uint32_t BLOCK_SRF_OFFSET = 0x020000;
    uint32_t BLOCK_CSR_OFFSET = 0x1FF000;
    uint32_t BLOCK_CSR_NUMRET_OFFSET = 0x2C;
    uint32_t BLOCK_CSR_RET0ADDR_OFFSET = 0x30;
    uint32_t BLOCK_CSR_RET0LEN_OFFSET = 0x34;

    uint32_t CLUSTER_PERSPECTIVE_BASEADDR = 0x80000000;
    uint32_t CLUSTER_BLOCKS_OFFSET = 0x2000000;

    bool verbose;
    bool venus_verbose;
    bool dsl_mode;
    bool compare_result;
    bool trap;
    bool ebreak;
    uint32_t irq;

    bool irq_active;
    bool eoi;

    uint32_t pc;
    uint32_t cpuregs[RISCV::REGNUM];
    uint32_t sram[CPU::SRAMSIZE];

    uint32_t next_pc;
    uint32_t instr;
    char *instr_name;
    uint32_t load_store_addr;

    uint32_t jump_offset;

    uint32_t trap_cycle;

    uint8_t rd;
    uint8_t rs1;
    uint8_t rs2;
    int branch_offset;
    uint32_t irq_mask;
    uint32_t timer;

    struct Counter
    {
        uint64_t instr_count;
        uint64_t cycle_count;
    } counter;

    struct TaskInfo
    {
        char *debug_task_name;
    } taskinfo;

    char regname[32][6] = {"zero", "ra", "sp", "gp", "tp",
                           "t0", "t1", "t2", "s0", "s1",
                           "a0", "a1", "a2", "a3", "a4",
                           "a5", "a6", "a7", "s2", "s3",
                           "s4", "s5", "s6", "s7", "s8",
                           "s9", "s10", "s11", "t3", "t4",
                           "t5", "t6"};

    void init_emulator(char *firmware_name);
    virtual void init_param() = 0;
    void emulate();

    // private:

    void decode_lui();
    void decode_auipc();
    void decode_jal();
    void decode_jalr();
    void decode_branch();
    void decode_load();
    void decode_store();
    void decode_arthimetic_imm();
    void decode_arthimetic_reg();
    void decode_RV32M();
    void decode_IRQ();

    void panic(const char *format, ...);
    int integerDivision(int dividend, int divisor);
    uint32_t unsignedDivision(uint32_t dividend, uint32_t divisor);
    int remainderDivision(int dividend, int divisor);
    int signed_sim(uint32_t reg_range);

    char *getRISCVRegABI(int id);
};

#endif