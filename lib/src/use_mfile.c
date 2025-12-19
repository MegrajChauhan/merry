#include <use_mfile.h>
#include <lib/merry_mapped_file.h>

_ALWAYS_INLINE_ result_t mapped_file_create(MappedFile **fmap) {
	return merry_mapped_file_create(fmap);
}

_ALWAYS_INLINE_ result_t mapped_file_map(MappedFile *fmap, str_t path, size_t flags) {
	return merry_mapped_file_map(fmap, path, flags);
}

_ALWAYS_INLINE_ result_t mapped_file_unmap(MappedFile *fmap) {
	return merry_mapped_file_unmap(fmap);
}

_ALWAYS_INLINE_ result_t mapped_file_obtain_ptr(MappedFile *fmap, bptr_t *ptr, size_t off) {
	return merry_mapped_file_obtain_ptr(fmap, ptr, off);
}

_ALWAYS_INLINE_ result_t mapped_file_destroy(MappedFile *fmap) {
	return merry_mapped_file_destroy(fmap);
}
