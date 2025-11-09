#include <test_core/comps/test_core_input.h>

tcret_t tc_read_input(mstr_t fname, TCInp *inp) {
  // mbool_t res = mfalse;
  mresult_t res = merry_open_file(&inp->file, fname, _MERRY_FOPEN_READ_, 0);
  if (res != MRES_SUCCESS) {
    MFATAL("TC", "Failed to read input file: FILE=%s because %s", fname,
           strerror(errno));
    return TC_FAILURE;
  }
  // Now that we have the file
  // Read that file into memory
  msize_t fsize = 0;
  if (merry_file_size(inp->file, &fsize) != INTERFACE_SUCCESS)
    merry_unreachable();
  if (fsize == 0) {
    MFATAL("TC", "File is empty! Nothing to execute! FILE=%s", fname);
    merry_destroy_file(inp->file);
    return TC_FAILURE;
  }
  res = merry_get_anonymous_memory(
      (mptr_t)&inp->mem, fsize); // the OS should get us the appropriate
                                 // memory chunk since we have no memory
                                 // structure at all so we don't care
  if (res != MRES_SUCCESS) {
    MFATAL("TC", "Failed to obtain memory for execution: FILE=%s", fname);
    merry_destroy_file(inp->file);
    return TC_FAILURE;
  }

  if (merry_map_file(inp->mem, inp->file) == RET_FAILURE) {
    MFATAL("TC", "Failed to map memory for execution: FILE=%s", fname);
    merry_destroy_file(inp->file);
    merry_return_memory(inp->mem, fsize);
    return TC_FAILURE;
  }

  return TC_SUCCESS;
}

void tc_destroy_input(TCInp *inp) {
  msize_t fsize = 0;
  merry_file_size(inp->file, &fsize);
  merry_return_memory(inp->mem, fsize);
  merry_destroy_file(inp->file);
}
