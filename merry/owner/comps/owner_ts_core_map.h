#ifndef _OWNER_TS_CORE_MAP_
#define _OWNER_TS_CORE_MAP_

#include <owner_platform.h>
#include <owner_logger.h>
#include <stdbool.h>
#include <stdlib.h>

typedef struct OwnerTSCoreMap OwnerTSCoreMap;
typedef struct OwnerTSCoreMapBucket OwnerTSCoreMapBucket;
typedef struct OwnerTSCoreMapResult OwnerTSCoreMapResult;

struct OwnerTSCoreMapResult {
  char value[33];
  socket_t sock;
};

struct OwnerTSCoreMapBucket {
  char key[65];
  size_t key_len;
  OwnerTSCoreMapResult result;
  OwnerTSCoreMapBucket *nxt_bucket;
};

struct OwnerTSCoreMap {
  OwnerTSCoreMapBucket *buckets;
  size_t bucket_count;
  mutex_t lock;
};

OwnerTSCoreMap* owner_ts_map_create(msize_t bucket_count);

bool owner_ts_map_insert(OwnerTSCoreMap *map, char* key, char* value, socket_t sock);

bool owner_ts_map_find(OwnerTSCoreMap *map, char* key, char *value,  OwnerTSCoreMapResult* res);

void owner_ts_map_destroy(OwnerTSCoreMap *map);

#endif
