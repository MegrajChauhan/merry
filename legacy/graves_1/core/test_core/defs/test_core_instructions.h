#ifndef _TEST_CORE_INSTRUCTIONS_
#define _TEST_CORE_INSTRUCTIONS_

typedef enum tcinst_t tcinst_t;

/*
 * One instruction of test core represents one test
 * and hence the bizzare opcodes it has.
 * It really doesn't have anyother useful instructions
 * */
enum tcinst_t {
  TEST_1, // Just basic check
  TEST_2, // Exit
  TEST_3, // Input Output
  TEST_4, // Create new core(same group)
  TEST_5, // Create new core(different group)
  TEST_6, // Print self information
  TEST_7, // Create new group
  TEST_8, // Get group details
  TEST_9, // Get system details
};

#endif
