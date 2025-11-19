#include <merry_conf_load.h>

mresult_t merry_conf_load_init(MerryConfLoad **conf) {
	if (!conf)
		return MRES_INVALID_ARGS;

	MerryConfLoad *c = (MerryConfLoad*)malloc(sizeof(MerryConfLoad));
	if (!c)
		return MRES_SYS_FAILURE;

	c->_conf_file = NULL;
	c->entry_count = 0;
	c->entries = NULL;
	return MRES_SUCCESS;
}

mresult_t merry_conf_load_destroy(MerryConfLoad *conf) {
	if (!conf)
		return MRES_INVALID_ARGS;

	if (conf->entries)
		merry_umap_destroy(conf->entries);

	if (conf->_conf_file)
		fclose(conf->_conf_file);

	free(conf);
	return MRES_SUCCESS;
}

mbool_t merry_conf_key_cmp(mptr_t k1, mptr_t k2) {
   return strcmp(k1, k2) == 0? mtrue: mfalse;	
}

mresult_t merry_conf_load_load(MerryConfLoad *conf, mstr_t path) {
	if (!conf || !path)
		return MRES_INVALID_ARGS;

	// We need to parse the file
	// First prepare
	mresult_t res = merry_umap_create(&conf->entries, 10, merry_string_hash, merry_conf_key_cmp, free, free);
	if (res != MRES_SUCCESS)
		return res;

	if ((conf->_conf_file = fopen(path, "r")) == NULL)
		return MRES_SYS_FAILURE;

	mstr_t curr_str = NULL;
	mbool_t expect_path = mfalse;
	msize_t lnum = 1;
	msize_t off_curr = 0;
    int curr = fgetc(conf->_conf_file);
    
	while (!feof(conf->_conf_file)) {
		if (curr == '#') {
			while (curr != EOF && curr != '\n') {
				curr = fgetc(conf->_conf_file);
				off_curr++;
			}
			lnum++;
		} else if (isspace(curr)) {
			while (curr != EOF && isspace(curr)) {
				if (curr == '\n')
					lnum++;
				curr = fgetc(conf->_conf_file);
				off_curr++;
			}
		} else if (curr == '=') {
			if (!curr_str) {
				MFATAL("Config Loader", "Stray '=' at line number %zu", lnum);
				return MRES_NOT_MERRY_FAILURE;
			}
			if (expect_path == mtrue) {
				MFATAL("Config Loader", "Multiple '=' when expected a path at line number %zu", lnum);
				return MRES_NOT_MERRY_FAILURE;
			}
			expect_path = mtrue;
		} else {
			// must be either the name or the path
			// Since we are not checking for what characters are being used,
			// there is a lot of freedom here but checks will be made to ensure
			// that the path is valid
			if (curr_str) {
				if (expect_path) {
					// so this is a path
					// Everything from here is considered a part of the path
					// including spaces, newlines and weird characters
					// the path terminates with a '~' character
					msize_t st = off_curr;
					while (curr != EOF && curr != '~') {
					    if (curr == '\n')
					       lnum++;
						curr = fgetc(conf->_conf_file);
						off_curr++;
					}
					if (curr == EOF) {
						MFATAL("Config Loader", "Expected '~' to terminate the path but got EOF at line %zu", lnum);
						return MRES_NOT_MERRY_FAILURE;
					}
					// We have the full path
					msize_t ed = off_curr;
					mstr_t path = (mstr_t)calloc((ed - st) + 2, 1);
					if (!path) {
						MFATAL("Config Loader", "Memory allocation failed", NULL);
						return MRES_SYS_FAILURE;
					}
					fseek(conf->_conf_file, st, SEEK_SET);
					fread(path, 1, (ed - st) + 1, conf->_conf_file);
					// just store it
					res = merry_umap_insert(conf->entries, curr_str, path);
					if (res != MRES_SUCCESS) {
						MFATAL("Config Loader", "Failed to store entry %s", curr_str);
						return res;
					}
					conf->entry_count++;
					MLOG("Config Loader", "ENTRY: %s, PATH: %s", curr_str, path);
					curr_str = NULL;
					expect_path = mfalse;
				} else {
					// no '=' was encountered at all
					MFATAL("Config Loader", "Path must follow '=' after the name is provided line %zu", lnum);
					return MRES_NOT_MERRY_FAILURE;
				}
			} else {
				// must be the name 
				// It is important to note that, if the same name is encountered twice then
				// the newer path will overwrite the old value
				msize_t st = off_curr;
				while (curr != EOF && curr != '=') {
				    if (curr == '\n')
					       lnum++;
					curr = fgetc(conf->_conf_file);
					off_curr++;
				}
				if (curr == EOF) {
					MFATAL("Config Loader", "Expected '=' after the name but got EOF at line %zu", lnum);
					return MRES_NOT_MERRY_FAILURE;
				}
				msize_t ed = off_curr;
				curr_str = (mstr_t)calloc((ed - st) + 2, 1);
				if (!curr_str) {
					MFATAL("Config Loader", "Memory allocation failed", NULL);
					return MRES_SYS_FAILURE;
				}
				fseek(conf->_conf_file, st, SEEK_SET);
				fread(curr_str, 1, (ed - st) + 1, conf->_conf_file);
			}
		}
	}

	if (curr_str) {
		// we don't have a value for this
		MFATAL("Config Loader", "No path for name %s found", curr_str);
		return MRES_NOT_MERRY_FAILURE;
	}

	return MRES_SUCCESS;
}

mresult_t merry_conf_load_find(MerryConfLoad *conf, mstr_t key, mstr_t* result) {
	if (!conf || !key || !result) return MRES_INVALID_ARGS;
	if (!conf->_conf_file || !conf->entries) return MRES_NOT_CONFIGURED;
	return merry_umap_find(conf->entries, key, (mptr_t*)result);
}
