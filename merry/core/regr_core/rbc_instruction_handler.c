#include <float.h>
#include <math.h>
#include <regr_core/internal/rbc_instruction_handler.h>

void rbc_compare_f32(float a, float b, RBCFFlagsRegr *regr) {
  regr->zf = (a == b && a == 0.0f);
  regr->sf = ((a - b) < 0.0f);
  regr->uof = isnan(a) || isnan(b);
  regr->inv = isnan(a - b) || (a == 0.0f && signbit(a) != signbit(b));

  float diff = a - b;
  regr->of = diff > FLT_MAX;
  regr->uf = fabsf(diff) < FLT_MIN && diff != 0.0f;
}

void rbc_compare_f64(double a, double b, RBCFFlagsRegr *regr) {
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
 * since it is more likely to be used less.
 * Users are free to give any importance to any register
 * The result will also be passed in R15
 * */
rbc_ihdlr(sysint) {
  merry_check_ptr(base);
  switch (base->REGISTER_FILE[RBC_R15]) {
  case RBC_SI_EXIT: // doesn't require any result
    *kcore = mtrue;
    break;
  default:
    base->REGISTER_FILE[RBC_R15] = RBC_SIR_UNKNOWN_TYPE;
    break;
  }
}

/*
 * This will use R15 for input and result the same as sysint
 * R15 = status of the request
 * R14 = the actual error if any
 * */
rbc_ihdlr(mint) {
  merry_check_ptr(base);
  MerryRequestArgs *args = &base->args;
  switch (base->REGISTER_FILE[RBC_R15]) {
  case KILL_SELF:
    break;
  case CREATE_CORE: {
    /*
     * R10 = new core type
     * R11 = start address for new core
     * R12(byte 0) = same group as this group?
     * R12(byte 1) = new group?(no conflicts of vals)
     * R13 = if no new group and no same group then
     *       provide the group id here
     * RESULTS:
     * R10 = new core's id(if created)
     * R11 = new core's uid(if created)
     * R12 = new core's gid(if created)
     * */
    args->create_core.new_core_type = base->REGISTER_FILE[RBC_R10];
    args->create_core.st_addr = base->REGISTER_FILE[RBC_R11];
    args->create_core.same_group = base->REGISTER_FILE[RBC_R12] >> 56;
    args->create_core.new_group = (base->REGISTER_FILE[RBC_R12] >> 48) & 255;
    args->create_core.gid = base->REGISTER_FILE[RBC_R13];
    break;
  }
  case CREATE_GROUP: {
    /*
     * Results:
     * R10 = new gid
     * */
    break;
  }
  case GET_GROUP_DETAILS: {
    /*
     * Results:
     * R10 = group's id
     * R11 = number of cores in the group
     * R12 = number of active cores in the group
     * */
    break;
  }
  case GET_SYSTEM_DETAILS: {
    /*
     * Results:
     * R10 = number of groups
     * R11 = number of active cores overall
     * R12 = number of cores overall
     * */
    break;
  }
  default:
    base->REGISTER_FILE[RBC_R15] = (mqword_t)-1;
    return;
  }
  base->req.type = base->REGISTER_FILE[RBC_R15];
  merry_SEND_REQUEST(&base->req);

  // results here
  base->REGISTER_FILE[RBC_R15] = base->req.res;
  if (base->req.res != GREQUEST_SUCCESS) {
    base->REGISTER_FILE[RBC_R14] = base->req.failed.errno_if_err;
    return;
  }
  switch (base->REGISTER_FILE[RBC_R10]) {
  case CREATE_CORE: {
    base->REGISTER_FILE[RBC_R10] = args->create_core.new_id;
    base->REGISTER_FILE[RBC_R11] = args->create_core.new_uid;
    base->REGISTER_FILE[RBC_R12] = args->create_core.gid;
    break;
  }
  case CREATE_GROUP: {
    base->REGISTER_FILE[RBC_R10] = args->create_group.new_guid;
    break;
  }
  case GET_GROUP_DETAILS: {
    base->REGISTER_FILE[RBC_R10] = args->get_group_details.guid;
    base->REGISTER_FILE[RBC_R11] = args->get_group_details.core_count;
    base->REGISTER_FILE[RBC_R12] = args->get_group_details.active_core_count;
    break;
  }
  case GET_SYSTEM_DETAILS: {
    base->REGISTER_FILE[RBC_R10] = args->get_system_details.grp_count;
    base->REGISTER_FILE[RBC_R11] = args->get_system_details.active_core_count;
    base->REGISTER_FILE[RBC_R12] = args->get_system_details.core_count;
    break;
  }
  }
}

rbc_ihdlrX(add_imm) {
  register mbyte_t op1 = instruction & RBC_R15;
  mqword_t op2;
  base->PC += 8;
  if (rbc_memory_read_qword(base->iram, base->PC, &op2) !=
      RBC_MEM_OPER_SUCCESS) {
    // kill the core since the given instructions themselves
    // are incomplete, hence, being strict makes sense
    *kcore = mtrue;
    MFATAL("RBC",
           "MEM READ Failed at PC=%zu likely because the access was invalid",
           base->PC);
    return;
  }
  base->REGISTER_FILE[op1] += op2;
  base->flags.regr = merry_obtain_flags_regr();
}

rbc_ihdlrX(add_reg) {
  register mbyte_t op1 = instruction & RBC_R15;
  register mbyte_t op2 = instruction & RBC_R15;
  base->REGISTER_FILE[op1] += base->REGISTER_FILE[op2];
  base->flags.regr = merry_obtain_flags_regr();
}

rbc_ihdlrX(sub_imm) {
  register mbyte_t op1 = instruction & RBC_R15;
  mqword_t op2;
  base->PC += 8;
  if (rbc_memory_read_qword(base->dram, base->PC, &op2) !=
      RBC_MEM_OPER_SUCCESS) {
    *kcore = mtrue;
    MFATAL("RBC",
           "MEM READ Failed at PC=%zu likely because the access was invalid",
           base->PC);
    return;
  }
  base->REGISTER_FILE[op1] -= op2;
  base->flags.regr = merry_obtain_flags_regr();
}

rbc_ihdlrX(sub_reg) {
  register mbyte_t op1 = instruction & RBC_R15;
  register mbyte_t op2 = instruction & RBC_R15;
  base->REGISTER_FILE[op1] -= base->REGISTER_FILE[op2];
  base->flags.regr = merry_obtain_flags_regr();
}

rbc_ihdlrX(mul_imm) {
  register mbyte_t op1 = instruction & RBC_R15;
  mqword_t op2;
  base->PC += 8;
  if (rbc_memory_read_qword(base->iram, base->PC, &op2) !=
      RBC_MEM_OPER_SUCCESS) {
    *kcore = mtrue;
    MFATAL("RBC",
           "MEM READ Failed at PC=%zu likely because the access was invalid",
           base->PC);
    return;
  }
  base->REGISTER_FILE[op1] *= op2;
  base->flags.regr = merry_obtain_flags_regr();
}

rbc_ihdlrX(mul_reg) {
  register mbyte_t op1 = instruction & RBC_R15;
  register mbyte_t op2 = instruction & RBC_R15;
  base->REGISTER_FILE[op1] *= base->REGISTER_FILE[op2];
  base->flags.regr = merry_obtain_flags_regr();
}

rbc_ihdlrX(div_imm) {
  register mbyte_t op1 = instruction & RBC_R15;
  mqword_t op2;
  base->PC += 8;
  if (rbc_memory_read_qword(base->iram, base->PC, &op2) !=
      RBC_MEM_OPER_SUCCESS) {
    *kcore = mtrue;
    MFATAL("RBC", "Div by Zero(NO HDLR) at PC=%zu", base->PC);
    return;
  }
  if (op2 == 0) {
    *kcore = mtrue;
    MFATAL("RBC", "Div by Zero(NO HDLR) at PC=%zu", base->PC);
    return;
  }
  base->REGISTER_FILE[op1] /= op2;
  base->flags.regr = merry_obtain_flags_regr();
}

rbc_ihdlrX(div_reg) {
  register mbyte_t op1 = instruction & RBC_R15;
  register mbyte_t op2 = instruction & RBC_R15;
  if (base->REGISTER_FILE[op2] == 0) {
    *kcore = mtrue;
    MFATAL("RBC", "Div by Zero(NO HDLR) at PC=%zu", base->PC);
    return;
  }
  base->REGISTER_FILE[op1] /= base->REGISTER_FILE[op2];
  base->flags.regr = merry_obtain_flags_regr();
}

rbc_ihdlrX(mod_imm) {
  register mbyte_t op1 = instruction & RBC_R15;
  mqword_t op2;
  base->PC += 8;
  if (rbc_memory_read_qword(base->iram, base->PC, &op2) !=
      RBC_MEM_OPER_SUCCESS) {
    *kcore = mtrue;
    MFATAL("RBC",
           "MEM READ Failed at PC=%zu likely because the access was invalid",
           base->PC);
    return;
  }
  if (op2 == 0) {
    *kcore = mtrue;
    MFATAL("RBC", "Div by Zero(NO HDLR) at PC=%zu", base->PC);
    return;
  }
  base->REGISTER_FILE[op1] %= op2;
  base->flags.regr = merry_obtain_flags_regr();
}

rbc_ihdlrX(mod_reg) {
  register mbyte_t op1 = instruction & RBC_R15;
  register mbyte_t op2 = instruction & RBC_R15;
  if (base->REGISTER_FILE[op2] == 0) {
    *kcore = mtrue;
    MFATAL("RBC",
           "MEM READ Failed at PC=%zu likely because the access was invalid",
           base->PC);
    return;
  }
  if (base->REGISTER_FILE[op2] == 0) {
    *kcore = mtrue;
    MFATAL("RBC", "Div by Zero(NO HDLR) at PC=%zu", base->PC);
    return;
  }
  base->REGISTER_FILE[op1] %= base->REGISTER_FILE[op2];
  base->flags.regr = merry_obtain_flags_regr();
}

rbc_ihdlrX(fadd) {
  register mqword_t op1 = (instruction)&RBC_R15;
  register mqword_t op2 = (instruction)&RBC_R15;
  register MerryDoubleToQword a;
  register MerryDoubleToQword b;
  a.q_val = base->REGISTER_FILE[op1];
  b.q_val = base->REGISTER_FILE[op2];
  rbc_compare_f64(a.d_val, b.d_val, &base->fflags);
  a.d_val += b.d_val;
  base->REGISTER_FILE[op1] = a.q_val;
}

rbc_ihdlrX(fsub) {
  register mqword_t op1 = (instruction)&RBC_R15;
  register mqword_t op2 = (instruction)&RBC_R15;
  register MerryDoubleToQword a;
  register MerryDoubleToQword b;
  a.q_val = base->REGISTER_FILE[op1];
  b.q_val = base->REGISTER_FILE[op2];
  rbc_compare_f64(a.d_val, b.d_val, &base->fflags);
  a.d_val -= b.d_val;
  base->REGISTER_FILE[op1] = a.q_val;
}

rbc_ihdlrX(fmul) {
  register mqword_t op1 = (instruction)&RBC_R15;
  register mqword_t op2 = (instruction)&RBC_R15;
  register MerryDoubleToQword a;
  register MerryDoubleToQword b;
  a.q_val = base->REGISTER_FILE[op1];
  b.q_val = base->REGISTER_FILE[op2];
  rbc_compare_f64(a.d_val, b.d_val, &base->fflags);
  a.d_val *= b.d_val;
  base->REGISTER_FILE[op1] = a.q_val;
}

rbc_ihdlrX(fdiv) {
  register mqword_t op1 = (instruction)&RBC_R15;
  register mqword_t op2 = (instruction)&RBC_R15;
  register MerryDoubleToQword a;
  register MerryDoubleToQword b;
  a.q_val = base->REGISTER_FILE[op1];
  b.q_val = base->REGISTER_FILE[op2];
  if (b.d_val == 0.0) {
    *kcore = mtrue;
    MFATAL("RBC", "Div by Zero(NO HDLR|FP) at PC=%zu", base->PC);
    return;
  }
  rbc_compare_f64(a.d_val, b.d_val, &base->fflags);
  a.d_val /= b.d_val;
  base->REGISTER_FILE[op1] = a.q_val;
}

rbc_ihdlrX(fadd32) {
  register mqword_t op1 = (instruction)&RBC_R15;
  register mqword_t op2 = (instruction)&RBC_R15;
  register MerryFloatToDword a;
  register MerryFloatToDword b;
  a.d_val = base->REGISTER_FILE[op1];
  b.d_val = base->REGISTER_FILE[op2];
  rbc_compare_f32(a.fl_val, b.fl_val, &base->fflags);
  a.fl_val += b.fl_val;
  base->REGISTER_FILE[op1] = a.d_val;
}

rbc_ihdlrX(fsub32) {
  register mqword_t op1 = (instruction)&RBC_R15;
  register mqword_t op2 = (instruction)&RBC_R15;
  register MerryFloatToDword a;
  register MerryFloatToDword b;
  a.d_val = base->REGISTER_FILE[op1];
  b.d_val = base->REGISTER_FILE[op2];
  rbc_compare_f32(a.fl_val, b.fl_val, &base->fflags);
  a.fl_val -= b.fl_val;
  base->REGISTER_FILE[op1] = a.d_val;
}

rbc_ihdlrX(fmul32) {
  register mqword_t op1 = (instruction)&RBC_R15;
  register mqword_t op2 = (instruction)&RBC_R15;
  register MerryFloatToDword a;
  register MerryFloatToDword b;
  a.d_val = base->REGISTER_FILE[op1];
  b.d_val = base->REGISTER_FILE[op2];
  rbc_compare_f32(a.fl_val, b.fl_val, &base->fflags);
  a.fl_val *= b.fl_val;
  base->REGISTER_FILE[op1] = a.d_val;
}

rbc_ihdlrX(fdiv32) {
  register mqword_t op1 = (instruction)&RBC_R15;
  register mqword_t op2 = (instruction)&RBC_R15;
  register MerryFloatToDword a;
  register MerryFloatToDword b;
  a.d_val = base->REGISTER_FILE[op1];
  b.d_val = base->REGISTER_FILE[op2];
  if (b.d_val == 0.0) {
    *kcore = mtrue;
    MFATAL("RBC", "Div by Zero(NO HDLR|FP) at PC=%zu", base->PC);
    return;
  }
  rbc_compare_f32(a.fl_val, b.fl_val, &base->fflags);
  a.fl_val /= b.fl_val;
  base->REGISTER_FILE[op1] = a.d_val;
}

rbc_ihdlrX(add_memb) {
  register mqword_t op1 = (instruction)&RBC_R15;
  register maddress_t addr = (instruction & 0xFFFFFFFFFFFF);
  mbyte_t temp = 0;
  if (rbc_memory_read_byte(base->dram, addr, &temp) != RBC_MEM_OPER_SUCCESS) {
    *kcore = mtrue;
    MFATAL("RBC",
           "MEM READ Failed at ADDR=%zu likely because the access was invalid",
           addr);
    return;
  }
  base->REGISTER_FILE[op1] += temp;
  base->flags.regr = merry_obtain_flags_regr();
}

rbc_ihdlrX(add_memw) {
  register mqword_t op1 = (instruction)&RBC_R15;
  register maddress_t addr = (instruction & 0xFFFFFFFFFFFF);
  mword_t temp = 0;
  if (rbc_memory_read_word(base->dram, addr, &temp) != RBC_MEM_OPER_SUCCESS) {
    *kcore = mtrue;
    MFATAL("RBC",
           "MEM READ Failed at ADDR=%zu likely because the access was invalid",
           addr);
    return;
  }
  base->REGISTER_FILE[op1] += temp;
  base->flags.regr = merry_obtain_flags_regr();
}

rbc_ihdlrX(add_memd) {
  register mqword_t op1 = (instruction)&RBC_R15;
  register maddress_t addr = (instruction & 0xFFFFFFFFFFFF);
  mdword_t temp = 0;
  if (rbc_memory_read_dword(base->dram, addr, &temp) != RBC_MEM_OPER_SUCCESS) {
    *kcore = mtrue;
    MFATAL("RBC",
           "MEM READ Failed at ADDR=%zu likely because the access was invalid",
           addr);
    return;
  }
  base->REGISTER_FILE[op1] += temp;
  base->flags.regr = merry_obtain_flags_regr();
}

rbc_ihdlrX(add_memq) {
  register mqword_t op1 = (instruction)&RBC_R15;
  register maddress_t addr = (instruction & 0xFFFFFFFFFFFF);
  mqword_t temp = 0;
  if (rbc_memory_read_qword(base->dram, addr, &temp) != RBC_MEM_OPER_SUCCESS) {
    *kcore = mtrue;
    MFATAL("RBC",
           "MEM READ Failed at ADDR=%zu likely because the access was invalid",
           addr);
    return;
  }
  base->REGISTER_FILE[op1] += temp;
  base->flags.regr = merry_obtain_flags_regr();
}

rbc_ihdlrX(sub_memb) {
  register mqword_t op1 = (instruction)&RBC_R15;
  register maddress_t addr = (instruction & 0xFFFFFFFFFFFF);
  mbyte_t temp = 0;
  if (rbc_memory_read_byte(base->dram, addr, &temp) != RBC_MEM_OPER_SUCCESS) {
    *kcore = mtrue;
    MFATAL("RBC",
           "MEM READ Failed at ADDR=%zu likely because the access was invalid",
           addr);
    return;
  }
  base->REGISTER_FILE[op1] -= temp;
  base->flags.regr = merry_obtain_flags_regr();
}

rbc_ihdlrX(sub_memw) {
  register mqword_t op1 = (instruction)&RBC_R15;
  register maddress_t addr = (instruction & 0xFFFFFFFFFFFF);
  mword_t temp = 0;
  if (rbc_memory_read_word(base->dram, addr, &temp) != RBC_MEM_OPER_SUCCESS) {
    *kcore = mtrue;
    MFATAL("RBC",
           "MEM READ Failed at ADDR=%zu likely because the access was invalid",
           addr);
    return;
  }
  base->REGISTER_FILE[op1] -= temp;
  base->flags.regr = merry_obtain_flags_regr();
}

rbc_ihdlrX(sub_memd) {
  register mqword_t op1 = (instruction)&RBC_R15;
  register maddress_t addr = (instruction & 0xFFFFFFFFFFFF);
  mdword_t temp = 0;
  if (rbc_memory_read_dword(base->dram, addr, &temp) != RBC_MEM_OPER_SUCCESS) {
    *kcore = mtrue;
    MFATAL("RBC",
           "MEM READ Failed at ADDR=%zu likely because the access was invalid",
           addr);
    return;
  }
  base->REGISTER_FILE[op1] -= temp;
  base->flags.regr = merry_obtain_flags_regr();
}

rbc_ihdlrX(sub_memq) {
  register mqword_t op1 = (instruction)&RBC_R15;
  register maddress_t addr = (instruction & 0xFFFFFFFFFFFF);
  mqword_t temp = 0;
  if (rbc_memory_read_qword(base->dram, addr, &temp) != RBC_MEM_OPER_SUCCESS) {
    *kcore = mtrue;
    MFATAL("RBC",
           "MEM READ Failed at ADDR=%zu likely because the access was invalid",
           addr);
    return;
  }
  base->REGISTER_FILE[op1] -= temp;
  base->flags.regr = merry_obtain_flags_regr();
}

rbc_ihdlrX(mul_memb) {
  register mqword_t op1 = (instruction)&RBC_R15;
  register maddress_t addr = (instruction & 0xFFFFFFFFFFFF);
  mbyte_t temp = 0;
  if (rbc_memory_read_byte(base->dram, addr, &temp) != RBC_MEM_OPER_SUCCESS) {
    *kcore = mtrue;
    MFATAL("RBC",
           "MEM READ Failed at ADDR=%zu likely because the access was invalid",
           addr);
    return;
  }
  base->REGISTER_FILE[op1] *= temp;
  base->flags.regr = merry_obtain_flags_regr();
}

rbc_ihdlrX(mul_memw) {
  register mqword_t op1 = (instruction)&RBC_R15;
  register maddress_t addr = (instruction & 0xFFFFFFFFFFFF);
  mword_t temp = 0;
  if (rbc_memory_read_word(base->dram, addr, &temp) != RBC_MEM_OPER_SUCCESS) {
    *kcore = mtrue;
    MFATAL("RBC",
           "MEM READ Failed at ADDR=%zu likely because the access was invalid",
           addr);
    return;
  }
  base->REGISTER_FILE[op1] *= temp;
  base->flags.regr = merry_obtain_flags_regr();
}

rbc_ihdlrX(mul_memd) {
  register mqword_t op1 = (instruction)&RBC_R15;
  register maddress_t addr = (instruction & 0xFFFFFFFFFFFF);
  mdword_t temp = 0;
  if (rbc_memory_read_dword(base->dram, addr, &temp) != RBC_MEM_OPER_SUCCESS) {
    *kcore = mtrue;
    MFATAL("RBC",
           "MEM READ Failed at ADDR=%zu likely because the access was invalid",
           addr);
    return;
  }
  base->REGISTER_FILE[op1] *= temp;
  base->flags.regr = merry_obtain_flags_regr();
}

rbc_ihdlrX(mul_memq) {
  register mqword_t op1 = (instruction)&RBC_R15;
  register maddress_t addr = (instruction & 0xFFFFFFFFFFFF);
  mqword_t temp = 0;
  if (rbc_memory_read_qword(base->dram, addr, &temp) != RBC_MEM_OPER_SUCCESS) {
    *kcore = mtrue;
    MFATAL("RBC",
           "MEM READ Failed at ADDR=%zu likely because the access was invalid",
           addr);
    return;
  }
  base->REGISTER_FILE[op1] *= temp;
  base->flags.regr = merry_obtain_flags_regr();
}

rbc_ihdlrX(div_memb) {
  register mqword_t op1 = (instruction)&RBC_R15;
  register maddress_t addr = (instruction & 0xFFFFFFFFFFFF);
  mbyte_t temp = 0;
  if (rbc_memory_read_byte(base->dram, addr, &temp) != RBC_MEM_OPER_SUCCESS) {
    *kcore = mtrue;
    MFATAL("RBC",
           "MEM READ Failed at ADDR=%zu likely because the access was invalid",
           addr);
    return;
  }
  if (temp == 0) {
    *kcore = mtrue;
    MFATAL("RBC", "Div by Zero(NO HDLR) at PC=%zu", base->PC);
    return;
  }
  base->REGISTER_FILE[op1] /= temp;
  base->flags.regr = merry_obtain_flags_regr();
}

rbc_ihdlrX(div_memw) {
  register mqword_t op1 = (instruction)&RBC_R15;
  register maddress_t addr = (instruction & 0xFFFFFFFFFFFF);
  mword_t temp = 0;
  if (rbc_memory_read_word(base->dram, addr, &temp) != RBC_MEM_OPER_SUCCESS) {
    *kcore = mtrue;
    MFATAL("RBC",
           "MEM READ Failed at ADDR=%zu likely because the access was invalid",
           addr);
    return;
  }
  if (temp == 0) {
    *kcore = mtrue;
    MFATAL("RBC", "Div by Zero(NO HDLR) at PC=%zu", base->PC);
    return;
  }
  base->REGISTER_FILE[op1] /= temp;
  base->flags.regr = merry_obtain_flags_regr();
}

rbc_ihdlrX(div_memd) {
  register mqword_t op1 = (instruction)&RBC_R15;
  register maddress_t addr = (instruction & 0xFFFFFFFFFFFF);
  mdword_t temp = 0;
  if (rbc_memory_read_dword(base->dram, addr, &temp) != RBC_MEM_OPER_SUCCESS) {
    *kcore = mtrue;
    MFATAL("RBC",
           "MEM READ Failed at ADDR=%zu likely because the access was invalid",
           addr);
    return;
  }
  if (temp == 0) {
    *kcore = mtrue;
    MFATAL("RBC", "Div by Zero(NO HDLR) at PC=%zu", base->PC);
    return;
  }
  base->REGISTER_FILE[op1] /= temp;
  base->flags.regr = merry_obtain_flags_regr();
}

rbc_ihdlrX(div_memq) {
  register mqword_t op1 = (instruction)&RBC_R15;
  register maddress_t addr = (instruction & 0xFFFFFFFFFFFF);
  mqword_t temp = 0;
  if (rbc_memory_read_qword(base->dram, addr, &temp) != RBC_MEM_OPER_SUCCESS) {
    *kcore = mtrue;
    MFATAL("RBC",
           "MEM READ Failed at ADDR=%zu likely because the access was invalid",
           addr);
    return;
  }
  if (temp == 0) {
    *kcore = mtrue;
    MFATAL("RBC", "Div by Zero(NO HDLR) at PC=%zu", base->PC);
    return;
  }
  base->REGISTER_FILE[op1] /= temp;
  base->flags.regr = merry_obtain_flags_regr();
}

rbc_ihdlrX(mod_memb) {
  register mqword_t op1 = (instruction)&RBC_R15;
  register maddress_t addr = (instruction & 0xFFFFFFFFFFFF);
  mbyte_t temp = 0;
  if (rbc_memory_read_byte(base->dram, addr, &temp) != RBC_MEM_OPER_SUCCESS) {
    *kcore = mtrue;
    MFATAL("RBC",
           "MEM READ Failed at ADDR=%zu likely because the access was invalid",
           addr);
    return;
  }
  if (temp == 0) {
    *kcore = mtrue;
    MFATAL("RBC", "Div by Zero(NO HDLR) at PC=%zu", base->PC);
    return;
  }
  base->REGISTER_FILE[op1] %= temp;
  base->flags.regr = merry_obtain_flags_regr();
}

rbc_ihdlrX(mod_memw) {
  register mqword_t op1 = (instruction)&RBC_R15;
  register maddress_t addr = (instruction & 0xFFFFFFFFFFFF);
  mword_t temp = 0;
  if (rbc_memory_read_word(base->dram, addr, &temp) != RBC_MEM_OPER_SUCCESS) {
    *kcore = mtrue;
    MFATAL("RBC",
           "MEM READ Failed at ADDR=%zu likely because the access was invalid",
           addr);
    return;
  }
  if (temp == 0) {
    *kcore = mtrue;
    MFATAL("RBC", "Div by Zero(NO HDLR) at PC=%zu", base->PC);
    return;
  }
  base->REGISTER_FILE[op1] %= temp;
  base->flags.regr = merry_obtain_flags_regr();
}

rbc_ihdlrX(mod_memd) {
  register mqword_t op1 = (instruction)&RBC_R15;
  register maddress_t addr = (instruction & 0xFFFFFFFFFFFF);
  mdword_t temp = 0;
  if (rbc_memory_read_dword(base->dram, addr, &temp) != RBC_MEM_OPER_SUCCESS) {
    *kcore = mtrue;
    MFATAL("RBC",
           "MEM READ Failed at ADDR=%zu likely because the access was invalid",
           addr);
    return;
  }
  if (temp == 0) {
    *kcore = mtrue;
    MFATAL("RBC", "Div by Zero(NO HDLR) at PC=%zu", base->PC);
    return;
  }
  base->REGISTER_FILE[op1] %= temp;
  base->flags.regr = merry_obtain_flags_regr();
}

rbc_ihdlrX(mod_memq) {
  register mqword_t op1 = (instruction)&RBC_R15;
  register maddress_t addr = (instruction & 0xFFFFFFFFFFFF);
  mqword_t temp = 0;
  if (rbc_memory_read_qword(base->dram, addr, &temp) != RBC_MEM_OPER_SUCCESS) {
    *kcore = mtrue;
    MFATAL("RBC",
           "MEM READ Failed at ADDR=%zu likely because the access was invalid",
           addr);
    return;
  }
  if (temp == 0) {
    *kcore = mtrue;
    MFATAL("RBC", "Div by Zero(NO HDLR) at PC=%zu", base->PC);
    return;
  }
  base->REGISTER_FILE[op1] %= temp;
  base->flags.regr = merry_obtain_flags_regr();
}

rbc_ihdlrX(fadd64_mem) {
  register mqword_t op1 = (instruction)&RBC_R15;
  register mqword_t addr = (instruction & 0xFFFFFFFFFFFF);
  MerryDoubleToQword temp;
  if (rbc_memory_read_qword(base->dram, addr, &temp.q_val) !=
      RBC_MEM_OPER_SUCCESS) {
    *kcore = mtrue;
    MFATAL("RBC",
           "MEM READ Failed at ADDR=%zu likely because the access was invalid",
           addr);
    return;
  }
  MerryDoubleToQword rval;
  rval.q_val = base->REGISTER_FILE[op1];

  rbc_compare_f64(rval.d_val, temp.d_val, &base->fflags);
  temp.d_val += rval.d_val;
  base->REGISTER_FILE[op1] = temp.q_val;
}

rbc_ihdlrX(fsub64_mem) {
  register mqword_t op1 = (instruction)&RBC_R15;
  register mqword_t addr = (instruction & 0xFFFFFFFFFFFF);
  MerryDoubleToQword temp;
  if (rbc_memory_read_qword(base->dram, addr, &temp.q_val) !=
      RBC_MEM_OPER_SUCCESS) {
    *kcore = mtrue;
    MFATAL("RBC",
           "MEM READ Failed at ADDR=%zu likely because the access was invalid",
           addr);
    return;
  }
  MerryDoubleToQword rval;
  rval.q_val = base->REGISTER_FILE[op1];

  rbc_compare_f64(rval.d_val, temp.d_val, &base->fflags);
  temp.d_val -= rval.d_val;
  base->REGISTER_FILE[op1] = temp.q_val;
}

rbc_ihdlrX(fmul64_mem) {
  register mqword_t op1 = (instruction)&RBC_R15;
  register mqword_t addr = (instruction & 0xFFFFFFFFFFFF);
  MerryDoubleToQword temp;
  if (rbc_memory_read_qword(base->dram, addr, &temp.q_val) !=
      RBC_MEM_OPER_SUCCESS) {
    *kcore = mtrue;
    MFATAL("RBC",
           "MEM READ Failed at ADDR=%zu likely because the access was invalid",
           addr);
    return;
  }
  MerryDoubleToQword rval;
  rval.q_val = base->REGISTER_FILE[op1];

  rbc_compare_f64(rval.d_val, temp.d_val, &base->fflags);
  temp.d_val *= rval.d_val;
  base->REGISTER_FILE[op1] = temp.q_val;
}

rbc_ihdlrX(fdiv64_mem) {
  register mqword_t op1 = (instruction)&RBC_R15;
  register mqword_t addr = (instruction & 0xFFFFFFFFFFFF);
  MerryDoubleToQword temp;
  if (rbc_memory_read_qword(base->dram, addr, &temp.q_val) !=
      RBC_MEM_OPER_SUCCESS) {
    *kcore = mtrue;
    MFATAL("RBC",
           "MEM READ Failed at ADDR=%zu likely because the access was invalid",
           addr);
    return;
  }
  if (temp.d_val == 0.0) {
    *kcore = mtrue;
    MFATAL("RBC", "Div by Zero(NO HDLR) at PC=%zu", base->PC);
    return;
  }
  MerryDoubleToQword rval;
  rval.q_val = base->REGISTER_FILE[op1];

  rbc_compare_f64(rval.d_val, temp.d_val, &base->fflags);
  temp.d_val /= rval.d_val;
  base->REGISTER_FILE[op1] = temp.q_val;
}

rbc_ihdlrX(fadd32_mem) {
  register mqword_t op1 = (instruction)&RBC_R15;
  register mqword_t addr = (instruction & 0xFFFFFFFFFFFF);
  MerryFloatToDword temp;
  if (rbc_memory_read_dword(base->dram, addr, &temp.d_val) !=
      RBC_MEM_OPER_SUCCESS) {
    *kcore = mtrue;
    MFATAL("RBC",
           "MEM READ Failed at ADDR=%zu likely because the access was invalid",
           addr);
    return;
  }
  MerryFloatToDword rval;
  rval.d_val = base->REGISTER_FILE[op1];

  rbc_compare_f32(rval.fl_val, temp.fl_val, &base->fflags);
  temp.fl_val += rval.fl_val;
  base->REGISTER_FILE[op1] = temp.d_val;
  base->REGISTER_FILE[op1] = temp.d_val;
}

rbc_ihdlrX(fsub32_mem) {
  register mqword_t op1 = (instruction)&RBC_R15;
  register mqword_t addr = (instruction & 0xFFFFFFFFFFFF);
  MerryFloatToDword temp;
  if (rbc_memory_read_dword(base->dram, addr, &temp.d_val) !=
      RBC_MEM_OPER_SUCCESS) {
    *kcore = mtrue;
    MFATAL("RBC",
           "MEM READ Failed at ADDR=%zu likely because the access was invalid",
           addr);
    return;
  }
  MerryFloatToDword rval;
  rval.d_val = base->REGISTER_FILE[op1];

  rbc_compare_f32(rval.fl_val, temp.fl_val, &base->fflags);
  temp.fl_val -= rval.fl_val;
  base->REGISTER_FILE[op1] = temp.d_val;
  base->REGISTER_FILE[op1] = temp.d_val;
}

rbc_ihdlrX(fmul32_mem) {
  register mqword_t op1 = (instruction)&RBC_R15;
  register mqword_t addr = (instruction & 0xFFFFFFFFFFFF);
  MerryFloatToDword temp;
  if (rbc_memory_read_dword(base->dram, addr, &temp.d_val) !=
      RBC_MEM_OPER_SUCCESS) {
    *kcore = mtrue;
    MFATAL("RBC",
           "MEM READ Failed at ADDR=%zu likely because the access was invalid",
           addr);
    return;
  }
  MerryFloatToDword rval;
  rval.d_val = base->REGISTER_FILE[op1];

  rbc_compare_f32(rval.fl_val, temp.fl_val, &base->fflags);
  temp.fl_val *= rval.fl_val;
  base->REGISTER_FILE[op1] = temp.d_val;
  base->REGISTER_FILE[op1] = temp.d_val;
}

rbc_ihdlrX(fdiv32_mem) {
  register mqword_t op1 = (instruction)&RBC_R15;
  register mqword_t addr = (instruction & 0xFFFFFFFFFFFF);
  MerryFloatToDword temp;
  if (rbc_memory_read_dword(base->dram, addr, &temp.d_val) !=
      RBC_MEM_OPER_SUCCESS) {
    *kcore = mtrue;
    MFATAL("RBC",
           "MEM READ Failed at ADDR=%zu likely because the access was invalid",
           addr);
    return;
  }
  if (temp.d_val == 0.0) {
    *kcore = mtrue;
    MFATAL("RBC", "Div by Zero(NO HDLR) at PC=%zu", base->PC);
    return;
  }
  MerryFloatToDword rval;
  rval.d_val = base->REGISTER_FILE[op1];

  rbc_compare_f32(rval.fl_val, temp.fl_val, &base->fflags);
  temp.fl_val /= rval.fl_val;
  base->REGISTER_FILE[op1] = temp.d_val;
  base->REGISTER_FILE[op1] = temp.d_val;
}

rbc_ihdlrX(mov_imm) {
  register mbyte_t op1 = instruction & RBC_R15;
  mqword_t val;

  base->PC += 8;
  if (rbc_memory_read_qword(base->iram, base->PC, &val) !=
      RBC_MEM_OPER_SUCCESS) {
    *kcore = mtrue;
    MFATAL("RBC",
           "MEM READ Failed at PC=%zu likely because the access was invalid",
           base->PC);
    return;
  }

  base->REGISTER_FILE[op1] = val;
}

rbc_ihdlrX(movf32) {
  register mbyte_t op1 = instruction & RBC_R15;
  mqword_t val;

  base->PC += 8;
  if (rbc_memory_read_qword(base->iram, base->PC, &val) !=
      RBC_MEM_OPER_SUCCESS) {
    *kcore = mtrue;
    MFATAL("RBC",
           "MEM READ Failed at PC=%zu likely because the access was invalid",
           base->PC);
    return;
  }

  base->REGISTER_FILE[op1] = val & 0xFFFFFFFF;
}

rbc_ihdlrX(movesx_imm8) {
  register mbyte_t op1 = instruction & RBC_R15;
  register mqword_t op2 = instruction;
  sign_extend8(op2);
  base->REGISTER_FILE[op1] = op2;
}

rbc_ihdlrX(movesx_imm16) {
  register mbyte_t op1 = instruction & RBC_R15;
  register mqword_t op2 = instruction.half_half_words.w3;
  sign_extend16(op2);
  base->REGISTER_FILE[op1] = op2;
}

rbc_ihdlrX(movesx_imm32) {
  register mbyte_t op1 = instruction & RBC_R15;
  register mqword_t op2 = instruction.half_words.w1;
  sign_extend32(op2);
  base->REGISTER_FILE[op1] = op2;
}

rbc_ihdlrX(movesx_reg8) {
  register mbyte_t op1 = instruction & RBC_R15;
  register mbyte_t op2 = instruction & RBC_R15;
  sign_extend8(base->REGISTER_FILE[op2]);
  base->REGISTER_FILE[op1] = op2;
}

rbc_ihdlrX(movesx_reg16) {
  register mbyte_t op1 = instruction & RBC_R15;
  register mbyte_t op2 = instruction & RBC_R15;
  sign_extend16(base->REGISTER_FILE[op2]);
  base->REGISTER_FILE[op1] = op2;
}

rbc_ihdlrX(movesx_reg32) {
  register mbyte_t op1 = instruction & RBC_R15;
  register mbyte_t op2 = instruction & RBC_R15;
  sign_extend32(base->REGISTER_FILE[op2]);
  base->REGISTER_FILE[op1] = op2;
}

rbc_ihdlrX(excg) {
  register mbyte_t op1 = instruction & RBC_R15;
  register mbyte_t op2 = instruction & RBC_R15;
  register mqword_t temp = base->REGISTER_FILE[op1];
  base->REGISTER_FILE[op1] = base->REGISTER_FILE[op2];
  base->REGISTER_FILE[op2] = temp;
}

rbc_ihdlrX(excg8) {
  register mbyte_t op1 = instruction & RBC_R15;
  register mbyte_t op2 = instruction & RBC_R15;
  register mqword_t r1 = base->REGISTER_FILE[op1];
  base->REGISTER_FILE[op1] |= base->REGISTER_FILE[op2] & 0x00000000000000FF;
  base->REGISTER_FILE[op2] |= r1 & 0x00000000000000FF;
}

rbc_ihdlrX(excg16) {
  register mbyte_t op1 = instruction & RBC_R15;
  register mbyte_t op2 = instruction & RBC_R15;
  register mqword_t r1 = base->REGISTER_FILE[op1];
  base->REGISTER_FILE[op1] |= base->REGISTER_FILE[op2] & 0x000000000000FFFF;
  base->REGISTER_FILE[op2] |= r1 & 0x000000000000FFFF;
}

rbc_ihdlrX(excg32) {
  register mbyte_t op1 = instruction & RBC_R15;
  register mbyte_t op2 = instruction & RBC_R15;
  register mqword_t r1 = base->REGISTER_FILE[op1];
  base->REGISTER_FILE[op1] |= base->REGISTER_FILE[op2] & 0x00000000FFFFFFFF;
  base->REGISTER_FILE[op2] |= r1 & 0x00000000FFFFFFFF;
}

rbc_ihdlrX(call) {
  mqword_t addr = instruction & 0xFFFFFFFFFFFF;
  if (merry_stack_push(core->tbs, (mptr_t)base->PC) != RBC_MEM_OPER_SUCCESS) {

    return;
  }
  if (merry_stack_push(core->cstack, (mptr_t)addr) != RBC_MEM_OPER_SUCCESS) {

    return;
  }
  base->PC =
      addr - 8; // the address to the first instructionruction of the procedure
}

rbc_ihdlrX(ret) {
  MerryPtrToQword a;

  // restore PC
  if ((a.ptr = merry_stack_pop(core->tbs)) == RET_NULL) {

    merry_assign_state(core->base->state, _MERRY_PROGRAM_ERROR_,
                       _INVALID_PROCEDURE_RETURN_);
    merry_provide_context(core->base->state, _MERRY_CORE_EXECUTING_);
    core->req->type = PROBLEM_ENCOUNTERED;
    merry_SEND_REQUEST(core->req);
    return;
  }

  // if the above failed then this will also fail and the failure
  // of above won't reach this
  if ((merry_stack_pop(core->cstack)) == RET_NULL) {

    return;
  }

  base->PC = a.qword;
}

rbc_ihdlrX(call_reg) {
  MerryPtrToQword addr;
  addr.qword = base->REGISTER_FILE[instruction & RBC_R15];
  if (merry_stack_push(core->tbs, (mqptr_t)base->PC) != RBC_MEM_OPER_SUCCESS) {

    return;
  }
  if (merry_stack_push(core->cstack, addr.ptr) != RBC_MEM_OPER_SUCCESS) {

    return;
  }
  base->PC = addr.qword - 8;
}

rbc_ihdlrX(push_immb) {
  MerryHostMemLayout imm;
  base->PC += 8;
  if (rbc_memory_read_qword(base->iram, base->PC, &imm.whole_word) ==
      RET_FAILURE) {

    return;
  }
  if (merry_RAM_write_byte(core->stack, base->REGISTER_FILE[SP],
                           imm.bytes.b7, ) != RBC_MEM_OPER_SUCCESS) {
    merry_assign_state(core->base->state, _MERRY_INTERNAL_SYSTEM_ERROR_,
                       _MERRY_STACK_OVERFLOW_);
    merry_provide_context(core->base->state, _MERRY_CORE_EXECUTING_);
    core->req->type = PROBLEM_ENCOUNTERED;
    merry_SEND_REQUEST(core->req);

    return;
  }
  base->REGISTER_FILE[SP]++;
}

rbc_ihdlrX(push_immw) {
  MerryHostMemLayout imm;
  base->PC += 8;
  if (rbc_memory_read_qword(base->iram, base->PC, &imm.whole_word) ==
      RET_FAILURE) {

    return;
  }
  if (merry_RAM_write_word(core->stack, base->REGISTER_FILE[SP],
                           imm.half_half_words.w3, ) != RBC_MEM_OPER_SUCCESS) {
    merry_assign_state(core->base->state, _MERRY_INTERNAL_SYSTEM_ERROR_,
                       _MERRY_STACK_OVERFLOW_);
    merry_provide_context(core->base->state, _MERRY_CORE_EXECUTING_);
    core->req->type = PROBLEM_ENCOUNTERED;
    merry_SEND_REQUEST(core->req);

    return;
  }
  base->REGISTER_FILE[SP] += 2;
}

rbc_ihdlrX(push_immd) {
  MerryHostMemLayout imm;
  base->PC += 8;
  if (rbc_memory_read_qword(base->iram, base->PC, &imm.whole_word) ==
      RET_FAILURE) {

    return;
  }
  if (merry_RAM_write_dword(core->stack, base->REGISTER_FILE[SP],
                            imm.half_words.w1, ) != RBC_MEM_OPER_SUCCESS) {
    merry_assign_state(core->base->state, _MERRY_INTERNAL_SYSTEM_ERROR_,
                       _MERRY_STACK_OVERFLOW_);
    merry_provide_context(core->base->state, _MERRY_CORE_EXECUTING_);
    core->req->type = PROBLEM_ENCOUNTERED;
    merry_SEND_REQUEST(core->req);

    return;
  }
  base->REGISTER_FILE[SP] += 4;
}

rbc_ihdlrX(push_immq) {
  MerryHostMemLayout imm;
  base->PC += 8;
  if (rbc_memory_read_qword(base->iram, base->PC, &imm.whole_word) ==
      RET_FAILURE) {

    return;
  }
  if (merry_RAM_write_qword(core->stack, base->REGISTER_FILE[SP],
                            imm.whole_word, ) != RBC_MEM_OPER_SUCCESS) {
    merry_assign_state(core->base->state, _MERRY_INTERNAL_SYSTEM_ERROR_,
                       _MERRY_STACK_OVERFLOW_);
    merry_provide_context(core->base->state, _MERRY_CORE_EXECUTING_);
    core->req->type = PROBLEM_ENCOUNTERED;
    merry_SEND_REQUEST(core->req);

    return;
  }
  base->REGISTER_FILE[SP] += 8;
}

rbc_ihdlrX(push_reg) {
  if (merry_RAM_write_qword(core->stack, base->REGISTER_FILE[SP],
                            base->REGISTER_FILE[instruction & RBC_R15], ) ==
      RET_FAILURE) {
    merry_assign_state(core->base->state, _MERRY_INTERNAL_SYSTEM_ERROR_,
                       _MERRY_STACK_OVERFLOW_);
    merry_provide_context(core->base->state, _MERRY_CORE_EXECUTING_);
    core->req->type = PROBLEM_ENCOUNTERED;
    merry_SEND_REQUEST(core->req);

    return;
  }
  base->REGISTER_FILE[SP] += 8;
}

rbc_ihdlrX(popb) {
  if (base->REGISTER_FILE[SP] == 0) {
    merry_assign_state(core->base->state, _MERRY_INTERNAL_SYSTEM_ERROR_,
                       _MERRY_STACK_UNDERFLOW_);
  }
  mbyte_t tmp = 0;
  if (merry_RAM_read_byte(core->stack, base->REGISTER_FILE[SP], &tmp, ) ==
      RET_FAILURE) {
    // This will not fail at all
    // i.e this section should never be executed

    return;
  }
  base->REGISTER_FILE[instruction & RBC_R15] = tmp;
  base->REGISTER_FILE[SP] -= 1;
}

rbc_ihdlrX(popw) {
  if (base->REGISTER_FILE[SP] == 0) {
    merry_assign_state(core->base->state, _MERRY_INTERNAL_SYSTEM_ERROR_,
                       _MERRY_STACK_UNDERFLOW_);
  }
  mword_t tmp = 0;
  if (merry_RAM_read_word(core->stack, base->REGISTER_FILE[SP], &tmp, ) ==
      RET_FAILURE) {
    // This will not fail at all
    // i.e this section should never be executed

    return;
  }
  base->REGISTER_FILE[instruction & RBC_R15] = tmp;
  base->REGISTER_FILE[SP] -= 2;
}

rbc_ihdlrX(popd) {
  if (base->REGISTER_FILE[SP] == 0) {
    merry_assign_state(core->base->state, _MERRY_INTERNAL_SYSTEM_ERROR_,
                       _MERRY_STACK_UNDERFLOW_);
  }
  mdword_t tmp = 0;
  if (merry_RAM_read_dword(core->stack, base->REGISTER_FILE[SP], &tmp, ) ==
      RET_FAILURE) {
    // This will not fail at all
    // i.e this section should never be executed

    return;
  }
  base->REGISTER_FILE[instruction & RBC_R15] = tmp;
  base->REGISTER_FILE[SP] -= 4;
}

rbc_ihdlrX(popq) {
  if (base->REGISTER_FILE[SP] == 0) {
    merry_assign_state(core->base->state, _MERRY_INTERNAL_SYSTEM_ERROR_,
                       _MERRY_STACK_UNDERFLOW_);
  }
  if (merry_RAM_read_qword(core->stack, base->REGISTER_FILE[SP],
                           &base->REGISTER_FILE[instruction & RBC_R15], ) ==
      RET_FAILURE) {
    // This will not fail at all
    // i.e this section should never be executed

    return;
  }
  base->REGISTER_FILE[SP] -= 8;
}

rbc_ihdlrX(pusha) {
  for (msize_t i = 0; i < REG_COUNT_GPC_64; i++) {
    if (merry_RAM_write_qword(core->stack, base->REGISTER_FILE[SP],
                              base->REGISTER_FILE[i], ) !=
        RBC_MEM_OPER_SUCCESS) {
      merry_assign_state(core->base->state, _MERRY_INTERNAL_SYSTEM_ERROR_,
                         _MERRY_STACK_OVERFLOW_);
      merry_provide_context(core->base->state, _MERRY_CORE_EXECUTING_);
      core->req->type = PROBLEM_ENCOUNTERED;
      merry_SEND_REQUEST(core->req);

      return;
    }
    base->REGISTER_FILE[SP] += 8;
  }
}

rbc_ihdlrX(popa) {
  if (base->REGISTER_FILE[SP] < (REG_COUNT_GPC_64 * 8)) {
    merry_assign_state(core->base->state, _MERRY_INTERNAL_SYSTEM_ERROR_,
                       _MERRY_STACK_UNDERFLOW_);
  }
  for (msize_t i = REG_COUNT_GPC_64 - 1; i >= 0; i++) {
    if (merry_RAM_read_qword(core->stack, base->REGISTER_FILE[SP],
                             &base->REGISTER_FILE[i], ) !=
        RBC_MEM_OPER_SUCCESS) {
      merry_assign_state(core->base->state, _MERRY_INTERNAL_SYSTEM_ERROR_,
                         _MERRY_STACK_OVERFLOW_);
      merry_provide_context(core->base->state, _MERRY_CORE_EXECUTING_);
      core->req->type = PROBLEM_ENCOUNTERED;
      merry_SEND_REQUEST(core->req);

      return;
    }
    base->REGISTER_FILE[SP] -= 8;
  }
}

rbc_ihdlrX(push_memb) {
  MerryHostMemLayout imm;
  if (rbc_memory_read_byte(base->iram, instruction & 0xFFFFFFFFFFFF,
                           &imm.bytes.b7) != RBC_MEM_OPER_SUCCESS) {

    *kcore = mtrue;
    MFATAL("RBC",
           "MEM READ Failed at ADDR=%zu likely because the access was invalid",
           addr);
    return;
    return;
  }
  if (merry_RAM_write_byte(core->stack, base->REGISTER_FILE[SP],
                           imm.bytes.b7, ) != RBC_MEM_OPER_SUCCESS) {
    merry_assign_state(core->base->state, _MERRY_INTERNAL_SYSTEM_ERROR_,
                       _MERRY_STACK_OVERFLOW_);
    merry_provide_context(core->base->state, _MERRY_CORE_EXECUTING_);
    core->req->type = PROBLEM_ENCOUNTERED;
    merry_SEND_REQUEST(core->req);

    return;
  }
  base->REGISTER_FILE[SP] += 1;
}

rbc_ihdlrX(push_memw) {
  MerryHostMemLayout imm;
  if (rbc_memory_read_word(base->iram, instruction & 0xFFFFFFFFFFFF,
                           &imm.half_half_words.w3) != RBC_MEM_OPER_SUCCESS) {

    *kcore = mtrue;
    MFATAL("RBC",
           "MEM READ Failed at ADDR=%zu likely because the access was invalid",
           addr);
    return;
    return;
  }
  if (merry_RAM_write_word(core->stack, base->REGISTER_FILE[SP],
                           imm.half_half_words.w3, ) != RBC_MEM_OPER_SUCCESS) {
    merry_assign_state(core->base->state, _MERRY_INTERNAL_SYSTEM_ERROR_,
                       _MERRY_STACK_OVERFLOW_);
    merry_provide_context(core->base->state, _MERRY_CORE_EXECUTING_);
    core->req->type = PROBLEM_ENCOUNTERED;
    merry_SEND_REQUEST(core->req);

    return;
  }
  base->REGISTER_FILE[SP] += 2;
}

rbc_ihdlrX(push_memd) {
  MerryHostMemLayout imm;
  if (rbc_memory_read_dword(base->iram, instruction & 0xFFFFFFFFFFFF,
                            &imm.half_words.w1) != RBC_MEM_OPER_SUCCESS) {

    *kcore = mtrue;
    MFATAL("RBC",
           "MEM READ Failed at ADDR=%zu likely because the access was invalid",
           addr);
    return;
    return;
  }
  if (merry_RAM_write_dword(core->stack, base->REGISTER_FILE[SP],
                            imm.half_words.w1, ) != RBC_MEM_OPER_SUCCESS) {
    merry_assign_state(core->base->state, _MERRY_INTERNAL_SYSTEM_ERROR_,
                       _MERRY_STACK_OVERFLOW_);
    merry_provide_context(core->base->state, _MERRY_CORE_EXECUTING_);
    core->req->type = PROBLEM_ENCOUNTERED;
    merry_SEND_REQUEST(core->req);

    return;
  }
  base->REGISTER_FILE[SP] += 4;
}

rbc_ihdlrX(push_memq) {
  MerryHostMemLayout imm;
  if (rbc_memory_read_qword(base->iram, instruction & 0xFFFFFFFFFFFF,
                            &imm.whole_word) != RBC_MEM_OPER_SUCCESS) {

    *kcore = mtrue;
    MFATAL("RBC",
           "MEM READ Failed at ADDR=%zu likely because the access was invalid",
           addr);
    return;
    return;
  }
  if (merry_RAM_write_qword(core->stack, base->REGISTER_FILE[SP],
                            imm.whole_word, ) != RBC_MEM_OPER_SUCCESS) {
    merry_assign_state(core->base->state, _MERRY_INTERNAL_SYSTEM_ERROR_,
                       _MERRY_STACK_OVERFLOW_);
    merry_provide_context(core->base->state, _MERRY_CORE_EXECUTING_);
    core->req->type = PROBLEM_ENCOUNTERED;
    merry_SEND_REQUEST(core->req);

    return;
  }
  base->REGISTER_FILE[SP] += 8;
}

rbc_ihdlrX(pop_memb) {
  if (base->REGISTER_FILE[SP] == 0) {
    merry_assign_state(core->base->state, _MERRY_INTERNAL_SYSTEM_ERROR_,
                       _MERRY_STACK_UNDERFLOW_);
  }
  mbyte_t tmp;
  if (merry_RAM_read_byte(core->stack, base->REGISTER_FILE[SP], &tmp, ) ==
      RET_FAILURE) {
    // This will not fail at all
    // i.e this section should never be executed

    return;
  }
  if (merry_core_mem_access_WRITE_DATA_byte(, base->iram,
                                            instruction & 0xFFFFFFFFFFFF,
                                            tmp) != RBC_MEM_OPER_SUCCESS) {

    *kcore = mtrue;
    MFATAL("RBC",
           "MEM WRITE Failed at ADDR=%zu likely because the access was invalid",
           addr);
    return;
    return;
  }
  base->REGISTER_FILE[SP] -= 1;
}

rbc_ihdlrX(pop_memw) {
  if (base->REGISTER_FILE[SP] == 0) {
    merry_assign_state(core->base->state, _MERRY_INTERNAL_SYSTEM_ERROR_,
                       _MERRY_STACK_UNDERFLOW_);
  }
  mword_t tmp;
  if (merry_RAM_read_word(core->stack, base->REGISTER_FILE[SP], &tmp, ) ==
      RET_FAILURE) {
    // This will not fail at all
    // i.e this section should never be executed

    return;
  }
  if (merry_core_mem_access_WRITE_DATA_word(, base->iram,
                                            instruction & 0xFFFFFFFFFFFF,
                                            tmp) != RBC_MEM_OPER_SUCCESS) {

    *kcore = mtrue;
    MFATAL("RBC",
           "MEM WRITE Failed at ADDR=%zu likely because the access was invalid",
           addr);
    return;
  }
  base->REGISTER_FILE[SP] -= 2;
}

rbc_ihdlrX(pop_memd) {
  if (base->REGISTER_FILE[SP] == 0) {
    merry_assign_state(core->base->state, _MERRY_INTERNAL_SYSTEM_ERROR_,
                       _MERRY_STACK_UNDERFLOW_);
  }
  mdword_t tmp;
  if (merry_RAM_read_dword(core->stack, base->REGISTER_FILE[SP], &tmp, ) ==
      RET_FAILURE) {
    // This will not fail at all
    // i.e this section should never be executed

    return;
  }
  if (merry_core_mem_access_WRITE_DATA_dword(, base->iram,
                                             instruction & 0xFFFFFFFFFFFF,
                                             tmp) != RBC_MEM_OPER_SUCCESS) {

    *kcore = mtrue;
    MFATAL("RBC",
           "MEM WRITE Failed at ADDR=%zu likely because the access was invalid",
           addr);
    return;
  }
  base->REGISTER_FILE[SP] -= 4;
}

rbc_ihdlrX(pop_memq) {
  if (base->REGISTER_FILE[SP] == 0) {
    merry_assign_state(core->base->state, _MERRY_INTERNAL_SYSTEM_ERROR_,
                       _MERRY_STACK_UNDERFLOW_);
  }
  mqword_t tmp;
  if (merry_RAM_read_qword(core->stack, base->REGISTER_FILE[SP], &tmp, ) ==
      RET_FAILURE) {
    // This will not fail at all
    // i.e this section should never be executed

    return;
  }
  if (merry_core_mem_access_WRITE_DATA_qword(, base->iram,
                                             instruction & 0xFFFFFFFFFFFF,
                                             tmp) != RBC_MEM_OPER_SUCCESS) {

    *kcore = mtrue;
    MFATAL("RBC",
           "MEM WRITE Failed at ADDR=%zu likely because the access was invalid",
           addr);
    return;
  }
  base->REGISTER_FILE[SP] -= 8;
}

rbc_ihdlrX(loadsb) {
  register mqword_t off = instruction.half_half_words.w1;
  sign_extend16(off);
  mbyte_t tmp;
  if (merry_RAM_read_byte(core->stack, base->REGISTER_FILE[BP] + off, &tmp, ) ==
      RET_FAILURE) {
    merry_assign_state(core->base->state, _MERRY_PROGRAM_ERROR_,
                       _INVALID_STACK_ACCESS_);
    merry_provide_context(core->base->state, _MERRY_CORE_EXECUTING_);
    core->req->type = PROBLEM_ENCOUNTERED;
    merry_SEND_REQUEST(core->req);

    return;
  }
  base->REGISTER_FILE[instruction & RBC_R15] = tmp;
}

rbc_ihdlrX(loadsw) {
  register mqword_t off = instruction.half_half_words.w1;
  sign_extend16(off);
  mword_t tmp;
  if (merry_RAM_read_word(core->stack, base->REGISTER_FILE[BP] + off, &tmp, ) ==
      RET_FAILURE) {
    merry_assign_state(core->base->state, _MERRY_PROGRAM_ERROR_,
                       _INVALID_STACK_ACCESS_);
    merry_provide_context(core->base->state, _MERRY_CORE_EXECUTING_);
    core->req->type = PROBLEM_ENCOUNTERED;
    merry_SEND_REQUEST(core->req);

    return;
  }
  base->REGISTER_FILE[instruction & RBC_R15] = tmp;
}

rbc_ihdlrX(loadsd) {
  register mqword_t off = instruction.half_half_words.w1;
  sign_extend16(off);
  mdword_t tmp;
  if (merry_RAM_read_dword(core->stack, base->REGISTER_FILE[BP] + off,
                           &tmp, ) != RBC_MEM_OPER_SUCCESS) {
    merry_assign_state(core->base->state, _MERRY_PROGRAM_ERROR_,
                       _INVALID_STACK_ACCESS_);
    merry_provide_context(core->base->state, _MERRY_CORE_EXECUTING_);
    core->req->type = PROBLEM_ENCOUNTERED;
    merry_SEND_REQUEST(core->req);

    return;
  }
  base->REGISTER_FILE[instruction & RBC_R15] = tmp;
}

rbc_ihdlrX(loadsq) {
  register mqword_t off = instruction.half_half_words.w1;
  sign_extend16(off);
  mqword_t tmp;
  if (merry_RAM_read_qword(core->stack, base->REGISTER_FILE[BP] + off,
                           &tmp, ) != RBC_MEM_OPER_SUCCESS) {
    merry_assign_state(core->base->state, _MERRY_PROGRAM_ERROR_,
                       _INVALID_STACK_ACCESS_);
    merry_provide_context(core->base->state, _MERRY_CORE_EXECUTING_);
    core->req->type = PROBLEM_ENCOUNTERED;
    merry_SEND_REQUEST(core->req);

    return;
  }
  base->REGISTER_FILE[instruction & RBC_R15] = tmp;
}

rbc_ihdlrX(storesb) {
  register mqword_t off = instruction.half_half_words.w1;
  sign_extend16(off);
  if (merry_RAM_write_byte(core->stack, base->REGISTER_FILE[BP] + off,
                           base->REGISTER_FILE[instruction & RBC_R15] &
                               0xFF, ) != RBC_MEM_OPER_SUCCESS) {
    merry_assign_state(core->base->state, _MERRY_PROGRAM_ERROR_,
                       _INVALID_STACK_ACCESS_);
    merry_provide_context(core->base->state, _MERRY_CORE_EXECUTING_);
    core->req->type = PROBLEM_ENCOUNTERED;
    merry_SEND_REQUEST(core->req);

    return;
  }
}

rbc_ihdlrX(storesw) {
  register mqword_t off = instruction.half_half_words.w1;
  sign_extend16(off);
  if (merry_RAM_write_word(core->stack, base->REGISTER_FILE[BP] + off,
                           base->REGISTER_FILE[instruction & RBC_R15] &
                               0xFFFF, ) != RBC_MEM_OPER_SUCCESS) {
    merry_assign_state(core->base->state, _MERRY_PROGRAM_ERROR_,
                       _INVALID_STACK_ACCESS_);
    merry_provide_context(core->base->state, _MERRY_CORE_EXECUTING_);
    core->req->type = PROBLEM_ENCOUNTERED;
    merry_SEND_REQUEST(core->req);

    return;
  }
}

rbc_ihdlrX(storesd) {
  register mqword_t off = instruction.half_half_words.w1;
  sign_extend16(off);
  if (merry_RAM_write_dword(core->stack, base->REGISTER_FILE[BP] + off,
                            base->REGISTER_FILE[instruction & RBC_R15] &
                                0xFFFFFFFF, ) != RBC_MEM_OPER_SUCCESS) {
    merry_assign_state(core->base->state, _MERRY_PROGRAM_ERROR_,
                       _INVALID_STACK_ACCESS_);
    merry_provide_context(core->base->state, _MERRY_CORE_EXECUTING_);
    core->req->type = PROBLEM_ENCOUNTERED;
    merry_SEND_REQUEST(core->req);

    return;
  }
}

rbc_ihdlrX(storesq) {
  register mqword_t off = instruction.half_half_words.w1;
  sign_extend16(off);
  if (merry_RAM_write_qword(core->stack, base->REGISTER_FILE[BP] + off,
                            base->REGISTER_FILE[instruction & RBC_R15], ) ==
      RET_FAILURE) {
    merry_assign_state(core->base->state, _MERRY_PROGRAM_ERROR_,
                       _INVALID_STACK_ACCESS_);
    merry_provide_context(core->base->state, _MERRY_CORE_EXECUTING_);
    core->req->type = PROBLEM_ENCOUNTERED;
    merry_SEND_REQUEST(core->req);

    return;
  }
}

rbc_ihdlrX(and_imm) {
  mqword_t imm;
  base->PC += 8;
  if (rbc_memory_read_qword(base->iram, base->PC, &imm) !=
      RBC_MEM_OPER_SUCCESS) {

    return;
  }
  base->REGISTER_FILE[instruction & RBC_R15] &= imm;
}

rbc_ihdlrX(or_imm) {
  mqword_t imm;
  base->PC += 8;
  if (rbc_memory_read_qword(base->iram, base->PC, &imm) !=
      RBC_MEM_OPER_SUCCESS) {

    return;
  }
  base->REGISTER_FILE[instruction & RBC_R15] |= imm;
}

rbc_ihdlrX(xor_imm) {
  mqword_t imm;
  base->PC += 8;
  if (rbc_memory_read_qword(base->iram, base->PC, &imm) !=
      RBC_MEM_OPER_SUCCESS) {

    return;
  }
  base->REGISTER_FILE[instruction & RBC_R15] ^= imm;
}

rbc_ihdlrX(cmp_imm) {
  mqword_t imm;
  base->PC += 8;
  if (rbc_memory_read_qword(base->iram, base->PC, &imm) !=
      RBC_MEM_OPER_SUCCESS) {

    return;
  }
  merry_compare_two_values(imm, base->REGISTER_FILE[instruction & RBC_R15],
                           &core->fregr);
}

rbc_ihdlrX(cmp_imm_memb) {
  MerryHostMemLayout imm;
  if (rbc_memory_read_byte(base->iram, instruction & 0xFFFFFFFFFFFF,
                           &imm.bytes.b7) != RBC_MEM_OPER_SUCCESS) {

    *kcore = mtrue;
    MFATAL("RBC",
           "MEM WRITE Failed at ADDR=%zu likely because the access was invalid",
           addr);
    return;
  }
  merry_compare_two_values(
      imm.whole_word, base->REGISTER_FILE[instruction & RBC_R15], &core->fregr);
}

rbc_ihdlrX(cmp_imm_memw) {
  MerryHostMemLayout imm;
  if (rbc_memory_read_word(base->iram, instruction & 0xFFFFFFFFFFFF,
                           &imm.half_half_words.w3) != RBC_MEM_OPER_SUCCESS) {

    *kcore = mtrue;
    MFATAL("RBC",
           "MEM WRITE Failed at ADDR=%zu likely because the access was invalid",
           addr);
    return;
  }
  merry_compare_two_values(
      imm.whole_word, base->REGISTER_FILE[instruction & RBC_R15], &core->fregr);
}

rbc_ihdlrX(cmp_imm_memd) {
  MerryHostMemLayout imm;
  if (rbc_memory_read_dword(base->iram, instruction & 0xFFFFFFFFFFFF,
                            &imm.half_words.w1) != RBC_MEM_OPER_SUCCESS) {

    *kcore = mtrue;
    MFATAL("RBC",
           "MEM WRITE Failed at ADDR=%zu likely because the access was invalid",
           addr);
    return;
  }
  merry_compare_two_values(
      imm.whole_word, base->REGISTER_FILE[instruction & RBC_R15], &core->fregr);
}

rbc_ihdlrX(cmp_imm_memq) {
  MerryHostMemLayout imm;
  if (rbc_memory_read_qword(base->iram, instruction & 0xFFFFFFFFFFFF,
                            &imm.whole_word) != RBC_MEM_OPER_SUCCESS) {

    *kcore = mtrue;
    MFATAL("RBC",
           "MEM WRITE Failed at ADDR=%zu likely because the access was invalid",
           addr);
    return;
  }
  merry_compare_two_values(
      imm.whole_word, base->REGISTER_FILE[instruction & RBC_R15], &core->fregr);
}

rbc_ihdlrX(sin) {
  register mqword_t len = base->REGISTER_FILE[R0];
  register mqword_t addr = instruction & 0xFFFFFFFFFFFF;
  mbptr_t temp = (mbptr_t)malloc(len);
  if (temp == NULL) {
    merry_assign_state(core->base->state, _MERRY_INTERNAL_SYSTEM_ERROR_,
                       _MERRY_MEM_ALLOCATION_FAILURE_);
    merry_provide_context(core->base->state, _MERRY_CORE_EXECUTING_);
    core->req->type = PROBLEM_ENCOUNTERED;
    merry_SEND_REQUEST(core->req);

    return;
  }
  for (msize_t i = 0; i < len; i++) {
    temp[i] = getchar();
  }
  if (merry_core_base_mem_access_bulk_write(base->iram, addr, temp, len) ==
      RET_FAILURE)

    free(temp);
}

rbc_ihdlrX(sout) {
  register mqword_t len = base->REGISTER_FILE[R0];
  register mqword_t addr = instruction & 0xFFFFFFFFFFFF;
  mbptr_t temp;
  if (merry_core_base_mem_access_bulk_read(base->iram, addr, &temp, len) ==
      RET_FAILURE) {

  } else {
    printf("%s", temp);
    free(temp);
  }
}

rbc_ihdlrX(loadb) {
  register mbyte_t op1 = instruction & RBC_R15;
  mbyte_t imm;
  if (rbc_memory_read_byte(base->iram, instruction & 0xFFFFFFFFFFFF, &imm) ==
      RET_FAILURE) {

    return;
  }
  base->REGISTER_FILE[op1] = imm;
}

rbc_ihdlrX(storeb) {
  register mbyte_t op1 = instruction & RBC_R15;
  if (merry_core_mem_access_WRITE_DATA_byte(
          , base->iram, instruction & 0xFFFFFFFFFFFF,
          base->REGISTER_FILE[op1] & 0xFF) != RBC_MEM_OPER_SUCCESS) {

    return;
  }
}

rbc_ihdlrX(loadw) {
  register mbyte_t op1 = instruction & RBC_R15;
  mword_t imm;
  if (rbc_memory_read_word(base->iram, instruction & 0xFFFFFFFFFFFF, &imm) ==
      RET_FAILURE) {

    return;
  }
  base->REGISTER_FILE[op1] = imm;
}

rbc_ihdlrX(storew) {
  register mbyte_t op1 = instruction & RBC_R15;
  if (merry_core_mem_access_WRITE_DATA_word(
          , base->iram, instruction & 0xFFFFFFFFFFFF,
          base->REGISTER_FILE[op1] & 0xFFFF) != RBC_MEM_OPER_SUCCESS) {

    return;
  }
}

rbc_ihdlrX(loadd) {
  register mbyte_t op1 = instruction & RBC_R15;
  mdword_t imm;
  if (rbc_memory_read_dword(base->iram, instruction & 0xFFFFFFFFFFFF, &imm) ==
      RET_FAILURE) {

    return;
  }
  base->REGISTER_FILE[op1] = imm;
}

rbc_ihdlrX(stored) {
  register mbyte_t op1 = instruction & RBC_R15;
  if (merry_core_mem_access_WRITE_DATA_dword(
          , base->iram, instruction & 0xFFFFFFFFFFFF,
          base->REGISTER_FILE[op1] & 0xFFFFFFFF) != RBC_MEM_OPER_SUCCESS) {

    return;
  }
}

rbc_ihdlrX(loadq) {
  register mbyte_t op1 = instruction & RBC_R15;
  mqword_t imm;
  if (rbc_memory_read_qword(base->iram, instruction & 0xFFFFFFFFFFFF, &imm) ==
      RET_FAILURE) {

    return;
  }
  base->REGISTER_FILE[op1] = imm;
}

rbc_ihdlrX(storeq) {
  register mbyte_t op1 = instruction & RBC_R15;
  if (merry_core_mem_access_WRITE_DATA_qword(
          , base->iram, instruction & 0xFFFFFFFFFFFF,
          base->REGISTER_FILE[op1]) != RBC_MEM_OPER_SUCCESS) {

    return;
  }
}

rbc_ihdlrX(loadb_reg) {
  register mbyte_t op1 = instruction & RBC_R15;
  register mbyte_t op2 = instruction & RBC_R15;
  mbyte_t imm;
  if (rbc_memory_read_byte(base->iram, base->REGISTER_FILE[op2], &imm) ==
      RET_FAILURE) {

    return;
  }
  base->REGISTER_FILE[op1] = imm;
}

rbc_ihdlrX(storeb_reg) {
  register mbyte_t op1 = instruction & RBC_R15;
  register mbyte_t op2 = instruction & RBC_R15;
  if (merry_core_mem_access_WRITE_DATA_byte(
          , base->iram, base->REGISTER_FILE[op2],
          base->REGISTER_FILE[op1] & 0xFF) != RBC_MEM_OPER_SUCCESS) {

    return;
  }
}

rbc_ihdlrX(loadw_reg) {
  register mbyte_t op1 = instruction & RBC_R15;
  register mbyte_t op2 = instruction & RBC_R15;
  mword_t imm;
  if (rbc_memory_read_word(base->iram, base->REGISTER_FILE[op2], &imm) ==
      RET_FAILURE) {

    return;
  }
  base->REGISTER_FILE[op1] = imm;
}

rbc_ihdlrX(storew_reg) {
  register mbyte_t op1 = instruction & RBC_R15;
  register mbyte_t op2 = instruction & RBC_R15;
  if (merry_core_mem_access_WRITE_DATA_word(
          , base->iram, base->REGISTER_FILE[op2],
          base->REGISTER_FILE[op1] & 0xFFFF) != RBC_MEM_OPER_SUCCESS) {

    return;
  }
}

rbc_ihdlrX(loadd_reg) {
  register mbyte_t op1 = instruction & RBC_R15;
  register mbyte_t op2 = instruction & RBC_R15;
  mdword_t imm;
  if (rbc_memory_read_dword(base->iram, base->REGISTER_FILE[op2], &imm) ==
      RET_FAILURE) {

    return;
  }
  base->REGISTER_FILE[op1] = imm;
}

rbc_ihdlrX(stored_reg) {
  register mbyte_t op1 = instruction & RBC_R15;
  register mbyte_t op2 = instruction & RBC_R15;
  if (merry_core_mem_access_WRITE_DATA_dword(
          , base->iram, base->REGISTER_FILE[op2],
          base->REGISTER_FILE[op1] & 0xFFFFFFFF) != RBC_MEM_OPER_SUCCESS) {

    return;
  }
}

rbc_ihdlrX(loadq_reg) {
  register mbyte_t op1 = instruction & RBC_R15;
  register mbyte_t op2 = instruction & RBC_R15;
  if (rbc_memory_read_qword(base->iram, base->REGISTER_FILE[op2],
                            &base->REGISTER_FILE[op1]) !=
      RBC_MEM_OPER_SUCCESS) {

    return;
  }
}

rbc_ihdlrX(storeq_reg) {
  register mbyte_t op1 = instruction & RBC_R15;
  register mbyte_t op2 = instruction & RBC_R15;
  if (merry_core_mem_access_WRITE_DATA_qword(
          , base->iram, base->REGISTER_FILE[op2], base->REGISTER_FILE[op1]) ==
      RET_FAILURE) {

    return;
  }
}

rbc_ihdlrX(atm_loadb) {
  register mbyte_t op1 = instruction & RBC_R15;
  mbyte_t imm;
  if (rbc_memory_read_byte_atm(base->iram, instruction & 0xFFFFFFFFFFFF,
                               &imm) != RBC_MEM_OPER_SUCCESS) {

    return;
  }
  base->REGISTER_FILE[op1] = imm;
}

rbc_ihdlrX(atm_loadw) {
  register mbyte_t op1 = instruction & RBC_R15;
  mword_t imm;
  if (rbc_memory_read_word_atm(base->iram, instruction & 0xFFFFFFFFFFFF,
                               &imm) != RBC_MEM_OPER_SUCCESS) {

    return;
  }
  base->REGISTER_FILE[op1] = imm;
}

rbc_ihdlrX(atm_loadd) {
  register mbyte_t op1 = instruction & RBC_R15;
  mdword_t imm;
  if (rbc_memory_read_dword_atm(base->iram, instruction & 0xFFFFFFFFFFFF,
                                &imm) != RBC_MEM_OPER_SUCCESS) {

    return;
  }
  base->REGISTER_FILE[op1] = imm;
}

rbc_ihdlrX(atm_loadq) {
  register mbyte_t op1 = instruction & RBC_R15;
  mqword_t imm;
  if (rbc_memory_read_qword_atm(base->iram, instruction & 0xFFFFFFFFFFFF,
                                &imm) != RBC_MEM_OPER_SUCCESS) {

    return;
  }
  base->REGISTER_FILE[op1] = imm;
}

rbc_ihdlrX(atm_storeb) {
  register mbyte_t op1 = instruction & RBC_R15;
  if (merry_core_mem_access_WRITE_DATA_byte_atm(
          , base->iram, instruction & 0xFFFFFFFFFFFF,
          base->REGISTER_FILE[op1]) != RBC_MEM_OPER_SUCCESS) {

    return;
  }
}

rbc_ihdlrX(atm_storew) {
  register mbyte_t op1 = instruction & RBC_R15;
  if (merry_core_mem_access_WRITE_DATA_word_atm(
          , base->iram, instruction & 0xFFFFFFFFFFFF,
          base->REGISTER_FILE[op1]) != RBC_MEM_OPER_SUCCESS) {

    return;
  }
}

rbc_ihdlrX(atm_stored) {
  register mbyte_t op1 = instruction & RBC_R15;
  if (merry_core_mem_access_WRITE_DATA_dword_atm(
          , base->iram, instruction & 0xFFFFFFFFFFFF,
          base->REGISTER_FILE[op1]) != RBC_MEM_OPER_SUCCESS) {

    return;
  }
}

rbc_ihdlrX(atm_storeq) {
  register mbyte_t op1 = instruction & RBC_R15;
  if (merry_core_mem_access_WRITE_DATA_qword_atm(
          , base->iram, instruction & 0xFFFFFFFFFFFF,
          base->REGISTER_FILE[op1]) != RBC_MEM_OPER_SUCCESS) {

    return;
  }
}

rbc_ihdlrX(atm_loadb_reg) {
  register mbyte_t op1 = instruction & RBC_R15;
  register mbyte_t op2 = instruction & RBC_R15;
  mbyte_t imm;
  if (rbc_memory_read_byte_atm(base->iram, base->REGISTER_FILE[op2], &imm) ==
      RET_FAILURE) {

    return;
  }
  base->REGISTER_FILE[op1] = imm;
}

rbc_ihdlrX(atm_loadw_reg) {
  register mbyte_t op1 = instruction & RBC_R15;
  register mbyte_t op2 = instruction & RBC_R15;
  mword_t imm;
  if (rbc_memory_read_word_atm(base->iram, base->REGISTER_FILE[op2], &imm) ==
      RET_FAILURE) {

    return;
  }
  base->REGISTER_FILE[op1] = imm;
}

rbc_ihdlrX(atm_loadd_reg) {
  register mbyte_t op1 = instruction & RBC_R15;
  register mbyte_t op2 = instruction & RBC_R15;
  mdword_t imm;
  if (rbc_memory_read_dword_atm(base->iram, base->REGISTER_FILE[op2], &imm) ==
      RET_FAILURE) {

    return;
  }
  base->REGISTER_FILE[op1] = imm;
}

rbc_ihdlrX(atm_loadq_reg) {
  register mbyte_t op1 = instruction & RBC_R15;
  register mbyte_t op2 = instruction & RBC_R15;
  if (rbc_memory_read_qword_atm(base->iram, base->REGISTER_FILE[op2],
                                &base->REGISTER_FILE[op1]) !=
      RBC_MEM_OPER_SUCCESS) {

    return;
  }
}

rbc_ihdlrX(atm_storeb_reg) {
  register mbyte_t op1 = instruction & RBC_R15;
  register mbyte_t op2 = instruction & RBC_R15;
  if (merry_core_mem_access_WRITE_DATA_byte_atm(
          , base->iram, base->REGISTER_FILE[op1],
          base->REGISTER_FILE[op2] & 0xFF) != RBC_MEM_OPER_SUCCESS) {

    return;
  }
}

rbc_ihdlrX(atm_storew_reg) {
  register mbyte_t op1 = instruction & RBC_R15;
  register mbyte_t op2 = instruction & RBC_R15;
  if (merry_core_mem_access_WRITE_DATA_word_atm(
          , base->iram, base->REGISTER_FILE[op1],
          base->REGISTER_FILE[op2] & 0xFFFF) != RBC_MEM_OPER_SUCCESS) {

    return;
  }
}

rbc_ihdlrX(atm_stored_reg) {
  register mbyte_t op1 = instruction & RBC_R15;
  register mbyte_t op2 = instruction & RBC_R15;
  if (merry_core_mem_access_WRITE_DATA_dword_atm(
          , base->iram, base->REGISTER_FILE[op1],
          base->REGISTER_FILE[op2] & 0xFFFFFFFF) != RBC_MEM_OPER_SUCCESS) {

    return;
  }
}

rbc_ihdlrX(atm_storeq_reg) {
  register mbyte_t op1 = instruction & RBC_R15;
  register mbyte_t op2 = instruction & RBC_R15;
  if (merry_core_mem_access_WRITE_DATA_qword_atm(
          , base->iram, base->REGISTER_FILE[op1], base->REGISTER_FILE[op2]) ==
      RET_FAILURE) {

    return;
  }
}

rbc_ihdlrX(cmpxchg) {
  register mbyte_t desired = instruction;
  register mbyte_t expected = instruction;
  mqword_t address;
  base->PC += 8;
  if (rbc_memory_read_qword(core->base->iram, base->PC, &address) ==
      RET_FAILURE) {

    return;
  }
  MerryFlagsRegr reg = core->fregr;
  mret_t ret = merry_RAM_cmpxchg(base->iram, address, desired, expected, );
  base->flags.regr = merry_obtain_flags_regr();
  if (ret != RBC_MEM_OPER_SUCCESS) {
    core->req->type = TRY_LOADING_NEW_PAGE_DATA;
    core->req->args[0] = address / _MERRY_PAGE_LEN_;
    if (merry_SEND_REQUEST(core->req) != RBC_MEM_OPER_SUCCESS ||
        core->req->args[0] == 1) {

      core->fregr = reg;
      return;
    }
    ret = merry_RAM_cmpxchg(base->iram, address, desired, expected,
                            ); // this shouldn't fail now
    base->flags.regr = merry_obtain_flags_regr();
  }
}

rbc_ihdlrX(cmpxchg_reg) {
  register mbyte_t desired = instruction & RBC_R15;
  register mbyte_t expected = instruction & RBC_R15;
  register mqword_t address =
      base->REGISTER_FILE[instruction.bytes.b5 & RBC_R15];
  MerryFlagsRegr reg = core->fregr;
  mret_t ret = merry_RAM_cmpxchg(base->iram, address, desired, expected, );
  base->flags.regr = merry_obtain_flags_regr();
  if (ret != RBC_MEM_OPER_SUCCESS) {
    core->req->type = TRY_LOADING_NEW_PAGE_DATA;
    core->req->args[0] = address / _MERRY_PAGE_LEN_;
    if (merry_SEND_REQUEST(core->req) != RBC_MEM_OPER_SUCCESS ||
        core->req->args[0] == 1) {

      core->fregr = reg;
      return;
    }
    ret = merry_RAM_cmpxchg(base->iram, address, desired, expected,
                            ); // this shouldn't fail now
    base->flags.regr = merry_obtain_flags_regr();
  }
}
rbc_ihdlrX(add) {
  // Break down the instructionrucion
}
