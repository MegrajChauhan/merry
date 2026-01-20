#include <merry_core.h>
#include <merry_core_instruction_handler.h>

_MERRY_INTERNAL_ 

_MERRY_DEFINE_STATIC_LIST_(Interface, MerryInterface *);
_MERRY_DEFINE_STACK_(CoreProcFrame, MerryCoreStackFrame);

mresult_t merry_core_create(maddress_t st_addr, MerryCore **core) {
  mresult_t ret;
  MerryCore *c = (MerryCore *)malloc(sizeof(MerryCore));
  if (!c) {
    MERR("Failed to allocate memory for the core", NULL);
    ret = MRES_SYS_FAILURE;
    goto MERRY_CC_FAILED;
  }
  if ((ret = merry_Interface_list_create(10, &c->interfaces)) !=
      MRES_SUCCESS) {
    MERR("Failed to allocate memory for a component", NULL);
    free(c);
    goto MERRY_CC_FAILED;
  }
  if ((ret = merry_mapped_memory_create(&c->st)) != MRES_SUCCESS) {
    MERR("Failed to initialize the stack", NULL);
    merry_Interface_list_destroy(c->interfaces);
    free(c);
    goto MERRY_CC_FAILED;
  }
  if ((ret = merry_mapped_memory_map(c->st, _MERRY_CORE_STACK_LEN_)) !=
      MRES_SUCCESS) {
    MERR("Failed to initialize the stack", NULL);
    merry_Interface_list_destroy(c->interfaces);
    merry_mapped_memory_destroy(c->st);
    free(c);
    goto MERRY_CC_FAILED;
  }
  merry_mapped_memory_obtain_ptr(c->st, &c->stack);
  if ((ret = merry_CoreProcFrame_stack_init(&c->stack_frames,
                                           _MERRY_CORE_CALL_DEPTH_)) != MRES_SUCCESS) {
    MERR("Failed to initialize the stack", NULL);
    merry_Interface_list_destroy(c->interfaces);
    merry_mapped_memory_unmap(c->st);
    merry_mapped_memory_destroy(c->st);
    free(c);
    goto MERRY_CC_FAILED;
  }

  c->SP = 0;
  c->BP = 0;
  c->PC = st_addr;
  *core = c;
  return MRES_SUCCESS;
MERRY_CC_FAILED:
  return ret;
}

void merry_core_destroy(MerryCore* c) {
  merry_check_ptr(c);

  merry_Interface_list_destroy(c->interfaces);
  merry_CoreProcFrame_stack_destroy(c->stack_frames);
  merry_core_memory_destroy(c->iram);
  merry_core_memory_destroy(c->dram);
  merry_mapped_memory_unmap(c->st);
  merry_mapped_memory_destroy(c->st);
  free(c);
}

