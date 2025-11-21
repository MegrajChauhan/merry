#ifndef _MERRY_FILE_
#define _MERRY_FILE_

#include <merry_file_definitions.h>
#include <merry_operations.h>
#include <stdlib.h>
#include <string.h>

typedef mfd_t MerryFile;

mresult_t merry_open_file(MerryFile *file, mstr_t file_path,
                                int modes, int flags);

mresult_t merry_close_file(MerryFile *file);

mresult_t merry_file_size(MerryFile *file, msize_t *res);

mresult_t merry_file_seek(MerryFile *file, msqword_t off, msize_t whence);

mresult_t merry_file_tell(MerryFile *file, msize_t *off);

mresult_t merry_file_read(MerryFile *file, mbptr_t buf,
                                msize_t num_of_bytes);

mresult_t merry_file_write(MerryFile *file, mbptr_t buf,
                                 msize_t num_of_bytes);

#endif
