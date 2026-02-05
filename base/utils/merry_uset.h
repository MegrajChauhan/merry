#ifndef _MERRY_USET_
#define _MERRY_USET_

#include <merry_results.h>
#include <merry_types.h>
#include <merry_utils.h>
#include <stdlib.h>

typedef struct MerryUSet MerryUSet;
typedef struct MerryUSetBucket MerryUSetBucket;

struct MerryUSetBucket {
  mptr_t key;
  MerryUSetBucket *nxt_bucket;
};

struct MerryUSet {
  MerryUSetBucket **buckets;
  size_t bucket_count;
  mhhfunc_t hash_func;
  mhkeycmpfunc_t cmp_func;
  mhcleanfunc_t key_clean_func; // cleanup function for the key
};

mresult_t merry_uset_create(MerryUSet **uset, msize_t bucket_count,
                            mhhfunc_t hash_func, mhkeycmpfunc_t cmp_func,
                            mhcleanfunc_t kcf);

mresult_t merry_uset_insert(MerryUSet *set, mptr_t key);

mresult_t merry_uset_contains(MerryUSet *set, mptr_t key);

void merry_uset_destroy(MerryUSet *set);

#endif
