#include <test_core/tc/tc.h>

void tc_TEST_1(TC *tc) {
  _MERRY_INTERNAL_ msize_t count = 0;
  MLOG("TC", "TC RUNNING: count=%zu", count);
}

void tc_TEST_2(TC *tc) {
  // this actually kills the core
  tc->base->interrupt = mtrue;
  tc->base->terminate = mtrue;
  MLOG("TC", "TC TERMINATING....", NULL);
}
