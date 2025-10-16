#ifndef _RBC_FILE_
#define _RBC_FILE_

#include <merry_file.h>
#include <merry_interface.h>
#include <merry_logger.h>
#include <merry_types.h>
#include <merry_utils.h>
#include <regr_core/lib/fs/rbc_file_defs.h>
#include <stdlib.h>

typedef struct RBCFile RBCFile;

// RBC will support buffering
struct RBCFile {
  MerryFile *file;
  mbyte_t buf[_RBC_FILE_BUF_LEN_];
  msqword_t BP; // buffer pointer
};

RBCFile *rbc_file_open(mstr_t fpath, mstr_t mode, int flags);

RBCFile *rbc_file_reopen(RBCFile *file, mstr_t fpath, mstr_t mode, int flags);

minterfaceRet_t rbc_fseek(MerryFile *file, msize_t off, msize_t whence);

minterfaceRet_t rbc_ftell(MerryFile *file, msize_t *off);

minterfaceRet_t rbc_file_close(RBCFile *file);

void rbc_file_destroy(RBCFile *file);

#endif
