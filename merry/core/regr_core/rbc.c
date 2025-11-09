#include <internal/rbc.h>
#include <internal/rbc_instruction_handler.h>

_MERRY_DEFINE_STATIC_LIST_(RBCThread, mthread_t);
_MERRY_DEFINE_STATIC_LIST_(Interface, MerryInterface *);
_MERRY_DEFINE_STACK_(RBCProcFrame, RBCStackFrame);

mptr_t rbc_master_core_create(MerryCoreBase *base, maddress_t st_addr,
                              msize_t *CODE) {
  merry_check_ptr(base);
  RBCMasterCore *core = (RBCMasterCore *)malloc(sizeof(RBCMasterCore));
  if (!core) {
    MFATAL("RBC", "Failed to allocate memory for the master core", NULL);
    *CODE = RBC_SYS_FAILURE;
    return RET_NULL;
  }

  core->base = base;

  mresult_t res;

  if ((res = merry_RBCThread_list_create(10, &core->rbc_cbase.child_threads)) !=
      MRES_SUCCESS) {
    MFATAL("RBC", "Failed to allocate memory for a component", NULL);
    free(core);
    return RET_NULL;
  }

  if ((res = merry_Interface_list_create(10, &core->rbc_cbase.interfaces)) !=
      MRES_SUCCESS) {
    MFATAL("RBC", "Failed to allocate memory for a component", NULL);
    merry_RBCThread_list_destroy(core->rbc_cbase.child_threads);
    free(core);
    return RET_NULL;
  }

  core->rbc_cbase.PC = st_addr;

  if ((res = merry_get_anonymous_memory((mptr_t)&core->rbc_cbase.stack,
                                        _RBC_STACK_LEN_)) != MRES_SUCCESS) {
    MFATAL("RBC", "Failed to initialize the stack", NULL);
    merry_RBCThread_list_destroy(core->rbc_cbase.child_threads);
    merry_Interface_list_destroy(core->rbc_cbase.interfaces);
    free(core);
    return RET_NULL;
  }

  core->rbc_cbase.SP = 0;
  core->rbc_cbase.BP = 0;

  if ((res = merry_RBCProcFrame_stack_init(&core->rbc_cbase.stack_frames,
                                           _RBC_CALL_DEPTH_)) != MRES_SUCCESS) {
    MFATAL("RBC", "Failed to initialize the stack", NULL);
    merry_RBCThread_list_destroy(core->rbc_cbase.child_threads);
    merry_Interface_list_destroy(core->rbc_cbase.interfaces);
    merry_return_memory(core->rbc_cbase.stack, _RBC_STACK_LEN_);
    free(core);
    return RET_NULL;
  }

  if (merry_cond_init(&core->local_shared_cond) != MRES_SUCCESS) {
    MFATAL("RBC", "Failed to initialize component", NULL);
    merry_RBCThread_list_destroy(core->rbc_cbase.child_threads);
    merry_Interface_list_destroy(core->rbc_cbase.interfaces);
    merry_return_memory(core->rbc_cbase.stack, _RBC_STACK_LEN_);
    merry_RBCProcFrame_stack_destroy(core->rbc_cbase.stack_frames);
    free(core);
    return RET_NULL;
  }

  return (mptr_t)core;
}

void rbc_master_core_destroy(mptr_t c) {
  merry_check_ptr(c);
  RBCMasterCore *core = (RBCMasterCore *)c;

  if (core->rbc_cbase.child_threads)
    merry_RBCThread_list_destroy(core->rbc_cbase.child_threads);

  if (core->rbc_cbase.interfaces)
    merry_Interface_list_destroy(core->rbc_cbase.interfaces);

  if (core->rbc_cbase.stack_frames)
    merry_RBCProcFrame_stack_destroy(core->rbc_cbase.stack_frames);

  if (core->rbc_cbase.iram)
    rbc_memory_destroy(core->rbc_cbase.iram);

  if (core->rbc_cbase.dram)
    rbc_memory_destroy(core->rbc_cbase.dram);

  if (core->inp)
    rbc_input_destroy(core->inp);

  if (core->rbc_cbase.stack)
    merry_return_memory(core->rbc_cbase.stack, _RBC_STACK_LEN_);

  merry_cond_destroy(&core->local_shared_cond);
  free(core);
}

