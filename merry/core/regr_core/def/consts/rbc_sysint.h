#ifndef _RBC_SYSINT_
#define _RBC_SYSINT_

typedef enum rbcSysInt_t rbcSysInt_t;
typedef enum rbcSysIntRes_t rbcSysIntRes_t;

enum rbcSysInt_t {
  RBC_SI_EXIT, // Kill the core(Not Merry)
};

enum rbcSysIntRes_t {
  RBC_SIR_SUCCESS,
  RBC_SIR_UNKNOWN_TYPE,
};

#endif
