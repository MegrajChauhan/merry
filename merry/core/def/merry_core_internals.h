#ifndef _MERRY_INTERNALS_
#define _MERRY_INTERNALS_

#include <merry_helpers.h>
#include <merry_types.h>

typedef union MerryCoreFlagsRegr MerryCoreFlagsRegr;
typedef struct MerryCoreFFlagsRegr MerryCoreFFlagsRegr;
typedef struct MerryCoreStackFrame MerryCoreStackFrame;

struct MerryCoreFFlagsRegr {
  bit_group(zf, 1);  // zero flag
  bit_group(sf, 1);  // sign flag
  bit_group(uof, 1); // unordered flag
  bit_group(of, 1);  // overflow flag
  bit_group(uf, 1);  // underflow flag
  bit_group(inv, 1); // invalid flag
  bit_group(res, 2); // reserved
};

union MerryCoreFlagsRegr {
  struct {
#if defined(_MERRY_HOST_CPU_x86_64_ARCH_)
    // This defines the flags structure for the AMD64 processors
    unsigned long carry : 1;     /*0th bit is the CF in AMD64 EFlags*/
    bit_group(r1, 1);            /*1 bit reserved here*/
    unsigned long parity : 1;    /*2th bit is the PF*/
    bit_group(r2, 1);            /*1 bit reserved here*/
    unsigned long aux_carry : 1; /*4th bit Aux Carry flag[NOT REALLY NEEDED AS
                                    BCD INSTRUCTIONS ARE NOT SUPPORTED]*/
    bit_group(r3, 1);            /*1 bit reserved here*/
    unsigned long zero : 1;      /*6th bit ZF*/
    unsigned long negative : 1;  /*7th bit SF or NG*/
    bit_group(r4, 2);            /*2 bit reserved here*/
    unsigned long direction
        : 1; /*11th bit is the DF[NOT REALLY USEFUL YET BUT MAYBE WHEN
                IMPLEMENTING STRING RELATED INSTRUCTIONS]*/
    unsigned long overflow : 1; /*10th bit is the OF*/
    bit_group(rem_32, 20);
    bit_group(top_32, 32);
#endif
  } flags;
  mqword_t regr;
};

struct MerryCoreStackFrame {
  maddress_t RET_ADDR;
  maddress_t FRAME_BP;
  maddress_t JMP_TO;
};

#endif
