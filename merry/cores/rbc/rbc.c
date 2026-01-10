#include <internal/rbc.h>
#include <internal/rbc_instruction_handler.h>

_MERRY_DEFINE_STATIC_LIST_(Interface, MerryInterface *);
_MERRY_DEFINE_STACK_(RBCProcFrame, RBCStackFrame);

mresult_t rbc_core_create(MerryCoreIdentity iden,
                          maddress_t st_addr, mptr_t *ptr) {
  mresult_t ret;
  RBCCore *core = (RBCCore *)malloc(sizeof(RBCCore));
  if (!core) {
    MFATAL("RBC", "Failed to allocate memory for the master core", NULL);
    ret = RBC_SYS_FAILURE;
    goto RBC_CC_FAILED;
  }
  if ((ret = merry_Interface_list_create(10, &core->interfaces)) !=
      MRES_SUCCESS) {
    MFATAL("RBC", "Failed to allocate memory for a component", NULL);
    free(core);
    goto RBC_CC_FAILED;
  }
  if ((ret = merry_mapped_memory_create(&core->st)) != MRES_SUCCESS) {
    MFATAL("RBC", "Failed to initialize the stack", NULL);
    merry_Interface_list_destroy(core->interfaces);
    free(core);
    goto RBC_CC_FAILED;
  }
  if ((ret = merry_mapped_memory_map(core->st, _RBC_STACK_LEN_)) !=
      MRES_SUCCESS) {
    MFATAL("RBC", "Failed to initialize the stack", NULL);
    merry_Interface_list_destroy(core->interfaces);
    merry_mapped_memory_destroy(core->st);
    free(core);
    goto RBC_CC_FAILED;
  }
  merry_mapped_memory_obtain_ptr(core->st, &core->stack);
  if ((ret = merry_RBCProcFrame_stack_init(&core->stack_frames,
                                           _RBC_CALL_DEPTH_)) != MRES_SUCCESS) {
    MFATAL("RBC", "Failed to initialize the stack", NULL);
    merry_Interface_list_destroy(core->rbc_core->interfaces);
    merry_mapped_memory_unmap(core->st);
    merry_mapped_memory_destroy(core->st);
    free(core);
    goto RBC_CC_FAILED;
  }

  core->SP = 0;
  core->BP = 0;
  core->iden = iden;
  core->PC = st_addr;
  *ptr = (mptr_t)core;
  core->state.stop = mfalse;
  core->state.interrupt = mfalse;

  return MRES_SUCCESS;
RBC_CC_FAILED:
  return ret;
}

void rbc_core_destroy(mptr_t c) {
  merry_check_ptr(c);
  RBCCore *core = (RBCCore *)c;

  merry_Interface_list_destroy(core->interfaces);
  merry_RBCProcFrame_stack_destroy(core->stack_frames);
  rbc_memory_destroy(core->iram);
  rbc_memory_destroy(core->dram);
  rbc_input_destroy(core->inp);
  merry_mapped_memory_unmap(core->st);
  merry_mapped_memory_destroy(core->st);
  free(core);
}

