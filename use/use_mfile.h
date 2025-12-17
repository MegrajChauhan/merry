#ifndef _USE_MFILE_
#define _USE_MFILE_

#include <use_file.h>
#include <use_interface.h>
#include <use_defs.h>
#include <use_types.h>

typedef Interface MappedFile;

extern result_t mapped_file_create(MappedFile **fmap) _ALIAS_(merry_mapped_file_create);

extern result_t mapped_file_map(MappedFile *fmap, str_t path, size_t flags) _ALIAS_(merry_mapped_file_map);

extern result_t mapped_file_unmap(MappedFile *fmap) _ALIAS_(merry_mapped_file_unmap);

extern result_t mapped_file_obtain_ptr(MappedFile *fmap, bptr_t *ptr, size_t off) _ALIAS_(merry_mapped_file_obtain_ptr);

extern result_t mapped_file_destroy(MappedFile *fmap) _ALIAS_(merry_mapped_file_destroy);

#endif
