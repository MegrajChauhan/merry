#include <merry_core.h>
#include <merry_core_instruction_handler.h>

void merry_core_compare_f32(float a, float b, MerryCoreFFlagsRegr *regr) {
  regr->zf = (a == b && a == 0.0f);
  regr->sf = ((a - b) < 0.0f);
  regr->uof = isnan(a) || isnan(b);
  regr->inv = isnan(a - b) || (a == 0.0f && signbit(a) != signbit(b));

  float diff = a - b;
  regr->of = diff > FLT_MAX;
  regr->uf = fabsf(diff) < FLT_MIN && diff != 0.0f;
}

void merry_core_compare_f64(double a, double b, MerryCoreFFlagsRegr *regr) {
  regr->zf = (a == b && a == 0.0);
  regr->sf = ((a - b) < 0.0);
  regr->uof = isnan(a) || isnan(b);
  regr->inv = isnan(a - b) || (a == 0.0 && signbit(a) != signbit(b));

  double diff = a - b;
  regr->of = diff > DBL_MAX;
  regr->uf = fabs(diff) < DBL_MIN && diff != 0.0;
}

/*
 * The various system interrupts will use various registers
 * to obtain the required arguments.
 * The type of system interrupt is determined by the value in R15
 * Users are free to give any importance to any register
 * The result will also be passed in R15
 * */
merry_core_ihdlr(sysint) {
  switch (core->REGISTER_FILE[MERRY_CORE_R15]) {
  default:
    core->REGISTER_FILE[MERRY_CORE_R15] = MRES_CORE_INVALID_SIR;
    break;
  }
  return mtrue;
}

/*
 * This will use R15 for input and result the same as sysint
 * R15 = status of the request
 * R14 = the actual error if any
 * */
merry_core_ihdlr(mint) {
  // MerryRequestArgs *args = &core->args;
  switch (core->REGISTER_FILE[MERRY_CORE_R15]) {
  default:
    core->REGISTER_FILE[MERRY_CORE_R15] = MRES_INVALID_ARGS;
    break;
  }
  return mtrue;
}

merry_core_ihdlr(add_imm) {
  register mbyte_t op1 = core->IR.whole_word & MERRY_CORE_R15;
  mqword_t op2;
  core->PC += 8;
  if (merry_core_memory_read_qword(core->iram, core->PC, &op2) !=
      MRES_SUCCESS) {
    MERR("INCOMPLETE INSTRUCTION at PC=%zu", core->PC);
    return mfalse;
  }
  core->REGISTER_FILE[op1] += op2;
  core->flags.regr = merry_obtain_flags_regr();
  return mtrue;
}

merry_core_ihdlr(add_reg) {
  register mbyte_t op1 = (core->IR.whole_word >> 4) & MERRY_CORE_R15;
  register mbyte_t op2 = core->IR.whole_word & MERRY_CORE_R15;
  core->REGISTER_FILE[op1] += core->REGISTER_FILE[op2];
  core->flags.regr = merry_obtain_flags_regr();
  return mtrue;
}

merry_core_ihdlr(sub_imm) {
  register mbyte_t op1 = core->IR.whole_word & MERRY_CORE_R15;
  mqword_t op2;
  core->PC += 8;
  if (merry_core_memory_read_qword(core->iram, core->PC, &op2) !=
      MRES_SUCCESS) {
    MERR("INCOMPLETE INSTRUCTION at PC=%zu", core->PC);
    return mfalse;
  }
  core->REGISTER_FILE[op1] -= op2;
  core->flags.regr = merry_obtain_flags_regr();
  return mtrue;
}

merry_core_ihdlr(sub_reg) {
  register mbyte_t op1 = (core->IR.whole_word >> 4) & MERRY_CORE_R15;
  register mbyte_t op2 = core->IR.whole_word & MERRY_CORE_R15;
  core->REGISTER_FILE[op1] -= core->REGISTER_FILE[op2];
  core->flags.regr = merry_obtain_flags_regr();
  return mtrue;
}

merry_core_ihdlr(mul_imm) {
  register mbyte_t op1 = core->IR.whole_word & MERRY_CORE_R15;
  mqword_t op2;
  core->PC += 8;
  if (merry_core_memory_read_qword(core->iram, core->PC, &op2) !=
      MRES_SUCCESS) {
    MERR("INCOMPLETE INSTRUCTION at PC=%zu", core->PC);
    return mfalse;
  }
  core->REGISTER_FILE[op1] *= op2;
  core->flags.regr = merry_obtain_flags_regr();
  return mtrue;
}

merry_core_ihdlr(mul_reg) {
  register mbyte_t op1 = (core->IR.whole_word >> 4) & MERRY_CORE_R15;
  register mbyte_t op2 = core->IR.whole_word & MERRY_CORE_R15;
  core->REGISTER_FILE[op1] *= core->REGISTER_FILE[op2];
  core->flags.regr = merry_obtain_flags_regr();
  return mtrue;
}

merry_core_ihdlr(div_imm) {
  register mbyte_t op1 = core->IR.whole_word & MERRY_CORE_R15;
  mqword_t op2;
  core->PC += 8;
  if (merry_core_memory_read_qword(core->iram, core->PC, &op2) !=
      MRES_SUCCESS) {
    MERR("INCOMPLETE INSTRUCTION at PC=%zu", core->PC);
    return mfalse;
  }
  if (op2 == 0) {
    MERR("Div by Zero: PC=%zu", core->PC);
    return mfalse;
  }
  core->REGISTER_FILE[op1] /= op2;
  core->flags.regr = merry_obtain_flags_regr();
  return mtrue;
}

merry_core_ihdlr(div_reg) {
  register mbyte_t op1 = (core->IR.whole_word >> 4) & MERRY_CORE_R15;
  register mbyte_t op2 = core->IR.whole_word & MERRY_CORE_R15;
  if (core->REGISTER_FILE[op2] == 0) {
    MERR("Div by Zero: PC=%zu", core->PC);
    return mfalse;
  }
  core->REGISTER_FILE[op1] /= core->REGISTER_FILE[op2];
  core->flags.regr = merry_obtain_flags_regr();
  return mtrue;
}

merry_core_ihdlr(mod_imm) {
  register mbyte_t op1 = core->IR.whole_word & MERRY_CORE_R15;
  mqword_t op2;
  core->PC += 8;
  if (merry_core_memory_read_qword(core->iram, core->PC, &op2) !=
      MRES_SUCCESS) {
    MERR("INCOMPLETE INSTRUCTION at PC=%zu", core->PC);
    return mfalse;
  }
  if (op2 == 0) {
    MERR("Div by Zero: PC=%zu", core->PC);
    return mfalse;
  }
  core->REGISTER_FILE[op1] %= op2;
  core->flags.regr = merry_obtain_flags_regr();
  return mtrue;
}

merry_core_ihdlr(mod_reg) {
  register mbyte_t op1 = (core->IR.whole_word >> 4) & MERRY_CORE_R15;
  register mbyte_t op2 = core->IR.whole_word & MERRY_CORE_R15;
  if (core->REGISTER_FILE[op2] == 0) {
    MERR("Div by Zero: PC=%zu", core->PC);
    return mfalse;
  }
  core->REGISTER_FILE[op1] %= core->REGISTER_FILE[op2];
  core->flags.regr = merry_obtain_flags_regr();
  return mtrue;
}

merry_core_ihdlr(fadd) {
  register mqword_t op1 = (core->IR.whole_word >> 4) & MERRY_CORE_R15;
  register mqword_t op2 = (core->IR.whole_word) & MERRY_CORE_R15;
  register MerryDoubleToQword a;
  register MerryDoubleToQword b;
  a.q_val = core->REGISTER_FILE[op1];
  b.q_val = core->REGISTER_FILE[op2];
  merry_core_compare_f64(a.d_val, b.d_val, &core->fflags);
  a.d_val += b.d_val;
  core->REGISTER_FILE[op1] = a.q_val;
  return mtrue;
}