msize_t rbc_core_run(mptr_t c) {
  merry_check_ptr(c);

  RBCCore *core = (RBCCore *)c;
  register atm_mbool_t* stop = &core->state.stop;
  register atm_mbool_t* intr = &core->state.interrupt;
  MerryHostMemLayout layout, mem;
  MerryFloatToDword ftod;
  MerryDoubleToQword dtoq;
  msize_t ret = 0;

  while (mtrue) {
    if (!surelyF()) {
      if (*interrupt) {
        // handle the interrupt
        if (core->state->stop) {
          break;
        }
        *interrupt = mfalse;
      }
      chk = 3;
    }
    chk--;
    if (surelyF(
            rbc_memory_read_qword(core->iram, core->PC, &layout.whole_word) ==
            RBC_MEM_OPER_ACCESS_INVALID)) {
      MFATAL(
          "RBC",
          "Memory access invalid: Accessing address that doesn't exist PC=%zu",
          core->PC);
          ret = 1;
          break;
    } else {
      switch (layout.bytes.b0) {
      case RBC_OP_NOP:
      case RBC_OP_RES1:
      case RBC_OP_RES2:
      case RBC_OP_RES3:
      case RBC_OP_RES4:
      case RBC_OP_RES5:
      case RBC_OP_RES6:
      case RBC_OP_RES7:
      case RBC_OP_RES8:
      case RBC_OP_RES9:
      case RBC_OP_RES10:
      case RBC_OP_RES11:
      case RBC_OP_RES12:
      case RBC_OP_RES13:
      case RBC_OP_RES14:
      case RBC_OP_RES15:
      case RBC_OP_RES16:
      case RBC_OP_RES17:
      case RBC_OP_RES18:
      case RBC_OP_RES19:
      case RBC_OP_RES20:
      case RBC_OP_RES21:
      case RBC_OP_RES22:
      case RBC_OP_RES23:
      case RBC_OP_RES24:
        break;
      case RBC_OP_SYSINT:
        rbc_isysint(core, );
        break;
      case RBC_OP_MINT:
        rbc_imint(&core->& core->kill_core);
        break;
      case RBC_OP_ADD_IMM:
        rbc_iadd_imm(&core->& core->kill_core, layout.whole_word);
        break;
      case RBC_OP_ADD_REG:
        rbc_iadd_reg(&core->& core->kill_core, layout.whole_word);
        break;
      case RBC_OP_SUB_IMM:
        rbc_isub_imm(&core->& core->kill_core, layout.whole_word);
        break;
      case RBC_OP_SUB_REG:
        rbc_isub_reg(&core->& core->kill_core, layout.whole_word);
        break;
      case RBC_OP_MUL_IMM:
        rbc_imul_imm(&core->& core->kill_core, layout.whole_word);
        break;
      case RBC_OP_MUL_REG:
        rbc_imul_reg(&core->& core->kill_core, layout.whole_word);
        break;
      case RBC_OP_DIV_IMM:
        rbc_idiv_imm(&core->& core->kill_core, layout.whole_word);
        break;
      case RBC_OP_DIV_REG:
        rbc_idiv_reg(&core->& core->kill_core, layout.whole_word);
        break;
      case RBC_OP_MOD_IMM:
        rbc_imod_imm(&core->& core->kill_core, layout.whole_word);
        break;
      case RBC_OP_MOD_REG:
        rbc_imod_reg(&core->& core->kill_core, layout.whole_word);
        break;
      case RBC_OP_FADD:
        rbc_ifadd(&core->& core->kill_core, layout.whole_word);
        break;
      case RBC_OP_FSUB:
        rbc_ifsub(&core->& core->kill_core, layout.whole_word);
        break;
      case RBC_OP_FMUL:
        rbc_ifmul(&core->& core->kill_core, layout.whole_word);
        break;
      case RBC_OP_FDIV:
        rbc_ifdiv(&core->& core->kill_core, layout.whole_word);
        break;
      case RBC_OP_FADD32:
        rbc_ifadd32(&core->& core->kill_core, layout.whole_word);
        break;
      case RBC_OP_FSUB32:
        rbc_ifsub32(&core->& core->kill_core, layout.whole_word);
        break;
      case RBC_OP_FMUL32:
        rbc_ifmul32(&core->& core->kill_core, layout.whole_word);
        break;
      case RBC_OP_FDIV32:
        rbc_ifdiv32(&core->& core->kill_core, layout.whole_word);
        break;
      case RBC_OP_ADD_MEMB:
        rbc_iadd_memb(&core->& core->kill_core, layout.whole_word);
        break;
      case RBC_OP_ADD_MEMW:
        rbc_iadd_memw(&core->& core->kill_core, layout.whole_word);
        break;
      case RBC_OP_ADD_MEMD:
        rbc_iadd_memd(&core->& core->kill_core, layout.whole_word);
        break;
      case RBC_OP_ADD_MEMQ:
        rbc_iadd_memq(&core->& core->kill_core, layout.whole_word);
        break;
      case RBC_OP_SUB_MEMB:
        rbc_isub_memb(&core->& core->kill_core, layout.whole_word);
        break;
      case RBC_OP_SUB_MEMW:
        rbc_isub_memw(&core->& core->kill_core, layout.whole_word);
        break;
      case RBC_OP_SUB_MEMD:
        rbc_isub_memd(&core->& core->kill_core, layout.whole_word);
        break;
      case RBC_OP_SUB_MEMQ:
        rbc_isub_memq(&core->& core->kill_core, layout.whole_word);
        break;
      case RBC_OP_MUL_MEMB:
        rbc_imul_memb(&core->& core->kill_core, layout.whole_word);
        break;
      case RBC_OP_MUL_MEMW:
        rbc_imul_memw(&core->& core->kill_core, layout.whole_word);
        break;
      case RBC_OP_MUL_MEMD:
        rbc_imul_memd(&core->& core->kill_core, layout.whole_word);
        break;
      case RBC_OP_MUL_MEMQ:
        rbc_imul_memq(&core->& core->kill_core, layout.whole_word);
        break;
      case RBC_OP_DIV_MEMB:
        rbc_idiv_memb(&core->& core->kill_core, layout.whole_word);
        break;
      case RBC_OP_DIV_MEMW:
        rbc_idiv_memw(&core->& core->kill_core, layout.whole_word);
        break;
      case RBC_OP_DIV_MEMD:
        rbc_idiv_memd(&core->& core->kill_core, layout.whole_word);
        break;
      case RBC_OP_DIV_MEMQ:
        rbc_idiv_memq(&core->& core->kill_core, layout.whole_word);
        break;
      case RBC_OP_MOD_MEMB:
        rbc_imod_memb(&core->& core->kill_core, layout.whole_word);
        break;
      case RBC_OP_MOD_MEMW:
        rbc_imod_memw(&core->& core->kill_core, layout.whole_word);
        break;
      case RBC_OP_MOD_MEMD:
        rbc_imod_memd(&core->& core->kill_core, layout.whole_word);
        break;
      case RBC_OP_MOD_MEMQ:
        rbc_imod_memq(&core->& core->kill_core, layout.whole_word);
        break;
      case RBC_OP_FADD_MEM:
        rbc_ifadd64_mem(&core->& core->kill_core, layout.whole_word);
        break;
      case RBC_OP_FSUB_MEM:
        rbc_ifsub64_mem(&core->& core->kill_core, layout.whole_word);
        break;
      case RBC_OP_FMUL_MEM:
        rbc_ifmul64_mem(&core->& core->kill_core, layout.whole_word);
        break;
      case RBC_OP_FDIV_MEM:
        rbc_ifdiv64_mem(&core->& core->kill_core, layout.whole_word);
        break;
      case RBC_OP_FADD32_MEM:
        rbc_ifadd32_mem(&core->& core->kill_core, layout.whole_word);
        break;
      case RBC_OP_FSUB32_MEM:
        rbc_ifsub32_mem(&core->& core->kill_core, layout.whole_word);
        break;
      case RBC_OP_FMUL32_MEM:
        rbc_ifmul32_mem(&core->& core->kill_core, layout.whole_word);
        break;
      case RBC_OP_FDIV32_MEM:
        rbc_ifdiv32_mem(&core->& core->kill_core, layout.whole_word);
        break;
      case RBC_OP_INC:
        core->REGISTER_FILE[layout.bytes.b7 & RBC_R15]++;
        break;
      case RBC_OP_DEC:
        core->REGISTER_FILE[layout.bytes.b7 & RBC_R15]--;
        break;
      case RBC_OP_MOVE_IMM_64:
      case RBC_OP_MOVF:
        rbc_imov_imm(&core->& core->kill_core, layout.whole_word);
        break;
      case RBC_OP_MOVF32:
        rbc_imovf32(&core->& core->kill_core, layout.whole_word);
        break;
      case RBC_OP_MOVE_REG:
        core->REGISTER_FILE[layout.bytes.b6 & RBC_R15] =
            core->REGISTER_FILE[layout.bytes.b7 & RBC_R15];
        break;
      case RBC_OP_MOVE_REG8:
        core->REGISTER_FILE[layout.bytes.b6 & RBC_R15] =
            core->REGISTER_FILE[layout.bytes.b7 & RBC_R15] & 255;
        break;
      case RBC_OP_MOVE_REG16:
        core->REGISTER_FILE[layout.bytes.b6 & RBC_R15] =
            core->REGISTER_FILE[layout.bytes.b7 & RBC_R15] & 0xFFFF;
        break;
      case RBC_OP_MOVE_REG32:
        core->REGISTER_FILE[layout.bytes.b6 & RBC_R15] =
            core->REGISTER_FILE[layout.bytes.b7 & RBC_R15] & 0xFFFFFFFF;
        break;
      case RBC_OP_MOVESX_IMM8:
        rbc_imovesx_imm8(&core->& core->kill_core, layout.whole_word);
        break;
      case RBC_OP_MOVESX_IMM16:
        rbc_imovesx_imm16(&core->& core->kill_core, layout.whole_word);
        break;
      case RBC_OP_MOVESX_IMM32:
        rbc_imovesx_imm32(&core->& core->kill_core, layout.whole_word);
        break;
      case RBC_OP_MOVESX_REG8:
        rbc_imovesx_reg8(&core->& core->kill_core, layout.whole_word);
        break;
      case RBC_OP_MOVESX_REG16:
        rbc_imovesx_reg16(&core->& core->kill_core, layout.whole_word);
        break;
      case RBC_OP_MOVESX_REG32:
        rbc_imovesx_reg32(&core->& core->kill_core, layout.whole_word);
        break;
      case RBC_OP_EXCG8:
        rbc_iexcg8(&core->& core->kill_core, layout.whole_word);
        break;
      case RBC_OP_EXCG16:
        rbc_iexcg16(&core->& core->kill_core, layout.whole_word);
        break;
      case RBC_OP_EXCG32:
        rbc_iexcg32(&core->& core->kill_core, layout.whole_word);
        break;
      case RBC_OP_EXCG:
        rbc_iexcg(&core->& core->kill_core, layout.whole_word);
        break;
      case RBC_OP_MOV8:
        core->REGISTER_FILE[layout.bytes.b6 & RBC_R15] &=
            (0xFFFFFFFFFFFFFF00 |
             (core->REGISTER_FILE[layout.bytes.b7 & RBC_R15] & 0xFF));
        break;
      case RBC_OP_MOV16:
        core->REGISTER_FILE[layout.bytes.b6 & RBC_R15] &=
            (0xFFFFFFFFFFFF0000 |
             (core->REGISTER_FILE[layout.bytes.b7 & RBC_R15] & 0xFFFF));
        break;
      case RBC_OP_MOV32:
        core->REGISTER_FILE[layout.bytes.b6 & RBC_R15] &=
            (0xFFFFFFFF00000000 |
             (core->REGISTER_FILE[layout.bytes.b7 & RBC_R15] & 0xFFFFFFFF));
        break;
      case RBC_OP_MOVNZ:
      case RBC_OP_MOVNE:
        if (core->flags.flags.zero == 0)
          rbc_imov_imm(&core->& core->kill_core, layout.whole_word);
        break;
      case RBC_OP_MOVE:
      case RBC_OP_MOVZ:
        if (core->flags.flags.zero == 1)
          rbc_imov_imm(&core->& core->kill_core, layout.whole_word);
        break;
      case RBC_OP_MOVNC:
        if (core->flags.flags.carry == 0)
          rbc_imov_imm(&core->& core->kill_core, layout.whole_word);
        break;
      case RBC_OP_MOVC:
        if (core->flags.flags.carry == 1)
          rbc_imov_imm(&core->& core->kill_core, layout.whole_word);
        break;
      case RBC_OP_MOVNO:
        if (core->flags.flags.overflow == 0)
          rbc_imov_imm(&core->& core->kill_core, layout.whole_word);
        break;
      case RBC_OP_MOVO:
        if (core->flags.flags.overflow == 1)
          rbc_imov_imm(&core->& core->kill_core, layout.whole_word);
        break;
      case RBC_OP_MOVNN:
        if (core->flags.flags.negative == 0)
          rbc_imov_imm(&core->& core->kill_core, layout.whole_word);
        break;
      case RBC_OP_MOVN:
        if (core->flags.flags.negative == 1)
          rbc_imov_imm(&core->& core->kill_core, layout.whole_word);
        break;
      case RBC_OP_MOVS:
      case RBC_OP_MOVNG:
        if (core->flags.flags.negative == 1)
          rbc_imov_imm(&core->& core->kill_core, layout.whole_word);
        break;
      case RBC_OP_MOVNS:
      case RBC_OP_MOVG:
        if (core->flags.flags.negative == 0)
          rbc_imov_imm(&core->& core->kill_core, layout.whole_word);
        break;
      case RBC_OP_MOVGE:
        if (core->flags.flags.negative == 0 || core->flags.flags.zero == 1)
          rbc_imov_imm(&core->& core->kill_core, layout.whole_word);
        break;
      case RBC_OP_MOVSE:
        if (core->flags.flags.zero == 1 || core->flags.flags.negative == 1)
          rbc_imov_imm(&core->& core->kill_core, layout.whole_word);
        break;
      case RBC_OP_JMP_OFF:
        sign_extend32(layout.half_words.w1);
        core->PC += layout.whole_word;
        continue;
      case RBC_OP_JMP_ADDR:
        core->PC = layout.whole_word & 0xFFFFFFFFFFFF;
        continue;
      case RBC_OP_JZ:
      case RBC_OP_JE:
        // the address to jmp should follow the layout.whole_word
        if (core->flags.flags.zero == 1) {
          core->PC = (layout.whole_word & 0xFFFFFFFFFFFF);
          continue;
        }
        break;
      case RBC_OP_JNZ:
      case RBC_OP_JNE:
        // the address to jmp should follow the layout.whole_word
        if (core->flags.flags.zero == 0) {
          core->PC = (layout.whole_word & 0xFFFFFFFFFFFF);
          continue;
        }
        break;
      case RBC_OP_JNC:
        if (core->flags.flags.carry == 0) {
          core->PC = (layout.whole_word & 0xFFFFFFFFFFFF);
          continue;
        }
        break;
      case RBC_OP_JC:
        if (core->flags.flags.carry == 1) {
          core->PC = (layout.whole_word & 0xFFFFFFFFFFFF);
          continue;
        }
        break;
      case RBC_OP_JNO:
        if (core->flags.flags.overflow == 0) {
          core->PC = (layout.whole_word & 0xFFFFFFFFFFFF);
          continue;
        }
        break;
      case RBC_OP_JO:
        if (core->flags.flags.overflow == 1) {
          core->PC = (layout.whole_word & 0xFFFFFFFFFFFF);
          continue;
        }
        break;
      case RBC_OP_JNN:
        if (core->flags.flags.negative == 0) {
          core->PC = (layout.whole_word & 0xFFFFFFFFFFFF);
          continue;
        }
        break;
      case RBC_OP_JN:
        if (core->flags.flags.negative == 1) {
          core->PC = (layout.whole_word & 0xFFFFFFFFFFFF);
          continue;
        }
        break;
      case RBC_OP_JS:
      case RBC_OP_JNG:
        if (core->flags.flags.negative == 1) {
          core->PC = (layout.whole_word & 0xFFFFFFFFFFFF);
          continue;
        }
        break;
      case RBC_OP_JNS:
      case RBC_OP_JG:
        if (core->flags.flags.negative == 0) {
          core->PC = (layout.whole_word & 0xFFFFFFFFFFFF);
          continue;
        }
        break;
      case RBC_OP_JGE:
        if (core->flags.flags.negative == 0 || core->flags.flags.zero == 0) {
          core->PC = (layout.whole_word & 0xFFFFFFFFFFFF);
          continue;
        }
        break;
      case RBC_OP_JSE:
        if (core->flags.flags.negative == 1 || core->flags.flags.zero == 0) {
          core->PC = (layout.whole_word & 0xFFFFFFFFFFFF);
          continue;
        }
        break;
      case RBC_OP_CALL:
        rbc_icall(&core->& core->kill_core, layout.whole_word);
        continue;
      case RBC_OP_RET:
        rbc_iret(&core->& core->kill_core, layout.whole_word);
        break;
      case RBC_OP_RETZ:
      case RBC_OP_RETE:
        if (core->flags.flags.zero == 1)
          rbc_iret(&core->& core->kill_core, layout.whole_word);
        break;
      case RBC_OP_RETNZ:
      case RBC_OP_RETNE:
        if (core->flags.flags.zero == 0)
          rbc_iret(&core->& core->kill_core, layout.whole_word);
        break;
      case RBC_OP_RETNC:
        if (core->flags.flags.carry == 0)
          rbc_iret(&core->& core->kill_core, layout.whole_word);
        break;
      case RBC_OP_RETC:
        if (core->flags.flags.carry == 1)
          rbc_iret(&core->& core->kill_core, layout.whole_word);
        break;
      case RBC_OP_RETNO:
        if (core->flags.flags.overflow == 0)
          rbc_iret(&core->& core->kill_core, layout.whole_word);
        break;
      case RBC_OP_RETO:
        if (core->flags.flags.overflow == 1)
          rbc_iret(&core->& core->kill_core, layout.whole_word);
        break;
      case RBC_OP_RETNN:
        if (core->flags.flags.negative == 0)
          rbc_iret(&core->& core->kill_core, layout.whole_word);
        break;
      case RBC_OP_RETN:
        if (core->flags.flags.negative == 1)
          rbc_iret(&core->& core->kill_core, layout.whole_word);
        break;
      case RBC_OP_RETS:
      case RBC_OP_RETNG:
        if (core->flags.flags.negative == 1)
          rbc_iret(&core->& core->kill_core, layout.whole_word);
        break;
      case RBC_OP_RETNS:
      case RBC_OP_RETG:
        if (core->flags.flags.negative == 0)
          rbc_iret(&core->& core->kill_core, layout.whole_word);
        break;
      case RBC_OP_RETGE:
        if (core->flags.flags.negative == 0 || core->flags.flags.zero == 1)
          rbc_iret(&core->& core->kill_core, layout.whole_word);
        break;
      case RBC_OP_RETSE:
        if (core->flags.flags.negative == 1 || core->flags.flags.zero == 1)
          rbc_iret(&core->& core->kill_core, layout.whole_word);
        break;
      case RBC_OP_LOOP:
        if (core->REGISTER_FILE[RBC_R2] != 0) {
          core->PC = layout.whole_word & 0xFFFFFFFFFFFF;
          continue;
        }
        break;
      case RBC_OP_CALL_REG:
        rbc_icall_reg(&core->& core->kill_core, layout.whole_word);
        continue;
      case RBC_OP_JMP_REGR:
        core->PC = core->REGISTER_FILE[layout.bytes.b7 & RBC_R15];
        continue;
      case RBC_OP_PUSH_IMM64:
        rbc_ipush_immq(&core->& core->kill_core, layout.whole_word);
        break;
      case RBC_OP_PUSH_REG:
        rbc_ipush_reg(&core->& core->kill_core, layout.whole_word);
        break;
      case RBC_OP_POP64:
        rbc_ipopq(&core->& core->kill_core, layout.whole_word);
        break;
      case RBC_OP_PUSHA:
        rbc_ipusha(&core->& core->kill_core, layout.whole_word);
        break;
      case RBC_OP_POPA:
        rbc_ipopa(&core->& core->kill_core, layout.whole_word);
        break;
      case RBC_OP_PUSH_MEMB:
        rbc_ipush_memb(&core->& core->kill_core, layout.whole_word);
        break;
      case RBC_OP_PUSH_MEMW:
        rbc_ipush_memw(&core->& core->kill_core, layout.whole_word);
        break;
      case RBC_OP_PUSH_MEMD:
        rbc_ipush_memd(&core->& core->kill_core, layout.whole_word);
        break;
      case RBC_OP_PUSH_MEMQ:
        rbc_ipush_memq(&core->& core->kill_core, layout.whole_word);
        break;
      case RBC_OP_POP_MEMB:
        rbc_ipop_memb(&core->& core->kill_core, layout.whole_word);
        break;
      case RBC_OP_POP_MEMW:
        rbc_ipop_memw(&core->& core->kill_core, layout.whole_word);
        break;
      case RBC_OP_POP_MEMD:
        rbc_ipop_memd(&core->& core->kill_core, layout.whole_word);
        break;
      case RBC_OP_POP_MEMQ:
        rbc_ipop_memq(&core->& core->kill_core, layout.whole_word);
        break;
      case RBC_OP_LOADSQ:
        rbc_iloadsq(&core->& core->kill_core, layout.whole_word);
        break;
      case RBC_OP_STORESQ:
        rbc_istoresq(&core->& core->kill_core, layout.whole_word);
        break;
      case RBC_OP_AND_IMM:
        rbc_icmp_imm(&core->& core->kill_core, layout.whole_word);
        break;
      case RBC_OP_AND_REG:
        core->REGISTER_FILE[layout.bytes.b6 & RBC_R15] &=
            core->REGISTER_FILE[layout.bytes.b7 & RBC_R15];
        break;
      case RBC_OP_OR_IMM:
        rbc_ior_imm(&core->& core->kill_core, layout.whole_word);
        break;
      case RBC_OP_OR_REG:
        core->REGISTER_FILE[layout.bytes.b6 & RBC_R15] |=
            core->REGISTER_FILE[layout.bytes.b7 & RBC_R15];
        break;
      case RBC_OP_XOR_IMM:
        rbc_ixor_imm(&core->& core->kill_core, layout.whole_word);
        break;
      case RBC_OP_XOR_REG:
        core->REGISTER_FILE[layout.bytes.b6 & RBC_R15] ^=
            core->REGISTER_FILE[layout.bytes.b7 & RBC_R15];
        break;
      case RBC_OP_NOT:
        core->REGISTER_FILE[layout.bytes.b7 & RBC_R15] =
            ~core->REGISTER_FILE[layout.bytes.b7];
        break;
      case RBC_OP_LSHIFT:
        core->REGISTER_FILE[layout.bytes.b1 & RBC_R15] <<= layout.bytes.b7 & 63;
        break;
      case RBC_OP_RSHIFT:
        core->REGISTER_FILE[layout.bytes.b1 & RBC_R15] >>= layout.bytes.b7 & 63;
        break;
      case RBC_OP_LSHIFT_REGR:
        core->REGISTER_FILE[layout.bytes.b6 & RBC_R15] <<=
            core->REGISTER_FILE[layout.bytes.b7 & RBC_R15] & 63;
        break;
      case RBC_OP_RSHIFT_REGR:
        core->REGISTER_FILE[layout.bytes.b6 & RBC_R15] >>=
            core->REGISTER_FILE[layout.bytes.b7 & RBC_R15] & 63;
        break;
      case RBC_OP_CMP_IMM:
        rbc_icmp_imm(&core->& core->kill_core, layout.whole_word);
        break;
      case RBC_OP_CMP_REG:
        merry_compare_two_values(
            core->REGISTER_FILE[layout.bytes.b6 & RBC_R15],
            core->REGISTER_FILE[layout.bytes.b7 & RBC_R15]);
        core->flags.regr = merry_obtain_flags_regr();
        break;
      case RBC_OP_CMP_IMM_MEMB:
        rbc_icmp_imm_memb(&core->& core->kill_core, layout.whole_word);
        break;
      case RBC_OP_CMP_IMM_MEMW:
        rbc_icmp_imm_memw(&core->& core->kill_core, layout.whole_word);
        break;
      case RBC_OP_CMP_IMM_MEMD:
        rbc_icmp_imm_memd(&core->& core->kill_core, layout.whole_word);
        break;
      case RBC_OP_CMP_IMM_MEMQ:
        rbc_icmp_imm_memq(&core->& core->kill_core, layout.whole_word);
        break;
      case RBC_OP_FCMP:
        rbc_compare_f64(core->REGISTER_FILE[layout.bytes.b6 & RBC_R15],
                        core->REGISTER_FILE[layout.bytes.b7 & RBC_R15],
                        &core->fflags);
        break;
      case RBC_OP_FCMP32:
        rbc_compare_f32(core->REGISTER_FILE[layout.bytes.b6 & RBC_R15],
                        core->REGISTER_FILE[layout.bytes.b7 & RBC_R15],
                        &core->fflags);
        break;
      case RBC_OP_CIN:
        core->REGISTER_FILE[layout.bytes.b7 & RBC_R15] = getchar();
        break;
      case RBC_OP_COUT:
        putchar((int)core->REGISTER_FILE[layout.bytes.b7 & RBC_R15]);
        break;
      case RBC_OP_SIN_REG:
        layout.whole_word = (core->REGISTER_FILE[layout.bytes.b7 & RBC_R15]);
      case RBC_OP_SIN:
        rbc_isin(&core->& core->kill_core, layout.whole_word);
        break;
      case RBC_OP_SOUT_REG:
        layout.bytes.b7 =
            (core->REGISTER_FILE[layout.bytes.b7 & RBC_R15] & 0xFFFFFFFFFFFF);
      case RBC_OP_SOUT:
        rbc_isout(&core->& core->kill_core, layout.whole_word);
        break;
      case RBC_OP_IN:
        fscanf(stdin, "%hhi", &mem.bytes.b7);
        core->REGISTER_FILE[layout.bytes.b7 & RBC_R15] = mem.whole_word;
        break;
      case RBC_OP_OUT:
        mem.whole_word = core->REGISTER_FILE[layout.bytes.b7 & RBC_R15];
        fprintf(stdout, "%hhi", mem.bytes.b7);
        break;
      case RBC_OP_INW:
        fscanf(stdin, "%hd", &mem.half_half_words.w3);
        core->REGISTER_FILE[layout.bytes.b7 & RBC_R15] = mem.whole_word;
        break;
      case RBC_OP_OUTW:
        mem.whole_word = core->REGISTER_FILE[layout.bytes.b7 & RBC_R15];
        fprintf(stdout, "%hd", mem.half_half_words.w3);
        break;
      case RBC_OP_IND:
        fscanf(stdin, "%d", &mem.half_words.w1);
        core->REGISTER_FILE[layout.bytes.b7 & RBC_R15] = mem.whole_word;
        break;
      case RBC_OP_OUTD:
        mem.whole_word = core->REGISTER_FILE[layout.bytes.b7 & RBC_R15];
        fprintf(stdout, "%d", mem.half_words.w1);
        break;
      case RBC_OP_INQ:
        fscanf(stdin, "%zi", &core->REGISTER_FILE[layout.bytes.b7 & RBC_R15]);
        break;
      case RBC_OP_OUTQ:
        fprintf(stdout, "%zi", core->REGISTER_FILE[layout.bytes.b7 & RBC_R15]);
        break;
      case RBC_OP_UIN:
        fscanf(stdin, "%hhu", &mem.bytes.b7);
        core->REGISTER_FILE[layout.bytes.b7 & RBC_R15] = mem.whole_word;
        break;
      case RBC_OP_UOUT:
        mem.whole_word = core->REGISTER_FILE[layout.bytes.b7 & RBC_R15];
        fprintf(stdout, "%hhu", mem.bytes.b7);
        break;
      case RBC_OP_UINW:
        fscanf(stdin, "%hu", &mem.half_half_words.w3);
        core->REGISTER_FILE[layout.bytes.b7 & RBC_R15] = mem.whole_word;
        break;
      case RBC_OP_UOUTW:
        mem.whole_word = core->REGISTER_FILE[layout.bytes.b7 & RBC_R15];
        fprintf(stdout, "%hu", mem.half_half_words.w3);
        break;
      case RBC_OP_UIND:
        fscanf(stdin, "%u", &mem.half_words.w1);
        core->REGISTER_FILE[layout.bytes.b7 & RBC_R15] = mem.whole_word;
        break;
      case RBC_OP_UOUTD:
        mem.whole_word = core->REGISTER_FILE[layout.bytes.b7 & RBC_R15];
        fprintf(stdout, "%u", mem.half_words.w1);
        break;
      case RBC_OP_UINQ:
        fscanf(stdin, "%zu", &core->REGISTER_FILE[layout.bytes.b7 & RBC_R15]);
        break;
      case RBC_OP_UOUTQ:
        fprintf(stdout, "%zu", core->REGISTER_FILE[layout.bytes.b7 & RBC_R15]);
        break;
      case RBC_OP_INF:
        fscanf(stdin, "%lf", &dtoq.d_val);
        core->REGISTER_FILE[layout.bytes.b7 & RBC_R15] = dtoq.q_val;
        break;
      case RBC_OP_OUTF:
        dtoq.q_val = core->REGISTER_FILE[layout.bytes.b7 & RBC_R15];
        fprintf(stdout, "%lf", dtoq.d_val);
        break;
      case RBC_OP_INF32:
        fscanf(stdin, "%f", &ftod.fl_val);
        core->REGISTER_FILE[layout.bytes.b7 & RBC_R15] = ftod.d_val;
        break;
      case RBC_OP_OUTF32:
        ftod.d_val = core->REGISTER_FILE[layout.bytes.b7 & RBC_R15];
        fprintf(stdout, "%f", ftod.fl_val);
        break;
      case RBC_OP_OUTR:
        for (msize_t i = 0; i <= RBC_R15; i++)
          fprintf(stdout, "%zi\n", core->REGISTER_FILE[i]);
        break;
      case RBC_OP_UOUTR:
        for (msize_t i = 0; i <= RBC_R15; i++)
          fprintf(stdout, "%zu\n", core->REGISTER_FILE[i]);
        break;
      case RBC_OP_LOADB:
        rbc_iloadb(&core->& core->kill_core, layout.whole_word);
        break;
      case RBC_OP_LOADW:
        rbc_iloadw(&core->& core->kill_core, layout.whole_word);
        break;
      case RBC_OP_LOADD:
        rbc_iloadd(&core->& core->kill_core, layout.whole_word);
        break;
      case RBC_OP_LOADQ:
        rbc_iloadq(&core->& core->kill_core, layout.whole_word);
        break;
      case RBC_OP_STOREB:
        rbc_istoreb(&core->& core->kill_core, layout.whole_word);
        break;
      case RBC_OP_STOREW:
        rbc_istorew(&core->& core->kill_core, layout.whole_word);
        break;
      case RBC_OP_STORED:
        rbc_istored(&core->& core->kill_core, layout.whole_word);
        break;
      case RBC_OP_STOREQ:
        rbc_istoreq(&core->& core->kill_core, layout.whole_word);
        break;
      case RBC_OP_LOADB_REG:
        rbc_iloadb_reg(&core->& core->kill_core, layout.whole_word);
        break;
      case RBC_OP_LOADW_REG:
        rbc_iloadw_reg(&core->& core->kill_core, layout.whole_word);
        break;
      case RBC_OP_LOADD_REG:
        rbc_iloadd_reg(&core->& core->kill_core, layout.whole_word);
        break;
      case RBC_OP_LOADQ_REG:
        rbc_iloadq_reg(&core->& core->kill_core, layout.whole_word);
        break;
      case RBC_OP_STOREB_REG:
        rbc_istoreb_reg(&core->& core->kill_core, layout.whole_word);
        break;
      case RBC_OP_STOREW_REG:
        rbc_istorew_reg(&core->& core->kill_core, layout.whole_word);
        break;
      case RBC_OP_STORED_REG:
        rbc_istored_reg(&core->& core->kill_core, layout.whole_word);
        break;
      case RBC_OP_STOREQ_REG:
        rbc_istoreq_reg(&core->& core->kill_core, layout.whole_word);
        break;
      case RBC_OP_ATOMIC_LOADB:
        rbc_iatm_loadb(&core->& core->kill_core, layout.whole_word);
        break;
      case RBC_OP_ATOMIC_LOADW:
        rbc_iatm_loadw(&core->& core->kill_core, layout.whole_word);
        break;
      case RBC_OP_ATOMIC_LOADD:
        rbc_iatm_loadd(&core->& core->kill_core, layout.whole_word);
        break;
      case RBC_OP_ATOMIC_LOADQ:
        rbc_iatm_loadq(&core->& core->kill_core, layout.whole_word);
        break;
      case RBC_OP_ATOMIC_STOREB:
        rbc_iatm_storeb(&core->& core->kill_core, layout.whole_word);
        break;
      case RBC_OP_ATOMIC_STOREW:
        rbc_iatm_storew(&core->& core->kill_core, layout.whole_word);
        break;
      case RBC_OP_ATOMIC_STORED:
        rbc_iatm_stored(&core->& core->kill_core, layout.whole_word);
        break;
      case RBC_OP_ATOMIC_STOREQ:
        rbc_iatm_storeq(&core->& core->kill_core, layout.whole_word);
        break;
      case RBC_OP_ATOMIC_LOADB_REG:
        rbc_iatm_loadb_reg(&core->& core->kill_core, layout.whole_word);
        break;
      case RBC_OP_ATOMIC_LOADW_REG:
        rbc_iatm_loadw_reg(&core->& core->kill_core, layout.whole_word);
        break;
      case RBC_OP_ATOMIC_LOADD_REG:
        rbc_iatm_loadd_reg(&core->& core->kill_core, layout.whole_word);
        break;
      case RBC_OP_ATOMIC_LOADQ_REG:
        rbc_iatm_loadq_reg(&core->& core->kill_core, layout.whole_word);
        break;
      case RBC_OP_ATOMIC_STOREB_REG:
        rbc_iatm_storeb_reg(&core->& core->kill_core, layout.whole_word);
        break;
      case RBC_OP_ATOMIC_STOREW_REG:
        rbc_iatm_storew_reg(&core->& core->kill_core, layout.whole_word);
        break;
      case RBC_OP_ATOMIC_STORED_REG:
        rbc_iatm_stored_reg(&core->& core->kill_core, layout.whole_word);
        break;
      case RBC_OP_ATOMIC_STOREQ_REG:
        rbc_iatm_storeq_reg(&core->& core->kill_core, layout.whole_word);
        break;
      case RBC_OP_LEA:
        core->REGISTER_FILE[layout.bytes.b4 & RBC_R15] =
            core->REGISTER_FILE[layout.bytes.b5 & RBC_R15] +
            core->REGISTER_FILE[layout.bytes.b6 & RBC_R15] *
                core->REGISTER_FILE[layout.bytes.b7 & RBC_R15];
        break;
      case RBC_OP_CFLAGS:
        core->flags.flags.carry = 0;
        core->flags.flags.negative = 0;
        core->flags.flags.zero = 0;
        core->flags.flags.overflow = 0;
        break;
      case RBC_OP_RESET:
        for (msize_t i = 0; i < RBC_REG_COUNT; i++)
          core->REGISTER_FILE[i] = 0;
        break;
      case RBC_OP_CMPXCHG_REGR:
        rbc_icmpxchg_reg(&core->& core->kill_core, layout.whole_word);
        break;
      case RBC_OP_CMPXCHG:
        rbc_icmpxchg(&core->& core->kill_core, layout.whole_word);
        break;
      case 250:
      case 251:
      case 252:
      case 253:
      case 254:
        break;
      case 255: {
        switch (layout.bytes.b1) {
        case RBC_OP_MOVFZ:
          if (core->fflags.zf)
            rbc_imov_imm(&core->& core->kill_core, layout.whole_word);
          break;
        case RBC_OP_MOVFNZ:
          if (!core->fflags.zf)
            rbc_imov_imm(&core->& core->kill_core, layout.whole_word);
          break;
        case RBC_OP_MOVFN:
          if (core->fflags.sf)
            rbc_imov_imm(&core->& core->kill_core, layout.whole_word);
          break;
        case RBC_OP_MOVFNN:
          if (!core->fflags.sf)
            rbc_imov_imm(&core->& core->kill_core, layout.whole_word);
          break;
        case RBC_OP_MOVFUF:
          if (core->fflags.uof)
            rbc_imov_imm(&core->& core->kill_core, layout.whole_word);
          break;
        case RBC_OP_MOVFNUF:
          if (!core->fflags.uof)
            rbc_imov_imm(&core->& core->kill_core, layout.whole_word);
          break;
        case RBC_OP_MOVFO:
          if (core->fflags.of)
            rbc_imov_imm(&core->& core->kill_core, layout.whole_word);
          break;
        case RBC_OP_MOVFNO:
          if (!core->fflags.of)
            rbc_imov_imm(&core->& core->kill_core, layout.whole_word);
          break;
        case RBC_OP_MOVFU:
          if (core->fflags.uf)
            rbc_imov_imm(&core->& core->kill_core, layout.whole_word);
          break;
        case RBC_OP_MOVFNU:
          if (!core->fflags.uf)
            rbc_imov_imm(&core->& core->kill_core, layout.whole_word);
          break;
        case RBC_OP_MOVFI:
          if (core->fflags.inv)
            rbc_imov_imm(&core->& core->kill_core, layout.whole_word);
          break;
        case RBC_OP_MOVFNI:
          if (!core->fflags.inv)
            rbc_imov_imm(&core->& core->kill_core, layout.whole_word);
          break;
        case RBC_OP_JFZ:
          if (core->fflags.zf) {
            core->PC = (layout.whole_word & 0xFFFFFFFFFFFF);
            continue;
          }
          break;
        case RBC_OP_JFNZ:
          if (!core->fflags.zf) {
            core->PC = (layout.whole_word & 0xFFFFFFFFFFFF);
            continue;
          }
          break;
        case RBC_OP_JFN:
          if (core->fflags.sf) {
            core->PC = (layout.whole_word & 0xFFFFFFFFFFFF);
            continue;
          }
          break;
        case RBC_OP_JFNN:
          if (!core->fflags.sf) {
            core->PC = (layout.whole_word & 0xFFFFFFFFFFFF);
            continue;
          }
          break;
        case RBC_OP_JFUF:
          if (core->fflags.uof) {
            core->PC = (layout.whole_word & 0xFFFFFFFFFFFF);
            continue;
          }
          break;
        case RBC_OP_JFNUF:
          if (!core->fflags.uof) {
            core->PC = (layout.whole_word & 0xFFFFFFFFFFFF);
            continue;
          }
          break;
        case RBC_OP_JFO:
          if (core->fflags.of) {
            core->PC = (layout.whole_word & 0xFFFFFFFFFFFF);
            continue;
          }
          break;
        case RBC_OP_JFNO:
          if (!core->fflags.of) {
            core->PC = (layout.whole_word & 0xFFFFFFFFFFFF);
            continue;
          }
          break;
        case RBC_OP_JFU:
          if (core->fflags.uf) {
            core->PC = (layout.whole_word & 0xFFFFFFFFFFFF);
            continue;
          }
          break;
        case RBC_OP_JFNU:
          if (!core->fflags.uf) {
            core->PC = (layout.whole_word & 0xFFFFFFFFFFFF);
            continue;
          }
          break;
        case RBC_OP_JFI:
          if (core->fflags.inv) {
            core->PC = (layout.whole_word & 0xFFFFFFFFFFFF);
            continue;
          }
          break;
        case RBC_OP_JFNI:
          if (!core->fflags.inv) {
            core->PC = (layout.whole_word & 0xFFFFFFFFFFFF);
            continue;
          }
          break;
        case RBC_OP_RETFZ:
          if (core->fflags.zf)
            rbc_iret(&core->& core->kill_core, layout.whole_word);
          break;
        case RBC_OP_RETFNZ:
          if (!core->fflags.zf)
            rbc_iret(&core->& core->kill_core, layout.whole_word);
          break;
        case RBC_OP_RETFN:
          if (core->fflags.sf)
            rbc_iret(&core->& core->kill_core, layout.whole_word);
          break;
        case RBC_OP_RETFNN:
          if (!core->fflags.sf)
            rbc_iret(&core->& core->kill_core, layout.whole_word);
          break;
        case RBC_OP_RETFUF:
          if (core->fflags.uof)
            rbc_iret(&core->& core->kill_core, layout.whole_word);
          break;
        case RBC_OP_RETFNUF:
          if (!core->fflags.uof)
            rbc_iret(&core->& core->kill_core, layout.whole_word);
          break;
        case RBC_OP_RETFO:
          if (core->fflags.of)
            rbc_iret(&core->& core->kill_core, layout.whole_word);
          break;
        case RBC_OP_RETFNO:
          if (!core->fflags.of)
            rbc_iret(&core->& core->kill_core, layout.whole_word);
          break;
        case RBC_OP_RETFU:
          if (core->fflags.uf)
            rbc_iret(&core->& core->kill_core, layout.whole_word);
          break;
        case RBC_OP_RETFNU:
          if (!core->fflags.uf)
            rbc_iret(&core->& core->kill_core, layout.whole_word);
          break;
        case RBC_OP_RETFI:
          if (core->fflags.inv)
            rbc_iret(&core->& core->kill_core, layout.whole_word);
          break;
        case RBC_OP_RETFNI:
          if (!core->fflags.inv)
            rbc_iret(&core->& core->kill_core, layout.whole_word);
          break;
        default:
          break;
        }
      }
      }
      core->PC += 8;
    }
  }
  core->terminate = mtrue;
  core->interrupt = mtrue;
  for (msize_t i = 0;
       i < merry_RBCThread_list_size(core->rbc_core->child_threads); i++) {
    merry_thread_join(core->rbc_core->child_threads->buf[i], NULL);
  }
  return RET_SUCCESS;
}

