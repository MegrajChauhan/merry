#ifndef _MCOPCODES_
#define _MCOPCODES_

typedef enum mcoreop_t mcoreop_t;

enum mcoreop_t {
  MCOP_SYSINT,
  MCOP_MINT,

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
  MCOP_DEC,   

  /* --------------------- Internal Data Movement Instructions ------------*/
  MCOP_MOVE_IMM_64,
  MCOP_MOVF,       
  MCOP_MOVF32,     
  MCOP_MOVE_REG,   
  MCOP_MOVE_REG8,  
  MCOP_MOVE_REG16, 
  MCOP_MOVE_REG32,  
  MCOP_MOVESX_IMM8, //move a value and sign extend it as well[only 8-bit
                      // numbers are accepted]
  MCOP_MOVESX_IMM16, //move a value and sign extend it as well[only 16-bit
                       // numbers are accepted]
  MCOP_MOVESX_IMM32, //move a value and sign extend it as well[only 32-bit
                       // numbers are accepted]
  MCOP_MOVESX_REG8, //move the lower 1 byte to another register while sign
                      // extending it
  MCOP_MOVESX_REG16, //move the lower 2 bytes to another register while
                       // sign extending it
  MCOP_MOVESX_REG32, //move the lower 4 bytes to another register while
                       // sign extending it

  // exchanging values between two registers
  MCOP_EXCG8,  //exchange only 1 lower byte
  MCOP_EXCG16, //exchange only 2 lower bytes
  MCOP_EXCG32, //exchange only 4 lower bytes
  MCOP_EXCG,   //exchange the entire values

  // This also move the values but other bytes of the register is not affected
  MCOP_MOV8,  // 4f
  MCOP_MOV16, // 50
  MCOP_MOV32, // 51

  // Conditional move instructions
  MCOP_MOVNZ, // 52
  MCOP_MOVZ,  // 53
  MCOP_MOVNE, // 54
  MCOP_MOVE,  // 55
  MCOP_MOVNC, // 56
  MCOP_MOVC,  // 57
  MCOP_MOVNO, // 58
  MCOP_MOVO,  // 59
  MCOP_MOVNN, // 5a
  MCOP_MOVN,  // 5b
  MCOP_MOVNG, // 5c
  MCOP_MOVG,  // 5d
  MCOP_MOVNS, // 5e
  MCOP_MOVS,  // 5f
  MCOP_MOVGE, // 60
  MCOP_MOVSE, // 61

  // we won't need zero extend. It can simplyaliased with simple move

  /*-------------------Control Flow Instructions ------------------*/

  MCOP_JMP_OFF,  //JMP inst but the offset from current PC is provided[If
                   // the offset provided is in 2's complement then we can jump
                   // back]
  MCOP_JMP_ADDR, //JMP inst but the address is directly provided
  MCOP_JNZ,      // 64
  MCOP_JZ,       // 65
  MCOP_JNE,      // 66
  MCOP_JE,       // 67
  MCOP_JNC,      // 68
  MCOP_JC,       // 69
  MCOP_JNO,      // 6a
  MCOP_JO,       // 6b
  MCOP_JNN,      // 6c
  MCOP_JN,       // 6d
  MCOP_JNG,      // 6e
  MCOP_JG,       // 6f
  MCOP_JNS,      // 70
  MCOP_JS,       // 71
  MCOP_JGE,      // 72
  MCOP_JSE,      // 73

  MCOP_CALL, //the call instruction

  MCOP_RET,   //return from a call
  MCOP_RETNZ, // 76
  MCOP_RETZ,  // 77
  MCOP_RETNE, // 78
  MCOP_RETE,  // 79
  MCOP_RETNC, // 7a
  MCOP_RETC,  // 7b
  MCOP_RETNO, // 7c
  MCOP_RETO,  // 7d
  MCOP_RETNN, // 7e
  MCOP_RETN,  // 7f
  MCOP_RETNG, // 80
  MCOP_RETG,  // 81
  MCOP_RETNS, // 82
  MCOP_RETS,  // 83
  MCOP_RETGE, // 84
  MCOP_RETSE, // 85

  MCOP_LOOP, //automatically jumps to the given address until the
               // specified register is 0

  MCOP_CALL_REG, // 87
  MCOP_JMP_REGR, //address in register

  /*------------------------ Stack-Based Instructions-------------------*/

  MCOP_PUSH_IMM64, // 8d
  MCOP_PUSH_REG,   // 8e
  MCOP_POP64,      // 92
  MCOP_PUSHA,      //R0, R1, R2, ......
  MCOP_POPA,       //In reverse

  MCOP_LOADSQ, // a0

  MCOP_STORESQ, // a4

  /*--------------------Logical Instructions--------------------------*/

  MCOP_AND_IMM, //REG & IMM
  MCOP_AND_REG, //REG & REG

  MCOP_OR_IMM, //REG | TMM
  MCOP_OR_REG, //REG | REG

  MCOP_XOR_IMM, //REG ^ IMM
  MCOP_XOR_REG, //REG ^ REG

  MCOP_NOT, //~REG

  MCOP_LSHIFT,      //REG << <num>
  MCOP_RSHIFT,      //REG >> <num>
  MCOP_LSHIFT_REGR, // ae
  MCOP_RSHIFT_REGR, // af

  MCOP_CMP_IMM,      //CMP REG and IMM
  MCOP_CMP_REG,      //CMP REG and REG
  
  MCOP_FCMP,   // b6
  MCOP_FCMP32, // b7

