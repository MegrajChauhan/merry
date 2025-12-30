#include <abs/soft/merry_flags_regr.h>
#include <use_hardware_flags.h>

_ALWAYS_INLINE_ qword_t obtain_flags_regr() {
  return merry_obtain_flags_regr();
}

_ALWAYS_INLINE_ void cmp_two_values(qword_t a, qword_t b) {
  return merry_compare_two_values(a, b);
}
