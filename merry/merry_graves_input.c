#include <merry_graves_input.h>

mstr_t *merry_graves_parse_metadata_file(mstr_t mfile) {
  mstr_t *fnames = merry_list_create(mstr_t, __CORE_TYPE_COUNT);
  if (!fnames) {
    MFATAL("Graves Reader", "Failed to parse metadata file[%s]", mfile);
    return RET_NULL;
  }
  FILE *file = fopen(mfile, "rb");

  if (!file) {
    MFATAL("Graves Reader", "Failed to open metadata file [%s] because %s",
           mfile, strerror(errno));
    merry_list_destroy(fnames);
    return RET_NULL;
  }

  // Get the size of the file
  fseek(file, 0, SEEK_END);

  msize_t size = ftell(file);
  if (size <)

    // Parse the Identification Header
    mbyte_t magic[3];

  return fnames;
}