MerryCoreBase *rbc_core_create_base(MerryICRes *RES) {
  MerryCoreBase *base = (MerryCoreBase *)malloc(sizeof(MerryCoreBase));
  mICResSource_t source;
  msize_t ret;
  if (!base) {
    MFATAL("RBC", "Failed to initialize core base", NULL);
    ret = RBC_SYS_FAILURE;
    source = IC_SOURCE_CORE;
    goto RBC_BC_FAILED;
  }
  base->type = __REGR_CORE;
  base->createc = rbc_core_create;
  base->deletec = rbc_core_destroy;
  base->execc = rbc_core_run;
  base->predel = rbc_core_prep_for_deletion;
  base->setinp = rbc_core_set_input;
  base->prepcore = rbc_core_prepare_core;

  return base;
RBC_BC_FAILED:
  RES->source = source;
  RES->_core_code = ret;
  switch (source) {
  case IC_SOURCE_CORE:
    if (ret == RBC_SYS_FAILURE)
      RES->ERRNO = errno;
    break;
  case IC_SOURCE_MERRY:
    if (ret == MRES_SYS_FAILURE)
      RES->ERRNO = errno;
    break;
  case IC_SOURCE_INTERFACE:
    if (ret == INTERFACE_HOST_FAILURE)
      RES->ERRNO = errno;
    break;
  default:
    merry_unreachable();
  }
  return RET_NULL;
}

