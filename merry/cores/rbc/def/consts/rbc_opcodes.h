#ifndef _RBC_OPCODES_
#define _RBC_OPCODES_

typedef enum rbcRBC_OP_t rbcop_t;

enum rbcRBC_OP_t {
  RBC_OP_NOP,    // 00
  RBC_OP_RES24,   // 01
  RBC_OP_SYSINT, // 02 Program requests Handled by RBC Locally
  RBC_OP_MINT,   // 03 program requests Handled by Graves
  RBC_OP_RES1,   // 04
  RBC_OP_RES2,   // 05
  RBC_OP_RES3,   // 06
  RBC_OP_RES4,   // 07
  RBC_OP_RES5,   // 08
  RBC_OP_RES6,   // 09
  RBC_OP_RES7,   // 0a
  RBC_OP_RES8,   // 0b
  RBC_OP_RES9,   // 0c
  RBC_OP_RES10,  // 0d

  // Arithmetic instructions
  RBC_OP_ADD_IMM,    // 0e
  RBC_OP_ADD_REG,    // 0f
  RBC_OP_SUB_IMM,    // 10
  RBC_OP_SUB_REG,    // 11
  RBC_OP_MUL_IMM,    // 12
  RBC_OP_MUL_REG,    // 13
  RBC_OP_DIV_IMM,    // 14
  RBC_OP_DIV_REG,    // 15
  RBC_OP_MOD_IMM,    // 16
  RBC_OP_MOD_REG,    // 17
  RBC_OP_FADD,       // 18
  RBC_OP_FSUB,       // 19
  RBC_OP_FMUL,       // 1a
  RBC_OP_FDIV,       // 1b
  RBC_OP_FADD32,     // 1c
  RBC_OP_FSUB32,     // 1d
  RBC_OP_FMUL32,     // 1e
  RBC_OP_FDIV32,     // 1f
  RBC_OP_ADD_MEMB,   // 20
  RBC_OP_ADD_MEMW,   // 21
  RBC_OP_ADD_MEMD,   // 22
  RBC_OP_ADD_MEMQ,   // 23
  RBC_OP_SUB_MEMB,   // 24
  RBC_OP_SUB_MEMW,   // 25
  RBC_OP_SUB_MEMD,   // 26
  RBC_OP_SUB_MEMQ,   // 27
  RBC_OP_MUL_MEMB,   // 28
  RBC_OP_MUL_MEMW,   // 29
  RBC_OP_MUL_MEMD,   // 2a
  RBC_OP_MUL_MEMQ,   // 2b
  RBC_OP_DIV_MEMB,   // 2c
  RBC_OP_DIV_MEMW,   // 2d
  RBC_OP_DIV_MEMD,   // 2e
  RBC_OP_DIV_MEMQ,   // 2f
  RBC_OP_MOD_MEMB,   // 30
  RBC_OP_MOD_MEMW,   // 31
  RBC_OP_MOD_MEMD,   // 32
  RBC_OP_MOD_MEMQ,   // 33
  RBC_OP_FADD_MEM,   // 34
  RBC_OP_FSUB_MEM,   // 35
  RBC_OP_FMUL_MEM,   // 36
  RBC_OP_FDIV_MEM,   // 37
  RBC_OP_FADD32_MEM, // 38
  RBC_OP_FSUB32_MEM, // 39
  RBC_OP_FMUL32_MEM, // 3a
  RBC_OP_FDIV32_MEM, // 3b
  RBC_OP_INC,        // 3c
  RBC_OP_DEC,        // 3d
  /* --------------------- Internal Data Movement Instructions ------------*/
  RBC_OP_MOVE_IMM_64, // 3e MOVE 64-bit immediate into a register
  RBC_OP_MOVF,        // 3f
  RBC_OP_MOVF32,      // 40
  RBC_OP_MOVE_REG,    // 41 move a value from one register to another
  RBC_OP_MOVE_REG8,   // 42 move the lowest byte of one register to another
  RBC_OP_MOVE_REG16,  // 43 move the lowest two bytes of one register to another
  RBC_OP_MOVE_REG32,  // 44 move the lowest 4 bytes of one register to another
  RBC_OP_MOVESX_IMM8, // 45 move a value and sign extend it as well[only 8-bit
                      // numbers are accepted]
  RBC_OP_MOVESX_IMM16, // 46 move a value and sign extend it as well[only 16-bit
                       // numbers are accepted]
  RBC_OP_MOVESX_IMM32, // 47 move a value and sign extend it as well[only 32-bit
                       // numbers are accepted]
  RBC_OP_MOVESX_REG8, // 48 move the lower 1 byte to another register while sign
                      // extending it
  RBC_OP_MOVESX_REG16, // 49 move the lower 2 bytes to another register while
                       // sign extending it
  RBC_OP_MOVESX_REG32, // 4a move the lower 4 bytes to another register while
                       // sign extending it

