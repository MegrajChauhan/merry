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
  if ((base->REGISTER_FILE[RBC_R15] = merry_SEND_REQUEST(&base->req)) !=
      MRES_SUCCESS)
    return;

  // results here
  base->REGISTER_FILE[RBC_R14] = base->req.result.result;
  if (base->req.result.result == MRES_SYS_FAILURE) {
    base->REGISTER_FILE[RBC_R13] = base->req.result.ERRNO;
  } else if (base->req.result.result == MRES_NOT_MERRY_FAILURE) {
    base->REGISTER_FILE[RBC_R13] = base->req.result.ic_res.source;
    base->REGISTER_FILE[RBC_R12] = base->req.result.ic_res.ERRNO;
    base->REGISTER_FILE[RBC_R11] = base->req.result.ic_res._core_code;
  }
  if (base->req.result.result != MRES_SUCCESS)
    return;
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
  register mbyte_t op1 = (instruction >> 4) & RBC_R15;
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
  register mbyte_t op1 = (instruction >> 4) & RBC_R15;
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
  register mbyte_t op1 = (instruction >> 4) & RBC_R15;
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
  register mbyte_t op1 = (instruction >> 4) & RBC_R15;
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
  register mbyte_t op1 = (instruction >> 4) & RBC_R15;
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
  register mqword_t op1 = (instruction >> 4) & RBC_R15;
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
  register mqword_t op1 = (instruction >> 4) & RBC_R15;
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
  register mqword_t op1 = (instruction >> 4) & RBC_R15;
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
  register mqword_t op1 = (instruction >> 4) & RBC_R15;
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
  register mqword_t op1 = (instruction >> 4) & RBC_R15;
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
  register mqword_t op1 = (instruction >> 4) & RBC_R15;
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
  register mqword_t op1 = (instruction >> 4) & RBC_R15;
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
  register mqword_t op1 = (instruction >> 4) & RBC_R15;
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
  register mqword_t op1 = (instruction >> 48) & RBC_R15;
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
  register mqword_t op1 = (instruction >> 48) & RBC_R15;
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
  register mqword_t op1 = (instruction >> 48) & RBC_R15;
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
  register mqword_t op1 = (instruction >> 48) & RBC_R15;
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
  register mqword_t op1 = (instruction >> 48) & RBC_R15;
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
  register mqword_t op1 = (instruction >> 48) & RBC_R15;
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
  register mqword_t op1 = (instruction >> 48) & RBC_R15;
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
  register mqword_t op1 = (instruction >> 48) & RBC_R15;
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
  register mqword_t op1 = (instruction >> 48) & RBC_R15;
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
  register mqword_t op1 = (instruction >> 48) & RBC_R15;
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
  register mqword_t op1 = (instruction >> 48) & RBC_R15;
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
  register mqword_t op1 = (instruction >> 48) & RBC_R15;
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
  register mqword_t op1 = (instruction >> 48) & RBC_R15;
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
  register mqword_t op1 = (instruction >> 48) & RBC_R15;
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
  register mqword_t op1 = (instruction >> 48) & RBC_R15;
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
  register mqword_t op1 = (instruction >> 48) & RBC_R15;
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
  register mqword_t op1 = (instruction >> 48) & RBC_R15;
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
  register mqword_t op1 = (instruction >> 48) & RBC_R15;
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
  register mqword_t op1 = (instruction >> 48) & RBC_R15;
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
  register mqword_t op1 = (instruction >> 48) & RBC_R15;
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
  register mqword_t op1 = (instruction >> 48) & RBC_R15;
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
  register mqword_t op1 = (instruction >> 48) & RBC_R15;
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
  register mqword_t op1 = (instruction >> 48) & RBC_R15;
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
  register mqword_t op1 = (instruction >> 48) & RBC_R15;
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
  register mqword_t op1 = (instruction >> 48) & RBC_R15;
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
  register mqword_t op1 = (instruction >> 48) & RBC_R15;
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
  register mqword_t op1 = (instruction >> 48) & RBC_R15;
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
  register mqword_t op1 = (instruction >> 48) & RBC_R15;
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
  register mbyte_t op1 = (instruction >> 48) & RBC_R15;
  register mqword_t op2 = instruction & 0xFF;
  sign_extend8(op2);
  base->REGISTER_FILE[op1] = op2;
}

rbc_ihdlrX(movesx_imm16) {
  register mbyte_t op1 = (instruction >> 48) & RBC_R15;
  register mqword_t op2 = instruction & 0xFFFF;
  sign_extend16(op2);
  base->REGISTER_FILE[op1] = op2;
}

rbc_ihdlrX(movesx_imm32) {
  register mbyte_t op1 = (instruction >> 48) & RBC_R15;
  register mqword_t op2 = instruction & 0xFFFFFFFF;
  sign_extend32(op2);
  base->REGISTER_FILE[op1] = op2;
}

rbc_ihdlrX(movesx_reg8) {
  register mbyte_t op1 = (instruction >> 4) & RBC_R15;
  register mbyte_t op2 = instruction & RBC_R15;
  sign_extend8(base->REGISTER_FILE[op2]);
  base->REGISTER_FILE[op1] = op2;
}

rbc_ihdlrX(movesx_reg16) {
  register mbyte_t op1 = (instruction >> 4) & RBC_R15;
  register mbyte_t op2 = instruction & RBC_R15;
  sign_extend16(base->REGISTER_FILE[op2]);
  base->REGISTER_FILE[op1] = op2;
}

rbc_ihdlrX(movesx_reg32) {
  register mbyte_t op1 = (instruction >> 4) & RBC_R15;
  register mbyte_t op2 = instruction & RBC_R15;
  sign_extend32(base->REGISTER_FILE[op2]);
  base->REGISTER_FILE[op1] = op2;
}