merry_core_ihdlr(fsub) {
  register mqword_t op1 = (core->IR.whole_word >> 4) & MERRY_CORE_R15;
  register mqword_t op2 = (core->IR.whole_word) & MERRY_CORE_R15;
  register MerryDoubleToQword a;
  register MerryDoubleToQword b;
  a.q_val = core->REGISTER_FILE[op1];
  b.q_val = core->REGISTER_FILE[op2];
  merry_core_compare_f64(a.d_val, b.d_val, &core->fflags);
  a.d_val -= b.d_val;
  core->REGISTER_FILE[op1] = a.q_val;
  return mtrue;
}

merry_core_ihdlr(fmul) {
  register mqword_t op1 = (core->IR.whole_word >> 4) & MERRY_CORE_R15;
  register mqword_t op2 = (core->IR.whole_word) & MERRY_CORE_R15;
  register MerryDoubleToQword a;
  register MerryDoubleToQword b;
  a.q_val = core->REGISTER_FILE[op1];
  b.q_val = core->REGISTER_FILE[op2];
  merry_core_compare_f64(a.d_val, b.d_val, &core->fflags);
  a.d_val *= b.d_val;
  core->REGISTER_FILE[op1] = a.q_val;
  return mtrue;
}

merry_core_ihdlr(fdiv) {
  register mqword_t op1 = (core->IR.whole_word >> 4) & MERRY_CORE_R15;
  register mqword_t op2 = (core->IR.whole_word) & MERRY_CORE_R15;
  register MerryDoubleToQword a;
  register MerryDoubleToQword b;
  a.q_val = core->REGISTER_FILE[op1];
  b.q_val = core->REGISTER_FILE[op2];
  if (b.d_val == 0.0) {
    MERR("Div by Zero: PC=%zu", core->PC);
    return mfalse;
  }
  merry_core_compare_f64(a.d_val, b.d_val, &core->fflags);
  a.d_val /= b.d_val;
  core->REGISTER_FILE[op1] = a.q_val;
  return mtrue;
}

merry_core_ihdlr(fadd32) {
  register mqword_t op1 = (core->IR.whole_word >> 4) & MERRY_CORE_R15;
  register mqword_t op2 = (core->IR.whole_word) & MERRY_CORE_R15;
  register MerryFloatToDword a;
  register MerryFloatToDword b;
  a.d_val = core->REGISTER_FILE[op1];
  b.d_val = core->REGISTER_FILE[op2];
  merry_core_compare_f32(a.fl_val, b.fl_val, &core->fflags);
  a.fl_val += b.fl_val;
  core->REGISTER_FILE[op1] = a.d_val;
  return mtrue;
}

merry_core_ihdlr(fsub32) {
  register mqword_t op1 = (core->IR.whole_word >> 4) & MERRY_CORE_R15;
  register mqword_t op2 = (core->IR.whole_word) & MERRY_CORE_R15;
  register MerryFloatToDword a;
  register MerryFloatToDword b;
  a.d_val = core->REGISTER_FILE[op1];
  b.d_val = core->REGISTER_FILE[op2];
  merry_core_compare_f32(a.fl_val, b.fl_val, &core->fflags);
  a.fl_val -= b.fl_val;
  core->REGISTER_FILE[op1] = a.d_val;
  return mtrue;
}

merry_core_ihdlr(fmul32) {
  register mqword_t op1 = (core->IR.whole_word >> 4) & MERRY_CORE_R15;
  register mqword_t op2 = (core->IR.whole_word) & MERRY_CORE_R15;
  register MerryFloatToDword a;
  register MerryFloatToDword b;
  a.d_val = core->REGISTER_FILE[op1];
  b.d_val = core->REGISTER_FILE[op2];
  merry_core_compare_f32(a.fl_val, b.fl_val, &core->fflags);
  a.fl_val *= b.fl_val;
  core->REGISTER_FILE[op1] = a.d_val;
  return mtrue;
}

merry_core_ihdlr(fdiv32) {
  register mqword_t op1 = (core->IR.whole_word >> 4) & MERRY_CORE_R15;
  register mqword_t op2 = (core->IR.whole_word) & MERRY_CORE_R15;
  register MerryFloatToDword a;
  register MerryFloatToDword b;
  a.d_val = core->REGISTER_FILE[op1];
  b.d_val = core->REGISTER_FILE[op2];
  if (b.d_val == 0.0) {
    MERR("Div by Zero: PC=%zu", core->PC);
    return mfalse;
  }
  merry_core_compare_f32(a.fl_val, b.fl_val, &core->fflags);
  a.fl_val /= b.fl_val;
  core->REGISTER_FILE[op1] = a.d_val;
  return mtrue;
}

merry_core_ihdlr(inc) {
  core->REGISTER_FILE[core->IR.whole_word & MERRY_CORE_REG_COUNT]++;
  return mtrue;
}

merry_core_ihdlr(dec) {
  core->REGISTER_FILE[core->IR.whole_word & MERRY_CORE_REG_COUNT]--;
  return mtrue;
}

merry_core_ihdlr(mov_imm) {
  register mbyte_t op1 = core->IR.whole_word & MERRY_CORE_R15;
  mqword_t val;

  core->PC += 8;
  if (merry_core_memory_read_qword(core->iram, core->PC, &val) !=
      MRES_SUCCESS) {
    MERR("INCOMPLETE INTRUCTION at PC=%zu", core->PC);
    return mfalse;
  }

  core->REGISTER_FILE[op1] = val;
  return mtrue;
}

merry_core_ihdlr(mov_imm_32) {
  core->REGISTER_FILE[core->IR.bytes.b1 & MERRY_CORE_R15] =
      core->IR.half_words.w1;
  return mtrue;
}

merry_core_ihdlr(mov_reg) {
  core->REGISTER_FILE[core->IR.bytes.b7 >> 4] =
      core->REGISTER_FILE[core->IR.bytes.b7 & MERRY_CORE_R15];
  return mtrue;
}

merry_core_ihdlr(mov_reg8) {
  core->REGISTER_FILE[core->IR.bytes.b7 >> 4] =
      core->REGISTER_FILE[core->IR.bytes.b7 & MERRY_CORE_R15] & 0xFF;
  return mtrue;
}

merry_core_ihdlr(mov_reg16) {
  core->REGISTER_FILE[core->IR.bytes.b7 >> 4] =
      core->REGISTER_FILE[core->IR.bytes.b7 & MERRY_CORE_R15] & 0xFFFF;
  return mtrue;
}

merry_core_ihdlr(mov_reg32) {
  core->REGISTER_FILE[core->IR.bytes.b7 >> 4] =
      core->REGISTER_FILE[core->IR.bytes.b7 & MERRY_CORE_R15] & 0xFFFFFFFF;
  return mtrue;
}

merry_core_ihdlr(movesx_imm8) {
  register mbyte_t op1 = (core->IR.whole_word >> 48) & MERRY_CORE_R15;
  register mqword_t op2 = core->IR.whole_word & 0xFF;
  sign_extend8(op2);
  core->REGISTER_FILE[op1] = op2;
  return mtrue;
}

merry_core_ihdlr(movesx_imm16) {
  register mbyte_t op1 = (core->IR.whole_word >> 48) & MERRY_CORE_R15;
  register mqword_t op2 = core->IR.whole_word & 0xFFFF;
  sign_extend16(op2);
  core->REGISTER_FILE[op1] = op2;
  return mtrue;
}

merry_core_ihdlr(movesx_imm32) {
  register mbyte_t op1 = (core->IR.whole_word >> 48) & MERRY_CORE_R15;
  register mqword_t op2 = core->IR.whole_word & 0xFFFFFFFF;
  sign_extend32(op2);
  core->REGISTER_FILE[op1] = op2;
  return mtrue;
}