  // exchanging values between two registers
  RBC_OP_EXCG8,  // 4b exchange only 1 lower byte
  RBC_OP_EXCG16, // 4c exchange only 2 lower bytes
  RBC_OP_EXCG32, // 4d exchange only 4 lower bytes
  RBC_OP_EXCG,   // 4e exchange the entire values

  // This also move the values but other bytes of the register is not affected
  RBC_OP_MOV8,  // 4f
  RBC_OP_MOV16, // 50
  RBC_OP_MOV32, // 51

  // Conditional move instructions
  RBC_OP_MOVNZ, // 52
  RBC_OP_MOVZ,  // 53
  RBC_OP_MOVNE, // 54
  RBC_OP_MOVE,  // 55
  RBC_OP_MOVNC, // 56
  RBC_OP_MOVC,  // 57
  RBC_OP_MOVNO, // 58
  RBC_OP_MOVO,  // 59
  RBC_OP_MOVNN, // 5a
  RBC_OP_MOVN,  // 5b
  RBC_OP_MOVNG, // 5c
  RBC_OP_MOVG,  // 5d
  RBC_OP_MOVNS, // 5e
  RBC_OP_MOVS,  // 5f
  RBC_OP_MOVGE, // 60
  RBC_OP_MOVSE, // 61

  // we won't need zero extend. It can simply be aliased with simple move

  /*-------------------Control Flow Instructions ------------------*/

  RBC_OP_JMP_OFF,  // 62 JMP inst but the offset from current PC is provided[If
                   // the offset provided is in 2's complement then we can jump
                   // back]
  RBC_OP_JMP_ADDR, // 63 JMP inst but the address is directly provided
  RBC_OP_JNZ,      // 64
  RBC_OP_JZ,       // 65
  RBC_OP_JNE,      // 66
  RBC_OP_JE,       // 67
  RBC_OP_JNC,      // 68
  RBC_OP_JC,       // 69
  RBC_OP_JNO,      // 6a
  RBC_OP_JO,       // 6b
  RBC_OP_JNN,      // 6c
  RBC_OP_JN,       // 6d
  RBC_OP_JNG,      // 6e
  RBC_OP_JG,       // 6f
  RBC_OP_JNS,      // 70
  RBC_OP_JS,       // 71
  RBC_OP_JGE,      // 72
  RBC_OP_JSE,      // 73

  RBC_OP_CALL, // 74 the call instruction

  RBC_OP_RET,   // 75 return from a call
  RBC_OP_RETNZ, // 76
  RBC_OP_RETZ,  // 77
  RBC_OP_RETNE, // 78
  RBC_OP_RETE,  // 79
  RBC_OP_RETNC, // 7a
  RBC_OP_RETC,  // 7b
  RBC_OP_RETNO, // 7c
  RBC_OP_RETO,  // 7d
  RBC_OP_RETNN, // 7e
  RBC_OP_RETN,  // 7f
  RBC_OP_RETNG, // 80
  RBC_OP_RETG,  // 81
  RBC_OP_RETNS, // 82
  RBC_OP_RETS,  // 83
  RBC_OP_RETGE, // 84
  RBC_OP_RETSE, // 85

  RBC_OP_LOOP, // 86 automatically jumps to the given address until the
               // specified register is 0

  RBC_OP_CALL_REG, // 87
  RBC_OP_JMP_REGR, // 88 address in register

  RBC_OP_RES23, // 89

  /*------------------------ Stack-Based Instructions-------------------*/

  RBC_OP_RES11,      // 8a
  RBC_OP_RES12,      // 8b
  RBC_OP_RES13,      // 8c
  RBC_OP_PUSH_IMM64, // 8d
  RBC_OP_PUSH_REG,   // 8e
  RBC_OP_RES14,      // 8f
  RBC_OP_RES15,      // 90
  RBC_OP_RES16,      // 91
  RBC_OP_POP64,      // 92
  RBC_OP_PUSHA,      // 93 R0, R1, R2, ......
  RBC_OP_POPA,       // 94 In reverse