rbc_ihdlrX(excg) {
  register mbyte_t op1 = (instruction >> 4) & RBC_R15;
  register mbyte_t op2 = instruction & RBC_R15;
  register mqword_t temp = base->REGISTER_FILE[op1];
  base->REGISTER_FILE[op1] = base->REGISTER_FILE[op2];
  base->REGISTER_FILE[op2] = temp;
}

rbc_ihdlrX(excg8) {
  register mbyte_t op1 = (instruction >> 4) & RBC_R15;
  register mbyte_t op2 = instruction & RBC_R15;
  register mqword_t r1 = base->REGISTER_FILE[op1];
  base->REGISTER_FILE[op1] |= base->REGISTER_FILE[op2] & 0x00000000000000FF;
  base->REGISTER_FILE[op2] |= r1 & 0x00000000000000FF;
}

rbc_ihdlrX(excg16) {
  register mbyte_t op1 = (instruction >> 4) & RBC_R15;
  register mbyte_t op2 = instruction & RBC_R15;
  register mqword_t r1 = base->REGISTER_FILE[op1];
  base->REGISTER_FILE[op1] |= base->REGISTER_FILE[op2] & 0x000000000000FFFF;
  base->REGISTER_FILE[op2] |= r1 & 0x000000000000FFFF;
}

rbc_ihdlrX(excg32) {
  register mbyte_t op1 = (instruction >> 4) & RBC_R15;
  register mbyte_t op2 = instruction & RBC_R15;
  register mqword_t r1 = base->REGISTER_FILE[op1];
  base->REGISTER_FILE[op1] |= base->REGISTER_FILE[op2] & 0x00000000FFFFFFFF;
  base->REGISTER_FILE[op2] |= r1 & 0x00000000FFFFFFFF;
}

rbc_ihdlrX(call) {
  mqword_t addr = instruction & 0xFFFFFFFFFFFF;
  RBCStackFrame frame;
  frame.FRAME_BP = base->SP;
  frame.JMP_TO = addr;
  frame.RET_ADDR = base->PC;
  if (merry_is_stack_full(base->stack_frames)) {
    MFATAL("RBC", "Call depth reached: PC=%zu. Terminating...", base->PC);
    *kcore = mtrue;
    return;
  }
  if (merry_RBCProcFrame_stack_push(base->stack_frames, &frame) ==
      RET_FAILURE) {
    *kcore = mtrue;
    MFATAL("RBC", "Failed to create stack frame: PC=%zu, PROC=%zu", base->PC,
           addr);
    return;
  }
  base->BP = base->SP;
  base->PC = addr; // the address to the first instruction of the procedure
}

rbc_ihdlrX(ret) {

  if (merry_is_stack_empty(base->stack_frames)) {
    *kcore = mtrue;
    MFATAL("RBC", "Invalid RET instruction: PC=%zu", base->PC);
    return;
  }

  RBCStackFrame frame;
  if ((merry_RBCProcFrame_stack_pop(base->stack_frames, &frame)) !=
      MRES_SUCCESS) {
    *kcore = mtrue;
    MFATAL("RBC", "Failed to restore stack frame: PC=%zu", base->PC);
    return;
  }
  base->SP = base->BP;
  base->PC = frame.RET_ADDR;
  base->BP = frame.FRAME_BP;
}

rbc_ihdlrX(call_reg) {
  maddress_t addr = base->REGISTER_FILE[instruction & RBC_R15];
  RBCStackFrame frame;
  frame.FRAME_BP = base->SP;
  frame.JMP_TO = addr;
  frame.RET_ADDR = base->PC;
  if (merry_is_stack_full(base->stack_frames)) {
    MFATAL("RBC", "Call depth reached: PC=%zu. Terminating...", base->PC);
    *kcore = mtrue;
    return;
  }
  if (merry_RBCProcFrame_stack_push(base->stack_frames, &frame) ==
      RET_FAILURE) {
    *kcore = mtrue;
    MFATAL("RBC", "Failed to create stack frame: PC=%zu, PROC=%zu", base->PC,
           addr);
    return;
  }
  base->BP = base->SP;
  base->PC = addr;
}

rbc_ihdlrX(push_immq) {
  MerryHostMemLayout imm;
  if (base->SP >= (_RBC_STACK_LEN_ / 8)) {
    *kcore = mtrue;
    MFATAL("RBC", "Stack Overflow. Terminating...", NULL);
    return;
  }
  base->PC += 8;
  if (rbc_memory_read_qword(base->iram, base->PC, &imm.whole_word) ==
      RET_FAILURE) {
    *kcore = mtrue;
    MFATAL("RBC",
           "MEM READ Failed at PC=%zu likely because the access was invalid",
           base->PC);
    return;
  }
  base->stack[base->SP] = imm.whole_word;
  base->SP++;
}

rbc_ihdlrX(push_reg) {
  mqword_t imm = base->REGISTER_FILE[instruction & RBC_R15];
  if (base->SP >= (_RBC_STACK_LEN_ / 8)) {
    *kcore = mtrue;
    MFATAL("RBC", "Stack Overflow. Terminating...", NULL);
    return;
  }
  base->stack[base->SP] = imm;
  base->SP++;
  return;
}

rbc_ihdlrX(popq) {
  if (base->SP == 0) {
    *kcore = mtrue;
    MFATAL("RBC", "Stack Underflow. Terminating...", NULL);
    return;
  }
  base->SP--;
  base->REGISTER_FILE[instruction & RBC_R15] = base->stack[base->SP];
}

rbc_ihdlrX(pusha) {
  if (base->SP >= (_RBC_STACK_LEN_ / 8 - RBC_REG_COUNT)) {
    *kcore = mtrue;
    MFATAL("RBC", "Stack Overflow. Terminating...", NULL);
    return;
  }
  for (msize_t i = 0; i < RBC_REG_COUNT; i++) {
    base->stack[base->SP] = base->REGISTER_FILE[i];
    base->SP++;
  }
}

