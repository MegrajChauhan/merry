#include <merry_umap.h>

_MERRY_INTERNAL_ MerryUMapBucket *merry_find_bucket(MerryUMap* map, mptr_t key, msize_t *bucket_index) {
    *bucket_index = map->hash_func(key, map->bucket_count);
    MerryUMapBucket *bucket = &map->buckets[*bucket_index];

    while (bucket && bucket->key){
        if (map->cmp_func(bucket->key, key)) {
            return bucket;
        }
        bucket = bucket->nxt_bucket;
    }

    return NULL;
}

mresult_t merry_umap_create(MerryUMap **umap,msize_t bucket_count, mhhfunc_t hash_func,mhkeycmpfunc_t kcmp, mhcleanfunc_t kcf, mhcleanfunc_t vcf) {
    if (!umap || !hash_func || !key_clean_func || !value_clean_func || !cmp_func || bucket_count == 0) 
      return MRES_INVALID_ARGS;
    MerryUMap *map = (MerryUMap*)malloc(sizeof(umap));
    if (!map)
      return MRES_SYS_FAILURE;
    map->buckets = (MerryUMapBucket *)calloc(bucket_count, sizeof(MerryUMapBucket));
    if (!map->buckets) {
        free(map);
        return MRES_SYS_FAILURE;
    }
    for (msize_t i = 0; i < bucket_count; i++) {
        map->buckets[i].key = NULL;
    }
    map->bucket_count = bucket_count;
    map->hash_func = hash_func;
    map->cmp_func = cmp_func;
    map->key_clean_func = kcf;
    map->value_clean_func = vcf;
    *umap = map;
    return MRES_SUCCESS;
}

mresult_t merry_umap_insert(MerryUMap*map, mptr_t key, mptr_t value) {
    if (!map || !key || !value) return MRES_INVALID_ARGS;
    msize_t bucket_index;
    MerryUMapBucket *bucket = merry_find_bucket(map, key, &bucket_index);
    if (bucket) {
        // key already exists, update value
        bucket->value = value;
        return MRES_SUCCESS;
    }

    // insert new key-value pair
    if (map->buckets[bucket_index].key) {
        MerryUMapBucket *new_bucket = (MerryUMapBucket *)malloc(sizeof(MerryUMapBucket));
        if (!new_bucket)
            return MRES_SYS_FAILURE;
        new_bucket->key = key;
        new_bucket->value = value;
        new_bucket->nxt_bucket = map->buckets[bucket_index];
        map->buckets[bucket_index] = new_bucket;
    } else {
        map->buckets[bucket_index].key = key;
        map->buckets[bucket_index].value = value;
        map->buckets[bucket_index].nxt_bucket = NULL;
    }
    return MRES_SUCCESS;
}

// Find Value by Key
mresult_t merry_umap_find(MerryUMap *map, mptr_t key, mptr_t *res) {
    if (!map || !key || !res) return MRES_INVALID_ARGS;
    msize_t bucket_index;
    MerryUMapBucket *bucket = merry_find_bucket(map, key, &bucket_index);
    *res = bucket ? bucket->value : NULL;
    return MRES_SUCCESS;
}

void merry_umap_destroy(MerryUMap *map) {
    if (!map) return;
    for (msize_t i = 0; i < map->bucket_count; i++) {
        MerryUMapBucket *bucket = map->buckets[i].nxt_bucket;
        while (bucket) {
            MerryUMapBucket *next = bucket->nxt_bucket;
            if (map->key_clean_func)
                map->key_clean_func(bucket->key);
            if (map->value_clean_func)
                map->value_clean_func(bucket->value);
            free(bucket);
            bucket = next;
        }
    }
    map->buckets = NULL;
    free(map->buckets);
    free(map);
}