  RBC_OP_PUSH_MEMB, // 95
  RBC_OP_PUSH_MEMW, // 96
  RBC_OP_PUSH_MEMD, // 97
  RBC_OP_PUSH_MEMQ, // 98
  RBC_OP_POP_MEMB,  // 99
  RBC_OP_POP_MEMW,  // 9a
  RBC_OP_POP_MEMD,  // 9b
  RBC_OP_POP_MEMQ,  // 9c

  RBC_OP_RES17,  // 9d loadsb R0, 5 -> Load whatever byte is at BP+5 into R0
  RBC_OP_RES18,  // 9e
  RBC_OP_RES19,  // 9f
  RBC_OP_LOADSQ, // a0

  RBC_OP_RES20,   // a1 storesb R0, 5 -> Store the lowest byte of R0 at BP+5
  RBC_OP_RES21,   // a2
  RBC_OP_RES22,   // a3
  RBC_OP_STORESQ, // a4

  /*--------------------Logical Instructions--------------------------*/

  RBC_OP_AND_IMM, // a5 REG & IMM
  RBC_OP_AND_REG, // a6 REG & REG

  RBC_OP_OR_IMM, // a7 REG | TMM
  RBC_OP_OR_REG, // a8 REG | REG

  RBC_OP_XOR_IMM, // a9 REG ^ IMM
  RBC_OP_XOR_REG, // aa REG ^ REG

  RBC_OP_NOT, // ab ~REG

  RBC_OP_LSHIFT,      // ac REG << <num>
  RBC_OP_RSHIFT,      // ad REG >> <num>
  RBC_OP_LSHIFT_REGR, // ae
  RBC_OP_RSHIFT_REGR, // af

  RBC_OP_CMP_IMM,      // b0 CMP REG and IMM
  RBC_OP_CMP_REG,      // b1 CMP REG and REG
  RBC_OP_CMP_IMM_MEMB, // b2
  RBC_OP_CMP_IMM_MEMW, // b3
  RBC_OP_CMP_IMM_MEMD, // b4
  RBC_OP_CMP_IMM_MEMQ, // b5

  RBC_OP_FCMP,   // b6
  RBC_OP_FCMP32, // b7

  /*------------------------IO Instructions-----------------------------*/
  RBC_OP_CIN,  // b8 character input
  RBC_OP_COUT, // b9 character output
  RBC_OP_SIN, // ba input a given number of bytes[characters or a string][string
              // in]
  RBC_OP_SOUT, // bb output a given number of bytes[As characters][string out]

  RBC_OP_IN,   // bc input a byte
  RBC_OP_OUT,  // bd print a byte
  RBC_OP_INW,  // be input a word[signed]
  RBC_OP_OUTW, // bf output a word[signed]
  RBC_OP_IND,  // c0 input a dword[signed]
  RBC_OP_OUTD, // c1 output a dword[signed]
  RBC_OP_INQ,  // c2 input a qword[signed]
  RBC_OP_OUTQ, // c3 output a qword[signed]

  RBC_OP_UIN,   // c4 input a byte[unsigned]
  RBC_OP_UOUT,  // c5 output a byte[unsigned]
  RBC_OP_UINW,  // c6 input a word[unsigned]
  RBC_OP_UOUTW, // c7 output a word[unsigned]
  RBC_OP_UIND,  // c8 input a dword[unsigned]
  RBC_OP_UOUTD, // c9 output a dword[unsigned]
  RBC_OP_UINQ,  // ca input a qword[unsigned]
  RBC_OP_UOUTQ, // cb output a qword[unsigned]

  RBC_OP_INF,  // cc read a 64-bit float
  RBC_OP_OUTF, // cd print a 64-bit float

  RBC_OP_INF32,  // ce read a 32-bit float
  RBC_OP_OUTF32, // cf print a 32-bit float

  RBC_OP_OUTR,  // d0 print all register's contents as signed values[takes no
                // operands]
  RBC_OP_UOUTR, // d1 print all register's contents as unsigned values[takes no
                // operands]
  RBC_OP_SIN_REG,  // d2
  RBC_OP_SOUT_REG, // d3

  /*-----------------Mem-Regr Data Movement Instructions----------------*/

  // load/store DEST/SOURCE REGR, DEST/SOURCE ADDRESS
  RBC_OP_LOADB,  // d4 load byte
  RBC_OP_LOADW,  // d5 load 2 bytes
  RBC_OP_LOADD,  // d6 load 4 bytes
  RBC_OP_STOREB, // d7 store byte
  RBC_OP_STOREW, // d8 store 2 bytes
  RBC_OP_STORED, // d9 store 4 bytes
  RBC_OP_LOADQ,  // da
  RBC_OP_STOREQ, // db