msize_t rbc_master_core_run(mptr_t c) {
  merry_check_ptr(c);

  RBCMasterCore *core = (RBCMasterCore *)c;
  register MerryCoreBase *base = core->base;
  RBCCoreBase cbase = core->rbc_cbase;
  MerryHostMemLayout layout, mem;
  MerryFloatToDword ftod;
  MerryDoubleToQword dtoq;

  while (mtrue) {
    if (!surelyF(cbase.check_after)) {
      if (core->kill_core)
        break;
      if (surelyF(atomic_load_explicit((_Atomic mbool_t *)&core->interrupt,
                                       memory_order_relaxed) == mtrue)) {
        if (cbase.terminate)
          break; // break
        // interrupts....(coming soon...)
        core->interrupt = mfalse;
      }
      cbase.check_after = 3;
    }
    cbase.check_after--;
    if (surelyF(
            rbc_memory_read_qword(cbase.iram, cbase.PC, &layout.whole_word) ==
            RBC_MEM_OPER_ACCESS_INVALID)) {
      MFATAL(
          "RBC",
          "Memory access invalid: Accessing address that doesn't exist PC=%zu",
          cbase.PC);
      cbase.terminate = mtrue;
      cbase.check_after = 0;
      core->interrupt = mtrue;
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
        break;
      case RBC_OP_HALT:
        cbase.terminate = mtrue;
        core->interrupt = mtrue;
        cbase.check_after = 0;
        break;
      case RBC_OP_SYSINT:
        rbc_isysint(&cbase, &core->kill_core);
        break;
      case RBC_OP_MINT:
        rbc_imint(&cbase, &core->kill_core);
        break;
      case RBC_OP_ADD_IMM:
        rbc_iadd_imm(&cbase, &core->kill_core, layout.whole_word);
        break;
      case RBC_OP_ADD_REG:
        rbc_iadd_reg(&cbase, &core->kill_core, layout.whole_word);
        break;
      case RBC_OP_SUB_IMM:
        rbc_isub_imm(&cbase, &core->kill_core, layout.whole_word);
        break;
      case RBC_OP_SUB_REG:
        rbc_isub_reg(&cbase, &core->kill_core, layout.whole_word);
        break;
      case RBC_OP_MUL_IMM:
        rbc_imul_imm(&cbase, &core->kill_core, layout.whole_word);
        break;
      case RBC_OP_MUL_REG:
        rbc_imul_reg(&cbase, &core->kill_core, layout.whole_word);
        break;
      case RBC_OP_DIV_IMM:
        rbc_idiv_imm(&cbase, &core->kill_core, layout.whole_word);
        break;
      case RBC_OP_DIV_REG:
        rbc_idiv_reg(&cbase, &core->kill_core, layout.whole_word);
        break;
      case RBC_OP_MOD_IMM:
        rbc_imod_imm(&cbase, &core->kill_core, layout.whole_word);
        break;
      case RBC_OP_MOD_REG:
        rbc_imod_reg(&cbase, &core->kill_core, layout.whole_word);
        break;
      case RBC_OP_FADD:
        rbc_ifadd(&cbase, &core->kill_core, layout.whole_word);
        break;
      case RBC_OP_FSUB:
        rbc_ifsub(&cbase, &core->kill_core, layout.whole_word);
        break;
      case RBC_OP_FMUL:
        rbc_ifmul(&cbase, &core->kill_core, layout.whole_word);
        break;
      case RBC_OP_FDIV:
        rbc_ifdiv(&cbase, &core->kill_core, layout.whole_word);
        break;
      case RBC_OP_FADD32:
        rbc_ifadd32(&cbase, &core->kill_core, layout.whole_word);
        break;
      case RBC_OP_FSUB32:
        rbc_ifsub32(&cbase, &core->kill_core, layout.whole_word);
        break;
      case RBC_OP_FMUL32:
        rbc_ifmul32(&cbase, &core->kill_core, layout.whole_word);
        break;
      case RBC_OP_FDIV32:
        rbc_ifdiv32(&cbase, &core->kill_core, layout.whole_word);
        break;
      case RBC_OP_ADD_MEMB:
        rbc_iadd_memb(&cbase, &core->kill_core, layout.whole_word);
        break;
      case RBC_OP_ADD_MEMW:
        rbc_iadd_memw(&cbase, &core->kill_core, layout.whole_word);
        break;
      case RBC_OP_ADD_MEMD:
        rbc_iadd_memd(&cbase, &core->kill_core, layout.whole_word);
        break;
      case RBC_OP_ADD_MEMQ:
        rbc_iadd_memq(&cbase, &core->kill_core, layout.whole_word);
        break;
      case RBC_OP_SUB_MEMB:
        rbc_isub_memb(&cbase, &core->kill_core, layout.whole_word);
        break;
      case RBC_OP_SUB_MEMW:
        rbc_isub_memw(&cbase, &core->kill_core, layout.whole_word);
        break;
      case RBC_OP_SUB_MEMD:
        rbc_isub_memd(&cbase, &core->kill_core, layout.whole_word);
        break;
      case RBC_OP_SUB_MEMQ:
        rbc_isub_memq(&cbase, &core->kill_core, layout.whole_word);
        break;
      case RBC_OP_MUL_MEMB:
        rbc_imul_memb(&cbase, &core->kill_core, layout.whole_word);
        break;
      case RBC_OP_MUL_MEMW:
        rbc_imul_memw(&cbase, &core->kill_core, layout.whole_word);
        break;
      case RBC_OP_MUL_MEMD:
        rbc_imul_memd(&cbase, &core->kill_core, layout.whole_word);
        break;
      case RBC_OP_MUL_MEMQ:
        rbc_imul_memq(&cbase, &core->kill_core, layout.whole_word);
        break;
      case RBC_OP_DIV_MEMB:
        rbc_idiv_memb(&cbase, &core->kill_core, layout.whole_word);
        break;
      case RBC_OP_DIV_MEMW:
        rbc_idiv_memw(&cbase, &core->kill_core, layout.whole_word);
        break;
      case RBC_OP_DIV_MEMD:
        rbc_idiv_memd(&cbase, &core->kill_core, layout.whole_word);
        break;
      case RBC_OP_DIV_MEMQ:
        rbc_idiv_memq(&cbase, &core->kill_core, layout.whole_word);
        break;
      case RBC_OP_MOD_MEMB:
        rbc_imod_memb(&cbase, &core->kill_core, layout.whole_word);
        break;
      case RBC_OP_MOD_MEMW:
        rbc_imod_memw(&cbase, &core->kill_core, layout.whole_word);
        break;
      case RBC_OP_MOD_MEMD:
        rbc_imod_memd(&cbase, &core->kill_core, layout.whole_word);
        break;
      case RBC_OP_MOD_MEMQ:
        rbc_imod_memq(&cbase, &core->kill_core, layout.whole_word);
        break;
      case RBC_OP_FADD_MEM:
        rbc_ifadd64_mem(&cbase, &core->kill_core, layout.whole_word);
        break;
      case RBC_OP_FSUB_MEM:
        rbc_ifsub64_mem(&cbase, &core->kill_core, layout.whole_word);
        break;
      case RBC_OP_FMUL_MEM:
        rbc_ifmul64_mem(&cbase, &core->kill_core, layout.whole_word);
        break;
      case RBC_OP_FDIV_MEM:
        rbc_ifdiv64_mem(&cbase, &core->kill_core, layout.whole_word);
        break;
      case RBC_OP_FADD32_MEM:
        rbc_ifadd32_mem(&cbase, &core->kill_core, layout.whole_word);
        break;
      case RBC_OP_FSUB32_MEM:
        rbc_ifsub32_mem(&cbase, &core->kill_core, layout.whole_word);
        break;
      case RBC_OP_FMUL32_MEM:
        rbc_ifmul32_mem(&cbase, &core->kill_core, layout.whole_word);
        break;
      case RBC_OP_FDIV32_MEM:
        rbc_ifdiv32_mem(&cbase, &core->kill_core, layout.whole_word);
        break;
      case RBC_OP_INC:
        cbase.REGISTER_FILE[layout.bytes.b7 & RBC_R15]++;
        break;
      case RBC_OP_DEC:
        cbase.REGISTER_FILE[layout.bytes.b7 & RBC_R15]--;
        break;
      case RBC_OP_MOVE_IMM_64:
      case RBC_OP_MOVF:
        rbc_imov_imm(&cbase, &core->kill_core, layout.whole_word);
        break;
      case RBC_OP_MOVF32:
        rbc_imovf32(&cbase, &core->kill_core, layout.whole_word);
        break;
      case RBC_OP_MOVE_REG:
        cbase.REGISTER_FILE[layout.bytes.b6 & RBC_R15] =
            cbase.REGISTER_FILE[layout.bytes.b7 & RBC_R15];
        break;
      case RBC_OP_MOVE_REG8:
        cbase.REGISTER_FILE[layout.bytes.b6 & RBC_R15] =
            cbase.REGISTER_FILE[layout.bytes.b7 & RBC_R15] & 255;
        break;
      case RBC_OP_MOVE_REG16:
        cbase.REGISTER_FILE[layout.bytes.b6 & RBC_R15] =
            cbase.REGISTER_FILE[layout.bytes.b7 & RBC_R15] & 0xFFFF;
        break;
      case RBC_OP_MOVE_REG32:
        cbase.REGISTER_FILE[layout.bytes.b6 & RBC_R15] =
            cbase.REGISTER_FILE[layout.bytes.b7 & RBC_R15] & 0xFFFFFFFF;
        break;
      case RBC_OP_MOVESX_IMM8:
        rbc_imovesx_imm8(&cbase, &core->kill_core, layout.whole_word);
        break;
      case RBC_OP_MOVESX_IMM16:
        rbc_imovesx_imm16(&cbase, &core->kill_core, layout.whole_word);
        break;
      case RBC_OP_MOVESX_IMM32:
        rbc_imovesx_imm32(&cbase, &core->kill_core, layout.whole_word);
        break;
      case RBC_OP_MOVESX_REG8:
        rbc_imovesx_reg8(&cbase, &core->kill_core, layout.whole_word);
        break;
      case RBC_OP_MOVESX_REG16:
        rbc_imovesx_reg16(&cbase, &core->kill_core, layout.whole_word);
        break;
      case RBC_OP_MOVESX_REG32:
        rbc_imovesx_reg32(&cbase, &core->kill_core, layout.whole_word);
        break;
      case RBC_OP_EXCG8:
        rbc_iexcg8(&cbase, &core->kill_core, layout.whole_word);
        break;
      case RBC_OP_EXCG16:
        rbc_iexcg16(&cbase, &core->kill_core, layout.whole_word);
        break;
      case RBC_OP_EXCG32:
        rbc_iexcg32(&cbase, &core->kill_core, layout.whole_word);
        break;
      case RBC_OP_EXCG:
        rbc_iexcg(&cbase, &core->kill_core, layout.whole_word);
        break;
      case RBC_OP_MOV8:
        cbase.REGISTER_FILE[layout.bytes.b6 & RBC_R15] &=
            (0xFFFFFFFFFFFFFF00 |
             (cbase.REGISTER_FILE[layout.bytes.b7 & RBC_R15] & 0xFF));
        break;
      case RBC_OP_MOV16:
        cbase.REGISTER_FILE[layout.bytes.b6 & RBC_R15] &=
            (0xFFFFFFFFFFFF0000 |
             (cbase.REGISTER_FILE[layout.bytes.b7 & RBC_R15] & 0xFFFF));
        break;
      case RBC_OP_MOV32:
        cbase.REGISTER_FILE[layout.bytes.b6 & RBC_R15] &=
            (0xFFFFFFFF00000000 |
             (cbase.REGISTER_FILE[layout.bytes.b7 & RBC_R15] & 0xFFFFFFFF));
        break;
      case RBC_OP_MOVNZ:
      case RBC_OP_MOVNE:
        if (cbase.flags.flags.zero == 0)
          rbc_imov_imm(&cbase, &core->kill_core, layout.whole_word);
        break;
      case RBC_OP_MOVE:
      case RBC_OP_MOVZ:
        if (cbase.flags.flags.zero == 1)
          rbc_imov_imm(&cbase, &core->kill_core, layout.whole_word);
        break;
      case RBC_OP_MOVNC:
        if (cbase.flags.flags.carry == 0)
          rbc_imov_imm(&cbase, &core->kill_core, layout.whole_word);
        break;
      case RBC_OP_MOVC:
        if (cbase.flags.flags.carry == 1)
          rbc_imov_imm(&cbase, &core->kill_core, layout.whole_word);
        break;
      case RBC_OP_MOVNO:
        if (cbase.flags.flags.overflow == 0)
          rbc_imov_imm(&cbase, &core->kill_core, layout.whole_word);
        break;
      case RBC_OP_MOVO:
        if (cbase.flags.flags.overflow == 1)
          rbc_imov_imm(&cbase, &core->kill_core, layout.whole_word);
        break;
      case RBC_OP_MOVNN:
        if (cbase.flags.flags.negative == 0)
          rbc_imov_imm(&cbase, &core->kill_core, layout.whole_word);
        break;
      case RBC_OP_MOVN:
        if (cbase.flags.flags.negative == 1)
          rbc_imov_imm(&cbase, &core->kill_core, layout.whole_word);
        break;
      case RBC_OP_MOVS:
      case RBC_OP_MOVNG:
        if (cbase.flags.flags.negative == 1)
          rbc_imov_imm(&cbase, &core->kill_core, layout.whole_word);
        break;
      case RBC_OP_MOVNS:
      case RBC_OP_MOVG:
        if (cbase.flags.flags.negative == 0)
          rbc_imov_imm(&cbase, &core->kill_core, layout.whole_word);
        break;
      case RBC_OP_MOVGE:
        if (cbase.flags.flags.negative == 0 || cbase.flags.flags.zero == 1)
          rbc_imov_imm(&cbase, &core->kill_core, layout.whole_word);
        break;
      case RBC_OP_MOVSE:
        if (cbase.flags.flags.zero == 1 || cbase.flags.flags.negative == 1)
          rbc_imov_imm(&cbase, &core->kill_core, layout.whole_word);
        break;
      case RBC_OP_JMP_OFF:
        sign_extend32(layout.half_words.w1);
        cbase.PC += layout.whole_word;
        continue;
      case RBC_OP_JMP_ADDR:
        cbase.PC = layout.whole_word & 0xFFFFFFFFFFFF;
        continue;
      case RBC_OP_JZ:
      case RBC_OP_JE:
        // the address to jmp should follow the layout.whole_word
        if (cbase.flags.flags.zero == 1)
          cbase.PC = (layout.whole_word & 0xFFFFFFFFFFFF);
        continue;
      case RBC_OP_JNZ:
      case RBC_OP_JNE:
        // the address to jmp should follow the layout.whole_word
        if (cbase.flags.flags.zero == 0)
          cbase.PC = (layout.whole_word & 0xFFFFFFFFFFFF);
        continue;
      case RBC_OP_JNC:
        if (cbase.flags.flags.carry == 0)
          cbase.PC = (layout.whole_word & 0xFFFFFFFFFFFF);
        continue;
      case RBC_OP_JC:
        if (cbase.flags.flags.carry == 1)
          cbase.PC = (layout.whole_word & 0xFFFFFFFFFFFF);
        continue;
      case RBC_OP_JNO:
        if (cbase.flags.flags.overflow == 0)
          cbase.PC = (layout.whole_word & 0xFFFFFFFFFFFF);
        continue;
      case RBC_OP_JO:
        if (cbase.flags.flags.overflow == 1)
          cbase.PC = (layout.whole_word & 0xFFFFFFFFFFFF);
        continue;
      case RBC_OP_JNN:
        if (cbase.flags.flags.negative == 0)
          cbase.PC = (layout.whole_word & 0xFFFFFFFFFFFF);
        continue;
      case RBC_OP_JN:
        if (cbase.flags.flags.negative == 1)
          cbase.PC = (layout.whole_word & 0xFFFFFFFFFFFF);
        continue;
      case RBC_OP_JS:
      case RBC_OP_JNG:
        if (cbase.flags.flags.negative == 1)
          cbase.PC = (layout.whole_word & 0xFFFFFFFFFFFF);
        continue;
      case RBC_OP_JNS:
      case RBC_OP_JG:
        if (cbase.flags.flags.negative == 0)
          cbase.PC = (layout.whole_word & 0xFFFFFFFFFFFF);
        continue;
      case RBC_OP_JGE:
        if (cbase.flags.flags.negative == 0 || cbase.flags.flags.zero == 0)
          cbase.PC = (layout.whole_word & 0xFFFFFFFFFFFF);
        continue;
      case RBC_OP_JSE:
        if (cbase.flags.flags.negative == 1 || cbase.flags.flags.zero == 0)
          cbase.PC = (layout.whole_word & 0xFFFFFFFFFFFF);
        continue;
      case RBC_OP_CALL:
        rbc_icall(&cbase, &core->kill_core, layout.whole_word);
        continue;
      case RBC_OP_RET:
        rbc_iret(&cbase, &core->kill_core, layout.whole_word);
        break;
      case RBC_OP_RETZ:
      case RBC_OP_RETE:
        if (cbase.flags.flags.zero == 1)
          rbc_iret(&cbase, &core->kill_core, layout.whole_word);
        break;
      case RBC_OP_RETNZ:
      case RBC_OP_RETNE:
        if (cbase.flags.flags.zero == 0)
          rbc_iret(&cbase, &core->kill_core, layout.whole_word);
        break;
      case RBC_OP_RETNC:
        if (cbase.flags.flags.carry == 0)
          rbc_iret(&cbase, &core->kill_core, layout.whole_word);
        break;
      case RBC_OP_RETC:
        if (cbase.flags.flags.carry == 1)
          rbc_iret(&cbase, &core->kill_core, layout.whole_word);
        break;
      case RBC_OP_RETNO:
        if (cbase.flags.flags.overflow == 0)
          rbc_iret(&cbase, &core->kill_core, layout.whole_word);
        break;
      case RBC_OP_RETO:
        if (cbase.flags.flags.overflow == 1)
          rbc_iret(&cbase, &core->kill_core, layout.whole_word);
        break;
      case RBC_OP_RETNN:
        if (cbase.flags.flags.negative == 0)
          rbc_iret(&cbase, &core->kill_core, layout.whole_word);
        break;
      case RBC_OP_RETN:
        if (cbase.flags.flags.negative == 1)
          rbc_iret(&cbase, &core->kill_core, layout.whole_word);
        break;
      case RBC_OP_RETS:
      case RBC_OP_RETNG:
        if (cbase.flags.flags.negative == 1)
          rbc_iret(&cbase, &core->kill_core, layout.whole_word);
        break;
      case RBC_OP_RETNS:
      case RBC_OP_RETG:
        if (cbase.flags.flags.negative == 0)
          rbc_iret(&cbase, &core->kill_core, layout.whole_word);
        break;
      case RBC_OP_RETGE:
        if (cbase.flags.flags.negative == 0 || cbase.flags.flags.zero == 1)
          rbc_iret(&cbase, &core->kill_core, layout.whole_word);
        break;
      case RBC_OP_RETSE:
        if (cbase.flags.flags.negative == 1 || cbase.flags.flags.zero == 1)
          rbc_iret(&cbase, &core->kill_core, layout.whole_word);
        break;
      case RBC_OP_LOOP:
        if (cbase.REGISTER_FILE[RBC_R2] != 0) {
          cbase.PC = layout.whole_word & 0xFFFFFFFFFFFF;
          continue;
        }
        break;
      case RBC_OP_CALL_REG:
        rbc_icall_reg(&cbase, &core->kill_core, layout.whole_word);
        continue;
      case RBC_OP_JMP_REGR:
        cbase.PC = cbase.REGISTER_FILE[layout.bytes.b7 & RBC_R15];
        continue;
      case RBC_OP_PUSH_IMM64:
        rbc_ipush_immq(&cbase, &core->kill_core, layout.whole_word);
        break;
      case RBC_OP_PUSH_REG:
        rbc_ipush_reg(&cbase, &core->kill_core, layout.whole_word);
        break;
      case RBC_OP_POP64:
        rbc_ipopq(&cbase, &core->kill_core, layout.whole_word);
        break;
      case RBC_OP_PUSHA:
        rbc_ipusha(&cbase, &core->kill_core, layout.whole_word);
        break;
      case RBC_OP_POPA:
        rbc_ipopa(&cbase, &core->kill_core, layout.whole_word);
        break;
      case RBC_OP_PUSH_MEMB:
        rbc_ipush_memb(&cbase, &core->kill_core, layout.whole_word);
        break;
      case RBC_OP_PUSH_MEMW:
        rbc_ipush_memw(&cbase, &core->kill_core, layout.whole_word);
        break;
      case RBC_OP_PUSH_MEMD:
        rbc_ipush_memd(&cbase, &core->kill_core, layout.whole_word);
        break;
      case RBC_OP_PUSH_MEMQ:
        rbc_ipush_memq(&cbase, &core->kill_core, layout.whole_word);
        break;
      case RBC_OP_POP_MEMB:
        rbc_ipop_memb(&cbase, &core->kill_core, layout.whole_word);
        break;
      case RBC_OP_POP_MEMW:
        rbc_ipop_memw(&cbase, &core->kill_core, layout.whole_word);
        break;
      case RBC_OP_POP_MEMD:
        rbc_ipop_memd(&cbase, &core->kill_core, layout.whole_word);
        break;
      case RBC_OP_POP_MEMQ:
        rbc_ipop_memq(&cbase, &core->kill_core, layout.whole_word);
        break;
      case RBC_OP_LOADSQ:
        rbc_iloadsq(&cbase, &core->kill_core, layout.whole_word);
        break;
      case RBC_OP_STORESQ:
        rbc_istoresq(&cbase, &core->kill_core, layout.whole_word);
        break;
      case RBC_OP_AND_IMM:
        rbc_icmp_imm(&cbase, &core->kill_core, layout.whole_word);
        break;
      case RBC_OP_AND_REG:
        cbase.REGISTER_FILE[layout.bytes.b6 & RBC_R15] &=
            cbase.REGISTER_FILE[layout.bytes.b7 & RBC_R15];
        break;
      case RBC_OP_OR_IMM:
        rbc_ior_imm(&cbase, &core->kill_core, layout.whole_word);
        break;
      case RBC_OP_OR_REG:
        cbase.REGISTER_FILE[layout.bytes.b6 & RBC_R15] |=
            cbase.REGISTER_FILE[layout.bytes.b7 & RBC_R15];
        break;
      case RBC_OP_XOR_IMM:
        rbc_ixor_imm(&cbase, &core->kill_core, layout.whole_word);
        break;
      case RBC_OP_XOR_REG:
        cbase.REGISTER_FILE[layout.bytes.b6 & RBC_R15] ^=
            cbase.REGISTER_FILE[layout.bytes.b7 & RBC_R15];
        break;
      case RBC_OP_NOT:
        cbase.REGISTER_FILE[layout.bytes.b7 & RBC_R15] =
            ~cbase.REGISTER_FILE[layout.bytes.b7];
        break;
      case RBC_OP_LSHIFT:
        cbase.REGISTER_FILE[layout.bytes.b1 & RBC_R15] <<= layout.bytes.b7 & 63;
        break;
      case RBC_OP_RSHIFT:
        cbase.REGISTER_FILE[layout.bytes.b1 & RBC_R15] >>= layout.bytes.b7 & 63;
        break;
      case RBC_OP_LSHIFT_REGR:
        cbase.REGISTER_FILE[layout.bytes.b6 & RBC_R15] <<=
            cbase.REGISTER_FILE[layout.bytes.b7 & RBC_R15] & 63;
        break;
      case RBC_OP_RSHIFT_REGR:
        cbase.REGISTER_FILE[layout.bytes.b6 & RBC_R15] >>=
            cbase.REGISTER_FILE[layout.bytes.b7 & RBC_R15] & 63;
        break;
      case RBC_OP_CMP_IMM:
        rbc_icmp_imm(&cbase, &core->kill_core, layout.whole_word);
        break;
      case RBC_OP_CMP_REG:
        merry_compare_two_values(
            cbase.REGISTER_FILE[layout.bytes.b6 & RBC_R15],
            cbase.REGISTER_FILE[layout.bytes.b7 & RBC_R15]);
        cbase.flags.regr = merry_obtain_flags_regr();
        break;
      case RBC_OP_CMP_IMM_MEMB:
        rbc_icmp_imm_memb(&cbase, &core->kill_core, layout.whole_word);
        break;
      case RBC_OP_CMP_IMM_MEMW:
        rbc_icmp_imm_memw(&cbase, &core->kill_core, layout.whole_word);
        break;
      case RBC_OP_CMP_IMM_MEMD:
        rbc_icmp_imm_memd(&cbase, &core->kill_core, layout.whole_word);
        break;
      case RBC_OP_CMP_IMM_MEMQ:
        rbc_icmp_imm_memq(&cbase, &core->kill_core, layout.whole_word);
        break;
      case RBC_OP_FCMP:
        rbc_compare_f64(cbase.REGISTER_FILE[layout.bytes.b6 & RBC_R15],
                        cbase.REGISTER_FILE[layout.bytes.b7 & RBC_R15],
                        &cbase.fflags);
        break;
      case RBC_OP_FCMP32:
        rbc_compare_f32(cbase.REGISTER_FILE[layout.bytes.b6 & RBC_R15],
                        cbase.REGISTER_FILE[layout.bytes.b7 & RBC_R15],
                        &cbase.fflags);
        break;
      case RBC_OP_CIN:
        cbase.REGISTER_FILE[layout.bytes.b7 & RBC_R15] = getchar();
        break;
      case RBC_OP_COUT:
        putchar((int)cbase.REGISTER_FILE[layout.bytes.b7 & RBC_R15]);
        break;
      case RBC_OP_SIN_REG:
        layout.whole_word = (cbase.REGISTER_FILE[layout.bytes.b7 & RBC_R15]);
      case RBC_OP_SIN:
        rbc_isin(&cbase, &core->kill_core, layout.whole_word);
        break;
      case RBC_OP_SOUT_REG:
        layout.bytes.b7 =
            (cbase.REGISTER_FILE[layout.bytes.b7 & RBC_R15] & 0xFFFFFFFFFFFF);
      case RBC_OP_SOUT:
        rbc_isout(&cbase, &core->kill_core, layout.whole_word);
        break;
      case RBC_OP_IN:
        fscanf(stdin, "%hhi", &mem.bytes.b7);
        cbase.REGISTER_FILE[layout.bytes.b7 & RBC_R15] = mem.whole_word;
        break;
      case RBC_OP_OUT:
        mem.whole_word = cbase.REGISTER_FILE[layout.bytes.b7 & RBC_R15];
        fprintf(stdout, "%hhi", mem.bytes.b7);
        break;
      case RBC_OP_INW:
        fscanf(stdin, "%hd", &mem.half_half_words.w3);
        cbase.REGISTER_FILE[layout.bytes.b7 & RBC_R15] = mem.whole_word;
        break;
      case RBC_OP_OUTW:
        mem.whole_word = cbase.REGISTER_FILE[layout.bytes.b7 & RBC_R15];
        fprintf(stdout, "%hd", mem.half_half_words.w3);
        break;
      case RBC_OP_IND:
        fscanf(stdin, "%d", &mem.half_words.w1);
        cbase.REGISTER_FILE[layout.bytes.b7 & RBC_R15] = mem.whole_word;
        break;
      case RBC_OP_OUTD:
        mem.whole_word = cbase.REGISTER_FILE[layout.bytes.b7 & RBC_R15];
        fprintf(stdout, "%d", mem.half_words.w1);
        break;
      case RBC_OP_INQ:
        fscanf(stdin, "%zi", &cbase.REGISTER_FILE[layout.bytes.b7 & RBC_R15]);
        break;
      case RBC_OP_OUTQ:
        fprintf(stdout, "%zi", cbase.REGISTER_FILE[layout.bytes.b7 & RBC_R15]);
        break;
      case RBC_OP_UIN:
        fscanf(stdin, "%hhu", &mem.bytes.b7);
        cbase.REGISTER_FILE[layout.bytes.b7 & RBC_R15] = mem.whole_word;
        break;
      case RBC_OP_UOUT:
        mem.whole_word = cbase.REGISTER_FILE[layout.bytes.b7 & RBC_R15];
        fprintf(stdout, "%hhu", mem.bytes.b7);
        break;
      case RBC_OP_UINW:
        fscanf(stdin, "%hu", &mem.half_half_words.w3);
        cbase.REGISTER_FILE[layout.bytes.b7 & RBC_R15] = mem.whole_word;
        break;
      case RBC_OP_UOUTW:
        mem.whole_word = cbase.REGISTER_FILE[layout.bytes.b7 & RBC_R15];
        fprintf(stdout, "%hu", mem.half_half_words.w3);
        break;
      case RBC_OP_UIND:
        fscanf(stdin, "%u", &mem.half_words.w1);
        cbase.REGISTER_FILE[layout.bytes.b7 & RBC_R15] = mem.whole_word;
        break;
      case RBC_OP_UOUTD:
        mem.whole_word = cbase.REGISTER_FILE[layout.bytes.b7 & RBC_R15];
        fprintf(stdout, "%u", mem.half_words.w1);
        break;
      case RBC_OP_UINQ:
        fscanf(stdin, "%zu", &cbase.REGISTER_FILE[layout.bytes.b7 & RBC_R15]);
        break;
      case RBC_OP_UOUTQ:
        fprintf(stdout, "%zu", cbase.REGISTER_FILE[layout.bytes.b7 & RBC_R15]);
        break;
      case RBC_OP_INF:
        fscanf(stdin, "%lf", &dtoq.d_val);
        cbase.REGISTER_FILE[layout.bytes.b7 & RBC_R15] = dtoq.q_val;
        break;
      case RBC_OP_OUTF:
        dtoq.q_val = cbase.REGISTER_FILE[layout.bytes.b7 & RBC_R15];
        fprintf(stdout, "%lf", dtoq.d_val);
        break;
      case RBC_OP_INF32:
        fscanf(stdin, "%f", &ftod.fl_val);
        cbase.REGISTER_FILE[layout.bytes.b7 & RBC_R15] = ftod.d_val;
        break;
      case RBC_OP_OUTF32:
        ftod.d_val = cbase.REGISTER_FILE[layout.bytes.b7 & RBC_R15];
        fprintf(stdout, "%f", ftod.fl_val);
        break;
      case RBC_OP_OUTR:
        for (msize_t i = 0; i <= RBC_R15; i++)
          fprintf(stdout, "%zi\n", cbase.REGISTER_FILE[i]);
        break;
      case RBC_OP_UOUTR:
        for (msize_t i = 0; i <= RBC_R15; i++)
          fprintf(stdout, "%zu\n", cbase.REGISTER_FILE[i]);
        break;
      case RBC_OP_LOADB:
        rbc_iloadb(&cbase, &core->kill_core, layout.whole_word);
        break;
      case RBC_OP_LOADW:
        rbc_iloadw(&cbase, &core->kill_core, layout.whole_word);
        break;
      case RBC_OP_LOADD:
        rbc_iloadd(&cbase, &core->kill_core, layout.whole_word);
        break;
      case RBC_OP_LOADQ:
        rbc_iloadq(&cbase, &core->kill_core, layout.whole_word);
        break;
      case RBC_OP_STOREB:
        rbc_istoreb(&cbase, &core->kill_core, layout.whole_word);
        break;
      case RBC_OP_STOREW:
        rbc_istorew(&cbase, &core->kill_core, layout.whole_word);
        break;
      case RBC_OP_STORED:
        rbc_istored(&cbase, &core->kill_core, layout.whole_word);
        break;
      case RBC_OP_STOREQ:
        rbc_istoreq(&cbase, &core->kill_core, layout.whole_word);
        break;
      case RBC_OP_LOADB_REG:
        rbc_iloadb_reg(&cbase, &core->kill_core, layout.whole_word);
        break;
      case RBC_OP_LOADW_REG:
        rbc_iloadw_reg(&cbase, &core->kill_core, layout.whole_word);
        break;
      case RBC_OP_LOADD_REG:
        rbc_iloadd_reg(&cbase, &core->kill_core, layout.whole_word);
        break;
      case RBC_OP_LOADQ_REG:
        rbc_iloadq_reg(&cbase, &core->kill_core, layout.whole_word);
        break;
      case RBC_OP_STOREB_REG:
        rbc_istoreb_reg(&cbase, &core->kill_core, layout.whole_word);
        break;
      case RBC_OP_STOREW_REG:
        rbc_istorew_reg(&cbase, &core->kill_core, layout.whole_word);
        break;
      case RBC_OP_STORED_REG:
        rbc_istored_reg(&cbase, &core->kill_core, layout.whole_word);
        break;
      case RBC_OP_STOREQ_REG:
        rbc_istoreq_reg(&cbase, &core->kill_core, layout.whole_word);
        break;
      case RBC_OP_ATOMIC_LOADB:
        rbc_iatm_loadb(&cbase, &core->kill_core, layout.whole_word);
        break;
      case RBC_OP_ATOMIC_LOADW:
        rbc_iatm_loadw(&cbase, &core->kill_core, layout.whole_word);
        break;
      case RBC_OP_ATOMIC_LOADD:
        rbc_iatm_loadd(&cbase, &core->kill_core, layout.whole_word);
        break;
      case RBC_OP_ATOMIC_LOADQ:
        rbc_iatm_loadq(&cbase, &core->kill_core, layout.whole_word);
        break;
      case RBC_OP_ATOMIC_STOREB:
        rbc_iatm_storeb(&cbase, &core->kill_core, layout.whole_word);
        break;
      case RBC_OP_ATOMIC_STOREW:
        rbc_iatm_storew(&cbase, &core->kill_core, layout.whole_word);
        break;
      case RBC_OP_ATOMIC_STORED:
        rbc_iatm_stored(&cbase, &core->kill_core, layout.whole_word);
        break;
      case RBC_OP_ATOMIC_STOREQ:
        rbc_iatm_storeq(&cbase, &core->kill_core, layout.whole_word);
        break;
      case RBC_OP_ATOMIC_LOADB_REG:
        rbc_iatm_loadb_reg(&cbase, &core->kill_core, layout.whole_word);
        break;
      case RBC_OP_ATOMIC_LOADW_REG:
        rbc_iatm_loadw_reg(&cbase, &core->kill_core, layout.whole_word);
        break;
      case RBC_OP_ATOMIC_LOADD_REG:
        rbc_iatm_loadd_reg(&cbase, &core->kill_core, layout.whole_word);
        break;
      case RBC_OP_ATOMIC_LOADQ_REG:
        rbc_iatm_loadq_reg(&cbase, &core->kill_core, layout.whole_word);
        break;
      case RBC_OP_ATOMIC_STOREB_REG:
        rbc_iatm_storeb_reg(&cbase, &core->kill_core, layout.whole_word);
        break;
      case RBC_OP_ATOMIC_STOREW_REG:
        rbc_iatm_storew_reg(&cbase, &core->kill_core, layout.whole_word);
        break;
      case RBC_OP_ATOMIC_STORED_REG:
        rbc_iatm_stored_reg(&cbase, &core->kill_core, layout.whole_word);
        break;
      case RBC_OP_ATOMIC_STOREQ_REG:
        rbc_iatm_storeq_reg(&cbase, &core->kill_core, layout.whole_word);
        break;
      case RBC_OP_LEA:
        cbase.REGISTER_FILE[layout.bytes.b4 & RBC_R15] =
            cbase.REGISTER_FILE[layout.bytes.b5 & RBC_R15] +
            cbase.REGISTER_FILE[layout.bytes.b6 & RBC_R15] *
                cbase.REGISTER_FILE[layout.bytes.b7 & RBC_R15];
        break;
      case RBC_OP_CFLAGS:
        cbase.flags.flags.carry = 0;
        cbase.flags.flags.negative = 0;
        cbase.flags.flags.zero = 0;
        cbase.flags.flags.overflow = 0;
        break;
      case RBC_OP_RESET:
        for (msize_t i = 0; i < RBC_REG_COUNT; i++)
          cbase.REGISTER_FILE[i] = 0;
        break;
      case RBC_OP_CMPXCHG_REGR:
        rbc_icmpxchg_reg(&cbase, &core->kill_core, layout.whole_word);
        break;
      case RBC_OP_CMPXCHG:
        rbc_icmpxchg(&cbase, &core->kill_core, layout.whole_word);
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
          if (cbase.fflags.zf)
            rbc_imov_imm(&cbase, &core->kill_core, layout.whole_word);
          break;
        case RBC_OP_MOVFNZ:
          if (!cbase.fflags.zf)
            rbc_imov_imm(&cbase, &core->kill_core, layout.whole_word);
          break;
        case RBC_OP_MOVFN:
          if (cbase.fflags.sf)
            rbc_imov_imm(&cbase, &core->kill_core, layout.whole_word);
          break;
        case RBC_OP_MOVFNN:
          if (!cbase.fflags.sf)
            rbc_imov_imm(&cbase, &core->kill_core, layout.whole_word);
          break;
        case RBC_OP_MOVFUF:
          if (cbase.fflags.uof)
            rbc_imov_imm(&cbase, &core->kill_core, layout.whole_word);
          break;
        case RBC_OP_MOVFNUF:
          if (!cbase.fflags.uof)
            rbc_imov_imm(&cbase, &core->kill_core, layout.whole_word);
          break;
        case RBC_OP_MOVFO:
          if (cbase.fflags.of)
            rbc_imov_imm(&cbase, &core->kill_core, layout.whole_word);
          break;
        case RBC_OP_MOVFNO:
          if (!cbase.fflags.of)
            rbc_imov_imm(&cbase, &core->kill_core, layout.whole_word);
          break;
        case RBC_OP_MOVFU:
          if (cbase.fflags.uf)
            rbc_imov_imm(&cbase, &core->kill_core, layout.whole_word);
          break;
        case RBC_OP_MOVFNU:
          if (!cbase.fflags.uf)
            rbc_imov_imm(&cbase, &core->kill_core, layout.whole_word);
          break;
        case RBC_OP_MOVFI:
          if (cbase.fflags.inv)
            rbc_imov_imm(&cbase, &core->kill_core, layout.whole_word);
          break;
        case RBC_OP_MOVFNI:
          if (!cbase.fflags.inv)
            rbc_imov_imm(&cbase, &core->kill_core, layout.whole_word);
          break;
        case RBC_OP_JFZ:
          if (cbase.fflags.zf)
            cbase.PC = (layout.whole_word & 0xFFFFFFFFFFFF);
          continue;
        case RBC_OP_JFNZ:
          if (!cbase.fflags.zf)
            cbase.PC = (layout.whole_word & 0xFFFFFFFFFFFF);
          continue;
        case RBC_OP_JFN:
          if (cbase.fflags.sf)
            cbase.PC = (layout.whole_word & 0xFFFFFFFFFFFF);
          continue;
        case RBC_OP_JFNN:
          if (!cbase.fflags.sf)
            cbase.PC = (layout.whole_word & 0xFFFFFFFFFFFF);
          continue;
        case RBC_OP_JFUF:
          if (cbase.fflags.uof)
            cbase.PC = (layout.whole_word & 0xFFFFFFFFFFFF);
          continue;
        case RBC_OP_JFNUF:
          if (!cbase.fflags.uof)
            cbase.PC = (layout.whole_word & 0xFFFFFFFFFFFF);
          continue;
        case RBC_OP_JFO:
          if (cbase.fflags.of)
            cbase.PC = (layout.whole_word & 0xFFFFFFFFFFFF);
          continue;
        case RBC_OP_JFNO:
          if (!cbase.fflags.of)
            cbase.PC = (layout.whole_word & 0xFFFFFFFFFFFF);
          continue;
        case RBC_OP_JFU:
          if (cbase.fflags.uf)
            cbase.PC = (layout.whole_word & 0xFFFFFFFFFFFF);
          continue;
        case RBC_OP_JFNU:
          if (!cbase.fflags.uf)
            cbase.PC = (layout.whole_word & 0xFFFFFFFFFFFF);
          continue;
        case RBC_OP_JFI:
          if (cbase.fflags.inv)
            cbase.PC = (layout.whole_word & 0xFFFFFFFFFFFF);
          continue;
        case RBC_OP_JFNI:
          if (!cbase.fflags.inv)
            cbase.PC = (layout.whole_word & 0xFFFFFFFFFFFF);
          continue;
        case RBC_OP_RETFZ:
          if (cbase.fflags.zf)
            rbc_iret(&cbase, &core->kill_core, layout.whole_word);
          break;
        case RBC_OP_RETFNZ:
          if (!cbase.fflags.zf)
            rbc_iret(&cbase, &core->kill_core, layout.whole_word);
          break;
        case RBC_OP_RETFN:
          if (cbase.fflags.sf)
            rbc_iret(&cbase, &core->kill_core, layout.whole_word);
          break;
        case RBC_OP_RETFNN:
          if (!cbase.fflags.sf)
            rbc_iret(&cbase, &core->kill_core, layout.whole_word);
          break;
        case RBC_OP_RETFUF:
          if (cbase.fflags.uof)
            rbc_iret(&cbase, &core->kill_core, layout.whole_word);
          break;
        case RBC_OP_RETFNUF:
          if (!cbase.fflags.uof)
            rbc_iret(&cbase, &core->kill_core, layout.whole_word);
          break;
        case RBC_OP_RETFO:
          if (cbase.fflags.of)
            rbc_iret(&cbase, &core->kill_core, layout.whole_word);
          break;
        case RBC_OP_RETFNO:
          if (!cbase.fflags.of)
            rbc_iret(&cbase, &core->kill_core, layout.whole_word);
          break;
        case RBC_OP_RETFU:
          if (cbase.fflags.uf)
            rbc_iret(&cbase, &core->kill_core, layout.whole_word);
          break;
        case RBC_OP_RETFNU:
          if (!cbase.fflags.uf)
            rbc_iret(&cbase, &core->kill_core, layout.whole_word);
          break;
        case RBC_OP_RETFI:
          if (cbase.fflags.inv)
            rbc_iret(&cbase, &core->kill_core, layout.whole_word);
          break;
        case RBC_OP_RETFNI:
          if (!cbase.fflags.inv)
            rbc_iret(&cbase, &core->kill_core, layout.whole_word);
          break;
        default:
          break;
        }
      }
      }
      cbase.PC += 8;
    }
  }
  core->terminate = mtrue;
  core->interrupt = mtrue;
  for (msize_t i = 0;
       i < merry_RBCThread_list_size(core->rbc_cbase.child_threads); i++) {
    merry_thread_join(core->rbc_cbase.child_threads->buf[i], NULL);
  }
  return RET_SUCCESS;
}

