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

struct RBCFile {
  MerryFile *file;

  // Buffer and buffer manipulators
  mbptr_t buf;
  msword_t BP;                    // buffer pointer
  mword_t usable_bytes_in_buffer; // Usable bytes in buffer

  // Other information
  mbyte_t last_oper; // 0 for read and 1 for write
  mbool_t res;       // for making the structure aligned
  mqword_t actual_file_off;
};

RBCFile *rbc_file_open(mstr_t fpath, mstr_t mode, int flags);

RBCFile *rbc_file_reopen(RBCFile *file, mstr_t fpath, mstr_t mode, int flags);

minterfaceRet_t rbc_fseek(RBCFile *file, msqword_t off, msize_t whence);

minterfaceRet_t rbc_ftell(RBCFile *file, msize_t *off);

minterfaceRet_t rbc_fread(RBCFile *file, mbptr_t buf, msize_t num_of_bytes);

minterfaceRet_t rbc_fwrite(RBCFile *file, mbptr_t buf, msize_t num_of_bytes);

minterfaceRet_t rbc_file_close(RBCFile *file);

minterfaceRet_t rbc_file_flush(RBCFile *file);

void rbc_file_destroy(RBCFile *file);

#endif
