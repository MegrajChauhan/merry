#include <test_core/comps/test_core_input.h>

mret_t tc_read_input(mstr_t fname, TCInp *inp) {
  mbool_t res = mfalse;
  inp->file = merry_open_file(fname, _MERRY_FOPEN_READ_, 0, &res);
  // res shouldn't be mtrue since we pass valid flags
  if (res)
    merry_unreachable();
  if (!inp->file && !res) {
    MFATAL("TC", "Failed to read input file: FILE=%s because %s", fname,
           strerror(errno));
    return RET_FAILURE;
  }
  // Now that we have the file
  // Read that file into memory
  msize_t fsize = 0;
  if (merry_file_size(inp->file, &fsize) != INTERFACE_SUCCESS)
    merry_unreachable();
  if (fsize == 0) {
    MFATAL("TC", "File is empty! Nothing to execute! FILE=%s", fname);
    merry_close_file(inp->file);
    return RET_FAILURE;
  }
  inp->mem = (mbptr_t)merry_get_anonymous_memory(
      fsize); // the OS should get us the appropriate
              // memory chunk since we have no memory
              // structure at all so we don't care
  if (!inp->mem) {
    MFATAL("TC", "Failed to obtain memory for execution: FILE=%s", fname);
    merry_close_file(inp->file);
    return RET_FAILURE;
  }

  if (merry_map_file(inp->mem, inp->file) == RET_FAILURE) {
    MFATAL("TC", "Failed to map memory for execution: FILE=%s", fname);
    merry_close_file(inp->file);
    merry_return_memory(inp->mem, fsize);
    return RET_FAILURE;
  }

  return RET_SUCCESS;
}

void tc_destroy_input(TCInp *inp) {
  msize_t fsize = 0;
  merry_file_size(inp->file, &fsize);
  merry_return_memory(inp->mem, fsize);
  merry_close_file(inp->file);
}
