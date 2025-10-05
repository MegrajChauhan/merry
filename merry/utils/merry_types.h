#ifndef _MERRY_TYPES_
#define _MERRY_TYPES_

#include <merry_config.h>

// boolean types
#define mtrue 1
#define mfalse 0

#define RET_NULL (void *)0
#define RET_SUCCESS mtrue
#define RET_FAILURE mfalse

typedef unsigned char mbool_t; // bool

// these are the memory data types
typedef unsigned char mbyte_t;
typedef unsigned short mword_t;
typedef unsigned int mdword_t;
typedef unsigned _MERRY_LONG_ mqword_t;

typedef char msbyte_t;
typedef short msword_t;
typedef int msdword_t;
typedef _MERRY_LONG_ msqword_t;

// some extra definitions
typedef unsigned _MERRY_LONG_ maddress_t;
typedef unsigned _MERRY_LONG_ msize_t;
typedef msize_t mid_t;   // ID of something
typedef msize_t muid_t;  // UID of something
typedef msize_t mguid_t; // Group ID of something
typedef msbyte_t mret_t;

// define function pointers
#define _MERRY_DEFINE_FUNC_PTR_(ret, func_name, ...)                           \
  typedef ret (*func_name)(__VA_ARGS__);

// pointers for memory types
typedef mbyte_t *mbptr_t;
typedef mword_t *mwptr_t;
typedef mdword_t *mdptr_t;
typedef mqword_t *mqptr_t;

// redifinitions of C types
typedef char *mstr_t;
typedef const char *mcstr_t;
typedef void *mptr_t;

typedef msize_t merrot_t;

#if defined(_USE_LINUX_)
#define _THRET_T_ mptr_t
#elif defined(_USE_WIN_)
#define _THRET_T_ unsigned __stdcall
#endif

#endif