MerryCoreBase *rbc_master_core_create_base(msize_t *CODE) {
  MerryCoreBase *base = (MerryCoreBase *)malloc(sizeof(MerryCoreBase));
  if (!base) {
    MFATAL("RBC", "Failed to initialize core base", NULL);
    *CODE = RBC_SYS_FAILURE;
    return RET_NULL;
  }
  if (merry_cond_init(&base->cond) != MRES_SUCCESS) {
    MFATAL("RBC", "Failed to obtain condition variable", NULL);
    free(base);
    return RET_NULL;
  }
  base->type = __REGR_CORE;
  base->createc = rbc_master_core_create;
  base->deletec = rbc_master_core_destroy;
  base->execc = rbc_master_core_run;
  base->predel = rbc_master_core_prep_for_deletion;
  base->setinp = rbc_master_core_set_input;
  base->prepcore = rbc_master_core_prepare_core;

  return base;
}

void rbc_master_core_destroy_base(MerryCoreBase *base) {
  merry_check_ptr(base);
  merry_cond_destroy(&base->cond);
  free(base);
}

void rbc_master_core_prep_for_deletion(mptr_t c) {
  // This function's job is to prepare the core
  // for deletion by setting the correct flags
  // and such. It is a procedure in of itself
  merry_check_ptr(c);

  RBCMasterCore *core = (RBCMasterCore *)c;

  // For now, this is everything we need
  // This will command the child cores to terminate
  // and since Graves will command the master core, it will
  // work
  core->terminate = mtrue;
  core->interrupt = mtrue;
}

