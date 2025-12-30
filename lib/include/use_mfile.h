#ifndef _USE_MFILE_
#define _USE_MFILE_

#include <use_defs.h>
#include <use_file.h>
#include <use_interface.h>
#include <use_types.h>

typedef Interface MappedFile;

result_t mapped_file_create(MappedFile **fmap);

result_t mapped_file_map(MappedFile *fmap, str_t path, size_t flags);

result_t mapped_file_unmap(MappedFile *fmap);

result_t mapped_file_obtain_ptr(MappedFile *fmap, bptr_t *ptr, size_t off);

result_t mapped_file_destroy(MappedFile *fmap);

#endif
