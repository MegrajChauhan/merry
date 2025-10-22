#ifndef _RBC_MEM_DEFS_
#define _RBC_MEM_DEFS_

#define _RBC_PAGE_LEN_IN_BYTES_ 32768 // 32KB

typedef enum rbcmemOperRes_t rbcmemOperRes_t;

enum rbcmemOperRes_t {
  RBC_MEM_OPER_SUCCESS,
  RBC_MEM_OPER_ACCESS_INVALID,
};

#endif
