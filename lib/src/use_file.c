#include <use_file.h>
#include <lib/merry_file.h>

_ALWAYS_INLINE_ result_t open_file(File **file, str_t file_path, str_t modes, int flags) {
	return merry_open_file(file, file_path, modes, flags);
}

_ALWAYS_INLINE_ result_t close_file(File *file) {
	return merry_close_file(file);
}

_ALWAYS_INLINE_ result_t destroy_file(File *file) {
	return merry_destroy_file(file);
}

_ALWAYS_INLINE_ result_t file_size(File *file, size_t *res) {
	return merry_file_size(file, res);
}

_ALWAYS_INLINE_ result_t file_seek(File *file, qptr_t res, sqword_t off, size_t whence) {
	return merry_file_seek(file, res, off, whence);
}

_ALWAYS_INLINE_ result_t file_tell(File *file, size_t *off) {
	return merry_file_tell(file, off);
}

_ALWAYS_INLINE_ result_t file_read(File *file, qptr_t res, bptr_t buf, size_t num_of_bytes) {
	return merry_file_read(file,res, buf, num_of_bytes);
}

_ALWAYS_INLINE_ result_t file_write(File *file, qptr_t res,bptr_t buf, size_t num_of_bytes) {
	return merry_file_write(file, res, buf, num_of_bytes);
}
