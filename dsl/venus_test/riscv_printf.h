#ifndef __RISCV_PRINTF_H__
#define __RISCV_PRINTF_H__

#include "venus.h"

#define BARRIER()       asm volatile ("vns_barrier")
#define VSPM_OPEN()     *(volatile unsigned int*)(0x801ff004) = 0x00000001
#define VSPM_CLOSE()    *(volatile unsigned int*)(0x801ff004) = 0x00000000
#define EBREAK()        asm volatile ("ebreak")

#define VENUS_PRINTVEC_SHORT(name, len) \
    do { \
        printf(STR(name)"\n", NULL); \
        short array_##name[len]; \
        int vecaddr_##name = vaddr(name); \
        VSPM_OPEN(); \
        BARRIER(); \
        for (int i = 0; i < len; i++) { \
            array_##name[i] = *(volatile unsigned short*)(vecaddr_##name + (i<<1)); \
            printf("%hd\n", &array_##name[i]); \
        } \
        VSPM_CLOSE(); \
        printf("________________________\n", NULL); \
    } while(0)
    
#define VENUS_PRINTVEC_CHAR(name, len) \
    do { \
        printf(STR(name)"\n", NULL); \
        char array_##name[len]; \
        int vecaddr_##name = vaddr(name); \
        VSPM_OPEN(); \
        BARRIER(); \
        for (int i = 0; i < len; i++) { \
            array_##name[i] = *(volatile unsigned char*)(vecaddr_##name + i); \
            printf("%bd\n", &array_##name[i]); \
        } \
        VSPM_CLOSE(); \
        printf("________________________\n", NULL); \
    } while(0)

/* <stdint.h> */
typedef unsigned char uint8_t;
typedef unsigned short uint16_t;
typedef unsigned int uint32_t;
typedef unsigned long long uint64_t;

/* <stddef.h> */
#define NULL ((void*)0)
typedef unsigned int size_t;

/* <stdbool.h> */
#define true 1
#define false 0

/* RISCV32: register is 32bits width */
typedef uint32_t reg_t;

#define BLOCK_CTRLREGS  0x801ff000UL
#define BLOCK_DEBUGUART 0x801fff00UL
#define UART_FIFO_DEPTH 60
#define UART_TFL_OFFSET       0x80  // Transmit FIFO Level
#define UART_THR_OFFSET       0x00 

/* block physical interface */
#define READ_BURST_64(base, offset)         (*(volatile uint64_t*)((base) + (offset)))
#define WRITE_BURST_64(base, offset, value) (*(volatile uint64_t*)((base) + (offset)) = (value))
#define READ_BURST_32(base, offset)         (*(volatile uint32_t*)((base) + (offset)))
#define WRITE_BURST_32(base, offset, value) (*(volatile uint32_t*)((base) + (offset)) = (value))
#define READ_BURST_16(base, offset)         (*(volatile uint16_t*)((base) + (offset)))
#define WRITE_BURST_16(base, offset, value) (*(volatile uint16_t*)((base) + (offset)) = (value))


// #define REG_L	lw
// #define REG_S	sw
// #define SZREG	4
// /* void *memcpy(void *, const void *, size_t) */
// ENTRY(memcpy)
// 	move t6, a0  /* Preserve return value */

// 	/* Defer to byte-oriented copy for small sizes */
// 	sltiu a3, a2, 128
// 	bnez a3, 4f
// 	/* Use word-oriented copy only if low-order bits match */
// 	andi a3, t6, SZREG-1
// 	andi a4, a1, SZREG-1
// 	bne a3, a4, 4f

// 	beqz a3, 2f  /* Skip if already aligned */
// 	/*
// 	 * Round to nearest double word-aligned address
// 	 * greater than or equal to start address
// 	 */
// 	andi a3, a1, ~(SZREG-1)
// 	addi a3, a3, SZREG
// 	/* Handle initial misalignment */
// 	sub a4, a3, a1
// 1:
// 	lb a5, 0(a1)
// 	addi a1, a1, 1
// 	sb a5, 0(t6)
// 	addi t6, t6, 1
// 	bltu a1, a3, 1b
// 	sub a2, a2, a4  /* Update count */

// 2:
// 	andi a4, a2, ~((16*SZREG)-1)
// 	beqz a4, 4f
// 	add a3, a1, a4
// 3:
// 	REG_L a4,       0(a1)
// 	REG_L a5,   SZREG(a1)
// 	REG_L a6, 2*SZREG(a1)
// 	REG_L a7, 3*SZREG(a1)
// 	REG_L t0, 4*SZREG(a1)
// 	REG_L t1, 5*SZREG(a1)
// 	REG_L t2, 6*SZREG(a1)
// 	REG_L t3, 7*SZREG(a1)
// 	REG_L t4, 8*SZREG(a1)
// 	REG_L t5, 9*SZREG(a1)
// 	REG_S a4,       0(t6)
// 	REG_S a5,   SZREG(t6)
// 	REG_S a6, 2*SZREG(t6)
// 	REG_S a7, 3*SZREG(t6)
// 	REG_S t0, 4*SZREG(t6)
// 	REG_S t1, 5*SZREG(t6)
// 	REG_S t2, 6*SZREG(t6)
// 	REG_S t3, 7*SZREG(t6)
// 	REG_S t4, 8*SZREG(t6)
// 	REG_S t5, 9*SZREG(t6)
// 	REG_L a4, 10*SZREG(a1)
// 	REG_L a5, 11*SZREG(a1)
// 	REG_L a6, 12*SZREG(a1)
// 	REG_L a7, 13*SZREG(a1)
// 	REG_L t0, 14*SZREG(a1)
// 	REG_L t1, 15*SZREG(a1)
// 	addi a1, a1, 16*SZREG
// 	REG_S a4, 10*SZREG(t6)
// 	REG_S a5, 11*SZREG(t6)
// 	REG_S a6, 12*SZREG(t6)
// 	REG_S a7, 13*SZREG(t6)
// 	REG_S t0, 14*SZREG(t6)
// 	REG_S t1, 15*SZREG(t6)
// 	addi t6, t6, 16*SZREG
// 	bltu a1, a3, 3b
// 	andi a2, a2, (16*SZREG)-1  /* Update count */

// 4:
// 	/* Handle trailing misalignment */
// 	beqz a2, 6f
// 	add a3, a1, a2

// 	/* Use word-oriented copy if co-aligned to word boundary */
// 	or a5, a1, t6
// 	or a5, a5, a3
// 	andi a5, a5, 3
// 	bnez a5, 5f
// 7:
// 	lw a4, 0(a1)
// 	addi a1, a1, 4
// 	sw a4, 0(t6)
// 	addi t6, t6, 4
// 	bltu a1, a3, 7b

// 	ret

// 5:
// 	lb a4, 0(a1)
// 	addi a1, a1, 1
// 	sb a4, 0(t6)
// 	addi t6, t6, 1
// 	bltu a1, a3, 5b
// 6:
// 	ret
// END(memcpy)

/* Function: Transmit a byte */
VENUS_INLINE void uart_putc(char ch) {
  // while (READ_BURST_32(BLOCK_DEBUGUART, UART_TFL_OFFSET) > UART_FIFO_DEPTH)
  //   ;
  // WRITE_BURST_32(BLOCK_DEBUGUART, UART_THR_OFFSET, (uint32_t)ch);
  *((volatile int*)0x10000000) = ch;
}

/* Function: Transmit a string */
VENUS_INLINE void uart_puts(char* s) {
  while (*s) {
    uart_putc(*s++);
  }
}

VENUS_INLINE int _vsnprintf(char* out, size_t n, const char* s, void *input) {
  int format  = 0;
  int longarg = 0;
  int bytes = 4;
  size_t pos  = 0;
  for (; *s; s++) {
    if (format) {
      switch (*s) {
      case 'p': {
        longarg = 1;
        if (out && pos < n) {
          out[pos] = '0';
        }
        pos++;
        if (out && pos < n) {
          out[pos] = 'x';
        }
        pos++;
      }
      case 'x': {
        int hexdigits = 2 * sizeof(long) - 1;
        long num;
        switch (bytes) {
            case 1: num = *((char *)input); break;
            case 2: num = *((short *)input); break;
            case 4: num = *((int *)input); break;
        }
        for (int i = hexdigits; i >= 0; i--) {
            int d = (num >> (4 * i)) & 0xF;
            if (out && pos < n) {
                out[pos] = (d < 10 ? '0' + d : 'a' + d - 10);
            }
            pos++;
        }
        longarg = 0;
        format  = 0;
        break;
      }
      case 'b': {
        bytes = 1;
        break;
      }
      case 'h': {
        bytes = 2;
        break;
      }
      case 'd': {
        long num;
        switch (bytes) {
            case 1: num = *((char *)input); break;
            case 2: num = *((short *)input); break;
            case 4: num = *((int *)input); break;
        }
        if (num < 0) {
          num = -num;
          if (out && pos < n) {
            out[pos] = '-';
          }
          pos++;
        }
        long digits = 1;
        for (long nn = num; nn /= 10; digits++)
          ;
        for (int i = digits - 1; i >= 0; i--) {
          if (out && pos + i < n) {
            out[pos + i] = '0' + (num % 10);
          }
          num /= 10;
        }
        pos += digits;
        longarg = 0;
        format  = 0;
        break;
      }
      case 's': {
        const char* s2 = (const char*)input;
        while (*s2) {
          if (out && pos < n) {
            out[pos] = *s2;
          }
          pos++;
          s2++;
        }
        longarg = 0;
        format  = 0;
        break;
      }
      case 'c': {
        if (out && pos < n) {
          out[pos] = *((char *)input);
        }
        pos++;
        longarg = 0;
        format  = 0;
        break;
      }
      default:
        break;
      }
    } else if (*s == '%') {
      format = 1;
    } else {
      if (out && pos < n) {
        out[pos] = *s;
      }
      pos++;
    }
  }
  if (out && pos < n) {
    out[pos] = 0;
  } else if (out && n) {
    out[n - 1] = 0;
  }
  return pos;
}

static char out_buf[1000] = {0};

VENUS_INLINE int _vprintf(const char* s, void *input) {
  int res = _vsnprintf(NULL, -1, s, input);
  if (res + 1 >= sizeof(out_buf)) {
    uart_puts("error: output string size overflow\n");
    while (1) {}
  }
  _vsnprintf(out_buf, res + 1, s, input);
  uart_puts(out_buf);
  return res;
}

// use addresses (& or *) as the input
// support %p, %s, %d, %x only
// for short integer: use %hd
// for char integer: use %bd
VENUS_INLINE int printf(const char* s, void *input) {
  int res = 0;
  res = _vprintf(s, input);
  return res;
}

#endif