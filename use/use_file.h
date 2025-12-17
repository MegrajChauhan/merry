#ifndef _USE_FILE_
#define _USE_FILE_

#include <use_defs.h>
#include <use_types.h>
#include <use_utils.h>
#include <use_file_defs.h>

#define O_MODE_READ _FOPEN_READ_
#define O_MODE_WRITE _FOPEN_WRITE_
#define O_MODE_READ_WRITE _FOPEN_READ_WRITE_
#define O_MODE_APPEND _FOPEN_APPEND_

#define O_FLAG_CREATE _FOPEN_CREATE_

#define O_FLAG_READ_PERM_FOR_USR _FOPEN_READ_PERMISSION_USR_
#define O_FLAG_WRITE_PERM_FOR_USR _FOPEN_WRITE_PERMISSION_USR_
#define O_FLAG_EXEC_PERM_FOR_USR _FOPEN_EXECUTE_PERMISSION_USR_

#define O_FLAG_READ_PERM_FOR_GRP _FOPEN_READ_PERMISSION_GRP_
#define O_FLAG_WRITE_PERM_FOR_GRP _FOPEN_WRITE_PERMISSION_GRP_
#define O_FLAG_EXEC_PERM_FOR_GRP _FOPEN_EXECUTE_PERMISSION_GRP_

#define O_FLAG_READ_PERM_FOR_OTH _FOPEN_READ_PERMISSION_OTH_
#define O_FLAG_WRITE_PERM_FOR_OTH _FOPEN_WRITE_PERMISSION_OTH_
#define O_FLAG_EXEC_PERM_FOR_OTH _FOPEN_EXECUTE_PERMISSION_OTH_

typedef Interface File;

extern result_t open_file(File **file, str_t file_path, str_t modes,
                                int flags) _ALIAS_(merry_open_file);

extern result_t close_file(File *file) _ALIAS_(merry_close_file);

extern result_t destroy_file(File *file) _ALIAS_(merry_destroy_file);

extern result_t file_size(File *file, size_t *res)
    _ALIAS_(merry_file_size);

extern result_t file_seek(File *file, sqword_t off, size_t whence)
    _ALIAS_(merry_file_seek);

extern result_t file_tell(File *file, size_t *off)
    _ALIAS_(merry_file_tell);

extern result_t file_read(File *file, bptr_t buf, size_t num_of_bytes)
    _ALIAS_(merry_file_read);

extern result_t file_write(File *file, bptr_t buf, size_t num_of_bytes)
    _ALIAS_(merry_file_write);

#endif
