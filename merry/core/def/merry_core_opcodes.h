#ifndef _MCOPCODES_
#define _MCOPCODES_

typedef enum mcoreop_t mcoreop_t;

enum mcoreop_t {
  MCOP_HLT, 
  MCOP_SYSINT,
  MCOP_MINT, // 2

  // Arithmetic instructions
  MCOP_ADD_IMM,
  MCOP_ADD_REG,
  MCOP_SUB_IMM,
  MCOP_SUB_REG,
  MCOP_MUL_IMM,
  MCOP_MUL_REG,
  MCOP_DIV_IMM,
  MCOP_DIV_REG,
  MCOP_MOD_IMM,
  MCOP_MOD_REG,
  MCOP_FADD,
  MCOP_FSUB,
  MCOP_FMUL,
  MCOP_FDIV,
  MCOP_FADD32,
  MCOP_FSUB32,
  MCOP_FMUL32,
  MCOP_FDIV32,
  MCOP_INC,
  MCOP_DEC, // 22

  /* --------------------- Internal Data Movement Instructions ------------*/
  MCOP_MOVE_IMM_64,
  MCOP_MOVE_IMM_32,
  MCOP_MOVE_REG,
  MCOP_MOVE_REG8,
  MCOP_MOVE_REG16,
  MCOP_MOVE_REG32,
  MCOP_MOVESX_IMM8,
  MCOP_MOVESX_IMM16,
  MCOP_MOVESX_IMM32,
  MCOP_MOVESX_REG8,
  MCOP_MOVESX_REG16,
  MCOP_MOVESX_REG32, // 34

  // exchanging values between two registers
  MCOP_EXCG8,
  MCOP_EXCG16,
  MCOP_EXCG32,
  MCOP_EXCG, // 38

  // we won't need zero extend. It can simply aliased with simple move

  /*-------------------Control Flow Instructions ------------------*/

  MCOP_JMP_OFF,
  MCOP_JMP_ADDR,
  MCOP_JMP_REGR,
  MCOP_JNZ,
  MCOP_JZ,
  MCOP_JNE,
  MCOP_JE,
  MCOP_JNC,
  MCOP_JC,
  MCOP_JNO,
  MCOP_JO,
  MCOP_JNN,
  MCOP_JN,
  MCOP_JNG,
  MCOP_JG,
  MCOP_JNS,
  MCOP_JS,
  MCOP_JGE,
  MCOP_JSE,
  MCOP_JFZ,
  MCOP_JFNZ,
  MCOP_JFN,
  MCOP_JFNN,
  MCOP_JFUF,
  MCOP_JFNUF,
  MCOP_JFO,
  MCOP_JFNO,
  MCOP_JFU,
  MCOP_JFNU,
  MCOP_JFI,
  MCOP_JFNI, // 69

  MCOP_CALL_REG,
  MCOP_CALL,
  MCOP_RET,
  MCOP_RETNZ,
  MCOP_RETZ,
  MCOP_RETNE,
  MCOP_RETE,
  MCOP_RETNC,
  MCOP_RETC,
  MCOP_RETNO,
  MCOP_RETO,
  MCOP_RETNN,
  MCOP_RETN,
  MCOP_RETNG,
  MCOP_RETG,
  MCOP_RETNS,
  MCOP_RETS,
  MCOP_RETGE,
  MCOP_RETSE,
  MCOP_RETFNI,
  MCOP_RETFZ,
  MCOP_RETFNZ,
  MCOP_RETFN,
  MCOP_RETFNN,
  MCOP_RETFUF,
  MCOP_RETFNUF,
  MCOP_RETFO,
  MCOP_RETFNO,
  MCOP_RETFU,
  MCOP_RETFNU,
  MCOP_RETFI, // 100

  MCOP_LOOP,

  /*------------------------ Stack-Based Instructions-------------------*/

  MCOP_PUSH_IMM64,
  MCOP_PUSH_REG,
  MCOP_POP64,
  MCOP_PUSHA,
  MCOP_POPA, // 106

  MCOP_LOADSQ,
  MCOP_STORESQ,
  MCOP_LOADSQ_REG,
  MCOP_STORESQ_REG, // 110

  /*--------------------Logical Instructions--------------------------*/

  MCOP_AND_IMM,
  MCOP_AND_REG,

  MCOP_OR_IMM,
  MCOP_OR_REG,

  MCOP_XOR_IMM,
  MCOP_XOR_REG,

  MCOP_NOT,

  MCOP_LSHIFT,
  MCOP_RSHIFT,
  MCOP_LSHIFT_REGR,
  MCOP_RSHIFT_REGR,

  MCOP_CMP_IMM,
  MCOP_CMP_REG,

  MCOP_FCMP,
  MCOP_FCMP32, // 125

  /*------------------------IO Instructions-----------------------------*/
  MCOP_BIN,
  MCOP_BOUT,

  /*-----------------Mem-Regr Data Movement Instructions----------------*/

  MCOP_LOADB,
  MCOP_LOADW,
  MCOP_LOADD,
  MCOP_STOREB,
  MCOP_STOREW,
  MCOP_STORED,
  MCOP_LOADQ,
  MCOP_STOREQ, // 135

  MCOP_LOADB_REG,
  MCOP_STOREB_REG,
  MCOP_LOADW_REG,
  MCOP_STOREW_REG,
  MCOP_LOADD_REG,
  MCOP_STORED_REG,
  MCOP_LOADQ_REG,
  MCOP_STOREQ_REG, // 143

  MCOP_ATOMIC_LOADB,
  MCOP_ATOMIC_LOADW,
  MCOP_ATOMIC_LOADD,
  MCOP_ATOMIC_LOADQ,
  MCOP_ATOMIC_STOREB,
  MCOP_ATOMIC_STOREW,
  MCOP_ATOMIC_STORED,
  MCOP_ATOMIC_STOREQ, // 151

  MCOP_ATOMIC_LOADB_REG,
  MCOP_ATOMIC_LOADW_REG,
  MCOP_ATOMIC_LOADD_REG,
  MCOP_ATOMIC_LOADQ_REG,
  MCOP_ATOMIC_STOREB_REG,
  MCOP_ATOMIC_STOREW_REG,
  MCOP_ATOMIC_STORED_REG,
  MCOP_ATOMIC_STOREQ_REG, // 159

  /*-------------------Utility Instructions-----------------------------*/

  /*
    LEA will require many operands.
    The first operand is the base address which canany register.
    The second operand is the index which can alsoin any register
    The third operand is the scale which can alsoin any register
    The last is the destination register which can alsoany register
    dest = base + index * scale [Useful for arrays: Doesn't change flags]
  */
  MCOP_LEA,

  MCOP_CFLAGS,
  MCOP_CFFLAGS,
  MCOP_RESET,

  MCOP_CMPXCHG,
  MCOP_CMPXCHG_REGR,

};

#endif
