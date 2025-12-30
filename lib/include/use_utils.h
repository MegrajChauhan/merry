#ifndef _USE_UTILS_
#define _USE_UTILS_

#define stringfy(x) #x
#define glue(x, y) x##y
#define toggle(x) (~x) >> 63
#define to_bool(x) !!(x) // convert x to bool

#define _ALWAYS_INLINE_ inline __attribute__((always_inline))

#define _NO_DISCARD_ __attribute__((nodiscard))
#define _NO_THROW_ __attribute__((no_throw))
#define _NO_RETURN_ __attribute__((no_return))
#define _NO_NULL_ __attribute__((nonnull))
#define _ALIAS_(name) __attribute__((alias(#name)))

#define _INTERNAL_VAR_ static
#define _LOCAL_VAR_ static
#define _PRIVATE_FUNC_ static

#endif
