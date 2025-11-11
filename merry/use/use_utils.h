#ifndef _USE_UTILS_
#define _USE_UTILS_

#include <merry_utils.h>

#define stringfy(x) _stringify(x)
#define glue(x, y) _glued(x, y)
#define toggle(x) _toggle(x)
#define to_bool(x) !!(x) // convert x to bool

#define _ALWAYS_INLINE_ _MERRY_ALWAYS_INLINE_

#define _NO_DISCARD_ _MERRY_NO_DISCARD_
#define _NO_THROW_ _MERRY_NO_THROW_
#define _NO_RETURN_ _MERRY_NO_RETURN_
#define _NO_NULL_ _MERRY_NO_NULL_
#define _ALIAS_(name) _MERRY_ALIAS_((name))

#define _INTERNAL_VAR_ _MERRY_INTERNAL_
#define _LOCAL_VAR_ _MERRY_LOCAL_

#define Assert(cond) merry_assert(cond)
#define check_ptr(ptr) merry_check_ptr(ptr)

#endif