void rbc_core_destroy_base(MerryCoreBase *base) {
  merry_check_ptr(base);
  free(base);
}

void rbc_core_prep_for_deletion(mptr_t c) {
  // This function's job is to prepare the core
  // for deletion by setting the correct flags
  // and such. It is a procedure in of itself
  merry_check_ptr(c);

  RBCCore *core = (RBCCore *)c;

  // For now, this is everything we need
  // This will command the child cores to terminate
  // and since Graves will command the master core, it will
  // work
  core->terminate = mtrue;
  core->interrupt = mtrue;
}

mret_t rbc_core_set_input(mptr_t c, mstr_t path, MerryICRes *RES) {
  merry_check_ptr(c);
  merry_check_ptr(path);

  // For the moment, all it does is store the path of
  // the input Locally and allocate the reader
  RBCCore *core = (RBCCore *)c;
  core->rbc_core->inp_path = path;
  if ((core->inp = rbc_input_init()) == RET_NULL) {
    MLOG("RBC", "Failed to initialize the reader for input file %s", path);
    RES->source = IC_SOURCE_CORE;
    RES->ERRNO = errno;
    RES->_core_code = RBC_SYS_FAILURE;
    return RET_FAILURE;
  }
  return RET_SUCCESS;
}

mret_t rbc_core_prepare_core(mptr_t c, MerryICRes *RES) {
  // The only job of this function is to prepare the master
  // core for execution
  merry_check_ptr(c);

  RBCCore *core = (RBCCore *)c;

  if (!core->inp) {
    MFATAL("RBC", "Input not initialized before preparation", NULL);
    RES->source = IC_SOURCE_CORE;
    RES->_core_code = RBC_NOT_READY;
    return RET_FAILURE;
  }

  // Setup the RAMs
  if ((core->rbc_core->iram = rbc_memory_init()) == RET_NULL) {
    MFATAL("RBC", "Failed to initialize instrucion memory: PATH=%s",
           core->rbc_core->inp_path);
    RES->source = IC_SOURCE_CORE;
    RES->_core_code = RBC_SYS_FAILURE;
    RES->ERRNO = errno;
    return RET_FAILURE;
  }

  if ((core->rbc_core->dram = rbc_memory_init()) == RET_NULL) {
    MFATAL("RBC", "Failed to initialize data memory: PATH=%s",
           core->rbc_core->inp_path);
    RES->source = IC_SOURCE_CORE;
    RES->_core_code = RBC_SYS_FAILURE;
    RES->ERRNO = errno;
    return RET_FAILURE;
  }

  // Read the input file
  if ((rbc_input_read(core->inp, core->rbc_core->inp_path, RES)) !=
      RBC_SUCCESS) {
    MFATAL("RBC", "Failed to read input file %s", core->rbc_core->inp_path);
    return RET_FAILURE;
  }

  // Prep the memories
  if (rbc_memory_populate(core->rbc_core->iram, core->inp->instruction_len,
                          core->inp->instructions) == RET_FAILURE) {
    MFATAL("RBC", "Failed to populate the instrucion memory: PATH=%s",
           core->rbc_core->inp_path);
    RES->source = IC_SOURCE_CORE;
    RES->ERRNO = errno;
    RES->_core_code = RBC_SYS_FAILURE;
    return RET_FAILURE;
  }

  if (rbc_memory_populate(core->rbc_core->dram, core->inp->data_len,
                          core->inp->data) == RET_FAILURE) {
    MFATAL("RBC", "Failed to populate the data memory: PATH=%s",
           core->rbc_core->inp_path);
    RES->source = IC_SOURCE_CORE;
    RES->ERRNO = errno;
    RES->_core_code = RBC_SYS_FAILURE;
    return RET_FAILURE;
  }

  // Now other fields
  core->rbc_core->req.args = &core->rbc_core->args;
  core->rbc_core->req.base = core->base;
  core->rbc_core->req.used_cond = &core->private_cond;

  core->interrupt = mfalse;
  core->terminate = mfalse;
  core->pause = mfalse;
  core->rbc_core->terminate = mfalse;
  core->kill_core = mfalse;

  core->rbc_core->check_after = 3;

  return RET_SUCCESS;
}

