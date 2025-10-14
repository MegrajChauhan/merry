#include <test_core/tc/tc.h>

void tc_TEST_1(TC *tc) {
  _MERRY_INTERNAL_ msize_t count = 0;
  MLOG("TC[TEST_1]", "[ID=%zu, UID=%zu, GUID=%zu]TC RUNNING: count=%zu",
       tc->base->id, tc->base->uid, tc->base->guid, count);
  count++;
}

void tc_TEST_2(TC *tc) {
  // this actually kills the core
  tc->base->interrupt = mtrue;
  tc->base->terminate = mtrue;
  MLOG("TC[TEST_2]", "[ID=%zu, UID=%zu, GUID=%zu]TC TERMINATING....",
       tc->base->id, tc->base->uid, tc->base->guid);
  tc_make_request(tc, KILL_SELF);
}

void tc_TEST_3(TC *tc) {
  MLOG("TC[TEST_3]", "[ID=%zu, UID=%zu, GUID=%zu]INPUT OUTPUT....",
       tc->base->id, tc->base->uid, tc->base->guid);
  msize_t len;
  printf("ENTER LENGTH: ");
  scanf("%zu", &len);
  MLOG("TC[TEST_3]", "[ID=%zu, UID=%zu, GUID=%zu] You entered: %zu",
       tc->base->id, tc->base->uid, tc->base->guid, len);
}

void tc_TEST_4(TC *tc) {
  MLOG("TC[TEST_4]",
       "[ID=%zu, UID=%zu, GUID=%zu] NEW CORE CREATION(SAME GROUP)....",
       tc->base->id, tc->base->uid, tc->base->guid);
  MerryRequestArgs *args = &tc->args;
  args->create_core.new_core_type = __TEST_CORE;
  args->create_core.gid = tc->base->guid;
  args->create_core.new_group = mfalse;
  args->create_core.same_group = mtrue;
  args->create_core.st_addr = tc->PC + 1;
  tc_make_request(tc, CREATE_CORE);
  if (tc->req_failed) {
    MLOG("TC[TEST_4]", "[ID=%zu, UID=%zu, GUID=%zu] REQUEST FAILED TO REGISTER",
         tc->base->id, tc->base->uid, tc->base->guid);
    tc->req_failed = mfalse;
    return;
  }
  if (!tc->base->req_res) {
    MLOG("TC[TEST_4]", "[ID=%zu, UID=%zu, GUID=%zu] FAILED TO CREATE CORE",
         tc->base->id, tc->base->uid, tc->base->guid);
    return;
  }
  MLOG(
      "TC[TEST_4]",
      "[ID=%zu, UID=%zu, GUID=%zu] SPAWNED NEW CORE: ID=%zu, UID=%zu, GUID=%zu",
      tc->base->id, tc->base->uid, tc->base->guid, args->create_core.new_id,
      args->create_core.new_uid, args->create_core.gid);
}

void tc_TEST_5(TC *tc) {
  MLOG("TC[TEST_5]",
       "[ID=%zu, UID=%zu, GUID=%zu] NEW CORE CREATION(DIFFERENT GROUP) ",
       tc->base->id, tc->base->uid, tc->base->guid);
  MerryRequestArgs *args = &tc->args;
  args->create_core.new_core_type = __TEST_CORE;
  args->create_core.gid = tc->base->guid;
  args->create_core.new_group = mtrue;
  args->create_core.same_group = mfalse;
  args->create_core.st_addr = tc->PC + 1;
  tc_make_request(tc, CREATE_CORE);
  if (tc->req_failed) {
    MLOG("TC[TEST_5]", "[ID=%zu, UID=%zu, GUID=%zu] REQUEST FAILED TO REGISTER",
         tc->base->id, tc->base->uid, tc->base->guid);
    tc->req_failed = mfalse;
    return;
  }
  if (!tc->base->req_res) {
    MLOG("TC[TEST_5]", "[ID=%zu, UID=%zu, GUID=%zu] FAILED TO CREATE CORE",
         tc->base->id, tc->base->uid, tc->base->guid);
    return;
  }
  MLOG(
      "TC[TEST_5]",
      "[ID=%zu, UID=%zu, GUID=%zu] SPAWNED NEW CORE: ID=%zu, UID=%zu, GUID=%zu",
      tc->base->id, tc->base->uid, tc->base->guid, args->create_core.new_id,
      args->create_core.new_uid, args->create_core.gid);
}