mret_t rbc_master_core_set_input(mptr_t c, mstr_t path, msize_t *CODE) {
  merry_check_ptr(c);
  merry_check_ptr(path);

  // For the moment, all it does is store the path of
  // the input Locally and allocate the reader
  RBCMasterCore *core = (RBCMasterCore *)c;
  core->rbc_cbase.inp_path = path;
  if ((core->inp = rbc_input_init()) == RET_NULL) {
    MLOG("RBC", "Failed to initialize the reader for input file %s", path);
    return RET_FAILURE;
  }
  return RET_SUCCESS;
}

mret_t rbc_master_core_prepare_core(mptr_t c, msize_t *CODE) {
  // The only job of this function is to prepare the master
  // core for execution
  merry_check_ptr(c);

  RBCMasterCore *core = (RBCMasterCore *)c;

  if (!core->inp) {
    MFATAL("RBC", "Input not initialized before preparation", NULL);
    return RET_FAILURE;
  }

  // Setup the RAMs
  if ((core->rbc_cbase.iram = rbc_memory_init()) == RET_NULL) {
    MFATAL("RBC", "Failed to initialize instrucion memory: PATH=%s",
           core->rbc_cbase.inp_path);
    return RET_FAILURE;
  }

  if ((core->rbc_cbase.dram = rbc_memory_init()) == RET_NULL) {
    MFATAL("RBC", "Failed to initialize data memory: PATH=%s",
           core->rbc_cbase.inp_path);
    return RET_FAILURE;
  }

  // Read the input file
  if (rbc_input_read(core->inp, core->rbc_cbase.inp_path) == RET_FAILURE) {
    MFATAL("RBC", "Failed to read input file %s", core->rbc_cbase.inp_path);
    return RET_FAILURE;
  }

  // Prep the memories
  if (rbc_memory_populate(core->rbc_cbase.iram, core->inp->instruction_len,
                          core->inp->instructions) == RET_FAILURE) {
    MFATAL("RBC", "Failed to populate the instrucion memory: PATH=%s",
           core->rbc_cbase.inp_path);
    return RET_FAILURE;
  }

  if (rbc_memory_populate(core->rbc_cbase.dram, core->inp->data_len,
                          core->inp->data) == RET_FAILURE) {
    MFATAL("RBC", "Failed to populate the data memory: PATH=%s",
           core->rbc_cbase.inp_path);
    return RET_FAILURE;
  }

  // Now other fields
  core->rbc_cbase.req.args = &core->rbc_cbase.args;
  core->rbc_cbase.req.base = core->base;
  core->rbc_cbase.req.used_cond = &core->base->cond;

  core->interrupt = mfalse;
  core->terminate = mfalse;
  core->pause = mfalse;
  core->rbc_cbase.terminate = mfalse;
  core->kill_core = mfalse;

  core->rbc_cbase.check_after = 3;

  return RET_SUCCESS;
}