  /*------------------------IO Instructions-----------------------------*/
  MCOP_CIN,  //character input
  MCOP_COUT, //character output
  MCOP_SIN, //input a given number of bytes[characters or a string][string
              // in]
  MCOP_SOUT, //output a given number of bytes[As characters][string out]

  MCOP_IN,   //input a byte
  MCOP_OUT,  //print a byte
  MCOP_INW,  //input a word[signed]
  MCOP_OUTW, //output a word[signed]
  MCOP_IND,  //input a dword[signed]
  MCOP_OUTD, //output a dword[signed]
  MCOP_INQ,  //input a qword[signed]
  MCOP_OUTQ, //output a qword[signed]

  MCOP_UIN,   //input a byte[unsigned]
  MCOP_UOUT,  //output a byte[unsigned]
  MCOP_UINW,  //input a word[unsigned]
  MCOP_UOUTW, //output a word[unsigned]
  MCOP_UIND,  //input a dword[unsigned]
  MCOP_UOUTD, //output a dword[unsigned]
  MCOP_UINQ,  //input a qword[unsigned]
  MCOP_UOUTQ, //output a qword[unsigned]

  MCOP_INF,  //read a 64-bit float
  MCOP_OUTF, //print a 64-bit float

  MCOP_INF32,  //read a 32-bit float
  MCOP_OUTF32, //print a 32-bit float

  MCOP_OUTR,  //print all register's contents as signed values[takes no
                // operands]
  MCOP_UOUTR, //print all register's contents as unsigned values[takes no
                // operands]
  MCOP_SIN_REG,  // d2
  MCOP_SOUT_REG, // d3

  /*-----------------Mem-Regr Data Movement Instructions----------------*/

  // load/store DEST/SOURCE REGR, DEST/SOURCE ADDRESS
  MCOP_LOADB,  //load byte
  MCOP_LOADW,  //load 2 bytes
  MCOP_LOADD,  //load 4 bytes
  MCOP_STOREB, //store byte
  MCOP_STOREW, //store 2 bytes
  MCOP_STORED, //store 4 bytes
  MCOP_LOADQ,  // da
  MCOP_STOREQ, // db

  // the addresses are in register
  MCOP_LOADB_REG,  // dc
  MCOP_STOREB_REG, // dd
  MCOP_LOADW_REG,  // de
  MCOP_STOREW_REG, // df
  MCOP_LOADD_REG,  // e0
  MCOP_STORED_REG, // e1
  MCOP_LOADQ_REG,  // e2
  MCOP_STOREQ_REG, // e3

  MCOP_ATOMIC_LOADB,  //load byte atomically
  MCOP_ATOMIC_LOADW,  //load 2 bytes atomically
  MCOP_ATOMIC_LOADD,  //load 4 bytes atomically
  MCOP_ATOMIC_LOADQ,  //load 8 bytes atomically
  MCOP_ATOMIC_STOREB, //store byte atomically
  MCOP_ATOMIC_STOREW, //store 2 bytes atomically
  MCOP_ATOMIC_STORED, //store 4 bytes atomically
  MCOP_ATOMIC_STOREQ, //store 8 bytes atomically

  MCOP_ATOMIC_LOADB_REG,  //load byte atomically
  MCOP_ATOMIC_LOADW_REG,  //load 2 bytes atomically
  MCOP_ATOMIC_LOADD_REG,  //load 4 bytes atomically
  MCOP_ATOMIC_LOADQ_REG,  //load 8 bytes atomically
  MCOP_ATOMIC_STOREB_REG, //store byte atomically
  MCOP_ATOMIC_STOREW_REG, //store 2 bytes atomically
  MCOP_ATOMIC_STORED_REG, //store 4 bytes atomically
  MCOP_ATOMIC_STOREQ_REG, //store 8 bytes atomically

  /*-------------------Utility Instructions-----------------------------*/

  /*
    LEA will require many operands.
    The first operand is the base address which canany register.
    The second operand is the index which can alsoin any register
    The third operand is the scale which can alsoin any register
    The last is the destination register which can alsoany register
    dest = base + index * scale [Useful for arrays: Doesn't change flags]
  */
  MCOP_LEA, // f4

  MCOP_CFLAGS,  //clear the flags register
  MCOP_CFFLAGS, //clear the float flags register
  MCOP_RESET,   //reset all the registers

  MCOP_CMPXCHG, //the atomic compare and exchange instruction

  MCOP_CMPXCHG_REGR, // f9
  MCOP_RETFNI,       // fa

  MCOP_MOVFZ = 0, //00
  MCOP_MOVFNZ,    //  
  MCOP_MOVFN,     //  
  MCOP_MOVFNN,    //  
  MCOP_MOVFUF,    //  
  MCOP_MOVFNUF,   //  
  MCOP_MOVFO,     //  
  MCOP_MOVFNO,    //  
  MCOP_MOVFU,     //  
  MCOP_MOVFNU,    //  
  MCOP_MOVFI,     //  
  MCOP_MOVFNI,    //  
  MCOP_JFZ,       //  
  MCOP_JFNZ,      //  
  MCOP_JFN,       //  
  MCOP_JFNN,      //  
  MCOP_JFUF,      //  
  MCOP_JFNUF,     //  
  MCOP_JFO,       //  
  MCOP_JFNO,      //  
  MCOP_JFU,       //  
  MCOP_JFNU,      //  
  MCOP_JFI,       //  
  MCOP_JFNI,      
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
  MCOP_RETFI,
};

#endif