merry_core_ihdlr(movesx_reg8) {
  register mbyte_t op1 = (core->IR.whole_word >> 4) & MERRY_CORE_R15;
  register mbyte_t op2 =
      core->REGISTER_FILE[core->IR.whole_word & MERRY_CORE_R15];
  sign_extend8(op2);
  core->REGISTER_FILE[op1] = op2;
  return mtrue;
}

merry_core_ihdlr(movesx_reg16) {
  register mbyte_t op1 = (core->IR.whole_word >> 4) & MERRY_CORE_R15;
  register mbyte_t op2 =
      core->REGISTER_FILE[core->IR.whole_word & MERRY_CORE_R15];
  sign_extend16(op2);
  core->REGISTER_FILE[op1] = op2;
  return mtrue;
}

merry_core_ihdlr(movesx_reg32) {
  register mbyte_t op1 = (core->IR.whole_word >> 4) & MERRY_CORE_R15;
  register mbyte_t op2 =
      core->REGISTER_FILE[core->IR.whole_word & MERRY_CORE_R15];
  sign_extend32(op2);
  core->REGISTER_FILE[op1] = op2;
  return mtrue;
}

merry_core_ihdlr(excg) {
  register mbyte_t op1 = (core->IR.whole_word >> 4) & MERRY_CORE_R15;
  register mbyte_t op2 = core->IR.whole_word & MERRY_CORE_R15;
  register mqword_t temp = core->REGISTER_FILE[op1];
  core->REGISTER_FILE[op1] = core->REGISTER_FILE[op2];
  core->REGISTER_FILE[op2] = temp;
  return mtrue;
}

merry_core_ihdlr(excg8) {
  register mbyte_t op1 = (core->IR.whole_word >> 4) & MERRY_CORE_R15;
  register mbyte_t op2 = core->IR.whole_word & MERRY_CORE_R15;
  register mqword_t r1 = core->REGISTER_FILE[op1];
  core->REGISTER_FILE[op1] |= core->REGISTER_FILE[op2] & 0x00000000000000FF;
  core->REGISTER_FILE[op2] |= r1 & 0x00000000000000FF;
  return mtrue;
}

merry_core_ihdlr(excg16) {
  register mbyte_t op1 = (core->IR.whole_word >> 4) & MERRY_CORE_R15;
  register mbyte_t op2 = core->IR.whole_word & MERRY_CORE_R15;
  register mqword_t r1 = core->REGISTER_FILE[op1];
  core->REGISTER_FILE[op1] |= core->REGISTER_FILE[op2] & 0x000000000000FFFF;
  core->REGISTER_FILE[op2] |= r1 & 0x000000000000FFFF;
  return mtrue;
}

merry_core_ihdlr(excg32) {
  register mbyte_t op1 = (core->IR.whole_word >> 4) & MERRY_CORE_R15;
  register mbyte_t op2 = core->IR.whole_word & MERRY_CORE_R15;
  register mqword_t r1 = core->REGISTER_FILE[op1];
  core->REGISTER_FILE[op1] |= core->REGISTER_FILE[op2] & 0x00000000FFFFFFFF;
  core->REGISTER_FILE[op2] |= r1 & 0x00000000FFFFFFFF;
  return mtrue;
}

merry_core_ihdlr(jmp_off) {
  register mqword_t off = core->IR.half_words.w1;
  sign_extend32(off);
  core->PC += off;
  return mtrue;
}

merry_core_ihdlr(jmp_addr) {
  core->PC = core->IR.whole_word & 0xFFFFFFFFFFFF;
  return mtrue;
}

merry_core_ihdlr(jmp_reg) {
  core->PC = core->REGISTER_FILE[core->IR.whole_word & MERRY_CORE_R15];
  return mtrue;
}

#ifdef _MERRY_HOST_CPU_x86_64_ARCH_
merry_core_ihdlr(jnz) {
  if (core->flags.flags.zero == 0) {
    register mqword_t off = core->IR.half_words.w1;
    sign_extend32(off);
    core->PC += off;
    return mtrue;
  }
  return mtrue;
}

merry_core_ihdlr(jz) {
  if (core->flags.flags.zero == 1) {
    register mqword_t off = core->IR.half_words.w1;
    sign_extend32(off);
    core->PC += off;
    return mtrue;
  }
  return mtrue;
}

merry_core_ihdlr(jne) {
  if (core->flags.flags.zero == 0) {
    register mqword_t off = core->IR.half_words.w1;
    sign_extend32(off);
    core->PC += off;
    return mtrue;
  }
  return mtrue;
}

merry_core_ihdlr(je) {
  if (core->flags.flags.zero == 1) {
    register mqword_t off = core->IR.half_words.w1;
    sign_extend32(off);
    core->PC += off;
    return mtrue;
  }
  return mtrue;
}

merry_core_ihdlr(jnc) {
  if (core->flags.flags.carry == 0) {
    register mqword_t off = core->IR.half_words.w1;
    sign_extend32(off);
    core->PC += off;
    return mtrue;
  }
  return mtrue;
}

merry_core_ihdlr(jc) {
  if (core->flags.flags.carry == 1) {
    register mqword_t off = core->IR.half_words.w1;
    sign_extend32(off);
    core->PC += off;
    return mtrue;
  }
  return mtrue;
}

merry_core_ihdlr(jno) {
  if (core->flags.flags.overflow == 0) {
    register mqword_t off = core->IR.half_words.w1;
    sign_extend32(off);
    core->PC += off;
    return mtrue;
  }
  return mtrue;
}

merry_core_ihdlr(jo) {
  if (core->flags.flags.overflow == 1) {
    register mqword_t off = core->IR.half_words.w1;
    sign_extend32(off);
    core->PC += off;
    return mtrue;
  }
  return mtrue;
}

merry_core_ihdlr(jnn) {
  if (core->flags.flags.negative == 0) {
    register mqword_t off = core->IR.half_words.w1;
    sign_extend32(off);
    core->PC += off;
    return mtrue;
  }
  return mtrue;
}

merry_core_ihdlr(jn) {
  if (core->flags.flags.negative == 1) {
    register mqword_t off = core->IR.half_words.w1;
    sign_extend32(off);
    core->PC += off;
    return mtrue;
  }
  return mtrue;
}

merry_core_ihdlr(jng) {
  if (core->flags.flags.negative == 1) {
    register mqword_t off = core->IR.half_words.w1;
    sign_extend32(off);
    core->PC += off;
    return mtrue;
  }
  return mtrue;
}
merry_core_ihdlr(jg) {
  if (core->flags.flags.negative == 0) {
    register mqword_t off = core->IR.half_words.w1;
    sign_extend32(off);
    core->PC += off;
    return mtrue;
  }
  return mtrue;
}

merry_core_ihdlr(jns) {
  if (core->flags.flags.negative == 0) {
    register mqword_t off = core->IR.half_words.w1;
    sign_extend32(off);
    core->PC += off;
    return mtrue;
  }
  return mtrue;
}

merry_core_ihdlr(js) {
  if (core->flags.flags.negative == 1) {
    register mqword_t off = core->IR.half_words.w1;
    sign_extend32(off);
    core->PC += off;
    return mtrue;
  }
  return mtrue;
}

merry_core_ihdlr(jge) {
  if (core->flags.flags.negative == 0 || core->flags.flags.zero == 0) {
    register mqword_t off = core->IR.half_words.w1;
    sign_extend32(off);
    core->PC += off;
    return mtrue;
  }
  return mtrue;
}

merry_core_ihdlr(jse) {
  if (core->flags.flags.negative == 1 || core->flags.flags.zero == 0) {
    register mqword_t off = core->IR.half_words.w1;
    sign_extend32(off);
    core->PC += off;
    return mtrue;
  }
  return mtrue;
}
#endif

