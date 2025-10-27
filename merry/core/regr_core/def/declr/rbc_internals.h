#ifndef _RBC_INTERNALS_
#define _RBC_INTERNALS_

#include <merry_types.h>

#define flags_res(name, len) unsigned name : len

typedef union RBCFlagsRegr RBCFlagsRegr;
typedef struct RBCFFlagsRegr RBCFFlagsRegr;
typedef struct RBCStackFrame RBCStackFrame;

struct RBCFFlagsRegr {
  flags_res(zf, 1);  // zero flag
  flags_res(sf, 1);  // sign flag
  flags_res(uof, 1); // unordered flag
  flags_res(of, 1);  // overflow flag
  flags_res(uf, 1);  // underflow flag
  flags_res(inv, 1); // invalid flag
  flags_res(res, 2); // reserved
};

union RBCFlagsRegr {
  struct {
#if defined(_MERRY_HOST_CPU_x86_64_ARCH_)
    // This defines the flags structure for the AMD64 processors
    unsigned long carry : 1;     /*0th bit is the CF in AMD64 EFlags*/
    flags_res(r1, 1);            /*1 bit reserved here*/
    unsigned long parity : 1;    /*2th bit is the PF*/
    flags_res(r2, 1);            /*1 bit reserved here*/
    unsigned long aux_carry : 1; /*4th bit Aux Carry flag[NOT REALLY NEEDED AS
                                    BCD INSTRUCTIONS ARE NOT SUPPORTED]*/
    flags_res(r3, 1);            /*1 bit reserved here*/
    unsigned long zero : 1;      /*6th bit ZF*/
    unsigned long negative : 1;  /*7th bit SF or NG*/
    flags_res(r4, 2);            /*2 bit reserved here*/
    unsigned long direction
        : 1; /*11th bit is the DF[NOT REALLY USEFUL YET BUT MAYBE WHEN
                IMPLEMENTING STRING RELATED INSTRUCTIONS]*/
    unsigned long overflow : 1; /*10th bit is the OF*/
    flags_res(rem_32, 20);
    flags_res(top_32, 32);
#endif
  } flags;
  mqword_t regr;
};

struct RBCStackFrame {
  maddress_t RET_ADDR;
  maddress_t FRAME_BP;
  maddress_t JMP_TO;
};

#endif