  // the addresses are in register
  RBC_OP_LOADB_REG,  // dc
  RBC_OP_STOREB_REG, // dd
  RBC_OP_LOADW_REG,  // de
  RBC_OP_STOREW_REG, // df
  RBC_OP_LOADD_REG,  // e0
  RBC_OP_STORED_REG, // e1
  RBC_OP_LOADQ_REG,  // e2
  RBC_OP_STOREQ_REG, // e3

  RBC_OP_ATOMIC_LOADB,  // e4 load byte atomically
  RBC_OP_ATOMIC_LOADW,  // e5 load 2 bytes atomically
  RBC_OP_ATOMIC_LOADD,  // e6 load 4 bytes atomically
  RBC_OP_ATOMIC_LOADQ,  // e7 load 8 bytes atomically
  RBC_OP_ATOMIC_STOREB, // e8 store byte atomically
  RBC_OP_ATOMIC_STOREW, // e9 store 2 bytes atomically
  RBC_OP_ATOMIC_STORED, // ea store 4 bytes atomically
  RBC_OP_ATOMIC_STOREQ, // eb store 8 bytes atomically

  RBC_OP_ATOMIC_LOADB_REG,  // ec load byte atomically
  RBC_OP_ATOMIC_LOADW_REG,  // ed load 2 bytes atomically
  RBC_OP_ATOMIC_LOADD_REG,  // ee load 4 bytes atomically
  RBC_OP_ATOMIC_LOADQ_REG,  // ef load 8 bytes atomically
  RBC_OP_ATOMIC_STOREB_REG, // f0 store byte atomically
  RBC_OP_ATOMIC_STOREW_REG, // f1 store 2 bytes atomically
  RBC_OP_ATOMIC_STORED_REG, // f2 store 4 bytes atomically
  RBC_OP_ATOMIC_STOREQ_REG, // f3 store 8 bytes atomically

  /*-------------------Utility Instructions-----------------------------*/

  /*
    LEA will require many operands.
    The first operand is the base address which can be any register.
    The second operand is the index which can also be in any register
    The third operand is the scale which can also be in any register
    The last is the destination register which can also be any register
    dest = base + index * scale [Useful for arrays: Doesn't change flags]
  */
  RBC_OP_LEA, // f4

  RBC_OP_CFLAGS,  // f5 clear the flags register
  RBC_OP_CFFLAGS, // f6 clear the float flags register
  RBC_OP_RESET,   // f7 reset all the registers

  RBC_OP_CMPXCHG, // f8 the atomic compare and exchange instruction

  RBC_OP_CMPXCHG_REGR, // f9
  RBC_OP_RETFNI,       // fa

  RBC_OP_MOVFZ = 0, // ff 00
  RBC_OP_MOVFNZ,    // ff 01
  RBC_OP_MOVFN,     // ff 02
  RBC_OP_MOVFNN,    // ff 03
  RBC_OP_MOVFUF,    // ff 04
  RBC_OP_MOVFNUF,   // ff 05
  RBC_OP_MOVFO,     // ff 06
  RBC_OP_MOVFNO,    // ff 07
  RBC_OP_MOVFU,     // ff 08
  RBC_OP_MOVFNU,    // ff 09
  RBC_OP_MOVFI,     // ff 0a
  RBC_OP_MOVFNI,    // ff 0b
  RBC_OP_JFZ,       // ff 0c
  RBC_OP_JFNZ,      // ff 0d
  RBC_OP_JFN,       // ff 0e
  RBC_OP_JFNN,      // ff 0f
  RBC_OP_JFUF,      // ff 10
  RBC_OP_JFNUF,     // ff 11
  RBC_OP_JFO,       // ff 12
  RBC_OP_JFNO,      // ff 13
  RBC_OP_JFU,       // ff 14
  RBC_OP_JFNU,      // ff 15
  RBC_OP_JFI,       // ff 16
  RBC_OP_JFNI,      // ff 17
  RBC_OP_RETFZ,     // ff 18
  RBC_OP_RETFNZ,    // ff 19
  RBC_OP_RETFN,     // ff 1a
  RBC_OP_RETFNN,    // ff 1b
  RBC_OP_RETFUF,    // ff 1c
  RBC_OP_RETFNUF,   // ff 1d
  RBC_OP_RETFO,     // ff 1e
  RBC_OP_RETFNO,    // ff 1f
  RBC_OP_RETFU,     // ff 20
  RBC_OP_RETFNU,    // ff 21
  RBC_OP_RETFI,     // ff 22
};

#endif
