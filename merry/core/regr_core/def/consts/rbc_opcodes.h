#ifndef _RBC_OPCODES_
#define _RBC_OPCODES_

typedef enum rbcop_t rbcop_t;

enum rbcop_t {
  RBC_OP_NOP,
  RBC_OP_HALT,
  RBC_OP_SYSINT, // Program requests Handled by RBC Locally
  RBC_OP_MINT,   // program requests Handled by Graves

  // Arithmetic instructions
  RBC_OP_ADD,
  RBC_OP_SUB,
  RBC_OP_MUL,
  RBC_OP_DIV,
};

#endif