merry_core_ihdlr(jfz) {
  if (core->fflags.zf == 1) {
    register mqword_t off = core->IR.half_words.w1;
    sign_extend32(off);
    core->PC += off;
    return mtrue;
  }
  return mtrue;
}

merry_core_ihdlr(jfnz) {
  if (core->fflags.zf == 0) {
    register mqword_t off = core->IR.half_words.w1;
    sign_extend32(off);
    core->PC += off;
    return mtrue;
  }
  return mtrue;
}

merry_core_ihdlr(jfn) {
  if (core->fflags.sf == 1) {
    register mqword_t off = core->IR.half_words.w1;
    sign_extend32(off);
    core->PC += off;
    return mtrue;
  }
  return mtrue;
}

merry_core_ihdlr(jfnn) {
  if (core->fflags.sf == 0) {
    register mqword_t off = core->IR.half_words.w1;
    sign_extend32(off);
    core->PC += off;
    return mtrue;
  }
  return mtrue;
}

merry_core_ihdlr(jfuf) {
  if (core->fflags.uof == 1) {
    register mqword_t off = core->IR.half_words.w1;
    sign_extend32(off);
    core->PC += off;
    return mtrue;
  }
  return mtrue;
}

merry_core_ihdlr(jfnuf) {
  if (core->fflags.uof == 0) {
    register mqword_t off = core->IR.half_words.w1;
    sign_extend32(off);
    core->PC += off;
    return mtrue;
  }
  return mtrue;
}

merry_core_ihdlr(jfo) {
  if (core->fflags.of == 1) {
    register mqword_t off = core->IR.half_words.w1;
    sign_extend32(off);
    core->PC += off;
    return mtrue;
  }
  return mtrue;
}

merry_core_ihdlr(jfno) {
  if (core->fflags.of == 0) {
    register mqword_t off = core->IR.half_words.w1;
    sign_extend32(off);
    core->PC += off;
    return mtrue;
  }
  return mtrue;
}

merry_core_ihdlr(jfu) {
  if (core->fflags.uf == 1) {
    register mqword_t off = core->IR.half_words.w1;
    sign_extend32(off);
    core->PC += off;
    return mtrue;
  }
  return mtrue;
}

merry_core_ihdlr(jfnu) {
  if (core->fflags.uf == 0) {
    register mqword_t off = core->IR.half_words.w1;
    sign_extend32(off);
    core->PC += off;
    return mtrue;
  }
  return mtrue;
}

merry_core_ihdlr(jfi) {
  if (core->fflags.inv == 1) {
    register mqword_t off = core->IR.half_words.w1;
    sign_extend32(off);
    core->PC += off;
    return mtrue;
  }
  return mtrue;
}

merry_core_ihdlr(jfni) {
  if (core->fflags.inv == 0) {
    register mqword_t off = core->IR.half_words.w1;
    sign_extend32(off);
    core->PC += off;
    return mtrue;
  }
  return mtrue;
}

merry_core_ihdlr(call_reg) {
  register mqword_t addr =
      core->REGISTER_FILE[core->IR.bytes.b7 & MERRY_CORE_R15];
  MerryCoreStackFrame frame;
  frame.FRAME_BP = core->SP;
  frame.JMP_TO = addr;
  frame.RET_ADDR = core->PC;
  if (merry_is_stack_full(core->stack_frames)) {
    MERR("Call depth reached", NULL);
    return mfalse;
  }
  if (merry_CoreProcFrame_stack_push(core->stack_frames, &frame) !=
      MRES_SUCCESS) {
    MERR("Failed to register stack frame:PROC=%zu", addr);
    return mfalse;
  }
  core->BP = core->SP;
  core->PC = addr;
  return mtrue;
}

merry_core_ihdlr(call) {
  mqword_t addr = core->IR.whole_word & 0xFFFFFFFFFFFF;
  MerryCoreStackFrame frame;
  frame.FRAME_BP = core->SP;
  frame.JMP_TO = addr;
  frame.RET_ADDR = core->PC;
  if (merry_is_stack_full(core->stack_frames)) {
    MERR("Call depth reached", NULL);
    return mfalse;
  }
  if (merry_CoreProcFrame_stack_push(core->stack_frames, &frame) !=
      MRES_SUCCESS) {
    MERR("Failed to register stack frame:PROC=%zu", addr);
    return mfalse;
  }
  core->BP = core->SP;
  core->PC = addr;
  return mtrue;
}

merry_core_ihdlr(ret) {
  if (merry_is_stack_empty(core->stack_frames)) {
    MERR("Invalid RETURN: PC=%zu", core->PC);
    return mfalse;
  }

  MerryCoreStackFrame frame;
  if ((merry_CoreProcFrame_stack_pop(core->stack_frames, &frame)) !=
      MRES_SUCCESS) {
    MERR("Failed to restore stack frame: PC=%zu", core->PC);
    return mfalse;
  }
  core->SP = core->BP;
  core->PC = frame.RET_ADDR;
  core->BP = frame.FRAME_BP;
  return mtrue;
}

#ifdef _MERRY_HOST_CPU_x86_64_ARCH_
merry_core_ihdlr(retnz) {
  if (core->flags.flags.zero == 0)
    return merry_core_iret(core);
  return mtrue;
}

merry_core_ihdlr(retz) {
  if (core->flags.flags.zero == 1)
    return merry_core_iret(core);
  return mtrue;
}

merry_core_ihdlr(retne) {
  if (core->flags.flags.zero == 0)
    return merry_core_iret(core);
  return mtrue;
}

merry_core_ihdlr(rete) {
  if (core->flags.flags.zero == 1)
    return merry_core_iret(core);
  return mtrue;
}

merry_core_ihdlr(retnc) {
  if (core->flags.flags.carry == 0)
    return merry_core_iret(core);
  return mtrue;
}

merry_core_ihdlr(retc) {
  if (core->flags.flags.carry == 1)
    return merry_core_iret(core);
  return mtrue;
}

merry_core_ihdlr(retno) {
  if (core->flags.flags.overflow == 0)
    return merry_core_iret(core);
  return mtrue;
}

merry_core_ihdlr(reto) {
  if (core->flags.flags.overflow == 1)
    return merry_core_iret(core);
  return mtrue;
}

merry_core_ihdlr(retnn) {
  if (core->flags.flags.negative == 0)
    return merry_core_iret(core);
  return mtrue;
}

merry_core_ihdlr(retn) {
  if (core->flags.flags.negative == 1)
    return merry_core_iret(core);
  return mtrue;
}

merry_core_ihdlr(retng) {
  if (core->flags.flags.negative == 1)
    return merry_core_iret(core);
  return mtrue;
}

merry_core_ihdlr(retg) {
  if (core->flags.flags.negative == 0)
    return merry_core_iret(core);
  return mtrue;
}

merry_core_ihdlr(retns) {
  if (core->flags.flags.negative == 0)
    return merry_core_iret(core);
  return mtrue;
}

merry_core_ihdlr(rets) {
  if (core->flags.flags.negative == 1)
    return merry_core_iret(core);
  return mtrue;
}

merry_core_ihdlr(retge) {
  if (core->flags.flags.negative == 0 || core->flags.flags.zero == 0)
    return merry_core_iret(core);
  return mtrue;
}

merry_core_ihdlr(retse) {
  if (core->flags.flags.negative == 1 || core->flags.flags.zero == 0)
    return merry_core_iret(core);
  return mtrue;
}
#endif

merry_core_ihdlr(retfz) {
  if (core->fflags.zf == 1)
    return merry_core_iret(core);
  return mtrue;
}

merry_core_ihdlr(retfnz) {
  if (core->fflags.zf == 0)
    return merry_core_iret(core);
  return mtrue;
}

merry_core_ihdlr(retfn) {
  if (core->fflags.sf == 1)
    return merry_core_iret(core);
  return mtrue;
}