rbc_ihdlrX(popa) {
  if (base->SP < RBC_REG_COUNT) {
    *kcore = mtrue;
    MFATAL("RBC", "Stack Underflow. Terminating...", NULL);
    return;
  }
  for (msize_t i = RBC_R15; i >= 0; i--) {
    base->SP--;
    base->REGISTER_FILE[i] = base->stack[base->SP];
  }
}

rbc_ihdlrX(push_memb) {
  if (base->SP >= (_RBC_STACK_LEN_ / 8)) {
    *kcore = mtrue;
    MFATAL("RBC", "Stack Overflow. Terminating...", NULL);
    return;
  }
  mbyte_t imm;
  if (rbc_memory_read_byte(base->dram, instruction & 0xFFFFFFFFFFFF, &imm) !=
      RBC_MEM_OPER_SUCCESS) {
    *kcore = mtrue;
    MFATAL("RBC",
           "MEM READ Failed at ADDR=%zu likely because the access was invalid",
           instruction & 0xFFFFFFFFFFFF);
    return;
  }
  base->stack[base->SP] = imm;
  base->SP++;
}

rbc_ihdlrX(push_memw) {
  if (base->SP >= (_RBC_STACK_LEN_ / 8)) {
    *kcore = mtrue;
    MFATAL("RBC", "Stack Overflow. Terminating...", NULL);
    return;
  }
  mword_t imm;
  if (rbc_memory_read_word(base->dram, instruction & 0xFFFFFFFFFFFF, &imm) !=
      RBC_MEM_OPER_SUCCESS) {
    *kcore = mtrue;
    MFATAL("RBC",
           "MEM READ Failed at ADDR=%zu likely because the access was invalid",
           instruction & 0xFFFFFFFFFFFF);
    return;
  }
  base->stack[base->SP] = imm;
  base->SP++;
}

rbc_ihdlrX(push_memd) {
  if (base->SP >= (_RBC_STACK_LEN_ / 8)) {
    *kcore = mtrue;
    MFATAL("RBC", "Stack Overflow. Terminating...", NULL);
    return;
  }
  mdword_t imm;
  if (rbc_memory_read_dword(base->dram, instruction & 0xFFFFFFFFFFFF, &imm) !=
      RBC_MEM_OPER_SUCCESS) {
    *kcore = mtrue;
    MFATAL("RBC",
           "MEM READ Failed at ADDR=%zu likely because the access was invalid",
           instruction & 0xFFFFFFFFFFFF);
    return;
  }
  base->stack[base->SP] = imm;
  base->SP++;
}

rbc_ihdlrX(push_memq) {
  if (base->SP >= (_RBC_STACK_LEN_ / 8)) {
    *kcore = mtrue;
    MFATAL("RBC", "Stack Overflow. Terminating...", NULL);
    return;
  }
  mqword_t imm;
  if (rbc_memory_read_qword(base->dram, instruction & 0xFFFFFFFFFFFF, &imm) !=
      RBC_MEM_OPER_SUCCESS) {
    *kcore = mtrue;
    MFATAL("RBC",
           "MEM READ Failed at ADDR=%zu likely because the access was invalid",
           instruction & 0xFFFFFFFFFFFF);
    return;
  }
  base->stack[base->SP] = imm;
  base->SP++;
}

rbc_ihdlrX(pop_memb) {
  if (base->SP == 0) {
    *kcore = mtrue;
    MFATAL("RBC", "Stack Underflow. Terminating...", NULL);
    return;
  }
  base->SP--;
  mbyte_t imm = base->stack[base->SP] & 0xFF;
  if (rbc_memory_write_byte(base->dram, instruction & 0xFFFFFFFFFFFF, imm) !=
      RBC_MEM_OPER_SUCCESS) {
    *kcore = mtrue;
    MFATAL("RBC",
           "MEM WRITE Failed at ADDR=%zu likely because the access was invalid",
           instruction & 0xFFFFFFFFFFFF);
    return;
  }
}

rbc_ihdlrX(pop_memw) {
  if (base->SP == 0) {
    *kcore = mtrue;
    MFATAL("RBC", "Stack Underflow. Terminating...", NULL);
    return;
  }
  base->SP--;
  mword_t imm = base->stack[base->SP] & 0xFFFF;
  if (rbc_memory_write_word(base->dram, instruction & 0xFFFFFFFFFFFF, imm) !=
      RBC_MEM_OPER_SUCCESS) {
    *kcore = mtrue;
    MFATAL("RBC",
           "MEM WRITE Failed at ADDR=%zu likely because the access was invalid",
           instruction & 0xFFFFFFFFFFFF);
    return;
  }
}

rbc_ihdlrX(pop_memd) {
  if (base->SP == 0) {
    *kcore = mtrue;
    MFATAL("RBC", "Stack Underflow. Terminating...", NULL);
    return;
  }
  base->SP--;
  mdword_t imm = base->stack[base->SP] & 0xFFFFFFFF;
  if (rbc_memory_write_dword(base->dram, instruction & 0xFFFFFFFFFFFF, imm) !=
      RBC_MEM_OPER_SUCCESS) {
    *kcore = mtrue;
    MFATAL("RBC",
           "MEM WRITE Failed at ADDR=%zu likely because the access was invalid",
           instruction & 0xFFFFFFFFFFFF);
    return;
  }
}

rbc_ihdlrX(pop_memq) {
  if (base->SP == 0) {
    *kcore = mtrue;
    MFATAL("RBC", "Stack Underflow. Terminating...", NULL);
    return;
  }
  base->SP--;
  mqword_t imm = base->stack[base->SP];
  if (rbc_memory_write_qword(base->dram, instruction & 0xFFFFFFFFFFFF, imm) !=
      RBC_MEM_OPER_SUCCESS) {
    *kcore = mtrue;
    MFATAL("RBC",
           "MEM WRITE Failed at ADDR=%zu likely because the access was invalid",
           instruction & 0xFFFFFFFFFFFF);
    return;
  }
}

