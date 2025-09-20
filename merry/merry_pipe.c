#include <merry_pipe.h>

MerryPipe *merry_open_merrypipe(MerryErrorStack *st) {
  MerryPipe *p = (MerryPipe *)malloc(sizeof(MerryPipe));
  if (p == NULL) {
    PUSH(st, "Memory Allocation Failed", "Failed to allocate memory",
         "Creating PIPE");
    merry_error_stack_fatality(st);
    return RET_NULL;
  }

  if (merry_open_pipe(&p->_read_fd, &p->_write_fd, st) == RET_FAILURE) {
    PUSH(st, NULL, "Failed to initialize PIPE", "Creating PIPE");
    return RET_NULL;
  }

  p->_in_use = mtrue;
  p->_rclosed = mfalse;
  p->_wclosed = mfalse;
  return p;
}

void merry_pipe_close_read_end(MerryPipe *pipe) {
  merry_check_ptr(pipe);
#ifdef _USE_LINUX_
  if (pipe->_rclosed == mfalse)
    close(pipe->_read_fd);
  pipe->_rclosed = mtrue;
#elif defined(_USE_WIN_)
  if (!pipe->_rclosed && pipe->_read_handle != INVALID_HANDLE_VALUE)
    CloseHandle(pipe->_read_handle);
  pipe->_rclosed = mtrue;
#endif
}

void merry_pipe_close_write_end(MerryPipe *pipe) {
#ifdef _USE_LINUX_
  if (pipe->_wclosed == mfalse)
    close(pipe->_write_fd);
  pipe->_wclosed = mtrue;
#elif defined(_USE_WIN_)
  if (!pipe->_wclosed && pipe->_write_handle != INVALID_HANDLE_VALUE)
    CloseHandle(pipe->_write_handle);
  pipe->_wclosed = mtrue;
#endif
}

void merry_pipe_close_both_ends(MerryPipe *pipe) {
  merry_check_ptr(pipe);
  merry_pipe_close_read_end(pipe);
  merry_pipe_close_write_end(pipe);
  pipe->_in_use = mfalse;
}

mret_t merry_merrypipe_reopen(MerryPipe *pipe, MerryErrorStack *st) {
  merry_check_ptr(pipe);
  if (merry_open_pipe(&pipe->_read_fd, &pipe->_write_fd, st) == RET_FAILURE) {
    PUSH(st, NULL, "Failed to initialize PIPE", "Creating PIPE");
    return RET_FAILURE;
  }
  pipe->_in_use = mtrue;
  pipe->_rclosed = mfalse;
  pipe->_wclosed = mfalse;
  return RET_SUCCESS;
}

void merry_destroy_pipe(MerryPipe *pipe) {
  merry_pipe_close_both_ends(pipe);
  free(pipe);
}

#ifdef _USE_WIN_
void merry_pipe_prevent_inheritance(MerryPipe *pipe, int flag) {
  massert(pipe);
  switch (flag) {
  case _MERRY_IN_RFD_:
    SetHandleInformation(pipe->_read_handle, HANDLE_FLAG_INHERIT, 0);
    break;
  case _MERRY_IN_WFD_:
    SetHandleInformation(pipe->_write_handle, HANDLE_FLAG_INHERIT, 0);
    break;
  }
}
#endif
