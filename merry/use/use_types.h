#ifndef _USE_TYPES_
#define _USE_TYPES_

// Use these instead of the 'm' prefix which won't look good with the prefix of
// other core types. The cores may define their own types but since Merry works
// with the types below, they are recommended to typedef the below instead of
// redefining their own types It is unavoidable to use macros with _MERRY_
// prefix but the useful ones will be documented

#include <merry_types.h>

#define TRUE mtrue
#define FALSE mfalse

#define RET_NULL (void *)0
#define RET_SUCCESS mtrue
#define RET_FAILURE mfalse

typedef mbool_t bool_t;

typedef mbyte_t byte_t;
typedef mword_t word_t;
typedef mdword_t dword_t;
typedef mqword_t qword_t;

typedef msbyte_t sbyte_t;
typedef msword_t sword_t;
typedef msdword_t sdword_t;
typedef msqword_t sqword_t;

// some extra definitions
// msize_t will remain as is since size_t will conflict with glibc
typedef maddress_t address_t;
typedef msbyte_t ret_t;

// define function pointers
#define _MERRY_DEFINE_FUNC_PTR_(ret, func_name, ...)                           \
  typedef ret (*func_name)(__VA_ARGS__);

// pointers for memory types
typedef mbptr_t bptr_t;
typedef mwptr_t wptr_t;
typedef mdptr_t dptr_t;
typedef mqptr_t qptr_t;

// redifinitions of C types
typedef mstr_t str_t;
typedef mcstr_t cstr_t;
typedef mptr_t ptr_t;

#endif