rbc_ihdlrX(loadsq) {
  register mqword_t off = instruction & 0xFFFFFFFF;
  sign_extend32(off);
  register mqword_t res = base->BP + off;
  if (res >= base->SP) {
    *kcore = mtrue;
    MFATAL("RBC", "Invalid STACK Access: PC=%zu", base->PC);
    return;
  }
  base->REGISTER_FILE[(instruction >> 32) & RBC_R15] = base->stack[res];
}

rbc_ihdlrX(storesq) {
  register mqword_t off = instruction & 0xFFFFFFFF;
  sign_extend32(off);
  register mqword_t res = base->BP + off;
  if (res >= base->SP) {
    *kcore = mtrue;
    MFATAL("RBC", "Invalid STACK Access: PC=%zu", base->PC);
    return;
  }
  base->stack[res] = base->REGISTER_FILE[(instruction >> 32) & RBC_R15];
}

rbc_ihdlrX(and_imm) {
  mqword_t imm;
  base->PC += 8;
  if (rbc_memory_read_qword(base->iram, base->PC, &imm) !=
      RBC_MEM_OPER_SUCCESS) {
    *kcore = mtrue;
    MFATAL("RBC",
           "MEM READ Failed at PC=%zu likely because the access was invalid",
           base->PC);
    return;
  }
  base->REGISTER_FILE[instruction & RBC_R15] &= imm;
}

rbc_ihdlrX(or_imm) {
  mqword_t imm;
  base->PC += 8;
  if (rbc_memory_read_qword(base->iram, base->PC, &imm) !=
      RBC_MEM_OPER_SUCCESS) {
    *kcore = mtrue;
    MFATAL("RBC",
           "MEM READ Failed at PC=%zu likely because the access was invalid",
           base->PC);
    return;
  }
  base->REGISTER_FILE[instruction & RBC_R15] |= imm;
}

rbc_ihdlrX(xor_imm) {
  mqword_t imm;
  base->PC += 8;
  if (rbc_memory_read_qword(base->iram, base->PC, &imm) !=
      RBC_MEM_OPER_SUCCESS) {
    *kcore = mtrue;
    MFATAL("RBC",
           "MEM READ Failed at PC=%zu likely because the access was invalid",
           base->PC);
    return;
  }
  base->REGISTER_FILE[instruction & RBC_R15] ^= imm;
}

rbc_ihdlrX(cmp_imm) {
  mqword_t imm;
  base->PC += 8;
  if (rbc_memory_read_qword(base->iram, base->PC, &imm) !=
      RBC_MEM_OPER_SUCCESS) {
    *kcore = mtrue;
    MFATAL("RBC",
           "MEM READ Failed at PC=%zu likely because the access was invalid",
           base->PC);
    return;
  }
  merry_compare_two_values(imm, base->REGISTER_FILE[instruction & RBC_R15]);
  base->flags.regr = merry_obtain_flags_regr();
}

rbc_ihdlrX(cmp_imm_memb) {
  mbyte_t imm;
  if (rbc_memory_read_byte(base->dram, instruction & 0xFFFFFFFFFFFF, &imm) !=
      RBC_MEM_OPER_SUCCESS) {
    *kcore = mtrue;
    MFATAL("RBC",
           "MEM READ Failed at ADDR=%zu likely because the access was invalid",
           instruction & 0xFFFFFFFFFFFF);
    return;
  }
  merry_compare_two_values(imm,
                           base->REGISTER_FILE[(instruction >> 48) & RBC_R15]);
  base->flags.regr = merry_obtain_flags_regr();
}

rbc_ihdlrX(cmp_imm_memw) {
  mword_t imm;
  if (rbc_memory_read_word(base->dram, instruction & 0xFFFFFFFFFFFF, &imm) !=
      RBC_MEM_OPER_SUCCESS) {
    *kcore = mtrue;
    MFATAL("RBC",
           "MEM READ Failed at ADDR=%zu likely because the access was invalid",
           instruction & 0xFFFFFFFFFFFF);
    return;
  }
  merry_compare_two_values(imm,
                           base->REGISTER_FILE[(instruction >> 48) & RBC_R15]);
  base->flags.regr = merry_obtain_flags_regr();
}

rbc_ihdlrX(cmp_imm_memd) {
  mdword_t imm;
  if (rbc_memory_read_dword(base->dram, instruction & 0xFFFFFFFFFFFF, &imm) !=
      RBC_MEM_OPER_SUCCESS) {
    *kcore = mtrue;
    MFATAL("RBC",
           "MEM READ Failed at ADDR=%zu likely because the access was invalid",
           instruction & 0xFFFFFFFFFFFF);
    return;
  }
  merry_compare_two_values(imm,
                           base->REGISTER_FILE[(instruction >> 48) & RBC_R15]);
  base->flags.regr = merry_obtain_flags_regr();
}

rbc_ihdlrX(cmp_imm_memq) {
  mqword_t imm;
  if (rbc_memory_read_qword(base->dram, instruction & 0xFFFFFFFFFFFF, &imm) !=
      RBC_MEM_OPER_SUCCESS) {
    *kcore = mtrue;
    MFATAL("RBC",
           "MEM READ Failed at ADDR=%zu likely because the access was invalid",
           instruction & 0xFFFFFFFFFFFF);
    return;
  }
  merry_compare_two_values(imm,
                           base->REGISTER_FILE[(instruction >> 48) & RBC_R15]);
  base->flags.regr = merry_obtain_flags_regr();
}