merry_core_ihdlr(retfnn) {
  if (core->fflags.sf == 0)
    return merry_core_iret(core);
  return mtrue;
}

merry_core_ihdlr(retfuf) {
  if (core->fflags.uof == 1)
    return merry_core_iret(core);
  return mtrue;
}

merry_core_ihdlr(retfnuf) {
  if (core->fflags.uof == 0)
    return merry_core_iret(core);
  return mtrue;
}

merry_core_ihdlr(retfo) {
  if (core->fflags.of == 1)
    return merry_core_iret(core);
  return mtrue;
}

merry_core_ihdlr(retfno) {
  if (core->fflags.of == 0)
    return merry_core_iret(core);
  return mtrue;
}

merry_core_ihdlr(retfu) {
  if (core->fflags.uf == 1)
    return merry_core_iret(core);
  return mtrue;
}

merry_core_ihdlr(retfnu) {
  if (core->fflags.uf == 0)
    return merry_core_iret(core);
  return mtrue;
}

merry_core_ihdlr(retfi) {
  if (core->fflags.inv == 1)
    return merry_core_iret(core);
  return mtrue;
}

merry_core_ihdlr(retfni) {
  if (core->fflags.inv == 0)
    return merry_core_iret(core);
  return mtrue;
}

merry_core_ihdlr(loop) {
  if (core->REGISTER_FILE[MERRY_CORE_R2] != 0)
    return merry_core_ijmp_off(core);
  return mtrue;
}

merry_core_ihdlr(push_imm64) {
  MerryHostMemLayout imm;
  if (core->SP >= (_MERRY_CORE_STACK_LEN_ / 8)) {
    MERR("Stack Overflow", NULL);
    return mfalse;
  }
  core->PC += 8;
  if (merry_core_memory_read_qword(core->iram, core->PC, &imm.whole_word) !=
      MRES_SUCCESS) {
    MERR("INCOMPLETE INSTRUCTION at PC=%zu", core->PC);
    return mfalse;
  }
  core->stack[core->SP] = imm.whole_word;
  core->SP++;
  return mtrue;
}

merry_core_ihdlr(push_reg) {
  mqword_t imm = core->REGISTER_FILE[core->IR.whole_word & MERRY_CORE_R15];
  if (core->SP >= (_MERRY_CORE_STACK_LEN_ / 8)) {
    MERR("Stack Overflow", NULL);
    return mfalse;
  }
  core->stack[core->SP] = imm;
  core->SP++;
  return mtrue;
}

merry_core_ihdlr(pop64) {
  if (core->SP == 0) {
    MERR("Stack Underflow", NULL);
    return mfalse;
  }
  core->SP--;
  core->REGISTER_FILE[core->IR.whole_word & MERRY_CORE_R15] =
      core->stack[core->SP];
  return mtrue;
}

merry_core_ihdlr(pusha) {
  if (core->SP >= (_MERRY_CORE_STACK_LEN_ / 8 - MERRY_CORE_REG_COUNT)) {
    MERR("Stack Overflow", NULL);
    return mfalse;
  }
  for (msize_t i = 0; i < MERRY_CORE_REG_COUNT; i++) {
    core->stack[core->SP] = core->REGISTER_FILE[i];
    core->SP++;
  }
  return mtrue;
}

merry_core_ihdlr(popa) {
  if (core->SP < MERRY_CORE_REG_COUNT) {
    MERR("Stack Underflow", NULL);
    return mfalse;
  }
  for (msize_t i = MERRY_CORE_R15; i >= 0; i--) {
    core->SP--;
    core->REGISTER_FILE[i] = core->stack[core->SP];
  }
  return mtrue;
}

merry_core_ihdlr(loadsq) {
  register mqword_t off = core->IR.whole_word & 0xFFFFFFFF;
  sign_extend32(off);
  register mqword_t res = core->BP + off;
  if (res >= core->SP) {
    MERR("Invalid STACK Access: PC=%zu", core->PC);
    return mfalse;
  }
  core->REGISTER_FILE[(core->IR.whole_word >> 32) & MERRY_CORE_R15] =
      core->stack[res];
}

merry_core_ihdlr(storesq) {
  register mqword_t off = core->IR.whole_word & 0xFFFFFFFF;
  sign_extend32(off);
  register mqword_t res = core->BP + off;
  if (res >= core->SP) {
    MERR("Invalid STACK Access: PC=%zu", core->PC);
    return mfalse;
  }
  core->stack[res] =
      core->REGISTER_FILE[(core->IR.whole_word >> 32) & MERRY_CORE_R15];
}

merry_core_ihdlr(loadsq_reg) {
  register mqword_t off =
      core->REGISTER_FILE[core->IR.whole_word & MERRY_CORE_R15];
  sign_extend32(off);
  register mqword_t res = core->BP + off;
  if (res >= core->SP) {
    MERR("Invalid STACK Access: PC=%zu", core->PC);
    return mfalse;
  }
  core->REGISTER_FILE[(core->IR.whole_word >> 4) & MERRY_CORE_R15] =
      core->stack[res];
}

merry_core_ihdlr(storesq_reg) {
  register mqword_t off =
      core->REGISTER_FILE[core->IR.whole_word & MERRY_CORE_R15];
  sign_extend32(off);
  register mqword_t res = core->BP + off;
  if (res >= core->SP) {
    MERR("Invalid STACK Access: PC=%zu", core->PC);
    return mfalse;
  }
  core->stack[res] =
      core->REGISTER_FILE[(core->IR.whole_word >> 4) & MERRY_CORE_R15];
}

merry_core_ihdlr(and_imm) {
  mqword_t imm;
  core->PC += 8;
  if (merry_core_memory_read_qword(core->iram, core->PC, &imm) !=
      MRES_SUCCESS) {
    MERR("INCOMPLETE INSTRUCTION at PC=%zu", core->PC);
    return mfalse;
  }
  core->REGISTER_FILE[core->IR.whole_word & MERRY_CORE_R15] &= imm;
  core->flags.regr = merry_obtain_flags_regr();
  return mtrue;
}

merry_core_ihdlr(and_reg) {
  core->REGISTER_FILE[(core->IR.whole_word >> 4) & MERRY_CORE_R15] &=
      core->REGISTER_FILE[core->IR.whole_word & MERRY_CORE_R15];
  core->flags.regr = merry_obtain_flags_regr();
  return mtrue;
}

merry_core_ihdlr(or_imm) {
  mqword_t imm;
  core->PC += 8;
  if (merry_core_memory_read_qword(core->iram, core->PC, &imm) !=
      MRES_SUCCESS) {
    MERR("INCOMPLETE INSTRUCTION at PC=%zu", core->PC);
    return mfalse;
  }
  core->REGISTER_FILE[core->IR.whole_word & MERRY_CORE_R15] |= imm;
  core->flags.regr = merry_obtain_flags_regr();
  return mtrue;
}

merry_core_ihdlr(or_reg) {
  core->REGISTER_FILE[(core->IR.whole_word >> 4) & MERRY_CORE_R15] |=
      core->REGISTER_FILE[core->IR.whole_word & MERRY_CORE_R15];
  core->flags.regr = merry_obtain_flags_regr();
  return mtrue;
}

merry_core_ihdlr(xor_imm) {
  mqword_t imm;
  core->PC += 8;
  if (merry_core_memory_read_qword(core->iram, core->PC, &imm) !=
      MRES_SUCCESS) {
    MERR("INCOMPLETE INSTRUCTION at PC=%zu", core->PC);
    return mfalse;
  }
  core->REGISTER_FILE[core->IR.whole_word & MERRY_CORE_R15] ^= imm;
  core->flags.regr = merry_obtain_flags_regr();
  return mtrue;
}

