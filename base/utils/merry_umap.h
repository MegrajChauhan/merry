#ifndef _MERRY_UMAP_
#define _MERRY_UMAP_

#include <merry_types.h>
#include <merry_utils.h>
#include <merry_results.h>
#include <stdlib.h>

typedef struct MerryUMap MerryUMap;
typedef struct MerryUMapBucket MerryUMapBucket;

struct MerryUMapBucket {
    mptr_t value;
    mptr_t	key;
    MerryUMapBucket *nxt_bucket;
};

struct MerryUMap {
    MerryUMapBucket **buckets;
    msize_t bucket_count;
    mhhfunc_t hash_func;
    mhkeycmpfunc_t cmp_func;
    mhcleanfunc_t key_clean_func; // if the key needs to be cleaned
    mhcleanfunc_t value_clean_func; // if the value needs to be cleaned
};

mresult_t merry_umap_create(MerryUMap **umap,msize_t bucket_count, mhhfunc_t hash_func,mhkeycmpfunc_t kcmp, mhcleanfunc_t kcf, mhcleanfunc_t vcf);

mresult_t merry_umap_insert(MerryUMap *map, mptr_t key, mptr_t value);

mresult_t merry_umap_find(MerryUMap *map, mptr_t key, mptr_t *res);

void merry_umap_destroy(MerryUMap *map);

#endif
