#ifndef _USE_FILE_
#define _USE_FILE_

#include <merry_file.h>
#include <use_defs.h>
#include <use_types.h>
#include <use_utils.h>

#define O_MODE_READ _MERRY_FOPEN_READ_
#define O_MODE_WRITE _MERRY_FOPEN_WRITE_
#define O_MODE_READ_WRITE _MERRY_FOPEN_READ_WRITE_
#define O_MODE_APPEND _MERRY_FOPEN_APPEND_

#define O_FLAG_CREATE _MERRY_FOPEN_CREATE_

#define O_FLAG_READ_PERM_FOR_USR _MERRY_FOPEN_READ_PERMISSION_USR_
#define O_FLAG_WRITE_PERM_FOR_USR _MERRY_FOPEN_WRITE_PERMISSION_USR_
#define O_FLAG_EXEC_PERM_FOR_USR _MERRY_FOPEN_EXECUTE_PERMISSION_USR_

#define O_FLAG_READ_PERM_FOR_GRP _MERRY_FOPEN_READ_PERMISSION_GRP_
#define O_FLAG_WRITE_PERM_FOR_GRP _MERRY_FOPEN_WRITE_PERMISSION_GRP_
#define O_FLAG_EXEC_PERM_FOR_GRP _MERRY_FOPEN_EXECUTE_PERMISSION_GRP_

#define O_FLAG_READ_PERM_FOR_OTH _MERRY_FOPEN_READ_PERMISSION_OTH_
#define O_FLAG_WRITE_PERM_FOR_OTH _MERRY_FOPEN_WRITE_PERMISSION_OTH_
#define O_FLAG_EXEC_PERM_FOR_OTH _MERRY_FOPEN_EXECUTE_PERMISSION_OTH_

typedef MerryFile File;

extern interfaceRet_t open_file(File **file, str_t file_path, str_t modes,
                                int flags) _ALIAS_(merry_open_file);

extern interfaceRet_t close_file(File *file) _ALIAS_(merry_close_file);

extern interfaceRet_t destroy_file(File *file) _ALIAS_(merry_destroy_file);

extern interfaceRet_t file_size(File *file, msize_t *res)
    _ALIAS_(merry_file_size);

extern interfaceRet_t file_seek(File *file, sqword_t off, msize_t whence)
    _ALIAS_(merry_file_seek);

extern interfaceRet_t file_tell(File *file, msize_t *off)
    _ALIAS_(merry_file_tell);

extern interfaceRet_t file_read(File *file, bptr_t buf, msize_t num_of_bytes)
    _ALIAS_(merry_file_read);

extern interfaceRet_t file_write(File *file, bptr_t buf, msize_t num_of_bytes)
    _ALIAS_(merry_file_write);

#endif
