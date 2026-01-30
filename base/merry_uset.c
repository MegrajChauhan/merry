#include <merry_uset.h>

_MERRY_INTERNAL_ MerryUSetBucket *merry_find_bucket(MerryUSet *set, mptr_t key, msize_t *bucket_index) {
    *bucket_index = set->hash_func(key, set->bucket_count);
    MerryUSetBucket *bucket = &set->buckets[*bucket_index];
    while (bucket && bucket->key) {
        if (set->cmp_func(bucket->key, key))
            return bucket;
        bucket = bucket->nxt_bucket;
    }
    return NULL;
}

mresult_t merry_uset_create(MerryUSet **uset, msize_t bucket_count, mhhfunc_t hash_func, mhkeycmpfunc_t cmp_func, mhcleanfunc_t kcf) {
    if (!uset || !hash_func || !key_clean_func || !cmp_func || bucket_count == 0) return MRES_INVALID_ARGS;
	MerryUSet *set = (MerryUSet *)malloc(sizeof(MerryUSet));
	if (!set)
	    return MRES_SYS_FAILURE;;

	set->buckets = (MerryUSetBucket *)calloc(bucket_count, sizeof(MerryUSetBucket));
	if (!set->buckets) {
	    free(set);
	    return MRES_SYS_FAILURE;
	}

	for (msize_t i = 0; i < bucket_count; i++) {
	    set->buckets[i].key = NULL;
	}
	set->bucket_count = bucket_count;
	set->hash_func = hash_func;
	set->cmp_func = cmp_func;
	set->key_clean_func = kcf;
	*uset = set;
	return MRES_SUCCESS;
}

mresult_t merry_uset_insert(MerryUSet *set, mptr_t key) {
    if (!set || !key) return MRES_INVALID_ARGS;
	msize_t bucket_index;
	MerryUSetBucket *bucket = merry_find_bucket(set, key, &bucket_index);

	if (bucket)
	    return MRES_SUCCESS;

	if (set->buckets[bucket_index].key) {
	    MerryUSetBucket *new_bucket = (MerryUSetBucket *)malloc(sizeof(MerryUSetBucket));
	    if (!new_bucket)
	    	return MRES_SYS_FAILURE;
	    new_bucket->key = key;
	    new_bucket->nxt_bucket = set->buckets[bucket_index];
	    set->buckets[bucket_index] = new_bucket;
	} else {
	    set->buckets[bucket_index].key = key;
	    set->buckets[bucket_index].nxt_bucket = NULL;
	}
	return MRES_SUCCESS;
}

mresult_t merry_uset_contains(MerryUSet *set, mptr_t key) {
    if (!set || !key) return MRES_INVALID_ARGS;
    msize_t bucket_index;
    MerryUSetBucket *bucket = merry_find_bucket(set, key, &bucket_index);

    return (bucket != NULL)? MRES_SUCCESS: MRES_FAILURE;
}

void merry_uset_destroy(MerryUSet *set) {
    if (!set) return;
    for (msize_t i = 0; i < set->bucket_count; i++) {
        MerryUSetBucket *bucket = set->buckets[i].nxt_bucket;
        while (bucket) {
            MerryUSetBucket *next = bucket->nxt_bucket;
            if (set->key_clean_func)
                set->key_clean_func(bucket->key);
            free(bucket);
            bucket = next;
        }
    }
    free(set->buckets);
    free(set);
}
