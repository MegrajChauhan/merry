#include <merry_mapped_file.h>

mresult_t merry_mapped_file_create(MerryMappedFile **fmap) {
	if (!fmap)
		return MRES_INVALID_ARGS;
	MerryMappedFile *fm;
	mresult_t res;
	if ((res = merry_interface_init(&fm, INTERFACE_TYPE_MAPPED_FILE)) != MRES_SUCCESS)
		return res;
	fm->mem_mapped_file.file = NULL;
	fm->mem_mapped_file.map = NULL;
	*fmap = fm;
	return MRES_SUCCESS;
}

mresult_t merry_mapped_file_map(MerryMappedFile *fmap, mstr_t path, msize_t flags) {
	if (!fmap)
		return MRES_INVALID_ARGS;
	if (fmap->interface_t != INTERFACE_TYPE_MAPPED_FILE) 
		return MRES_CONFIGURATION_INVALID;
	if (fmap->mem_mapped_file.map)
		return MRES_NOT_ALLOWED;
	mresult_t res;
	if ((res = merry_open_file(&fmap->mem_mapped_file.file, path, _MERRY_FOPEN_READ_WRITE_, 0)) != MRES_SUCCESS) 
		return res;
	if ((res = merry_file_size(fmap->mem_mapped_file.file, &fmap->mem_mapped_file.len)) != MRES_SUCCESS) {
		merry_close_file(fmap->mem_mapped_file.file);
		merry_destroy_file(fmap->mem_mapped_file.file);
		return res;	
	}
#ifdef _USE_LINUX_
	int f = (flags & _MERRY_MAPPED_FILE_UPDATE_BACKING_FILE_)? MAP_SHARED: MAP_PRIVATE;
	int prot = PROT_READ | PROT_WRITE;
	if ((fmap->mem_mapped_file.map = mmap(NULL, fmap->mem_mapped_file.len, prot, f, fmap->mem_mapped_file.file->file.fd, 0)) == NULL) 
		return MRES_SYS_FAILURE;
	fmap->mem_mapped_file.flags.update_backing_file = f;
#endif

	return MRES_SUCCESS;
}

mresult_t merry_mapped_file_unmap(MerryMappedFile *fmap) {
	if (!fmap)
		return MRES_INVALID_ARGS;
	if (fmap->interface_t != INTERFACE_TYPE_MAPPED_FILE) 
		return MRES_CONFIGURATION_INVALID;
	if (!fmap->mem_mapped_file.map)
		return MRES_NOT_ALLOWED;
#ifdef _USE_LINUX_
	munmap(fmap->mem_mapped_file.map, fmap->mem_mapped_file.len);
#endif
	merry_close_file(fmap->mem_mapped_file.file);
	merry_destroy_file(fmap->mem_mapped_file.file);
	fmap->mem_mapped_file.map = NULL;
	return MRES_SUCCESS;
}

mresult_t merry_mapped_file_obtain_ptr(MerryMappedFile *fmap, mbptr_t *ptr, msize_t off) {
	if (!fmap)
		return MRES_INVALID_ARGS;
	if (fmap->interface_t != INTERFACE_TYPE_MAPPED_FILE) 
		return MRES_CONFIGURATION_INVALID;
	if (!fmap->mem_mapped_file.map)
		return MRES_NOT_ALLOWED;
	if (off > fmap->mem_mapped_file.len)
		return MRES_INVALID_ARGS;
	*ptr = (mbptr_t)fmap->mem_mapped_file.map + off; 
	return MRES_SUCCESS;
}

mresult_t merry_mapped_file_destroy(MerryMappedFile *fmap) {
	if (!fmap)
		return MRES_INVALID_ARGS;
	if (fmap->interface_t != INTERFACE_TYPE_MAPPED_FILE) 
		return MRES_CONFIGURATION_INVALID;
	if (fmap->mem_mapped_file.map)
		return MRES_NOT_ALLOWED;
    merry_interface_destroy(fmap);
    return MRES_SUCCESS;
}
