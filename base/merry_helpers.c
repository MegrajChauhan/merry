#include <merry_helpers.h>

mresult_t merry_open_pipe(mdataline_t *rline, mdataline_t *wline) {
  mdataline_t lines[2];
#ifdef _USE_LINUX_
  if (pipe(lines) == -1) {
    return MRES_SYS_FAILURE;
  }
#elif defined(_USE_WIN_)
  SECURITY_ATTRIBUTES saAttr;
  saAttr.nLength = sizeof(SECURITY_ATTRIBUTES);
  saAttr.bInheritHandle = TRUE; // Handles can be inherited by child processes
  saAttr.lpSecurityDescriptor = NULL;

  if (!CreatePipe(&lines[0], &lines[1], &saAttr, 0)) { // Default size
    return MRES_SYS_FAILURE;
  }
#endif
  *rline = lines[0];
  *wline = lines[1];
  return MRES_SUCCESS;
}

mbool_t merry_is_path_a_directory(mstr_t path) {
  struct stat s;
  if (stat(path, &s) == 0)
    return S_ISDIR(s.st_mode) ? mtrue : mfalse;
  return mfalse;
}

msize_t merry_align_value(msize_t val, msize_t align_to) {
  if (align_to == 0 || val == 0)
    return 0;
  if (val % align_to == 0)
    return val;
  return val + (align_to - (val % align_to));
}
