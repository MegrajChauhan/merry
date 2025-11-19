#include <merry_platform.h>

// mresult_t merry_open_pipe(mdataline_t *rline, mdataline_t *wline) {
//   merry_check_ptr(rline);
//   merry_check_ptr(wline);
// 
//   mdataline_t lines[2];
// #ifdef _USE_LINUX_
//   if (pipe(lines) == -1) {
//     return MRES_SYS_FAILURE;
//   }
// #elif defined(_USE_WIN_)
//   SECURITY_ATTRIBUTES saAttr;
//   saAttr.nLength = sizeof(SECURITY_ATTRIBUTES);
//   saAttr.bInheritHandle = TRUE; // Handles can be inherited by child processes
//   saAttr.lpSecurityDescriptor = NULL;
// 
//   if (!CreatePipe(&lines[0], &lines[1], &saAttr, 0)) { // Default size
//     return MRES_SYS_FAILURE;
//   }
// #endif
//   *rline = lines[0];
//   *wline = lines[1];
//   return MRES_SUCCESS;
// }

mbool_t merry_is_path_a_directory(mstr_t path) {
  if (!path)
  	return mfalse;
  struct stat s;
  if (stat(path, &s) == 0)
    return S_ISDIR(s.st_mode) ? mtrue : mfalse;
  return mfalse;
}

mbool_t merry_non_blocking_socket(msocket_t *sock) {
	if (!sock)
		return mfalse;
#ifdef _USE_LINUX_
	int flags = fcntl(*sock, F_GETFL, 0);
	fcntl(*sock, F_SETFL, flags | O_NONBLOCK);
#else
	
#endif
	return mtrue;
}

mbool_t merry_get_random_number(mqptr_t res) {
	if (!res)
		return mfalse;
#ifdef _USE_LINUX_
	getrandom(res, sizeof(*res), 0);
#else

#endif
	return mtrue;
}
