// #include <merry_pipe.h>
// 
// mresult_t merry_open_merrypipe(MerryPipe **pipe) {
//   mresult_t res = merry_interface_init(pipe, INTERFACE_TYPE_PIPE);
//   if (res != MRES_SUCCESS) {
//     return res;
//   }
//   MerryPipe *p = *pipe;
// 
//   if ((res = merry_open_pipe(&p->cpipe._read_fd, &p->cpipe._write_fd)) !=
//       MRES_SUCCESS)
//     return res;
// 
//   p->cpipe._in_use = mtrue;
//   p->cpipe._rclosed = mfalse;
//   p->cpipe._wclosed = mfalse;
//   return MRES_SUCCESS;
// }
// 
// mresult_t merry_pipe_close_read_end(MerryPipe *pipe) {
//   merry_check_ptr(pipe);
//   if (pipe->interface_t != INTERFACE_TYPE_PIPE)
//     return MRES_UNEXPECTED;
// #ifdef _USE_LINUX_
//   if (pipe->cpipe._rclosed == mfalse)
//     close(pipe->cpipe._read_fd);
//   pipe->cpipe._rclosed = mtrue;
// #elif defined(_USE_WIN_)
//   if (!pipe->cpipe._rclosed && pipe->cpipe._read_handle != INVALID_HANDLE_VALUE)
//     CloseHandle(pipe->cpipe._read_handle);
//   pipe->cpipe._rclosed = mtrue;
// #endif
//   return MRES_SUCCESS;
// }
// 
// mresult_t merry_pipe_close_write_end(MerryPipe *pipe) {
//   if (pipe->interface_t != INTERFACE_TYPE_PIPE)
//     return MRES_UNEXPECTED;
// #ifdef _USE_LINUX_
//   if (pipe->cpipe._wclosed == mfalse)
//     close(pipe->cpipe._write_fd);
//   pipe->cpipe._wclosed = mtrue;
// #elif defined(_USE_WIN_)
//   if (!pipe->cpipe._wclosed &&
//       pipe->cpipe._write_handle != INVALID_HANDLE_VALUE)
//     CloseHandle(pipe->cpipe._write_handle);
//   pipe->cpipe._wclosed = mtrue;
// #endif
//   return MRES_SUCCESS;
// }
// 
// mresult_t merry_pipe_close_both_ends(MerryPipe *pipe) {
//   merry_check_ptr(pipe);
//   if (pipe->interface_t != INTERFACE_TYPE_PIPE)
//     return MRES_UNEXPECTED;
//   merry_pipe_close_read_end(pipe);
//   merry_pipe_close_write_end(pipe);
//   pipe->cpipe._in_use = mfalse;
//   return MRES_SUCCESS;
// }
// 
// mresult_t merry_merrypipe_reopen(MerryPipe *pipe) {
//   merry_check_ptr(pipe);
//   if (pipe->interface_t != INTERFACE_TYPE_PIPE)
//     return MRES_UNEXPECTED;
//   mresult_t res;
//   if ((res = merry_open_pipe(&pipe->cpipe._read_fd, &pipe->cpipe._write_fd)) !=
//       MRES_SUCCESS) {
//     return res;
//   }
//   pipe->cpipe._in_use = mtrue;
//   pipe->cpipe._rclosed = mfalse;
//   pipe->cpipe._wclosed = mfalse;
//   return MRES_SUCCESS;
// }
// 
// mresult_t merry_destroy_pipe(MerryPipe *pipe) {
//   if (pipe->interface_t != INTERFACE_TYPE_PIPE)
//     return MRES_UNEXPECTED;
//   merry_pipe_close_both_ends(pipe);
//   free(pipe);
//   return MRES_SUCCESS;
// }
// 
// // #ifdef _USE_WIN_
// // void merry_pipe_prevent_inheritance(MerryPipe *pipe, int flag) {
// //   massert(pipe);
// //   switch (flag) {
// //   case _MERRY_IN_RFD_:
// //     SetHandleInformation(pipe->cpipe._read_handle, HANDLE_FLAG_INHERIT, 0);
// //     break;
// //   case _MERRY_IN_WFD_:
// //     SetHandleInformation(pipe->cpipe._write_handle, HANDLE_FLAG_INHERIT, 0);
// //     break;
// //   }
// // }
// // #endif