merry_core_ihdlr(xor_reg) {
  core->REGISTER_FILE[(core->IR.whole_word >> 4) & MERRY_CORE_R15] ^=
      core->REGISTER_FILE[core->IR.whole_word & MERRY_CORE_R15];
  core->flags.regr = merry_obtain_flags_regr();
  return mtrue;
}

merry_core_ihdlr(not) {
  mqword_t ind = core->IR.whole_word & MERRY_CORE_R15;
  core->REGISTER_FILE[ind] = ~core->REGISTER_FILE[ind];
  core->flags.regr = merry_obtain_flags_regr();
  return mtrue;
}

merry_core_ihdlr(lshift) {
  core->REGISTER_FILE[core->IR.bytes.b1 & MERRY_CORE_R15] <<=
      core->IR.bytes.b7 & 63;
  return mtrue;
}

merry_core_ihdlr(rshift) {
  core->REGISTER_FILE[core->IR.bytes.b1 & MERRY_CORE_R15] >>=
      core->IR.bytes.b7 & 63;
  return mtrue;
}

merry_core_ihdlr(lshift_reg) {
  core->REGISTER_FILE[core->IR.bytes.b6 & MERRY_CORE_R15] <<=
      core->REGISTER_FILE[core->IR.bytes.b7 & MERRY_CORE_R15] & 63;
  return mtrue;
}

merry_core_ihdlr(rshift_reg) {
  core->REGISTER_FILE[core->IR.bytes.b6 & MERRY_CORE_R15] >>=
      core->REGISTER_FILE[core->IR.bytes.b7 & MERRY_CORE_R15] & 63;
  return mtrue;
}

merry_core_ihdlr(cmp_imm) {
  mqword_t imm;
  core->PC += 8;
  if (merry_core_memory_read_qword(core->iram, core->PC, &imm) !=
      MRES_SUCCESS) {
    MERR("INSTRUCTION INCOMPLETE at PC=%zu", core->PC);
    return mfalse;
  }
  merry_compare_two_values(
      imm, core->REGISTER_FILE[core->IR.whole_word & MERRY_CORE_R15]);
  core->flags.regr = merry_obtain_flags_regr();
  return mtrue;
}

merry_core_ihdlr(cmp_reg) {
  merry_compare_two_values(
      core->REGISTER_FILE[(core->IR.whole_word >> 4) & MERRY_CORE_R15],
      core->REGISTER_FILE[core->IR.whole_word & MERRY_CORE_R15]);
  core->flags.regr = merry_obtain_flags_regr();
  return mtrue;
}

merry_core_ihdlr(fcmp) {
  merry_core_compare_f64(
      core->REGISTER_FILE[core->IR.bytes.b6 & MERRY_CORE_R15],
      core->REGISTER_FILE[core->IR.bytes.b7 & MERRY_CORE_R15], &core->fflags);
  return mtrue;
}

merry_core_ihdlr(fcmp32) {
  merry_core_compare_f32(
      core->REGISTER_FILE[core->IR.bytes.b6 & MERRY_CORE_R15],
      core->REGISTER_FILE[core->IR.bytes.b7 & MERRY_CORE_R15], &core->fflags);
  return mtrue;
}

merry_core_ihdlr(bin) {
  core->REGISTER_FILE[core->IR.whole_word & MERRY_CORE_R15] = getchar();
  return mtrue;
}

merry_core_ihdlr(bout) {
  putchar((int)core->REGISTER_FILE[core->IR.whole_word & MERRY_CORE_R15]);
  return mtrue;
}

merry_core_ihdlr(loadb) {
  register mbyte_t op1 = (core->IR.whole_word >> 48) & MERRY_CORE_R15;
  mbyte_t imm;
  if (merry_core_memory_read_byte(core->dram,
                                  core->IR.whole_word & 0xFFFFFFFFFFFF,
                                  &imm) != MRES_SUCCESS) {
    MERR("INVALID OPERAND at PC=%zu", core->PC);
    return mfalse;
  }
  core->REGISTER_FILE[op1] = imm;
  return mtrue;
}

merry_core_ihdlr(storeb) {
  register mbyte_t op1 = (core->IR.whole_word >> 48) & MERRY_CORE_R15;
  if (merry_core_memory_write_byte(
          core->dram, core->IR.whole_word & 0xFFFFFFFFFFFF,
          core->REGISTER_FILE[op1] & 0xFF) != MRES_SUCCESS) {
    MERR("INVALID OPERAND at PC=%zu", core->PC);
    return mfalse;
  }
  return mtrue;
}

merry_core_ihdlr(loadw) {
  register mbyte_t op1 = (core->IR.whole_word >> 48) & MERRY_CORE_R15;
  mword_t imm;
  if (merry_core_memory_read_word(core->dram,
                                  core->IR.whole_word & 0xFFFFFFFFFFFF,
                                  &imm) != MRES_SUCCESS) {
    MERR("INVALID OPERAND at PC=%zu", core->PC);
    return mfalse;
  }
  core->REGISTER_FILE[op1] = imm;
  return mtrue;
}

merry_core_ihdlr(storew) {
  register mbyte_t op1 = (core->IR.whole_word >> 48) & MERRY_CORE_R15;
  if (merry_core_memory_write_word(
          core->dram, core->IR.whole_word & 0xFFFFFFFFFFFF,
          core->REGISTER_FILE[op1] & 0xFFFF) != MRES_SUCCESS) {
    MERR("INVALID OPERAND at PC=%zu", core->PC);
    return mfalse;
  }
  return mtrue;
}

merry_core_ihdlr(loadd) {
  register mbyte_t op1 = (core->IR.whole_word >> 48) & MERRY_CORE_R15;
  mdword_t imm;
  if (merry_core_memory_read_dword(core->dram,
                                   core->IR.whole_word & 0xFFFFFFFFFFFF,
                                   &imm) != MRES_SUCCESS) {
    MERR("INVALID OPERAND at PC=%zu", core->PC);
    return mfalse;
  }
  core->REGISTER_FILE[op1] = imm;
  return mtrue;
}

merry_core_ihdlr(stored) {
  register mbyte_t op1 = (core->IR.whole_word >> 48) & MERRY_CORE_R15;
  if (merry_core_memory_write_dword(
          core->dram, core->IR.whole_word & 0xFFFFFFFFFFFF,
          core->REGISTER_FILE[op1] & 0xFFFFFFFF) != MRES_SUCCESS) {
    MERR("INVALID OPERAND at PC=%zu", core->PC);
    return mfalse;
  }
  return mtrue;
}

merry_core_ihdlr(loadq) {
  register mbyte_t op1 = (core->IR.whole_word >> 48) & MERRY_CORE_R15;
  mqword_t imm;
  if (merry_core_memory_read_qword(core->dram,
                                   core->IR.whole_word & 0xFFFFFFFFFFFF,
                                   &imm) != MRES_SUCCESS) {
    MERR("INVALID OPERAND at PC=%zu", core->PC);
    return mfalse;
  }
  core->REGISTER_FILE[op1] = imm;
  return mtrue;
}

merry_core_ihdlr(storeq) {
  register mbyte_t op1 = (core->IR.whole_word >> 48) & MERRY_CORE_R15;
  if (merry_core_memory_write_qword(core->dram,
                                    core->IR.whole_word & 0xFFFFFFFFFFFF,
                                    core->REGISTER_FILE[op1]) != MRES_SUCCESS) {
    MERR("INVALID OPERAND at PC=%zu", core->PC);
    return mfalse;
  }
  return mtrue;
}

merry_core_ihdlr(loadb_reg) {
  register mbyte_t op1 = (core->IR.whole_word >> 4) & MERRY_CORE_R15;
  register mbyte_t op2 = core->IR.whole_word & MERRY_CORE_R15;
  mbyte_t imm;
  if (merry_core_memory_read_byte(core->dram, core->REGISTER_FILE[op2], &imm) !=
      MRES_SUCCESS) {
    MERR("INVALID OPERAND at PC=%zu", core->PC);
    return mfalse;
  }
  core->REGISTER_FILE[op1] = imm;
  return mtrue;
}

