#ifndef _MERRY_CORE_TYPES_
#define _MERRY_CORE_TYPES_

typedef enum mcore_t mcore_t;

enum mcore_t {
  __TEST_CORE,
  // We can have many more types here
  __REGR_CORE,
  __CORE_TYPE_COUNT,
};

#endif
