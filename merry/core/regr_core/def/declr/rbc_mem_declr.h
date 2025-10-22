#ifndef _RBC_MEM_DECLR_
#define _RBC_MEM_DECLR_

#include <merry_types.h>

// RBC will use paged memory
typedef struct RBCMemPage RBCMemPage;

struct RBCMemPage {
  union {
    mbptr_t bytes;
    mwptr_t words;
    mdptr_t dwords;
    mqptr_t qwords;
  } repr;
  mbool_t pg_independant_map;
};

#endif
