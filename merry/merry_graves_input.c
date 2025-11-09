#include <merry_graves_input.h>

_MERRY_DEFINE_STATIC_LIST_(Entry, mstr_t);

MerryEntryList *merry_graves_parse_metadata_file(mstr_t mfile) {
  merry_check_ptr(mfile);
  mresult_t res;
  MerryEntryList *fnames;
  res = merry_Entry_list_create(__CORE_TYPE_COUNT, &fnames);
  if (!fnames) {
    MFATAL("Graves Reader", "Failed to parse metadata file[%s]", mfile);
    return RET_NULL;
  }
  if (merry_is_path_a_directory(mfile)) {
    MFATAL("Graves Reader",
           "Failed to open file %s: Either it doesn't exist, or is a directory "
           ", or permission denied",
           mfile);
  }
  FILE *file = fopen(mfile, "rb");

  if (!file) {
    MFATAL("Graves Reader", "Failed to open metadata file [%s] because %s",
           mfile, strerror(errno));
    merry_Entry_list_destroy(fnames);
    return RET_NULL;
  }

  // Get the size of the file
  fseek(file, 0, SEEK_END);

  msize_t size = ftell(file);
  if (size < 18) {
    MFATAL("Graves Reader", "Invalid metadata file structure [%s]", mfile);
    merry_Entry_list_destroy(fnames);
    fclose(file);
    return RET_NULL;
  }

  rewind(file);

  // Parse the Identification Header
  mbyte_t magic[4] = {0};
  fread(magic, 1, 3, file);

  if (!(magic[0] == 'M' && magic[1] == 'M' && magic[2] == 'F')) {
    MFATAL("Graves Reader", "Incorrect MAGIC BYTES '%s' when expected 'MMF'",
           magic);
    MLOG("Graves Reader", "Parsing metadata file %s", mfile);
    merry_Entry_list_destroy(fnames);
    fclose(file);
    return RET_NULL;
  }

  // Get the number of entries
  mbyte_t entries[5];
  MerryHostMemLayout mem_layout;
  mem_layout.whole_word = 0;
  fread(entries, 1, 5, file);
  mem_layout.bytes.b3 = entries[0];
  mem_layout.bytes.b4 = entries[1];
  mem_layout.bytes.b5 = entries[2];
  mem_layout.bytes.b6 = entries[3];
  mem_layout.bytes.b7 = entries[4];
  if (mem_layout.whole_word > __CORE_TYPE_COUNT) {
    MFATAL("Graves Reader",
           "Number of entries is beyond allowed: GIVEN=%zu, ALLOWED=%zu",
           mem_layout.whole_word, (msize_t)__CORE_TYPE_COUNT);
    MLOG("Graves Reader", "Parsing metadata file %s", mfile);
    merry_Entry_list_destroy(fnames);
    fclose(file);
    return RET_NULL;
  }
  mbool_t status_per_core_type[__CORE_TYPE_COUNT] = {mfalse};

  // Now we can get the entries
  for (msize_t i = 0; i < mem_layout.whole_word; i++) {
    // reading this will be quite slow
    mbyte_t type[8];
    MerryHostMemLayout to_type;
    to_type.whole_word = 0;
    if (fread(type, 1, 8, file) != 8) {
      MFATAL("Graves Reader", "Invalid file: Core type section is incomplete",
             NULL);
      MLOG("Graves Reader", "Parsing metadata file %s", mfile);
      merry_Entry_list_destroy(fnames);
      fclose(file);
      return RET_NULL;
    }
    to_type.bytes.b0 = type[0];
    to_type.bytes.b1 = type[1];
    to_type.bytes.b2 = type[2];
    to_type.bytes.b3 = type[3];
    to_type.bytes.b4 = type[4];
    to_type.bytes.b5 = type[5];
    to_type.bytes.b6 = type[6];
    to_type.bytes.b7 = type[7];
    if (to_type.whole_word >= __CORE_TYPE_COUNT) {
      MFATAL("Graves Reader", "Provided CORE TYPE is invalid: GIVEN=%zu",
             to_type.whole_word);
      MLOG("Graves Reader", "Parsing metadata file %s", mfile);
      merry_Entry_list_destroy(fnames);
      fclose(file);
      return RET_NULL;
    }
    // see if a file already exists for the given core type
    if (status_per_core_type[to_type.whole_word]) {
      MFATAL("Graves Reader",
             "Multiple entries for the same CORE TYPE is not allowed: CORE=%zu",
             to_type.whole_word);
      MLOG("Graves Reader", "Parsing metadata file %s", mfile);
      merry_Entry_list_destroy(fnames);
      fclose(file);
      return RET_NULL;
    }
    // finally read the file name
    msize_t off_now = ftell(file);
    msize_t off_then = off_now;
    mbool_t got_term = mfalse;
    while (!feof(file)) {
      if (fgetc(file) == '\0') {
        got_term = mtrue;
        break;
      }
      off_then++;
    }
    msize_t diff = off_then - off_now;
    if (!got_term || diff == 0) {
      MFATAL("Graves Reader",
             "Invalid Entry. File Name is invalid: while reading for CORE=%zu",
             to_type.whole_word);
      MLOG("Graves Reader", "Parsing metadata file %s", mfile);
      merry_Entry_list_destroy(fnames);
      fclose(file);
      return RET_NULL;
    }
    fseek(file, -(diff + 1), SEEK_CUR);
    mstr_t fname = calloc(diff + 1, 1);
    if (!fname) {
      MFATAL("Graves Reader", "Failed to allocate memory for file name parsing",
             NULL);
      MLOG("Graves Reader", "Parsing metadata file %s", mfile);
      merry_Entry_list_destroy(fnames);
      fclose(file);
      return RET_NULL;
    }
    fread(fname, 1, diff + 1, file); // will not fail
    fnames->buf[to_type.whole_word] = fname;
    status_per_core_type[to_type.whole_word] = mtrue;
  }
  MLOG("Graves Reader", "Finished parsing metadata file %s", mfile);
  fclose(file);
  return fnames;
}
