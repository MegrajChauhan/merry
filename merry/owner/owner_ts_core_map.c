#include "owner_ts_map.h"

static size_t string_hash(char* key, size_t bucket_count) {
  uint32_t c1 = 0xcc9e2d51;
  uint32_t c2 = 0x1b873593;
  uint32_t r1 = 15;
  uint32_t r2 = 13;
  uint32_t m = 5;
  uint32_t n = 0xe6546b64;
  uint32_t hash = 0;
  size_t len = strlen((const char *)key);

  const int32_t nblocks = len / 4;
  const uint32_t *blocks = (const uint32_t *)(key);
  const char* tail = (const char*)(key + nblocks * 4);

  uint32_t k;
  for (int32_t i = 0; i < nblocks; i++) {
    k = blocks[i];
    k *= c1;
    k = (k << r1) | (k >> (32 - r1));
    k *= c2;
    hash ^= k;
    hash = (hash << r2) | (hash >> (32 - r2));
    hash = hash * m + n;
  }

  k = 0;
  switch (len & 3) {
  case 3:
    k ^= tail[2] << 16;
  case 2:
    k ^= tail[1] << 8;
  case 1:
    k ^= tail[0];
    k *= c1;
    k = (k << r1) | (k >> (32 - r1));
    k *= c2;
    hash ^= k;
  }

  hash ^= len;
  hash ^= (hash >> 16);
  hash *= 0x85ebca6b;
  hash ^= (hash >> 13);
  hash *= 0xc2b2ae35;
  hash ^= (hash >> 16);

  return hash % bucket_count;
}

static OwnerTSCoreMapBucket *find_bucket(OwnerTSCoreMap *map, char* key, char *value,
                                                    size_t *bucket_index) {
  *bucket_index = string_hash(map, key);
  OwnerTSCoreMapBucket *bucket = &map->buckets[*bucket_index];

  while (bucket && bucket->key_len != 0) {
    if (strncmp(key, bucket->key, bucket->key_len) == 0 && strcmp(value,bucket->result.value) == 0) 
      return bucket;
    }
    bucket = bucket->nxt_bucket;
  }

  return NULL;
}

OwnerTSCoreMap* owner_ts_map_create(size_t bucket_count) {
  OwnerTSCoreMap *m = (OwnerTSCoreMap *)malloc(sizeof(OwnerTSCoreMap));
  if (!m) {
 	FATAL("Failed to allocate TSCoreMap", NULL);
 	return NULL; 	
  }
 
  m->buckets = (OwnerTSCoreMapBucket *)calloc(bucket_count, sizeof(OwnerTSCoreMapBucket));
  if (!m->buckets) {
    free(m);
    FATAL("Failed to allocate TSCoreMap buckets", NULL);
    return NULL;
  }
  
  for (size_t i = 0; i < bucket_count; i++) {
    m->buckets[i].key_len = 0;
  }

#ifdef _USE_LINUX_
  if (pthread_mutex_init(m->lock, NULL) != 0) 
#endif
 {
 	FATAL("Failed to obtain lock for TSCoreMap", NULL);
	free(m->buckets);
 	free(m);
 }
  
  m->bucket_count = bucket_count;
  return m;
}

bool owner_ts_map_insert(OwnerTSCoreMap *map, char* key, char* value, socket_t sock) {
#if defined(_USE_LINUX_)
  pthread_mutex_lock(&map->lock);
#endif
  size_t bucket_index;
  bool ret = false;
  OwnerTSCoreMapBucket *bucket = find_bucket(map, key, value, &bucket_index);

  if (bucket) {
    map->value_clean_func(bucket->value);
    bucket->value = value;
    goto __insert_end;
  }

  if (map->buckets[bucket_index].key_len != 0) {
    OwnerTSCoreMapBucket *new_bucket =
        (OwnerTSCoreMapBucket *)malloc(sizeof(OwnerTSCoreMapBucket));
    if (!new_bucket) {
    	FATAL("Couldn't insert new element to TSCoreMap with key=%s and value=%s", key, value);
    	goto __insert_end;
    }
    new_bucket->key = key;
    new_bucket->key_len = strlen(key);
    new_bucket->result.value = value;
    new_bucket->result.sock = sock;
    new_bucket->nxt_bucket = map->buckets[bucket_index].nxt_bucket;
    map->buckets[bucket_index].nxt_bucket = new_bucket;
  } else {
    map->buckets[bucket_index].key = key;
    map->buckets[bucket_index].key_len = strlen(key);
    map->buckets[bucket_index].result.value = value;
    map->buckets[bucket_index].result.sock = sock;
    map->buckets[bucket_index].nxt_bucket = NULL;
  }
  ret = true;
 __insert_end:
  #if defined(_USE_LINUX_)
    pthread_mutex_unlock(&map->lock);
  #endif
  return ret;
}

bool owner_ts_map_find(OwnerTSCoreMap *map, char* key, char *value, OwnerTSCoreMapResult *res) {
  if (!map || !key || !res)
    return false;

  size_t bucket_index;
  OwnerTSCoreMapBucket *bucket = find_bucket(map, key,value, &bucket_index);

  *res = bucket ? &bucket->result : NULL;
  return true;
}

void owner_ts_map_destroy(OwnerTSCoreMap *map) {
  if (!map)
    return;

  for (size_t i = 0; i < map->bucket_count; i++) {
    OwnerTSCoreMapBucket *bucket = map->buckets[i].nxt_bucket;

    while (bucket) {
      OwnerTSCoreMapBucket *next = bucket->nxt_bucket;
      free(bucket);
      bucket = next;
    }
  }
  map->buckets = NULL;
#ifdef _USE_LINUX_
    pthread_mutex_destroy(mutex);
#endif
  
  free(map->buckets);
  free(map);
}
