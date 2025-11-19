.intel_syntax noprefix
#include "merry_config.h"

// We believe that every machine that Merry runs on uses 2's complement for negative number representation and
// IEEE representation for floating point numbers

// mqword_t merry_obtain_flags_regr()
.global merry_obtain_flags_regr

// void merry_compare_two_values(mqword_t val1, mqword_t val2)
.global merry_compare_two_values

#if defined(_MERRY_HOST_CPU_x86_64_ARCH_)
merry_obtain_flags_regr:
   pushfq          // push the flags register unto the stack
   pop rax         // pop the flags
   ret             // return the flags value in RAX

#if defined(_USE_LINUX_)
merry_compare_two_values:
   cmp rdi, rsi
   mov rdi, rdx
   ret         // Now use the above to obtain the flags register
#endif

#if defined(_USE_WIN_)
merry_compare_two_values:
   cmp rcx, rdx
   mov rdi, r8
   ret
#endif

#endif
