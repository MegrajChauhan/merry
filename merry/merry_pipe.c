#include <merry_pipe.h>

MerryPipe *merry_open_merrypipe() {
  MerryPipe *p = (MerryPipe *)merry_interface_init(INTERFACE_TYPE_PIPE);
  if (p == NULL) {
    return RET_NULL;
  }

  if (merry_open_pipe(&p->cpipe._read_fd, &p->cpipe._write_fd) == RET_FAILURE) {
    return RET_NULL;
  }

  p->cpipe._in_use = mtrue;
  p->cpipe._rclosed = mfalse;
  p->cpipe._wclosed = mfalse;
  return p;
}

minterfaceRet_t merry_pipe_close_read_end(MerryPipe *pipe) {
  merry_check_ptr(pipe);
  if (pipe->interface_t != INTERFACE_TYPE_PIPE)
    return INTERFACE_TYPE_INVALID;
#ifdef _USE_LINUX_
  if (pipe->cpipe._rclosed == mfalse)
    close(pipe->cpipe._read_fd);
  pipe->cpipe._rclosed = mtrue;
#elif defined(_USE_WIN_)
  if (!pipe->cpipe._rclosed && pipe->cpipe._read_handle != INVALID_HANDLE_VALUE)
    CloseHandle(pipe->cpipe._read_handle);
  pipe->cpipe._rclosed = mtrue;
#endif
  return INTERFACE_SUCCESS;
}

minterfaceRet_t merry_pipe_close_write_end(MerryPipe *pipe) {
  if (pipe->interface_t != INTERFACE_TYPE_PIPE)
    return INTERFACE_TYPE_INVALID;
#ifdef _USE_LINUX_
  if (pipe->cpipe._wclosed == mfalse)
    close(pipe->cpipe._write_fd);
  pipe->cpipe._wclosed = mtrue;
#elif defined(_USE_WIN_)
  if (!pipe->cpipe._wclosed &&
      pipe->cpipe._write_handle != INVALID_HANDLE_VALUE)
    CloseHandle(pipe->cpipe._write_handle);
  pipe->cpipe._wclosed = mtrue;
#endif
  return INTERFACE_SUCCESS;
}

minterfaceRet_t merry_pipe_close_both_ends(MerryPipe *pipe) {
  merry_check_ptr(pipe);
  if (pipe->interface_t != INTERFACE_TYPE_PIPE)
    return INTERFACE_TYPE_INVALID;
  merry_pipe_close_read_end(pipe);
  merry_pipe_close_write_end(pipe);
  pipe->cpipe._in_use = mfalse;
  return INTERFACE_SUCCESS;
}

minterfaceRet_t merry_merrypipe_reopen(MerryPipe *pipe) {
  merry_check_ptr(pipe);
  if (pipe->interface_t != INTERFACE_TYPE_PIPE)
    return INTERFACE_TYPE_INVALID;
  if (merry_open_pipe(&pipe->cpipe._read_fd, &pipe->cpipe._write_fd) ==
      RET_FAILURE) {
    return INTERFACE_FAILURE;
  }
  pipe->cpipe._in_use = mtrue;
  pipe->cpipe._rclosed = mfalse;
  pipe->cpipe._wclosed = mfalse;
  return INTERFACE_SUCCESS;
}

minterfaceRet_t merry_destroy_pipe(MerryPipe *pipe) {
  if (pipe->interface_t != INTERFACE_TYPE_PIPE)
    return INTERFACE_TYPE_INVALID;
  merry_pipe_close_both_ends(pipe);
  free(pipe);
  return INTERFACE_SUCCESS;
}

#ifdef _USE_WIN_
void merry_pipe_prevent_inheritance(MerryPipe *pipe, int flag) {
  massert(pipe);
  switch (flag) {
  case _MERRY_IN_RFD_:
    SetHandleInformation(pipe->cpipe._read_handle, HANDLE_FLAG_INHERIT, 0);
    break;
  case _MERRY_IN_WFD_:
    SetHandleInformation(pipe->cpipe._write_handle, HANDLE_FLAG_INHERIT, 0);
    break;
  }
}
#endif
