#include <regr_core/internal/rbc_utils.h>

msize_t rbc_align(msize_t value, msize_t align_to) {
  if ((value % align_to) == 0)
    return value;
  return value + (align_to - (value % align_to));
}
