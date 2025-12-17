#ifndef _USE_HARDWARE_FLAGS_
#define _USE_HARDWARE_FLAGS_

#include <use_types.h>
#include <use_utils.h>

extern qword_t obtain_flags_regr() _ALIAS_(merry_obtain_flags_regr);
extern void cmp_two_values(qword_t a, qword_t b)
    _ALIAS_(merry_compare_two_values);

#endif
