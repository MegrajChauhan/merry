#include "merry_umap.h"

_MERRY_INTERNAL_ MerryUmapBucket *merry_find_bucket(MerryUmap *map, mptr_t key,
                                                    size_t *bucket_index) {
  *bucket_index = map->hash_func(key, map->bucket_count);
  MerryUmapBucket *bucket = &map->buckets[*bucket_index];

  while (bucket && bucket->key) {
    if (map->cmp_func(bucket->key, key)) {
      return bucket;
    }
    bucket = bucket->nxt_bucket;
  }

  return NULL;
}

mresult_t merry_umap_create(MerryUmap **map, size_t bucket_count,
                            mhash_func_t hash_func, mkeycmp_func_t cmp_func,
                            mcleanup_func_t kcf, mcleanup_func_t vcf) {
  if (!map || !hash_func || !cmp_func || !kcf || !vcf)
    return MRES_INVALID_ARGS;
  MerryUmap *m = (MerryUmap *)malloc(sizeof(MerryUmap));
  if (!m)
    return MRES_SYS_FAILURE;

  m->buckets = (MerryUmapBucket *)calloc(bucket_count, sizeof(MerryUmapBucket));
  if (!m->buckets) {
    free(m);
    return MRES_SYS_FAILURE;
  }
  for (size_t i = 0; i < bucket_count; i++) {
    m->buckets[i].key = NULL;
  }
  m->bucket_count = bucket_count;
  m->hash_func = hash_func;
  m->cmp_func = cmp_func;
  m->key_clean_func = kcf;
  m->value_clean_func = vcf;
  *map = m;
  return MRES_SUCCESS;
}

mresult_t merry_umap_insert(MerryUmap *map, mptr_t key, mptr_t value) {
  if (!map || !key || !value)
    return MRES_INVALID_ARGS;

  size_t bucket_index;
  MerryUmapBucket *bucket = merry_find_bucket(map, key, &bucket_index);

  if (bucket) {
    map->value_clean_func(bucket->value);
    bucket->value = value;
    return MRES_SUCCESS;
  }

  if (map->buckets[bucket_index].key) {
    MerryUmapBucket *new_bucket =
        (MerryUmapBucket *)malloc(sizeof(MerryUmapBucket));
    if (!new_bucket)
      return MRES_FAILURE;
    new_bucket->key = key;
    new_bucket->value = value;
    new_bucket->nxt_bucket = NULL;
    map->buckets[bucket_index].nxt_bucket = new_bucket;
  } else {
    map->buckets[bucket_index].key = key;
    map->buckets[bucket_index].value = value;
    map->buckets[bucket_index].nxt_bucket = NULL;
  }
  return MRES_SUCCESS;
}

mresult_t merry_umap_find(MerryUmap *map, mptr_t key, mptr_t *res) {
  if (!map || !key || !res)
    return MRES_INVALID_ARGS;

  size_t bucket_index;
  MerryUmapBucket *bucket = merry_find_bucket(map, key, &bucket_index);

  *res = bucket ? bucket->value : NULL;
  return MRES_SUCCESS;
}

mresult_t merry_umap_destroy(MerryUmap *map) {
  if (!map)
    return MRES_INVALID_ARGS;

  for (size_t i = 0; i < map->bucket_count; i++) {
    MerryUmapBucket *bucket = map->buckets[i].nxt_bucket;

    while (bucket) {
      MerryUmapBucket *next = bucket->nxt_bucket;
      map->key_clean_func(bucket->key);
      map->value_clean_func(bucket->value);
      free(bucket);
      bucket = next;
    }
  }
  map->buckets = NULL;
  free(map->buckets);
  free(map);
  return MRES_SUCCESS;
}
