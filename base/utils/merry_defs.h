#ifndef _MERRY_DEFS_
#define _MERRY_DEFS_

#ifndef surelyT
#define surelyT(x)                                                             \
  __builtin_expect(                                                            \
      !!(x),                                                                   \
      1) // tell the compiler that the expression x is most likely to be true
#define surelyF(x)                                                             \
  __builtin_expect(                                                            \
      !!(x),                                                                   \
      0) // tell the compiler that the expression x is most likely to be false
#endif

#if defined(_MERRY_OPTIMIZE_)
#define _MERRY_ALWAYS_INLINE_ inline __attribute__((always_inline))
#else
#define _MERRY_ALWAYS_INLINE_ static inline
#endif

#define _MERRY_NO_DISCARD_ __attribute__((nodiscard))
#define _MERRY_NO_THROW_ __attribute__((no_throw))
#define _MERRY_NO_RETURN_ __attribute__((no_return))
#define _MERRY_NO_NULL_ __attribute__((nonnull))
#define _MERRY_ALIAS_(name) __attribute__((alias(#name)))
#define _MERRY_CONSTRUCTOR_ __attribute__((constructor))

#define _MERRY_INTERNAL_                                                       \
  static // for a variable or a function that is localized to a module only
#define _MERRY_LOCAL_ static // any static variable inside a function
#define _MERRY_EXTERNAL_ extern

#endif
