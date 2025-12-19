#ifndef _USE_FILE_
#define _USE_FILE_

#include <use_defs.h>
#include <use_types.h>
#include <use_utils.h>
#include <use_file_defs.h>
#include <use_interface.h>

typedef Interface File;

result_t open_file(File **file, str_t file_path, str_t modes, int flags);

result_t close_file(File *file);

result_t destroy_file(File *file);

result_t file_size(File *file, size_t *res);

result_t file_seek(File *file, qptr_t res, sqword_t off, size_t whence);

result_t file_tell(File *file, size_t *off);

result_t file_read(File *file, qptr_t res, bptr_t buf, size_t num_of_bytes);

result_t file_write(File *file, qptr_t res, bptr_t buf, size_t num_of_bytes);

#endif
