#ifndef _MERRY_FILE_
#define _MERRY_FILE_

#include <merry_file_definitions.h>
#include <merry_file_defs.h>
#include <merry_interface.h>
#include <merry_operations.h>
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

minterfaceRet_t merry_open_file(MerryFile **file, mstr_t file_path,
                                mstr_t modes, int flags);

mret_t merry_figure_out_file_modes(mstr_t modex, int flags, int *res_mode,
                                   int *res_flag);

minterfaceRet_t merry_close_file(MerryFile *file);

minterfaceRet_t merry_destroy_file(MerryFile *file);

minterfaceRet_t merry_file_size(MerryFile *file, msize_t *res);

minterfaceRet_t merry_file_seek(MerryFile *file, msqword_t off, msize_t whence);

minterfaceRet_t merry_file_tell(MerryFile *file, msize_t *off);

minterfaceRet_t merry_file_read(MerryFile *file, mbptr_t buf,
                                msize_t num_of_bytes);

minterfaceRet_t merry_file_write(MerryFile *file, mbptr_t buf,
                                 msize_t num_of_bytes);

#endif