rbc_ihdlrX(sin) {
  register mqword_t len = base->REGISTER_FILE[RBC_R0];
  register mqword_t addr = instruction & 0xFFFFFFFFFFFF;
  if (!len)
    return;
  mbptr_t temp = (mbptr_t)malloc(len);
  if (temp == NULL) {
    *kcore = mtrue;
    MFATAL("RBC", "System Failure: MEM ALLOCATION FAILED- PC=%zu", base->PC);
    return;
  }
  for (msize_t i = 0; i < len; i++) {
    temp[i] = getchar();
  }
  if (rbc_memory_write_bulk(base->dram, addr, len, temp) !=
      RBC_MEM_OPER_SUCCESS) {
    *kcore = mtrue;
    MFATAL("RBC",
           "MEM WRITE Failed(Blk[len=%zu, from=%zu]) likely because of invalid "
           "access: PC=%zu",
           len, addr, base->PC);
  }
  free(temp);
}

rbc_ihdlrX(sout) {
  register mqword_t len = base->REGISTER_FILE[RBC_R0];
  register mqword_t addr = instruction & 0xFFFFFFFFFFFF;
  if (!len)
    return;
  mbptr_t temp = (mbptr_t)calloc(len + 1, 1);
  if (temp == NULL) {
    *kcore = mtrue;
    MFATAL("RBC", "System Failure: MEM ALLOCATION FAILED- PC=%zu", base->PC);
    return;
  }
  if (rbc_memory_read_bulk(base->dram, addr, len, temp) !=
      RBC_MEM_OPER_SUCCESS) {
    *kcore = mtrue;
    MFATAL("RBC",
           "MEM READ Failed(Blk[len=%zu, from=%zu]) likely because of invalid "
           "access: PC=%zu",
           len, addr, base->PC);
    free(temp);
    return;
  }
  printf("%s", temp);
  free(temp);
}

rbc_ihdlrX(loadb) {
  register mbyte_t op1 = (instruction >> 48) & RBC_R15;
  mbyte_t imm;
  if (rbc_memory_read_byte(base->dram, instruction & 0xFFFFFFFFFFFF, &imm) !=
      RBC_MEM_OPER_SUCCESS) {
    *kcore = mtrue;
    MFATAL("RBC",
           "MEM READ Failed likely because of invalid "
           "access: ADDR=%zu, PC=%zu",
           instruction & 0xFFFFFFFFFFFF, base->PC);
    return;
  }
  base->REGISTER_FILE[op1] = imm;
}

rbc_ihdlrX(storeb) {
  register mbyte_t op1 = (instruction >> 48) & RBC_R15;
  if (rbc_memory_write_byte(base->dram, instruction & 0xFFFFFFFFFFFF,
                            base->REGISTER_FILE[op1] & 0xFF) !=
      RBC_MEM_OPER_SUCCESS) {
    *kcore = mtrue;
    MFATAL("RBC",
           "MEM WRITE Failed likely because of invalid "
           "access: ADDR=%zu PC=%zu",
           instruction & 0xFFFFFFFFFFFF, base->PC);
    return;
  }
}

rbc_ihdlrX(loadw) {
  register mbyte_t op1 = (instruction >> 48) & RBC_R15;
  mword_t imm;
  if (rbc_memory_read_word(base->dram, instruction & 0xFFFFFFFFFFFF, &imm) !=
      RBC_MEM_OPER_SUCCESS) {
    *kcore = mtrue;
    MFATAL("RBC",
           "MEM READ Failed likely because of invalid "
           "access: ADDR=%zu PC=%zu",
           instruction & 0xFFFFFFFFFFFF, base->PC);
    return;
  }
  base->REGISTER_FILE[op1] = imm;
}

rbc_ihdlrX(storew) {
  register mbyte_t op1 = (instruction >> 48) & RBC_R15;
  if (rbc_memory_write_word(base->dram, instruction & 0xFFFFFFFFFFFF,
                            base->REGISTER_FILE[op1] & 0xFFFF) !=
      RBC_MEM_OPER_SUCCESS) {
    *kcore = mtrue;
    MFATAL("RBC",
           "MEM WRITE Failed likely because of invalid "
           "access: ADDR=%zu PC=%zu",
           instruction & 0xFFFFFFFFFFFF, base->PC);
    return;
  }
}

rbc_ihdlrX(loadd) {
  register mbyte_t op1 = (instruction >> 48) & RBC_R15;
  mdword_t imm;
  if (rbc_memory_read_dword(base->dram, instruction & 0xFFFFFFFFFFFF, &imm) !=
      RBC_MEM_OPER_SUCCESS) {
    *kcore = mtrue;
    MFATAL("RBC",
           "MEM READ Failed likely because of invalid "
           "access: ADDR=%zu PC=%zu",
           instruction & 0xFFFFFFFFFFFF, base->PC);
    return;
  }
  base->REGISTER_FILE[op1] = imm;
}

rbc_ihdlrX(stored) {
  register mbyte_t op1 = (instruction >> 48) & RBC_R15;
  if (rbc_memory_write_dword(base->dram, instruction & 0xFFFFFFFFFFFF,
                             base->REGISTER_FILE[op1] & 0xFFFFFFFF) !=
      RBC_MEM_OPER_SUCCESS) {
    *kcore = mtrue;
    MFATAL("RBC",
           "MEM WRITE Failed likely because of invalid "
           "access: ADDR=%zu PC=%zu",
           instruction & 0xFFFFFFFFFFFF, base->PC);
    return;
  }
}

rbc_ihdlrX(loadq) {
  register mbyte_t op1 = (instruction >> 48) & RBC_R15;
  mqword_t imm;
  if (rbc_memory_read_qword(base->dram, instruction & 0xFFFFFFFFFFFF, &imm) !=
      RBC_MEM_OPER_SUCCESS) {
    *kcore = mtrue;
    MFATAL("RBC",
           "MEM READ Failed likely because of invalid "
           "access: ADDR=%zu PC=%zu",
           instruction & 0xFFFFFFFFFFFF, base->PC);
    return;
  }
  base->REGISTER_FILE[op1] = imm;
}