RBCCore *rbc_core_create() {
  RBCCore *core = (RBCCore *)malloc(sizeof(RBCCore));
  if (!core) {
    MFATAL("RBC:C", "Failed to allocate memory for a new core", NULL);
    return RET_NULL;
  }
  mresult_t res;
  if ((res = merry_get_anonymous_memory((mptr_t *)&core->rbc_core->stack,
                                        _RBC_STACK_LEN_)) != MRES_SUCCESS) {
    MFATAL("RBC:C", "Failed to initialize the stack", NULL);
    merry_RBCThread_list_destroy(core->rbc_core->child_threads);
    merry_Interface_list_destroy(core->rbc_core->interfaces);
    free(core);
    return RET_NULL;
  }

  core->rbc_core->SP = 0;
  core->rbc_core->BP = 0;

  if ((res = merry_RBCProcFrame_stack_init(&core->rbc_core->stack_frames,
                                           _RBC_CALL_DEPTH_)) != MRES_SUCCESS) {
    MFATAL("RBC:C", "Failed to initialize the stack", NULL);
    merry_RBCThread_list_destroy(core->rbc_core->child_threads);
    merry_Interface_list_destroy(core->rbc_core->interfaces);
    merry_return_memory(core->rbc_core->stack, _RBC_STACK_LEN_);
    free(core);
    return RET_NULL;
  }

  if (merry_cond_init(&core->cond) != MRES_SUCCESS) {
    MFATAL("RBC:C", "Failed to initialize component", NULL);
    merry_RBCThread_list_destroy(core->rbc_core->child_threads);
    merry_Interface_list_destroy(core->rbc_core->interfaces);
    merry_return_memory(core->rbc_core->stack, _RBC_STACK_LEN_);
    merry_RBCProcFrame_stack_destroy(core->rbc_core->stack_frames);
    free(core);
    return RET_NULL;
  }
  return core;
}

void rbc_core_destroy(RBCCore *core) {
  merry_check_ptr(core);
  merry_cond_destroy(&core->cond);
  free(core);
}

_THRET_T_ rbc_core_run(mptr_t c) {
  merry_check_ptr(c);

  RBCCore *core = (RBCCore *)c;

  while (mtrue) {
  }

  for (msize_t i = 0;
       i < merry_RBCThread_list_size(core->rbc_core->child_threads); i++) {
    merry_thread_join(core->rbc_core->child_threads->buf[i], NULL);
  }
}

mret_t rbc_base_start(RBCCore *core, mthread_t *th) {
  merry_check_ptr(core);
  merry_check_ptr(th);
  return merry_thread_create(th, rbc_core_run, core);
}