msize_t merry_core_run(MerryCore* c) {
  merry_check_ptr(c);

  MerryHostMemLayout layout, mem;
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
            MERRY_memory_read_qword(core->iram, core->PC, &layout.whole_word) ==
            MERRY_MEM_OPER_ACCESS_INVALID)) {
      MERR(
          "MERRY",
          "Memory access invalid: Accessing address that doesn't exist PC=%zu",
          core->PC);
          ret = 1;
          break;
    } else {
      switch (layout.bytes.b0) {
      case MERRY_OP_NOP:
      case MERRY_OP_RES1:
      case MERRY_OP_RES2:
      case MERRY_OP_RES3:
      case MERRY_OP_RES4:
      case MERRY_OP_RES5:
      case MERRY_OP_RES6:
      case MERRY_OP_RES7:
      case MERRY_OP_RES8:
      case MERRY_OP_RES9:
      case MERRY_OP_RES10:
      case MERRY_OP_RES11:
      case MERRY_OP_RES12:
      case MERRY_OP_RES13:
      case MERRY_OP_RES14:
      case MERRY_OP_RES15:
      case MERRY_OP_RES16:
      case MERRY_OP_RES17:
      case MERRY_OP_RES18:
      case MERRY_OP_RES19:
      case MERRY_OP_RES20:
      case MERRY_OP_RES21:
      case MERRY_OP_RES22:
      case MERRY_OP_RES23:
      case MERRY_OP_RES24:
        break;
      case MERRY_OP_SYSINT:
        MERRY_isysint(core, );
        break;
      case MERRY_OP_MINT:
        MERRY_imint(&core->& core->kill_core);
        break;
      case MERRY_OP_ADD_IMM:
        MERRY_iadd_imm(&core->& core->kill_core, layout.whole_word);
        break;
      case MERRY_OP_ADD_REG:
        MERRY_iadd_reg(&core->& core->kill_core, layout.whole_word);
        break;
      case MERRY_OP_SUB_IMM:
        MERRY_isub_imm(&core->& core->kill_core, layout.whole_word);
        break;
      case MERRY_OP_SUB_REG:
        MERRY_isub_reg(&core->& core->kill_core, layout.whole_word);
        break;
      case MERRY_OP_MUL_IMM:
        MERRY_imul_imm(&core->& core->kill_core, layout.whole_word);
        break;
      case MERRY_OP_MUL_REG:
        MERRY_imul_reg(&core->& core->kill_core, layout.whole_word);
        break;
      case MERRY_OP_DIV_IMM:
        MERRY_idiv_imm(&core->& core->kill_core, layout.whole_word);
        break;
      case MERRY_OP_DIV_REG:
        MERRY_idiv_reg(&core->& core->kill_core, layout.whole_word);
        break;
      case MERRY_OP_MOD_IMM:
        MERRY_imod_imm(&core->& core->kill_core, layout.whole_word);
        break;
      case MERRY_OP_MOD_REG:
        MERRY_imod_reg(&core->& core->kill_core, layout.whole_word);
        break;
      case MERRY_OP_FADD:
        MERRY_ifadd(&core->& core->kill_core, layout.whole_word);
        break;
      case MERRY_OP_FSUB:
        MERRY_ifsub(&core->& core->kill_core, layout.whole_word);
        break;
      case MERRY_OP_FMUL:
        MERRY_ifmul(&core->& core->kill_core, layout.whole_word);
        break;
      case MERRY_OP_FDIV:
        MERRY_ifdiv(&core->& core->kill_core, layout.whole_word);
        break;
      case MERRY_OP_FADD32:
        MERRY_ifadd32(&core->& core->kill_core, layout.whole_word);
        break;
      case MERRY_OP_FSUB32:
        MERRY_ifsub32(&core->& core->kill_core, layout.whole_word);
        break;
      case MERRY_OP_FMUL32:
        MERRY_ifmul32(&core->& core->kill_core, layout.whole_word);
        break;
      case MERRY_OP_FDIV32:
        MERRY_ifdiv32(&core->& core->kill_core, layout.whole_word);
        break;
      case MERRY_OP_ADD_MEMB:
        MERRY_iadd_memb(&core->& core->kill_core, layout.whole_word);
        break;
      case MERRY_OP_ADD_MEMW:
        MERRY_iadd_memw(&core->& core->kill_core, layout.whole_word);
        break;
      case MERRY_OP_ADD_MEMD:
        MERRY_iadd_memd(&core->& core->kill_core, layout.whole_word);
        break;
      case MERRY_OP_ADD_MEMQ:
        MERRY_iadd_memq(&core->& core->kill_core, layout.whole_word);
        break;
      case MERRY_OP_SUB_MEMB:
        MERRY_isub_memb(&core->& core->kill_core, layout.whole_word);
        break;
      case MERRY_OP_SUB_MEMW:
        MERRY_isub_memw(&core->& core->kill_core, layout.whole_word);
        break;
      case MERRY_OP_SUB_MEMD:
        MERRY_isub_memd(&core->& core->kill_core, layout.whole_word);
        break;
      case MERRY_OP_SUB_MEMQ:
        MERRY_isub_memq(&core->& core->kill_core, layout.whole_word);
        break;
      case MERRY_OP_MUL_MEMB:
        MERRY_imul_memb(&core->& core->kill_core, layout.whole_word);
        break;
      case MERRY_OP_MUL_MEMW:
        MERRY_imul_memw(&core->& core->kill_core, layout.whole_word);
        break;
      case MERRY_OP_MUL_MEMD:
        MERRY_imul_memd(&core->& core->kill_core, layout.whole_word);
        break;
      case MERRY_OP_MUL_MEMQ:
        MERRY_imul_memq(&core->& core->kill_core, layout.whole_word);
        break;
      case MERRY_OP_DIV_MEMB:
        MERRY_idiv_memb(&core->& core->kill_core, layout.whole_word);
        break;
      case MERRY_OP_DIV_MEMW:
        MERRY_idiv_memw(&core->& core->kill_core, layout.whole_word);
        break;
      case MERRY_OP_DIV_MEMD:
        MERRY_idiv_memd(&core->& core->kill_core, layout.whole_word);
        break;
      case MERRY_OP_DIV_MEMQ:
        MERRY_idiv_memq(&core->& core->kill_core, layout.whole_word);
        break;
      case MERRY_OP_MOD_MEMB:
        MERRY_imod_memb(&core->& core->kill_core, layout.whole_word);
        break;
      case MERRY_OP_MOD_MEMW:
        MERRY_imod_memw(&core->& core->kill_core, layout.whole_word);
        break;
      case MERRY_OP_MOD_MEMD:
        MERRY_imod_memd(&core->& core->kill_core, layout.whole_word);
        break;
      case MERRY_OP_MOD_MEMQ:
        MERRY_imod_memq(&core->& core->kill_core, layout.whole_word);
        break;
      case MERRY_OP_FADD_MEM:
        MERRY_ifadd64_mem(&core->& core->kill_core, layout.whole_word);
        break;
      case MERRY_OP_FSUB_MEM:
        MERRY_ifsub64_mem(&core->& core->kill_core, layout.whole_word);
        break;
      case MERRY_OP_FMUL_MEM:
        MERRY_ifmul64_mem(&core->& core->kill_core, layout.whole_word);
        break;
      case MERRY_OP_FDIV_MEM:
        MERRY_ifdiv64_mem(&core->& core->kill_core, layout.whole_word);
        break;
      case MERRY_OP_FADD32_MEM:
        MERRY_ifadd32_mem(&core->& core->kill_core, layout.whole_word);
        break;
      case MERRY_OP_FSUB32_MEM:
        MERRY_ifsub32_mem(&core->& core->kill_core, layout.whole_word);
        break;
      case MERRY_OP_FMUL32_MEM:
        MERRY_ifmul32_mem(&core->& core->kill_core, layout.whole_word);
        break;
      case MERRY_OP_FDIV32_MEM:
        MERRY_ifdiv32_mem(&core->& core->kill_core, layout.whole_word);
        break;
      case MERRY_OP_INC:
        core->REGISTER_FILE[layout.bytes.b7 & MERRY_R15]++;
        break;
      case MERRY_OP_DEC:
        core->REGISTER_FILE[layout.bytes.b7 & MERRY_R15]--;
        break;
      case MERRY_OP_MOVE_IMM_64:
      case MERRY_OP_MOVF:
        MERRY_imov_imm(&core->& core->kill_core, layout.whole_word);
        break;
      case MERRY_OP_MOVF32:
        MERRY_imovf32(&core->& core->kill_core, layout.whole_word);
        break;
      case MERRY_OP_MOVE_REG:
        core->REGISTER_FILE[layout.bytes.b6 & MERRY_R15] =
            core->REGISTER_FILE[layout.bytes.b7 & MERRY_R15];
        break;
      case MERRY_OP_MOVE_REG8:
        core->REGISTER_FILE[layout.bytes.b6 & MERRY_R15] =
            core->REGISTER_FILE[layout.bytes.b7 & MERRY_R15] & 255;
        break;
      case MERRY_OP_MOVE_REG16:
        core->REGISTER_FILE[layout.bytes.b6 & MERRY_R15] =
            core->REGISTER_FILE[layout.bytes.b7 & MERRY_R15] & 0xFFFF;
        break;
      case MERRY_OP_MOVE_REG32:
        core->REGISTER_FILE[layout.bytes.b6 & MERRY_R15] =
            core->REGISTER_FILE[layout.bytes.b7 & MERRY_R15] & 0xFFFFFFFF;
        break;
      case MERRY_OP_MOVESX_IMM8:
        MERRY_imovesx_imm8(&core->& core->kill_core, layout.whole_word);
        break;
      case MERRY_OP_MOVESX_IMM16:
        MERRY_imovesx_imm16(&core->& core->kill_core, layout.whole_word);
        break;
      case MERRY_OP_MOVESX_IMM32:
        MERRY_imovesx_imm32(&core->& core->kill_core, layout.whole_word);
        break;
      case MERRY_OP_MOVESX_REG8:
        MERRY_imovesx_reg8(&core->& core->kill_core, layout.whole_word);
        break;
      case MERRY_OP_MOVESX_REG16:
        MERRY_imovesx_reg16(&core->& core->kill_core, layout.whole_word);
        break;
      case MERRY_OP_MOVESX_REG32:
        MERRY_imovesx_reg32(&core->& core->kill_core, layout.whole_word);
        break;
      case MERRY_OP_EXCG8:
        MERRY_iexcg8(&core->& core->kill_core, layout.whole_word);
        break;
      case MERRY_OP_EXCG16:
        MERRY_iexcg16(&core->& core->kill_core, layout.whole_word);
        break;
      case MERRY_OP_EXCG32:
        MERRY_iexcg32(&core->& core->kill_core, layout.whole_word);
        break;
      case MERRY_OP_EXCG:
        MERRY_iexcg(&core->& core->kill_core, layout.whole_word);
        break;
      case MERRY_OP_MOV8:
        core->REGISTER_FILE[layout.bytes.b6 & MERRY_R15] &=
            (0xFFFFFFFFFFFFFF00 |
             (core->REGISTER_FILE[layout.bytes.b7 & MERRY_R15] & 0xFF));
        break;
      case MERRY_OP_MOV16:
        core->REGISTER_FILE[layout.bytes.b6 & MERRY_R15] &=
            (0xFFFFFFFFFFFF0000 |
             (core->REGISTER_FILE[layout.bytes.b7 & MERRY_R15] & 0xFFFF));
        break;
      case MERRY_OP_MOV32:
        core->REGISTER_FILE[layout.bytes.b6 & MERRY_R15] &=
            (0xFFFFFFFF00000000 |
             (core->REGISTER_FILE[layout.bytes.b7 & MERRY_R15] & 0xFFFFFFFF));
        break;
      case MERRY_OP_MOVNZ:
      case MERRY_OP_MOVNE:
        if (core->flags.flags.zero == 0)
          MERRY_imov_imm(&core->& core->kill_core, layout.whole_word);
        break;
      case MERRY_OP_MOVE:
      case MERRY_OP_MOVZ:
        if (core->flags.flags.zero == 1)
          MERRY_imov_imm(&core->& core->kill_core, layout.whole_word);
        break;
      case MERRY_OP_MOVNC:
        if (core->flags.flags.carry == 0)
          MERRY_imov_imm(&core->& core->kill_core, layout.whole_word);
        break;
      case MERRY_OP_MOVC:
        if (core->flags.flags.carry == 1)
          MERRY_imov_imm(&core->& core->kill_core, layout.whole_word);
        break;
      case MERRY_OP_MOVNO:
        if (core->flags.flags.overflow == 0)
          MERRY_imov_imm(&core->& core->kill_core, layout.whole_word);
        break;
      case MERRY_OP_MOVO:
        if (core->flags.flags.overflow == 1)
          MERRY_imov_imm(&core->& core->kill_core, layout.whole_word);
        break;
      case MERRY_OP_MOVNN:
        if (core->flags.flags.negative == 0)
          MERRY_imov_imm(&core->& core->kill_core, layout.whole_word);
        break;
      case MERRY_OP_MOVN:
        if (core->flags.flags.negative == 1)
          MERRY_imov_imm(&core->& core->kill_core, layout.whole_word);
        break;
      case MERRY_OP_MOVS:
      case MERRY_OP_MOVNG:
        if (core->flags.flags.negative == 1)
          MERRY_imov_imm(&core->& core->kill_core, layout.whole_word);
        break;
      case MERRY_OP_MOVNS:
      case MERRY_OP_MOVG:
        if (core->flags.flags.negative == 0)
          MERRY_imov_imm(&core->& core->kill_core, layout.whole_word);
        break;
      case MERRY_OP_MOVGE:
        if (core->flags.flags.negative == 0 || core->flags.flags.zero == 1)
          MERRY_imov_imm(&core->& core->kill_core, layout.whole_word);
        break;
      case MERRY_OP_MOVSE:
        if (core->flags.flags.zero == 1 || core->flags.flags.negative == 1)
          MERRY_imov_imm(&core->& core->kill_core, layout.whole_word);
        break;
      case MERRY_OP_JMP_OFF:
        sign_extend32(layout.half_words.w1);
        core->PC += layout.whole_word;
        continue;
      case MERRY_OP_JMP_ADDR:
        core->PC = layout.whole_word & 0xFFFFFFFFFFFF;
        continue;
      case MERRY_OP_JZ:
      case MERRY_OP_JE:
        // the address to jmp should follow the layout.whole_word
        if (core->flags.flags.zero == 1) {
          core->PC = (layout.whole_word & 0xFFFFFFFFFFFF);
          continue;
        }
        break;
      case MERRY_OP_JNZ:
      case MERRY_OP_JNE:
        // the address to jmp should follow the layout.whole_word
        if (core->flags.flags.zero == 0) {
          core->PC = (layout.whole_word & 0xFFFFFFFFFFFF);
          continue;
        }
        break;
      case MERRY_OP_JNC:
        if (core->flags.flags.carry == 0) {
          core->PC = (layout.whole_word & 0xFFFFFFFFFFFF);
          continue;
        }
        break;
      case MERRY_OP_JC:
        if (core->flags.flags.carry == 1) {
          core->PC = (layout.whole_word & 0xFFFFFFFFFFFF);
          continue;
        }
        break;
      case MERRY_OP_JNO:
        if (core->flags.flags.overflow == 0) {
          core->PC = (layout.whole_word & 0xFFFFFFFFFFFF);
          continue;
        }
        break;
      case MERRY_OP_JO:
        if (core->flags.flags.overflow == 1) {
          core->PC = (layout.whole_word & 0xFFFFFFFFFFFF);
          continue;
        }
        break;
      case MERRY_OP_JNN:
        if (core->flags.flags.negative == 0) {
          core->PC = (layout.whole_word & 0xFFFFFFFFFFFF);
          continue;
        }
        break;
      case MERRY_OP_JN:
        if (core->flags.flags.negative == 1) {
          core->PC = (layout.whole_word & 0xFFFFFFFFFFFF);
          continue;
        }
        break;
      case MERRY_OP_JS:
      case MERRY_OP_JNG:
        if (core->flags.flags.negative == 1) {
          core->PC = (layout.whole_word & 0xFFFFFFFFFFFF);
          continue;
        }
        break;
      case MERRY_OP_JNS:
      case MERRY_OP_JG:
        if (core->flags.flags.negative == 0) {
          core->PC = (layout.whole_word & 0xFFFFFFFFFFFF);
          continue;
        }
        break;
      case MERRY_OP_JGE:
        if (core->flags.flags.negative == 0 || core->flags.flags.zero == 0) {
          core->PC = (layout.whole_word & 0xFFFFFFFFFFFF);
          continue;
        }
        break;
      case MERRY_OP_JSE:
        if (core->flags.flags.negative == 1 || core->flags.flags.zero == 0) {
          core->PC = (layout.whole_word & 0xFFFFFFFFFFFF);
          continue;
        }
        break;
      case MERRY_OP_CALL:
        MERRY_icall(&core->& core->kill_core, layout.whole_word);
        continue;
      case MERRY_OP_RET:
        MERRY_iret(&core->& core->kill_core, layout.whole_word);
        break;
      case MERRY_OP_RETZ:
      case MERRY_OP_RETE:
        if (core->flags.flags.zero == 1)
          MERRY_iret(&core->& core->kill_core, layout.whole_word);
        break;
      case MERRY_OP_RETNZ:
      case MERRY_OP_RETNE:
        if (core->flags.flags.zero == 0)
          MERRY_iret(&core->& core->kill_core, layout.whole_word);
        break;
      case MERRY_OP_RETNC:
        if (core->flags.flags.carry == 0)
          MERRY_iret(&core->& core->kill_core, layout.whole_word);
        break;
      case MERRY_OP_RETC:
        if (core->flags.flags.carry == 1)
          MERRY_iret(&core->& core->kill_core, layout.whole_word);
        break;
      case MERRY_OP_RETNO:
        if (core->flags.flags.overflow == 0)
          MERRY_iret(&core->& core->kill_core, layout.whole_word);
        break;
      case MERRY_OP_RETO:
        if (core->flags.flags.overflow == 1)
          MERRY_iret(&core->& core->kill_core, layout.whole_word);
        break;
      case MERRY_OP_RETNN:
        if (core->flags.flags.negative == 0)
          MERRY_iret(&core->& core->kill_core, layout.whole_word);
        break;
      case MERRY_OP_RETN:
        if (core->flags.flags.negative == 1)
          MERRY_iret(&core->& core->kill_core, layout.whole_word);
        break;
      case MERRY_OP_RETS:
      case MERRY_OP_RETNG:
        if (core->flags.flags.negative == 1)
          MERRY_iret(&core->& core->kill_core, layout.whole_word);
        break;
      case MERRY_OP_RETNS:
      case MERRY_OP_RETG:
        if (core->flags.flags.negative == 0)
          MERRY_iret(&core->& core->kill_core, layout.whole_word);
        break;
      case MERRY_OP_RETGE:
        if (core->flags.flags.negative == 0 || core->flags.flags.zero == 1)
          MERRY_iret(&core->& core->kill_core, layout.whole_word);
        break;
      case MERRY_OP_RETSE:
        if (core->flags.flags.negative == 1 || core->flags.flags.zero == 1)
          MERRY_iret(&core->& core->kill_core, layout.whole_word);
        break;
      case MERRY_OP_LOOP:
        if (core->REGISTER_FILE[MERRY_R2] != 0) {
          core->PC = layout.whole_word & 0xFFFFFFFFFFFF;
          continue;
        }
        break;
      case MERRY_OP_CALL_REG:
        MERRY_icall_reg(&core->& core->kill_core, layout.whole_word);
        continue;
      case MERRY_OP_JMP_REGR:
        core->PC = core->REGISTER_FILE[layout.bytes.b7 & MERRY_R15];
        continue;
      case MERRY_OP_PUSH_IMM64:
        MERRY_ipush_immq(&core->& core->kill_core, layout.whole_word);
        break;
      case MERRY_OP_PUSH_REG:
        MERRY_ipush_reg(&core->& core->kill_core, layout.whole_word);
        break;
      case MERRY_OP_POP64:
        MERRY_ipopq(&core->& core->kill_core, layout.whole_word);
        break;
      case MERRY_OP_PUSHA:
        MERRY_ipusha(&core->& core->kill_core, layout.whole_word);
        break;
      case MERRY_OP_POPA:
        MERRY_ipopa(&core->& core->kill_core, layout.whole_word);
        break;
      case MERRY_OP_PUSH_MEMB:
        MERRY_ipush_memb(&core->& core->kill_core, layout.whole_word);
        break;
      case MERRY_OP_PUSH_MEMW:
        MERRY_ipush_memw(&core->& core->kill_core, layout.whole_word);
        break;
      case MERRY_OP_PUSH_MEMD:
        MERRY_ipush_memd(&core->& core->kill_core, layout.whole_word);
        break;
      case MERRY_OP_PUSH_MEMQ:
        MERRY_ipush_memq(&core->& core->kill_core, layout.whole_word);
        break;
      case MERRY_OP_POP_MEMB:
        MERRY_ipop_memb(&core->& core->kill_core, layout.whole_word);
        break;
      case MERRY_OP_POP_MEMW:
        MERRY_ipop_memw(&core->& core->kill_core, layout.whole_word);
        break;
      case MERRY_OP_POP_MEMD:
        MERRY_ipop_memd(&core->& core->kill_core, layout.whole_word);
        break;
      case MERRY_OP_POP_MEMQ:
        MERRY_ipop_memq(&core->& core->kill_core, layout.whole_word);
        break;
      case MERRY_OP_LOADSQ:
        MERRY_iloadsq(&core->& core->kill_core, layout.whole_word);
        break;
      case MERRY_OP_STORESQ:
        MERRY_istoresq(&core->& core->kill_core, layout.whole_word);
        break;
      case MERRY_OP_AND_IMM:
        MERRY_icmp_imm(&core->& core->kill_core, layout.whole_word);
        break;
      case MERRY_OP_AND_REG:
        core->REGISTER_FILE[layout.bytes.b6 & MERRY_R15] &=
            core->REGISTER_FILE[layout.bytes.b7 & MERRY_R15];
        break;
      case MERRY_OP_OR_IMM:
        MERRY_ior_imm(&core->& core->kill_core, layout.whole_word);
        break;
      case MERRY_OP_OR_REG:
        core->REGISTER_FILE[layout.bytes.b6 & MERRY_R15] |=
            core->REGISTER_FILE[layout.bytes.b7 & MERRY_R15];
        break;
      case MERRY_OP_XOR_IMM:
        MERRY_ixor_imm(&core->& core->kill_core, layout.whole_word);
        break;
      case MERRY_OP_XOR_REG:
        core->REGISTER_FILE[layout.bytes.b6 & MERRY_R15] ^=
            core->REGISTER_FILE[layout.bytes.b7 & MERRY_R15];
        break;
      case MERRY_OP_NOT:
        core->REGISTER_FILE[layout.bytes.b7 & MERRY_R15] =
            ~core->REGISTER_FILE[layout.bytes.b7];
        break;
      case MERRY_OP_LSHIFT:
        core->REGISTER_FILE[layout.bytes.b1 & MERRY_R15] <<= layout.bytes.b7 & 63;
        break;
      case MERRY_OP_RSHIFT:
        core->REGISTER_FILE[layout.bytes.b1 & MERRY_R15] >>= layout.bytes.b7 & 63;
        break;
      case MERRY_OP_LSHIFT_REGR:
        core->REGISTER_FILE[layout.bytes.b6 & MERRY_R15] <<=
            core->REGISTER_FILE[layout.bytes.b7 & MERRY_R15] & 63;
        break;
      case MERRY_OP_RSHIFT_REGR:
        core->REGISTER_FILE[layout.bytes.b6 & MERRY_R15] >>=
            core->REGISTER_FILE[layout.bytes.b7 & MERRY_R15] & 63;
        break;
      case MERRY_OP_CMP_IMM:
        MERRY_icmp_imm(&core->& core->kill_core, layout.whole_word);
        break;
      case MERRY_OP_CMP_REG:
        merry_compare_two_values(
            core->REGISTER_FILE[layout.bytes.b6 & MERRY_R15],
            core->REGISTER_FILE[layout.bytes.b7 & MERRY_R15]);
        core->flags.regr = merry_obtain_flags_regr();
        break;
      case MERRY_OP_CMP_IMM_MEMB:
        MERRY_icmp_imm_memb(&core->& core->kill_core, layout.whole_word);
        break;
      case MERRY_OP_CMP_IMM_MEMW:
        MERRY_icmp_imm_memw(&core->& core->kill_core, layout.whole_word);
        break;
      case MERRY_OP_CMP_IMM_MEMD:
        MERRY_icmp_imm_memd(&core->& core->kill_core, layout.whole_word);
        break;
      case MERRY_OP_CMP_IMM_MEMQ:
        MERRY_icmp_imm_memq(&core->& core->kill_core, layout.whole_word);
        break;
      case MERRY_OP_FCMP:
        MERRY_compare_f64(core->REGISTER_FILE[layout.bytes.b6 & MERRY_R15],
                        core->REGISTER_FILE[layout.bytes.b7 & MERRY_R15],
                        &core->fflags);
        break;
      case MERRY_OP_FCMP32:
        MERRY_compare_f32(core->REGISTER_FILE[layout.bytes.b6 & MERRY_R15],
                        core->REGISTER_FILE[layout.bytes.b7 & MERRY_R15],
                        &core->fflags);
        break;
      case MERRY_OP_CIN:
        core->REGISTER_FILE[layout.bytes.b7 & MERRY_R15] = getchar();
        break;
      case MERRY_OP_COUT:
        putchar((int)core->REGISTER_FILE[layout.bytes.b7 & MERRY_R15]);
        break;
      case MERRY_OP_SIN_REG:
        layout.whole_word = (core->REGISTER_FILE[layout.bytes.b7 & MERRY_R15]);
      case MERRY_OP_SIN:
        MERRY_isin(&core->& core->kill_core, layout.whole_word);
        break;
      case MERRY_OP_SOUT_REG:
        layout.bytes.b7 =
            (core->REGISTER_FILE[layout.bytes.b7 & MERRY_R15] & 0xFFFFFFFFFFFF);
      case MERRY_OP_SOUT:
        MERRY_isout(&core->& core->kill_core, layout.whole_word);
        break;
      case MERRY_OP_IN:
        fscanf(stdin, "%hhi", &mem.bytes.b7);
        core->REGISTER_FILE[layout.bytes.b7 & MERRY_R15] = mem.whole_word;
        break;
      case MERRY_OP_OUT:
        mem.whole_word = core->REGISTER_FILE[layout.bytes.b7 & MERRY_R15];
        fprintf(stdout, "%hhi", mem.bytes.b7);
        break;
      case MERRY_OP_INW:
        fscanf(stdin, "%hd", &mem.half_half_words.w3);
        core->REGISTER_FILE[layout.bytes.b7 & MERRY_R15] = mem.whole_word;
        break;
      case MERRY_OP_OUTW:
        mem.whole_word = core->REGISTER_FILE[layout.bytes.b7 & MERRY_R15];
        fprintf(stdout, "%hd", mem.half_half_words.w3);
        break;
      case MERRY_OP_IND:
        fscanf(stdin, "%d", &mem.half_words.w1);
        core->REGISTER_FILE[layout.bytes.b7 & MERRY_R15] = mem.whole_word;
        break;
      case MERRY_OP_OUTD:
        mem.whole_word = core->REGISTER_FILE[layout.bytes.b7 & MERRY_R15];
        fprintf(stdout, "%d", mem.half_words.w1);
        break;
      case MERRY_OP_INQ:
        fscanf(stdin, "%zi", &core->REGISTER_FILE[layout.bytes.b7 & MERRY_R15]);
        break;
      case MERRY_OP_OUTQ:
        fprintf(stdout, "%zi", core->REGISTER_FILE[layout.bytes.b7 & MERRY_R15]);
        break;
      case MERRY_OP_UIN:
        fscanf(stdin, "%hhu", &mem.bytes.b7);
        core->REGISTER_FILE[layout.bytes.b7 & MERRY_R15] = mem.whole_word;
        break;
      case MERRY_OP_UOUT:
        mem.whole_word = core->REGISTER_FILE[layout.bytes.b7 & MERRY_R15];
        fprintf(stdout, "%hhu", mem.bytes.b7);
        break;
      case MERRY_OP_UINW:
        fscanf(stdin, "%hu", &mem.half_half_words.w3);
        core->REGISTER_FILE[layout.bytes.b7 & MERRY_R15] = mem.whole_word;
        break;
      case MERRY_OP_UOUTW:
        mem.whole_word = core->REGISTER_FILE[layout.bytes.b7 & MERRY_R15];
        fprintf(stdout, "%hu", mem.half_half_words.w3);
        break;
      case MERRY_OP_UIND:
        fscanf(stdin, "%u", &mem.half_words.w1);
        core->REGISTER_FILE[layout.bytes.b7 & MERRY_R15] = mem.whole_word;
        break;
      case MERRY_OP_UOUTD:
        mem.whole_word = core->REGISTER_FILE[layout.bytes.b7 & MERRY_R15];
        fprintf(stdout, "%u", mem.half_words.w1);
        break;
      case MERRY_OP_UINQ:
        fscanf(stdin, "%zu", &core->REGISTER_FILE[layout.bytes.b7 & MERRY_R15]);
        break;
      case MERRY_OP_UOUTQ:
        fprintf(stdout, "%zu", core->REGISTER_FILE[layout.bytes.b7 & MERRY_R15]);
        break;
      case MERRY_OP_INF:
        fscanf(stdin, "%lf", &dtoq.d_val);
        core->REGISTER_FILE[layout.bytes.b7 & MERRY_R15] = dtoq.q_val;
        break;
      case MERRY_OP_OUTF:
        dtoq.q_val = core->REGISTER_FILE[layout.bytes.b7 & MERRY_R15];
        fprintf(stdout, "%lf", dtoq.d_val);
        break;
      case MERRY_OP_INF32:
        fscanf(stdin, "%f", &ftod.fl_val);
        core->REGISTER_FILE[layout.bytes.b7 & MERRY_R15] = ftod.d_val;
        break;
      case MERRY_OP_OUTF32:
        ftod.d_val = core->REGISTER_FILE[layout.bytes.b7 & MERRY_R15];
        fprintf(stdout, "%f", ftod.fl_val);
        break;
      case MERRY_OP_OUTR:
        for (msize_t i = 0; i <= MERRY_R15; i++)
          fprintf(stdout, "%zi\n", core->REGISTER_FILE[i]);
        break;
      case MERRY_OP_UOUTR:
        for (msize_t i = 0; i <= MERRY_R15; i++)
          fprintf(stdout, "%zu\n", core->REGISTER_FILE[i]);
        break;
      case MERRY_OP_LOADB:
        MERRY_iloadb(&core->& core->kill_core, layout.whole_word);
        break;
      case MERRY_OP_LOADW:
        MERRY_iloadw(&core->& core->kill_core, layout.whole_word);
        break;
      case MERRY_OP_LOADD:
        MERRY_iloadd(&core->& core->kill_core, layout.whole_word);
        break;
      case MERRY_OP_LOADQ:
        MERRY_iloadq(&core->& core->kill_core, layout.whole_word);
        break;
      case MERRY_OP_STOREB:
        MERRY_istoreb(&core->& core->kill_core, layout.whole_word);
        break;
      case MERRY_OP_STOREW:
        MERRY_istorew(&core->& core->kill_core, layout.whole_word);
        break;
      case MERRY_OP_STORED:
        MERRY_istored(&core->& core->kill_core, layout.whole_word);
        break;
      case MERRY_OP_STOREQ:
        MERRY_istoreq(&core->& core->kill_core, layout.whole_word);
        break;
      case MERRY_OP_LOADB_REG:
        MERRY_iloadb_reg(&core->& core->kill_core, layout.whole_word);
        break;
      case MERRY_OP_LOADW_REG:
        MERRY_iloadw_reg(&core->& core->kill_core, layout.whole_word);
        break;
      case MERRY_OP_LOADD_REG:
        MERRY_iloadd_reg(&core->& core->kill_core, layout.whole_word);
        break;
      case MERRY_OP_LOADQ_REG:
        MERRY_iloadq_reg(&core->& core->kill_core, layout.whole_word);
        break;
      case MERRY_OP_STOREB_REG:
        MERRY_istoreb_reg(&core->& core->kill_core, layout.whole_word);
        break;
      case MERRY_OP_STOREW_REG:
        MERRY_istorew_reg(&core->& core->kill_core, layout.whole_word);
        break;
      case MERRY_OP_STORED_REG:
        MERRY_istored_reg(&core->& core->kill_core, layout.whole_word);
        break;
      case MERRY_OP_STOREQ_REG:
        MERRY_istoreq_reg(&core->& core->kill_core, layout.whole_word);
        break;
      case MERRY_OP_ATOMIC_LOADB:
        MERRY_iatm_loadb(&core->& core->kill_core, layout.whole_word);
        break;
      case MERRY_OP_ATOMIC_LOADW:
        MERRY_iatm_loadw(&core->& core->kill_core, layout.whole_word);
        break;
      case MERRY_OP_ATOMIC_LOADD:
        MERRY_iatm_loadd(&core->& core->kill_core, layout.whole_word);
        break;
      case MERRY_OP_ATOMIC_LOADQ:
        MERRY_iatm_loadq(&core->& core->kill_core, layout.whole_word);
        break;
      case MERRY_OP_ATOMIC_STOREB:
        MERRY_iatm_storeb(&core->& core->kill_core, layout.whole_word);
        break;
      case MERRY_OP_ATOMIC_STOREW:
        MERRY_iatm_storew(&core->& core->kill_core, layout.whole_word);
        break;
      case MERRY_OP_ATOMIC_STORED:
        MERRY_iatm_stored(&core->& core->kill_core, layout.whole_word);
        break;
      case MERRY_OP_ATOMIC_STOREQ:
        MERRY_iatm_storeq(&core->& core->kill_core, layout.whole_word);
        break;
      case MERRY_OP_ATOMIC_LOADB_REG:
        MERRY_iatm_loadb_reg(&core->& core->kill_core, layout.whole_word);
        break;
      case MERRY_OP_ATOMIC_LOADW_REG:
        MERRY_iatm_loadw_reg(&core->& core->kill_core, layout.whole_word);
        break;
      case MERRY_OP_ATOMIC_LOADD_REG:
        MERRY_iatm_loadd_reg(&core->& core->kill_core, layout.whole_word);
        break;
      case MERRY_OP_ATOMIC_LOADQ_REG:
        MERRY_iatm_loadq_reg(&core->& core->kill_core, layout.whole_word);
        break;
      case MERRY_OP_ATOMIC_STOREB_REG:
        MERRY_iatm_storeb_reg(&core->& core->kill_core, layout.whole_word);
        break;
      case MERRY_OP_ATOMIC_STOREW_REG:
        MERRY_iatm_storew_reg(&core->& core->kill_core, layout.whole_word);
        break;
      case MERRY_OP_ATOMIC_STORED_REG:
        MERRY_iatm_stored_reg(&core->& core->kill_core, layout.whole_word);
        break;
      case MERRY_OP_ATOMIC_STOREQ_REG:
        MERRY_iatm_storeq_reg(&core->& core->kill_core, layout.whole_word);
        break;
      case MERRY_OP_LEA:
        core->REGISTER_FILE[layout.bytes.b4 & MERRY_R15] =
            core->REGISTER_FILE[layout.bytes.b5 & MERRY_R15] +
            core->REGISTER_FILE[layout.bytes.b6 & MERRY_R15] *
                core->REGISTER_FILE[layout.bytes.b7 & MERRY_R15];
        break;
      case MERRY_OP_CFLAGS:
        core->flags.flags.carry = 0;
        core->flags.flags.negative = 0;
        core->flags.flags.zero = 0;
        core->flags.flags.overflow = 0;
        break;
      case MERRY_OP_RESET:
        for (msize_t i = 0; i < MERRY_REG_COUNT; i++)
          core->REGISTER_FILE[i] = 0;
        break;
      case MERRY_OP_CMPXCHG_REGR:
        MERRY_icmpxchg_reg(&core->& core->kill_core, layout.whole_word);
        break;
      case MERRY_OP_CMPXCHG:
        MERRY_icmpxchg(&core->& core->kill_core, layout.whole_word);
        break;
      case 250:
      case 251:
      case 252:
      case 253:
      case 254:
        break;
      case 255: {
        switch (layout.bytes.b1) {
        case MERRY_OP_MOVFZ:
          if (core->fflags.zf)
            MERRY_imov_imm(&core->& core->kill_core, layout.whole_word);
          break;
        case MERRY_OP_MOVFNZ:
          if (!core->fflags.zf)
            MERRY_imov_imm(&core->& core->kill_core, layout.whole_word);
          break;
        case MERRY_OP_MOVFN:
          if (core->fflags.sf)
            MERRY_imov_imm(&core->& core->kill_core, layout.whole_word);
          break;
        case MERRY_OP_MOVFNN:
          if (!core->fflags.sf)
            MERRY_imov_imm(&core->& core->kill_core, layout.whole_word);
          break;
        case MERRY_OP_MOVFUF:
          if (core->fflags.uof)
            MERRY_imov_imm(&core->& core->kill_core, layout.whole_word);
          break;
        case MERRY_OP_MOVFNUF:
          if (!core->fflags.uof)
            MERRY_imov_imm(&core->& core->kill_core, layout.whole_word);
          break;
        case MERRY_OP_MOVFO:
          if (core->fflags.of)
            MERRY_imov_imm(&core->& core->kill_core, layout.whole_word);
          break;
        case MERRY_OP_MOVFNO:
          if (!core->fflags.of)
            MERRY_imov_imm(&core->& core->kill_core, layout.whole_word);
          break;
        case MERRY_OP_MOVFU:
          if (core->fflags.uf)
            MERRY_imov_imm(&core->& core->kill_core, layout.whole_word);
          break;
        case MERRY_OP_MOVFNU:
          if (!core->fflags.uf)
            MERRY_imov_imm(&core->& core->kill_core, layout.whole_word);
          break;
        case MERRY_OP_MOVFI:
          if (core->fflags.inv)
            MERRY_imov_imm(&core->& core->kill_core, layout.whole_word);
          break;
        case MERRY_OP_MOVFNI:
          if (!core->fflags.inv)
            MERRY_imov_imm(&core->& core->kill_core, layout.whole_word);
          break;
        case MERRY_OP_JFZ:
          if (core->fflags.zf) {
            core->PC = (layout.whole_word & 0xFFFFFFFFFFFF);
            continue;
          }
          break;
        case MERRY_OP_JFNZ:
          if (!core->fflags.zf) {
            core->PC = (layout.whole_word & 0xFFFFFFFFFFFF);
            continue;
          }
          break;
        case MERRY_OP_JFN:
          if (core->fflags.sf) {
            core->PC = (layout.whole_word & 0xFFFFFFFFFFFF);
            continue;
          }
          break;
        case MERRY_OP_JFNN:
          if (!core->fflags.sf) {
            core->PC = (layout.whole_word & 0xFFFFFFFFFFFF);
            continue;
          }
          break;
        case MERRY_OP_JFUF:
          if (core->fflags.uof) {
            core->PC = (layout.whole_word & 0xFFFFFFFFFFFF);
            continue;
          }
          break;
        case MERRY_OP_JFNUF:
          if (!core->fflags.uof) {
            core->PC = (layout.whole_word & 0xFFFFFFFFFFFF);
            continue;
          }
          break;
        case MERRY_OP_JFO:
          if (core->fflags.of) {
            core->PC = (layout.whole_word & 0xFFFFFFFFFFFF);
            continue;
          }
          break;
        case MERRY_OP_JFNO:
          if (!core->fflags.of) {
            core->PC = (layout.whole_word & 0xFFFFFFFFFFFF);
            continue;
          }
          break;
        case MERRY_OP_JFU:
          if (core->fflags.uf) {
            core->PC = (layout.whole_word & 0xFFFFFFFFFFFF);
            continue;
          }
          break;
        case MERRY_OP_JFNU:
          if (!core->fflags.uf) {
            core->PC = (layout.whole_word & 0xFFFFFFFFFFFF);
            continue;
          }
          break;
        case MERRY_OP_JFI:
          if (core->fflags.inv) {
            core->PC = (layout.whole_word & 0xFFFFFFFFFFFF);
            continue;
          }
          break;
        case MERRY_OP_JFNI:
          if (!core->fflags.inv) {
            core->PC = (layout.whole_word & 0xFFFFFFFFFFFF);
            continue;
          }
          break;
        case MERRY_OP_RETFZ:
          if (core->fflags.zf)
            MERRY_iret(&core->& core->kill_core, layout.whole_word);
          break;
        case MERRY_OP_RETFNZ:
          if (!core->fflags.zf)
            MERRY_iret(&core->& core->kill_core, layout.whole_word);
          break;
        case MERRY_OP_RETFN:
          if (core->fflags.sf)
            MERRY_iret(&core->& core->kill_core, layout.whole_word);
          break;
        case MERRY_OP_RETFNN:
          if (!core->fflags.sf)
            MERRY_iret(&core->& core->kill_core, layout.whole_word);
          break;
        case MERRY_OP_RETFUF:
          if (core->fflags.uof)
            MERRY_iret(&core->& core->kill_core, layout.whole_word);
          break;
        case MERRY_OP_RETFNUF:
          if (!core->fflags.uof)
            MERRY_iret(&core->& core->kill_core, layout.whole_word);
          break;
        case MERRY_OP_RETFO:
          if (core->fflags.of)
            MERRY_iret(&core->& core->kill_core, layout.whole_word);
          break;
        case MERRY_OP_RETFNO:
          if (!core->fflags.of)
            MERRY_iret(&core->& core->kill_core, layout.whole_word);
          break;
        case MERRY_OP_RETFU:
          if (core->fflags.uf)
            MERRY_iret(&core->& core->kill_core, layout.whole_word);
          break;
        case MERRY_OP_RETFNU:
          if (!core->fflags.uf)
            MERRY_iret(&core->& core->kill_core, layout.whole_word);
          break;
        case MERRY_OP_RETFI:
          if (core->fflags.inv)
            MERRY_iret(&core->& core->kill_core, layout.whole_word);
          break;
        case MERRY_OP_RETFNI:
          if (!core->fflags.inv)
            MERRY_iret(&core->& core->kill_core, layout.whole_word);
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
       i < merry_MERRYThread_list_size(core->merry_core->child_threads); i++) {
    merry_thread_join(core->merry_core->child_threads->buf[i], NULL);
  }
  return RET_SUCCESS;
}
