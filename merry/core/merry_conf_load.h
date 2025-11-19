#ifndef _MERRY_CONF_LOAD_
#define _MERRY_CONF_LOAD_

#include <merry_types.h>
#include <merry_utils.h>
#include <merry_umap.h>
#include <merry_logger.h>
#include <merry_operations.h>
#include <merry_hash.h>
#include <stdio.h>
#include <ctype.h>
#include <string.h>

typedef struct MerryConfLoad MerryConfLoad;

struct MerryConfLoad {
	FILE *_conf_file;
	MerryUmap *entries;
	msize_t entry_count;
};

mbool_t merry_conf_key_cmp(mptr_t k1, mptr_t k2);

mresult_t merry_conf_load_init(MerryConfLoad **conf);

mresult_t merry_conf_load_destroy(MerryConfLoad *conf);

mresult_t merry_conf_load_load(MerryConfLoad *conf, mstr_t path);

mresult_t merry_conf_load_find(MerryConfLoad *conf, mstr_t key, mstr_t* result);

#endif
