#ifndef _MERRY_FIO_
#define _MERRY_FIO_

#include <merry_file.h>
#include <merry_nort.h>
#include <merry_types.h>
#include <merry_utils.h>

// Providing the IO functions

_MERRY_INTERNAL_ MerryNort *nort_instance_fIO;

void merry_fio_prepare();

msqword_t merry_fio_read(MerryFile *file, mbptr_t buf, msize_t N,
                         MerryNortRequest *req, MerryErrorStack *st);

msqword_t merry_fio_write(MerryFile *file, mbptr_t buf, msize_t N,
                          MerryNortRequest *req, MerryErrorStack *st);

#endif
