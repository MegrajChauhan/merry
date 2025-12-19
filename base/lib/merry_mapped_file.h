#ifndef _MERRY_MAPPED_FILE_
#define _MERRY_MAPPED_FILE_

#include <merry_interface.h>
#include <merry_file.h>
#include <merry_results.h>
#include <merry_mapped_file_defs.h>
#include <stdlib.h>

typedef struct MerryInterface MerryMappedFile;

mresult_t merry_mapped_file_create(MerryMappedFile **fmap);

mresult_t merry_mapped_file_map(MerryMappedFile *fmap, mstr_t path, msize_t flags);

mresult_t merry_mapped_file_unmap(MerryMappedFile *fmap);

mresult_t merry_mapped_file_obtain_ptr(MerryMappedFile *fmap, mbptr_t *ptr, msize_t off);

mresult_t merry_mapped_file_destroy(MerryMappedFile *fmap);

#endif