rbc_ihdlrX(storeq) {
  register mbyte_t op1 = (instruction >> 48) & RBC_R15;
  if (rbc_memory_write_qword(base->dram, instruction & 0xFFFFFFFFFFFF,
                             base->REGISTER_FILE[op1]) !=
      RBC_MEM_OPER_SUCCESS) {
    *kcore = mtrue;
    MFATAL("RBC",
           "MEM WRITE Failed likely because of invalid "
           "access: ADDR=%zu PC=%zu",
           instruction & 0xFFFFFFFFFFFF, base->PC);
    return;
  }
}

rbc_ihdlrX(loadb_reg) {
  register mbyte_t op1 = (instruction >> 4) & RBC_R15;
  register mbyte_t op2 = instruction & RBC_R15;
  mbyte_t imm;
  if (rbc_memory_read_byte(base->dram, base->REGISTER_FILE[op2], &imm) !=
      RBC_MEM_OPER_SUCCESS) {
    *kcore = mtrue;
    MFATAL("RBC",
           "MEM READ Failed likely because of invalid "
           "access: ADDR=%zu PC=%zu",
           instruction & 0xFFFFFFFFFFFF, base->PC);
    return;
  }
  base->REGISTER_FILE[op1] = imm;
}

rbc_ihdlrX(storeb_reg) {
  register mbyte_t op1 = (instruction >> 4) & RBC_R15;
  register mbyte_t op2 = instruction & RBC_R15;
  if (rbc_memory_write_byte(base->dram, base->REGISTER_FILE[op2],
                            base->REGISTER_FILE[op1] & 0xFF) !=
      RBC_MEM_OPER_SUCCESS) {
    *kcore = mtrue;
    MFATAL("RBC",
           "MEM WRITE Failed likely because of invalid "
           "access: ADDR=%zu PC=%zu",
           instruction & 0xFFFFFFFFFFFF, base->PC);
    return;
  }
}

rbc_ihdlrX(loadw_reg) {
  register mbyte_t op1 = (instruction >> 4) & RBC_R15;
  register mbyte_t op2 = instruction & RBC_R15;
  mword_t imm;
  if (rbc_memory_read_word(base->dram, base->REGISTER_FILE[op2], &imm) !=
      RBC_MEM_OPER_SUCCESS) {
    *kcore = mtrue;
    MFATAL("RBC",
           "MEM READ Failed likely because of invalid "
           "access: ADDR=%zu PC=%zu",
           instruction & 0xFFFFFFFFFFFF, base->PC);
    return;
  }
  base->REGISTER_FILE[op1] = imm;
}

rbc_ihdlrX(storew_reg) {
  register mbyte_t op1 = (instruction >> 4) & RBC_R15;
  register mbyte_t op2 = instruction & RBC_R15;
  if (rbc_memory_write_word(base->dram, base->REGISTER_FILE[op2],
                            base->REGISTER_FILE[op1] & 0xFFFF) !=
      RBC_MEM_OPER_SUCCESS) {
    *kcore = mtrue;
    MFATAL("RBC",
           "MEM WRITE Failed likely because of invalid "
           "access: ADDR=%zu PC=%zu",
           instruction & 0xFFFFFFFFFFFF, base->PC);
    return;
  }
}

rbc_ihdlrX(loadd_reg) {
  register mbyte_t op1 = (instruction >> 4) & RBC_R15;
  register mbyte_t op2 = instruction & RBC_R15;
  mdword_t imm;
  if (rbc_memory_read_dword(base->dram, base->REGISTER_FILE[op2], &imm) !=
      RBC_MEM_OPER_SUCCESS) {
    *kcore = mtrue;
    MFATAL("RBC",
           "MEM READ Failed likely because of invalid "
           "access: ADDR=%zu PC=%zu",
           instruction & 0xFFFFFFFFFFFF, base->PC);
    return;
  }
  base->REGISTER_FILE[op1] = imm;
}

rbc_ihdlrX(stored_reg) {
  register mbyte_t op1 = (instruction >> 4) & RBC_R15;
  register mbyte_t op2 = instruction & RBC_R15;
  if (rbc_memory_write_dword(base->dram, base->REGISTER_FILE[op2],
                             base->REGISTER_FILE[op1] & 0xFFFFFF) !=
      RBC_MEM_OPER_SUCCESS) {
    *kcore = mtrue;
    MFATAL("RBC",
           "MEM WRITE Failed likely because of invalid "
           "access: ADDR=%zu PC=%zu",
           instruction & 0xFFFFFFFFFFFF, base->PC);
    return;
  }
}

rbc_ihdlrX(loadq_reg) {
  register mbyte_t op1 = (instruction >> 4) & RBC_R15;
  register mbyte_t op2 = instruction & RBC_R15;
  mqword_t imm;
  if (rbc_memory_read_qword(base->dram, base->REGISTER_FILE[op2], &imm) !=
      RBC_MEM_OPER_SUCCESS) {
    *kcore = mtrue;
    MFATAL("RBC",
           "MEM READ Failed likely because of invalid "
           "access: ADDR=%zu PC=%zu",
           instruction & 0xFFFFFFFFFFFF, base->PC);
    return;
  }
  base->REGISTER_FILE[op1] = imm;
}

rbc_ihdlrX(storeq_reg) {
  register mbyte_t op1 = (instruction >> 4) & RBC_R15;
  register mbyte_t op2 = instruction & RBC_R15;
  if (rbc_memory_write_qword(base->dram, base->REGISTER_FILE[op2],
                             base->REGISTER_FILE[op1]) !=
      RBC_MEM_OPER_SUCCESS) {
    *kcore = mtrue;
    MFATAL("RBC",
           "MEM WRITE Failed likely because of invalid "
           "access: ADDR=%zu PC=%zu",
           instruction & 0xFFFFFFFFFFFF, base->PC);
    return;
  }
}
rbc_ihdlrX(atm_loadb) {
  register mbyte_t op1 = (instruction >> 48) & RBC_R15;
  mbyte_t imm;
  if (rbc_memory_read_byte_atm(base->dram, instruction & 0xFFFFFFFFFFFF,
                               &imm) != RBC_MEM_OPER_SUCCESS) {
    *kcore = mtrue;
    MFATAL("RBC",
           "MEM READ Failed likely because of invalid "
           "access: ADDR=%zu, PC=%zu",
           instruction & 0xFFFFFFFFFFFF, base->PC);
    return;
  }
  base->REGISTER_FILE[op1] = imm;
}