RBCCore *rbc_core_create() {
  RBCCore *core = (RBCCore *)malloc(sizeof(RBCCore));
  if (!core) {
    MFATAL("RBC:C", "Failed to allocate memory for a new core", NULL);
    return RET_NULL;
  }
  mresult_t res;
  if ((res = merry_get_anonymous_memory((mptr_t *)&core->rbc_cbase.stack,
                                        _RBC_STACK_LEN_)) != MRES_SUCCESS) {
    MFATAL("RBC:C", "Failed to initialize the stack", NULL);
    merry_RBCThread_list_destroy(core->rbc_cbase.child_threads);
    merry_Interface_list_destroy(core->rbc_cbase.interfaces);
    free(core);
    return RET_NULL;
  }

  core->rbc_cbase.SP = 0;
  core->rbc_cbase.BP = 0;

  if ((res = merry_RBCProcFrame_stack_init(&core->rbc_cbase.stack_frames,
                                           _RBC_CALL_DEPTH_)) != MRES_SUCCESS) {
    MFATAL("RBC:C", "Failed to initialize the stack", NULL);
    merry_RBCThread_list_destroy(core->rbc_cbase.child_threads);
    merry_Interface_list_destroy(core->rbc_cbase.interfaces);
    merry_return_memory(core->rbc_cbase.stack, _RBC_STACK_LEN_);
    free(core);
    return RET_NULL;
  }

  if (merry_cond_init(&core->cond) != MRES_SUCCESS) {
    MFATAL("RBC:C", "Failed to initialize component", NULL);
    merry_RBCThread_list_destroy(core->rbc_cbase.child_threads);
    merry_Interface_list_destroy(core->rbc_cbase.interfaces);
    merry_return_memory(core->rbc_cbase.stack, _RBC_STACK_LEN_);
    merry_RBCProcFrame_stack_destroy(core->rbc_cbase.stack_frames);
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
       i < merry_RBCThread_list_size(core->rbc_cbase.child_threads); i++) {
    merry_thread_join(core->rbc_cbase.child_threads->buf[i], NULL);
  }
}

mret_t rbc_base_start(RBCCore *core, mthread_t *th) {
  merry_check_ptr(core);
  merry_check_ptr(th);
  return merry_thread_create(th, rbc_core_run, core);
}