merry_core_ihdlr(storeb_reg) {
  register mbyte_t op1 = (core->IR.whole_word >> 4) & MERRY_CORE_R15;
  register mbyte_t op2 = core->IR.whole_word & MERRY_CORE_R15;
  if (merry_core_memory_write_byte(core->dram, core->REGISTER_FILE[op2],
                                   core->REGISTER_FILE[op1] & 0xFF) !=
      MRES_SUCCESS) {
    MERR("INVALID OPERAND at PC=%zu", core->PC);
    return mfalse;
  }
  return mtrue;
}

merry_core_ihdlr(loadw_reg) {
  register mbyte_t op1 = (core->IR.whole_word >> 4) & MERRY_CORE_R15;
  register mbyte_t op2 = core->IR.whole_word & MERRY_CORE_R15;
  mword_t imm;
  if (merry_core_memory_read_word(core->dram, core->REGISTER_FILE[op2], &imm) !=
      MRES_SUCCESS) {
    MERR("INVALID OPERAND at PC=%zu", core->PC);
    return mfalse;
  }
  core->REGISTER_FILE[op1] = imm;
  return mtrue;
}

merry_core_ihdlr(storew_reg) {
  register mbyte_t op1 = (core->IR.whole_word >> 4) & MERRY_CORE_R15;
  register mbyte_t op2 = core->IR.whole_word & MERRY_CORE_R15;
  if (merry_core_memory_write_word(core->dram, core->REGISTER_FILE[op2],
                                   core->REGISTER_FILE[op1] & 0xFFFF) !=
      MRES_SUCCESS) {
    MERR("INVALID OPERAND at PC=%zu", core->PC);
    return mfalse;
  }
  return mtrue;
}

merry_core_ihdlr(loadd_reg) {
  register mbyte_t op1 = (core->IR.whole_word >> 4) & MERRY_CORE_R15;
  register mbyte_t op2 = core->IR.whole_word & MERRY_CORE_R15;
  mdword_t imm;
  if (merry_core_memory_read_dword(core->dram, core->REGISTER_FILE[op2],
                                   &imm) != MRES_SUCCESS) {
    MERR("INVALID OPERAND at PC=%zu", core->PC);
    return mfalse;
  }
  core->REGISTER_FILE[op1] = imm;
  return mtrue;
}

merry_core_ihdlr(stored_reg) {
  register mbyte_t op1 = (core->IR.whole_word >> 4) & MERRY_CORE_R15;
  register mbyte_t op2 = core->IR.whole_word & MERRY_CORE_R15;
  if (merry_core_memory_write_dword(core->dram, core->REGISTER_FILE[op2],
                                    core->REGISTER_FILE[op1] & 0xFFFFFFFF) !=
      MRES_SUCCESS) {
    MERR("INVALID OPERAND at PC=%zu", core->PC);
    return mfalse;
  }
  return mtrue;
}

merry_core_ihdlr(loadq_reg) {
  register mbyte_t op1 = (core->IR.whole_word >> 4) & MERRY_CORE_R15;
  register mbyte_t op2 = core->IR.whole_word & MERRY_CORE_R15;
  mqword_t imm;
  if (merry_core_memory_read_qword(core->dram, core->REGISTER_FILE[op2],
                                   &imm) != MRES_SUCCESS) {
    MERR("INVALID OPERAND at PC=%zu", core->PC);
    return mfalse;
  }
  core->REGISTER_FILE[op1] = imm;
  return mtrue;
}

merry_core_ihdlr(storeq_reg) {
  register mbyte_t op1 = (core->IR.whole_word >> 4) & MERRY_CORE_R15;
  register mbyte_t op2 = core->IR.whole_word & MERRY_CORE_R15;
  if (merry_core_memory_write_qword(core->dram, core->REGISTER_FILE[op2],
                                    core->REGISTER_FILE[op1]) != MRES_SUCCESS) {
    MERR("INVALID OPERAND at PC=%zu", core->PC);
    return mfalse;
  }
  return mtrue;
}

merry_core_ihdlr(atm_loadb) {
  register mbyte_t op1 = (core->IR.whole_word >> 48) & MERRY_CORE_R15;
  mbyte_t imm;
  if (merry_core_memory_read_byte_atm(core->dram,
                                      core->IR.whole_word & 0xFFFFFFFFFFFF,
                                      &imm) != MRES_SUCCESS) {
    MERR("INVALID OPERAND at PC=%zu", core->PC);
    return mfalse;
  }
  core->REGISTER_FILE[op1] = imm;
  return mtrue;
}

merry_core_ihdlr(atm_storeb) {
  register mbyte_t op1 = (core->IR.whole_word >> 48) & MERRY_CORE_R15;
  if (merry_core_memory_write_byte_atm(
          core->dram, core->IR.whole_word & 0xFFFFFFFFFFFF,
          core->REGISTER_FILE[op1] & 0xFF) != MRES_SUCCESS) {
    MERR("INVALID OPERAND at PC=%zu", core->PC);
    return mfalse;
  }
  return mtrue;
}

merry_core_ihdlr(atm_loadw) {
  register mbyte_t op1 = (core->IR.whole_word >> 48) & MERRY_CORE_R15;
  mword_t imm;
  if (merry_core_memory_read_word_atm(core->dram,
                                      core->IR.whole_word & 0xFFFFFFFFFFFF,
                                      &imm) != MRES_SUCCESS) {
    MERR("INVALID OPERAND at PC=%zu", core->PC);
    return mfalse;
  }
  core->REGISTER_FILE[op1] = imm;
  return mtrue;
}

merry_core_ihdlr(atm_storew) {
  register mbyte_t op1 = (core->IR.whole_word >> 48) & MERRY_CORE_R15;
  if (merry_core_memory_write_word_atm(
          core->dram, core->IR.whole_word & 0xFFFFFFFFFFFF,
          core->REGISTER_FILE[op1] & 0xFFFF) != MRES_SUCCESS) {
    MERR("INVALID OPERAND at PC=%zu", core->PC);
    return mfalse;
  }
  return mtrue;
}

merry_core_ihdlr(atm_loadd) {
  register mbyte_t op1 = (core->IR.whole_word >> 48) & MERRY_CORE_R15;
  mdword_t imm;
  if (merry_core_memory_read_dword_atm(core->dram,
                                       core->IR.whole_word & 0xFFFFFFFFFFFF,
                                       &imm) != MRES_SUCCESS) {
    MERR("INVALID OPERAND at PC=%zu", core->PC);
    return mfalse;
  }
  core->REGISTER_FILE[op1] = imm;
  return mtrue;
}

merry_core_ihdlr(atm_stored) {
  register mbyte_t op1 = (core->IR.whole_word >> 48) & MERRY_CORE_R15;
  if (merry_core_memory_write_dword_atm(
          core->dram, core->IR.whole_word & 0xFFFFFFFFFFFF,
          core->REGISTER_FILE[op1] & 0xFFFFFFFF) != MRES_SUCCESS) {
    MERR("INVALID OPERAND at PC=%zu", core->PC);
    return mfalse;
  }
  return mtrue;
}

merry_core_ihdlr(atm_loadq) {
  register mbyte_t op1 = (core->IR.whole_word >> 48) & MERRY_CORE_R15;
  mqword_t imm;
  if (merry_core_memory_read_qword_atm(core->dram,
                                       core->IR.whole_word & 0xFFFFFFFFFFFF,
                                       &imm) != MRES_SUCCESS) {
    MERR("INVALID OPERAND at PC=%zu", core->PC);
    return mfalse;
  }
  core->REGISTER_FILE[op1] = imm;
  return mtrue;
}