void tc_TEST_6(TC *tc) {
  MLOG("TC[TEST_6]", "[ID=%zu, UID=%zu, GUID=%zu] CORE DETAILS", tc->base->id,
       tc->base->uid, tc->base->guid);
  MLOG("TC[TEST_6]", "[ID=%zu, UID=%zu, GUID=%zu] DETAILS: PC=%zu, ILEN=%zu",
       tc->base->id, tc->base->uid, tc->base->guid, tc->PC, tc->mem->len);
}

void tc_TEST_7(TC *tc) {
  MLOG("TC[TEST_7]", "[ID=%zu, UID=%zu, GUID=%zu] NEW GROUP CREATION",
       tc->base->id, tc->base->uid, tc->base->guid);
  MerryRequestArgs *args = &tc->args;
  tc_make_request(tc, CREATE_GROUP);
  if (tc->req_failed) {
    MLOG("TC[TEST_7]", "[ID=%zu, UID=%zu, GUID=%zu] REQUEST FAILED TO REGISTER",
         tc->base->id, tc->base->uid, tc->base->guid);
    tc->req_failed = mfalse;
    return;
  }
  if (!tc->base->req_res) {
    MLOG("TC[TEST_7]", "[ID=%zu, UID=%zu, GUID=%zu] FAILED TO CREATE GROUP",
         tc->base->id, tc->base->uid, tc->base->guid);
    return;
  }
  MLOG("TC[TEST_7]", "[ID=%zu, UID=%zu, GUID=%zu] CREATED NEW GROUP: GUID=%zu",
       tc->base->id, tc->base->uid, tc->base->guid,
       args->create_group.new_guid);
}

void tc_TEST_8(TC *tc) {
  MLOG("TC[TEST_8]", "[ID=%zu, UID=%zu, GUID=%zu] GETTING GROUP DETAILS",
       tc->base->id, tc->base->uid, tc->base->guid);
  MerryRequestArgs *args = &tc->args;
  args->get_group_details.guid = tc->base->guid;
  tc_make_request(tc, GET_GROUP_DETAILS);
  if (tc->req_failed) {
    MLOG("TC[TEST_8]", "[ID=%zu, UID=%zu, GUID=%zu] REQUEST FAILED TO REGISTER",
         tc->base->id, tc->base->uid, tc->base->guid);
    tc->req_failed = mfalse;
    return;
  }
  if (!tc->base->req_res) {
    MLOG("TC[TEST_8]",
         "[ID=%zu, UID=%zu, GUID=%zu] FAILED TO OBTAIN GROUP DETAILS",
         tc->base->id, tc->base->uid, tc->base->guid);
    return;
  }
  MLOG("TC[TEST_8]",
       "[ID=%zu, UID=%zu, GUID=%zu] GROUP DETAILS: core_count=%zu and "
       "active_core_count=%zu",
       tc->base->id, tc->base->uid, tc->base->guid,
       args->get_group_details.core_count,
       args->get_group_details.active_core_count);
}

void tc_TEST_9(TC *tc) {
  MLOG("TC[TEST_9]", "[ID=%zu, UID=%zu, GUID=%zu] GETTING SYSTEM DETAILS",
       tc->base->id, tc->base->uid, tc->base->guid);
  MerryRequestArgs *args = &tc->args;
  tc_make_request(tc, GET_SYSTEM_DETAILS);
  if (tc->req_failed) {
    MLOG("TC[TEST_9]", "[ID=%zu, UID=%zu, GUID=%zu] REQUEST FAILED TO REGISTER",
         tc->base->id, tc->base->uid, tc->base->guid);
    tc->req_failed = mfalse;
    return;
  }
  if (!tc->base->req_res) {
    MLOG("TC[TEST_9]",
         "[ID=%zu, UID=%zu, GUID=%zu] FAILED TO OBTAIN SYSTEM DETAILS",
         tc->base->id, tc->base->uid, tc->base->guid);
    return;
  }
  MLOG("TC[TEST_9]",
       "[ID=%zu, UID=%zu, GUID=%zu] SYSTEM DETAILS: group_count=%zu, "
       "core_count=%zu, and active_core_count=%zu",
       tc->base->id, tc->base->uid, tc->base->guid,
       args->get_system_details.grp_count, args->get_system_details.core_count,
       args->get_system_details.active_core_count);
}
