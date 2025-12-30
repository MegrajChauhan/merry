#ifndef _MERRY_FILE_
#define _MERRY_FILE_

#include <merry_file_defs.h>
#include <merry_interface.h>
#include <merry_results.h>
#include <stdlib.h>
#include <string.h>

/*
 * Here and mostly all components of 'lib' will not perform IO
 * directly nor will they give any way of doing them.
 * nort will provide the IO capabilities itself
 * These lib components will provide other features that doesn't
 * involve IO of any kind.
 * */

typedef MerryInterface MerryFile;

mresult_t merry_open_file(MerryFile **file, mstr_t file_path, mstr_t modes,
                          int flags);

mret_t merry_figure_out_file_modes(mstr_t modex, int flags, int *res_mode,
                                   int *res_flag);

mresult_t merry_close_file(MerryFile *file);

mresult_t merry_destroy_file(MerryFile *file);

mresult_t merry_file_size(MerryFile *file, msize_t *res);

mresult_t merry_file_seek(MerryFile *file, mqptr_t _res, msqword_t off,
                          msize_t whence);

mresult_t merry_file_tell(MerryFile *file, msize_t *off);

mresult_t merry_file_read(MerryFile *file, mqptr_t _res, mbptr_t buf,
                          msize_t num_of_bytes);

mresult_t merry_file_write(MerryFile *file, mqptr_t _res, mbptr_t buf,
                           msize_t num_of_bytes);

#endif