rbc_ihdlrX(atm_storeb) {
  register mbyte_t op1 = (instruction >> 48) & RBC_R15;
  if (rbc_memory_write_byte_atm(base->dram, instruction & 0xFFFFFFFFFFFF,
                                base->REGISTER_FILE[op1] & 0xFF) !=
      RBC_MEM_OPER_SUCCESS) {
    *kcore = mtrue;
    MFATAL("RBC",
           "MEM WRITE Failed likely because of invalid "
           "access: ADDR=%zu PC=%zu",
           instruction & 0xFFFFFFFFFFFF, base->PC);
    return;
  }
}

rbc_ihdlrX(atm_loadw) {
  register mbyte_t op1 = (instruction >> 48) & RBC_R15;
  mword_t imm;
  if (rbc_memory_read_word_atm(base->dram, instruction & 0xFFFFFFFFFFFF,
                               &imm) != RBC_MEM_OPER_SUCCESS) {
    *kcore = mtrue;
    MFATAL("RBC",
           "MEM READ Failed likely because of invalid "
           "access: ADDR=%zu PC=%zu",
           instruction & 0xFFFFFFFFFFFF, base->PC);
    return;
  }
  base->REGISTER_FILE[op1] = imm;
}

rbc_ihdlrX(atm_storew) {
  register mbyte_t op1 = (instruction >> 48) & RBC_R15;
  if (rbc_memory_write_word_atm(base->dram, instruction & 0xFFFFFFFFFFFF,
                                base->REGISTER_FILE[op1] & 0xFFFF) !=
      RBC_MEM_OPER_SUCCESS) {
    *kcore = mtrue;
    MFATAL("RBC",
           "MEM WRITE Failed likely because of invalid "
           "access: ADDR=%zu PC=%zu",
           instruction & 0xFFFFFFFFFFFF, base->PC);
    return;
  }
}

rbc_ihdlrX(atm_loadd) {
  register mbyte_t op1 = (instruction >> 48) & RBC_R15;
  mdword_t imm;
  if (rbc_memory_read_dword_atm(base->dram, instruction & 0xFFFFFFFFFFFF,
                                &imm) != RBC_MEM_OPER_SUCCESS) {
    *kcore = mtrue;
    MFATAL("RBC",
           "MEM READ Failed likely because of invalid "
           "access: ADDR=%zu PC=%zu",
           instruction & 0xFFFFFFFFFFFF, base->PC);
    return;
  }
  base->REGISTER_FILE[op1] = imm;
}

rbc_ihdlrX(atm_stored) {
  register mbyte_t op1 = (instruction >> 48) & RBC_R15;
  if (rbc_memory_write_dword_atm(base->dram, instruction & 0xFFFFFFFFFFFF,
                                 base->REGISTER_FILE[op1] & 0xFFFFFFFF) !=
      RBC_MEM_OPER_SUCCESS) {
    *kcore = mtrue;
    MFATAL("RBC",
           "MEM WRITE Failed likely because of invalid "
           "access: ADDR=%zu PC=%zu",
           instruction & 0xFFFFFFFFFFFF, base->PC);
    return;
  }
}

rbc_ihdlrX(atm_loadq) {
  register mbyte_t op1 = (instruction >> 48) & RBC_R15;
  mqword_t imm;
  if (rbc_memory_read_qword_atm(base->dram, instruction & 0xFFFFFFFFFFFF,
                                &imm) != RBC_MEM_OPER_SUCCESS) {
    *kcore = mtrue;
    MFATAL("RBC",
           "MEM READ Failed likely because of invalid "
           "access: ADDR=%zu PC=%zu",
           instruction & 0xFFFFFFFFFFFF, base->PC);
    return;
  }
  base->REGISTER_FILE[op1] = imm;
}

rbc_ihdlrX(atm_storeq) {
  register mbyte_t op1 = (instruction >> 48) & RBC_R15;
  if (rbc_memory_write_qword_atm(base->dram, instruction & 0xFFFFFFFFFFFF,
                                 base->REGISTER_FILE[op1]) !=
      RBC_MEM_OPER_SUCCESS) {
    *kcore = mtrue;
    MFATAL("RBC",
           "MEM WRITE Failed likely because of invalid "
           "access: ADDR=%zu PC=%zu",
           instruction & 0xFFFFFFFFFFFF, base->PC);
    return;
  }
}

rbc_ihdlrX(atm_loadb_reg) {
  register mbyte_t op1 = (instruction >> 4) & RBC_R15;
  register mbyte_t op2 = instruction & RBC_R15;
  mbyte_t imm;
  if (rbc_memory_read_byte_atm(base->dram, base->REGISTER_FILE[op2], &imm) !=
      RBC_MEM_OPER_SUCCESS) {
    *kcore = mtrue;
    MFATAL("RBC",
           "MEM READ Failed likely because of invalid "
           "access: ADDR=%zu PC=%zu",
           instruction & 0xFFFFFFFFFFFF, base->PC);
    return;
  }
  base->REGISTER_FILE[op1] = imm;
}

rbc_ihdlrX(atm_storeb_reg) {
  register mbyte_t op1 = (instruction >> 4) & RBC_R15;
  register mbyte_t op2 = instruction & RBC_R15;
  if (rbc_memory_write_byte_atm(base->dram, base->REGISTER_FILE[op2],
                                base->REGISTER_FILE[op1] & 0xFF) !=
      RBC_MEM_OPER_SUCCESS) {
    *kcore = mtrue;
    MFATAL("RBC",
           "MEM WRITE Failed likely because of invalid "
           "access: ADDR=%zu PC=%zu",
           instruction & 0xFFFFFFFFFFFF, base->PC);
    return;
  }
}

