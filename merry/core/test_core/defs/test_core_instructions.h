#ifndef _TEST_CORE_INSTRUCTIONS_
#define _TEST_CORE_INSTRUCTIONS_

typedef enum tcinst_t tcinst_t;

/*
 * One instruction of test core represents one test
 * and hence the bizzare opcodes it has.
 * It really doesn't have anyother useful instructions
 * */
enum tcinst_t {
  TEST_1,
  TEST_2,
  TEST_3,
  TEST_4,
  TEST_5,
};

#endif
