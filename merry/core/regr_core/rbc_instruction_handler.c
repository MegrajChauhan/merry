#include <regr_core/internal/rbc_instruction_handler.h>

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

rbc_ihdlrX(add) {
  // Break down the instrucion
}