merry_core_ihdlr(atm_storeq) {
  register mbyte_t op1 = (core->IR.whole_word >> 48) & MERRY_CORE_R15;
  if (merry_core_memory_write_qword_atm(
          core->dram, core->IR.whole_word & 0xFFFFFFFFFFFF,
          core->REGISTER_FILE[op1]) != MRES_SUCCESS) {
    MERR("INVALID OPERAND at PC=%zu", core->PC);
    return mfalse;
  }
  return mtrue;
}

merry_core_ihdlr(atm_loadb_reg) {
  register mbyte_t op1 = (core->IR.whole_word >> 4) & MERRY_CORE_R15;
  register mbyte_t op2 = core->IR.whole_word & MERRY_CORE_R15;
  mbyte_t imm;
  if (merry_core_memory_read_byte_atm(core->dram, core->REGISTER_FILE[op2],
                                      &imm) != MRES_SUCCESS) {
    MERR("INVALID OPERAND at PC=%zu", core->PC);
    return mfalse;
  }
  core->REGISTER_FILE[op1] = imm;
  return mtrue;
}

merry_core_ihdlr(atm_storeb_reg) {
  register mbyte_t op1 = (core->IR.whole_word >> 4) & MERRY_CORE_R15;
  register mbyte_t op2 = core->IR.whole_word & MERRY_CORE_R15;
  if (merry_core_memory_write_byte_atm(core->dram, core->REGISTER_FILE[op2],
                                       core->REGISTER_FILE[op1] & 0xFF) !=
      MRES_SUCCESS) {
    MERR("INVALID OPERAND at PC=%zu", core->PC);
    return mfalse;
  }
  return mtrue;
}

merry_core_ihdlr(atm_loadw_reg) {
  register mbyte_t op1 = (core->IR.whole_word >> 4) & MERRY_CORE_R15;
  register mbyte_t op2 = core->IR.whole_word & MERRY_CORE_R15;
  mword_t imm;
  if (merry_core_memory_read_word_atm(core->dram, core->REGISTER_FILE[op2],
                                      &imm) != MRES_SUCCESS) {
    MERR("INVALID OPERAND at PC=%zu", core->PC);
    return mfalse;
  }
  core->REGISTER_FILE[op1] = imm;
  return mtrue;
}

merry_core_ihdlr(atm_storew_reg) {
  register mbyte_t op1 = (core->IR.whole_word >> 4) & MERRY_CORE_R15;
  register mbyte_t op2 = core->IR.whole_word & MERRY_CORE_R15;
  if (merry_core_memory_write_word_atm(core->dram, core->REGISTER_FILE[op2],
                                       core->REGISTER_FILE[op1] & 0xFFFF) !=
      MRES_SUCCESS) {
    MERR("INVALID OPERAND at PC=%zu", core->PC);
    return mfalse;
  }
  return mtrue;
}

merry_core_ihdlr(atm_loadd_reg) {
  register mbyte_t op1 = (core->IR.whole_word >> 4) & MERRY_CORE_R15;
  register mbyte_t op2 = core->IR.whole_word & MERRY_CORE_R15;
  mdword_t imm;
  if (merry_core_memory_read_dword_atm(core->dram, core->REGISTER_FILE[op2],
                                       &imm) != MRES_SUCCESS) {
    MERR("INVALID OPERAND at PC=%zu", core->PC);
    return mfalse;
  }
  core->REGISTER_FILE[op1] = imm;
  return mtrue;
}

merry_core_ihdlr(atm_stored_reg) {
  register mbyte_t op1 = (core->IR.whole_word >> 4) & MERRY_CORE_R15;
  register mbyte_t op2 = core->IR.whole_word & MERRY_CORE_R15;
  if (merry_core_memory_write_dword_atm(core->dram, core->REGISTER_FILE[op2],
                                        core->REGISTER_FILE[op1] &
                                            0xFFFFFFFF) != MRES_SUCCESS) {
    MERR("INVALID OPERAND at PC=%zu", core->PC);
    return mfalse;
  }
  return mtrue;
}

merry_core_ihdlr(atm_loadq_reg) {
  register mbyte_t op1 = (core->IR.whole_word >> 4) & MERRY_CORE_R15;
  register mbyte_t op2 = core->IR.whole_word & MERRY_CORE_R15;
  mqword_t imm;
  if (merry_core_memory_read_qword_atm(core->dram, core->REGISTER_FILE[op2],
                                       &imm) != MRES_SUCCESS) {
    MERR("INVALID OPERAND at PC=%zu", core->PC);
    return mfalse;
  }
  core->REGISTER_FILE[op1] = imm;
  return mtrue;
}

merry_core_ihdlr(atm_storeq_reg) {
  register mbyte_t op1 = (core->IR.whole_word >> 4) & MERRY_CORE_R15;
  register mbyte_t op2 = core->IR.whole_word & MERRY_CORE_R15;
  if (merry_core_memory_write_qword_atm(core->dram, core->REGISTER_FILE[op2],
                                        core->REGISTER_FILE[op1]) !=
      MRES_SUCCESS) {
    MERR("INVALID OPERAND at PC=%zu", core->PC);
    return mfalse;
  }
  return mtrue;
}

merry_core_ihdlr(lea) {
  core->REGISTER_FILE[core->IR.bytes.b4 & MERRY_CORE_R15] =
      core->REGISTER_FILE[core->IR.bytes.b5 & MERRY_CORE_R15] +
      core->REGISTER_FILE[core->IR.bytes.b6 & MERRY_CORE_R15] *
          core->REGISTER_FILE[core->IR.bytes.b7 & MERRY_CORE_R15];
  return mtrue;
}

merry_core_ihdlr(cflags) {
  core->flags.flags.carry = 0;
  core->flags.flags.negative = 0;
  core->flags.flags.zero = 0;
  core->flags.flags.overflow = 0;
  return mtrue;
}

merry_core_ihdlr(cfflags) {
  core->fflags.zf = 0;
  core->fflags.sf = 0;
  core->fflags.uof = 0;
  core->fflags.of = 0;
  core->fflags.uf = 0;
  core->fflags.inv = 0;
  return mtrue;
}

merry_core_ihdlr(reset) {
  for (msize_t i = 0; i <= MERRY_CORE_R15; i++)
    core->REGISTER_FILE[i] = 0;
  return mtrue;
}

merry_core_ihdlr(cmpxchg) {
  register mbyte_t desired = (core->IR.whole_word >> 8) & 0xFF;
  register mbyte_t expected = core->IR.whole_word & 0xFF;
  mqword_t address;
  core->PC += 8;
  if (merry_core_memory_read_qword(core->iram, core->PC, &address) !=
      MRES_SUCCESS) {
    MERR("INVALID OPERAND at PC=%zu", core->PC);
    return mfalse;
  }
  mret_t ret =
      merry_core_memory_cmpxchg(core->dram, address, desired, expected);
  core->flags.regr = merry_obtain_flags_regr();
  if (ret != MRES_SUCCESS) {
    MERR("INVALID OPERAND at PC=%zu", core->PC);
    return mfalse;
  }
  return mtrue;
}

merry_core_ihdlr(cmpxchg_reg) {
  register mbyte_t desired = (core->IR.whole_word >> 8) & 0xFF;
  register mbyte_t expected = core->IR.whole_word & 0xFF;
  mqword_t address =
      core->REGISTER_FILE[(core->IR.whole_word >> 16) & MERRY_CORE_R15];
  mret_t ret =
      merry_core_memory_cmpxchg(core->dram, address, desired, expected);
  core->flags.regr = merry_obtain_flags_regr();
  if (ret != MRES_SUCCESS) {
    MERR("INVALID OPERAND at PC=%zu", core->PC);
    return mfalse;
  }
  return mtrue;
}

merry_core_ihdlr(invalid_inst) {
  MWARN("UNKNOWN INSTRUCTION at PC=%zu", core->PC);
  return mtrue;
}