rbc_ihdlrX(atm_loadw_reg) {
  register mbyte_t op1 = (instruction >> 4) & RBC_R15;
  register mbyte_t op2 = instruction & RBC_R15;
  mword_t imm;
  if (rbc_memory_read_word_atm(base->dram, base->REGISTER_FILE[op2], &imm) !=
      RBC_MEM_OPER_SUCCESS) {
    *kcore = mtrue;
    MFATAL("RBC",
           "MEM READ Failed likely because of invalid "
           "access: ADDR=%zu PC=%zu",
           instruction & 0xFFFFFFFFFFFF, base->PC);
    return;
  }
  base->REGISTER_FILE[op1] = imm;
}

rbc_ihdlrX(atm_storew_reg) {
  register mbyte_t op1 = (instruction >> 4) & RBC_R15;
  register mbyte_t op2 = instruction & RBC_R15;
  if (rbc_memory_write_word_atm(base->dram, base->REGISTER_FILE[op2],
                                base->REGISTER_FILE[op1] & 0xFFFF) !=
      RBC_MEM_OPER_SUCCESS) {
    *kcore = mtrue;
    MFATAL("RBC",
           "MEM WRITE Failed likely because of invalid "
           "access: ADDR=%zu PC=%zu",
           instruction & 0xFFFFFFFFFFFF, base->PC);
    return;
  }
}

rbc_ihdlrX(atm_loadd_reg) {
  register mbyte_t op1 = (instruction >> 4) & RBC_R15;
  register mbyte_t op2 = instruction & RBC_R15;
  mdword_t imm;
  if (rbc_memory_read_dword_atm(base->dram, base->REGISTER_FILE[op2], &imm) !=
      RBC_MEM_OPER_SUCCESS) {
    *kcore = mtrue;
    MFATAL("RBC",
           "MEM READ Failed likely because of invalid "
           "access: ADDR=%zu PC=%zu",
           instruction & 0xFFFFFFFFFFFF, base->PC);
    return;
  }
  base->REGISTER_FILE[op1] = imm;
}

rbc_ihdlrX(atm_stored_reg) {
  register mbyte_t op1 = (instruction >> 4) & RBC_R15;
  register mbyte_t op2 = instruction & RBC_R15;
  if (rbc_memory_write_dword_atm(base->dram, base->REGISTER_FILE[op2],
                                 base->REGISTER_FILE[op1] & 0xFFFFFF) !=
      RBC_MEM_OPER_SUCCESS) {
    *kcore = mtrue;
    MFATAL("RBC",
           "MEM WRITE Failed likely because of invalid "
           "access: ADDR=%zu PC=%zu",
           instruction & 0xFFFFFFFFFFFF, base->PC);
    return;
  }
}

rbc_ihdlrX(atm_loadq_reg) {
  register mbyte_t op1 = (instruction >> 4) & RBC_R15;
  register mbyte_t op2 = instruction & RBC_R15;
  mqword_t imm;
  if (rbc_memory_read_qword_atm(base->dram, base->REGISTER_FILE[op2], &imm) !=
      RBC_MEM_OPER_SUCCESS) {
    *kcore = mtrue;
    MFATAL("RBC",
           "MEM READ Failed likely because of invalid "
           "access: ADDR=%zu PC=%zu",
           instruction & 0xFFFFFFFFFFFF, base->PC);
    return;
  }
  base->REGISTER_FILE[op1] = imm;
}

rbc_ihdlrX(atm_storeq_reg) {
  register mbyte_t op1 = (instruction >> 4) & RBC_R15;
  register mbyte_t op2 = instruction & RBC_R15;
  if (rbc_memory_write_qword_atm(base->dram, base->REGISTER_FILE[op2],
                                 base->REGISTER_FILE[op1]) !=
      RBC_MEM_OPER_SUCCESS) {
    *kcore = mtrue;
    MFATAL("RBC",
           "MEM WRITE Failed likely because of invalid "
           "access: ADDR=%zu PC=%zu",
           instruction & 0xFFFFFFFFFFFF, base->PC);
    return;
  }
}

rbc_ihdlrX(cmpxchg) {
  register mbyte_t desired = (instruction >> 8) & 0xFF;
  register mbyte_t expected = instruction & 0xFF;
  mqword_t address;
  base->PC += 8;
  if (rbc_memory_read_qword(base->iram, base->PC, &address) == RET_FAILURE) {
    *kcore = mtrue;
    MFATAL("RBC",
           "MEM READ Failed likely because of invalid "
           "access: PC=%zu",
           base->PC);
    return;
  }
  mret_t ret = rbc_memory_cmpxchg(base->dram, address, desired, expected);
  base->flags.regr = merry_obtain_flags_regr();
  if (ret != RBC_MEM_OPER_SUCCESS) {
    *kcore = mtrue;
    MFATAL("RBC",
           "MEM READ Failed likely because of invalid "
           "access: PC=%zu",
           base->PC);
    return;
  }
}

rbc_ihdlrX(cmpxchg_reg) {
  register mbyte_t desired = (instruction >> 8) & 0xFF;
  register mbyte_t expected = instruction & 0xFF;
  mqword_t address = base->REGISTER_FILE[(instruction >> 16) & RBC_R15];
  mret_t ret = rbc_memory_cmpxchg(base->dram, address, desired, expected);
  base->flags.regr = merry_obtain_flags_regr();
  if (ret != RBC_MEM_OPER_SUCCESS) {
    *kcore = mtrue;
    MFATAL("RBC",
           "MEM READ Failed likely because of invalid "
           "access: PC=%zu",
           base->PC);
    return;
  }
}
