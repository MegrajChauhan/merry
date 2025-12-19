#ifndef _USE_TYPES_
#define _USE_TYPES_

// Use these instead of the 'm' prefix which won't look good with the prefix of
// other core types. The cores may define their own types but since Merry works
// with the types below, they are recommended to typedef the below instead of
// redefining their own types It is unavoidable to use macros with _MERRY_
// prefix but the useful ones will be documented

#include <stddef.h>
#include <utils/merry_config.h>

#define TRUE 1
#define FALSE 0

typedef unsigned char bool_t; // bool
typedef _Atomic bool_t atm_bool_t;

// these are the memory data types
typedef unsigned char byte_t;
typedef unsigned short word_t;
typedef unsigned int dword_t;
typedef unsigned _MERRY_LONG_ qword_t;

typedef char sbyte_t;
typedef short sword_t;
typedef int sdword_t;
typedef _MERRY_LONG_ sqword_t;

// some extra definitions
typedef unsigned _MERRY_LONG_ address_t;
typedef size_t mid_t;   // ID of something
typedef size_t muid_t;  // UID of something
typedef size_t mguid_t; // Group ID of something
typedef sbyte_t ret_t;

typedef _Atomic size_t atm_size_t;

// define function pointers
#define _DEFINE_FUNC_PTR_(ret, func_name, ...) typedef ret (*func_name)(__VA_ARGS__)

// pointers for memory types
typedef byte_t *bptr_t;
typedef word_t *wptr_t;
typedef dword_t *dptr_t;
typedef qword_t *qptr_t;

// redifinitions of C types
typedef char *str_t;
typedef const char *cstr_t;
typedef void *ptr_t;

#if defined(_USE_LINUX_)
#define thread_ret_t ptr_t
#elif defined(_USE_WIN_)
#define thread_ret_t unsigned __stdcall
#endif

_DEFINE_FUNC_PTR_(thread_ret_t, thexec_t, ptr_t);

#endif
