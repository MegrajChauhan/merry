#ifndef _MERRY_UMAP_
#define _MERRY_UMAP_

#include <merry_operations.h>
#include <merry_types.h>
#include <merry_utils.h>
#include <stdlib.h>

typedef struct MerryUmap MerryUmap;
typedef struct MerryUmapBucket MerryUmapBucket;

typedef void (*mcleanup_func_t)(mptr_t item);
typedef msize_t (*mhash_func_t)(mptr_t key, msize_t bucket_count);
typedef mbool_t (*mkeycmp_func_t)(mptr_t key1, mptr_t key2);

struct MerryUmapBucket {
  mptr_t value;
  mptr_t key;
  MerryUmapBucket *nxt_bucket;
};

struct MerryUmap {
  MerryUmapBucket *buckets;
  msize_t bucket_count;
  mhash_func_t hash_func;
  mkeycmp_func_t cmp_func;
  mcleanup_func_t key_clean_func;   // if the key needs to be cleaned
  mcleanup_func_t value_clean_func; // if the value needs to be cleaned
};

mresult_t merry_umap_create(MerryUmap **map, msize_t bucket_count,
                            mhash_func_t hash_func, mkeycmp_func_t cmp_func,
                            mcleanup_func_t kcf, mcleanup_func_t vcf);

mresult_t merry_umap_insert(MerryUmap *map, mptr_t key, mptr_t value);

mresult_t merry_umap_find(MerryUmap *map, mptr_t key, mptr_t *res);

mresult_t merry_umap_destroy(MerryUmap *map);

#endif
