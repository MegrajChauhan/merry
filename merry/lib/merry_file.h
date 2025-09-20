#ifndef _MERRY_FILE_
#define _MERRY_FILE_

#include <merry_error_stack.h>
#include <merry_file_definitions.h>
#include <merry_file_defs.h>
#include <merry_interface.h>
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

MerryFile *merry_open_file(mstr_t file_path, mstr_t modes, int flags,
                           mbool_t *failed, MerryErrorStack *st);

mret_t merry_figure_out_file_modes(mstr_t modex, int flags, int *res_mode,
                                   int *res_flag);

mret_t merry_close_file(MerryFile *file, MerryErrorStack *st);

#endif
