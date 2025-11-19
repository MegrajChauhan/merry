#ifndef _RBC_MEM_DEFS_
#define _RBC_MEM_DEFS_

#define _RBC_PAGE_LEN_IN_BYTES_ 32768 // 32KB
#define _RBC_STACK_LEN_ 1048576       // 1MB
#define _RBC_CALL_DEPTH_ 50

typedef enum rbcmemOperRes_t rbcmemOperRes_t;

enum rbcmemOperRes_t {
  RBC_MEM_OPER_SUCCESS,
  RBC_MEM_OPER_ACCESS_INVALID,
};

#endif
