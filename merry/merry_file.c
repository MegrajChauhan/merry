#include <merry_file.h>

MerryFile *merry_open_file(mstr_t file_path, mstr_t modes, int flags,
                           mbool_t *failed, MerryErrorStack *st) {
  MerryFile *file = (MerryFile *)merry_interface_init(INTERFACE_FILE, st);
  if (!file) {
    PUSH(st, NULL, "Failed to open a file", "Opening a file");
    *failed = mfalse; // mtrue implies it is not a system error
    return RET_NULL;
  }
  // Figure out the opening mode
  int mode, flag;
  if (merry_figure_out_file_modes(modes, flags, &mode, &flag) == RET_FAILURE) {
    merry_interface_destroy(file);
    *failed = mtrue;
    return RET_NULL;
  }

  // With modes and flags, we can continue
  if (flags & _MERRY_FOPEN_NONBLOCK_) {
    // non block
    file->file.blocking = mfalse;
  } else {
    file->file.blocking = mtrue;
  }

#ifdef _USE_LINUX_
  file->file.fd = open(file_path, mode, flag);
  if (file->file.fd == -1) {
    *failed = mtrue;
    merry_error_stack_errno(st);
    merry_interface_destroy(file);
    return RET_NULL;
  }
#else
// not yet
#endif

  return file;
}

mret_t merry_figure_out_file_modes(mstr_t modex, int flags, int *res_mode,
                                   int *res_flag) {
  if (*modex == *_MERRY_FOPEN_APPEND_)
    *res_mode = __FILE_MODE_APPEND;
  else if (*modex == *_MERRY_FOPEN_WRITE_)
    *res_mode = __FILE_MODE_WRITE;
  else if (*modex == *_MERRY_FOPEN_WRITE_)
    *res_mode = __FILE_MODE_READ;
  else if (strcmp(modex, _MERRY_FOPEN_READ_WRITE_) == 0)
    *res_mode = __FILE_MODE_READ_WRITE;
  else
    return RET_FAILURE;

  if (flags & _MERRY_FOPEN_CREATE_) {
    *res_mode |= __FILE_MODE_CREATE;
    *res_flag = 0;
    if (flags & _MERRY_FOPEN_READ_PERMISSION_USR_)
      *res_flag |= __FILE_CREATE_RUSR;
    if (flags & _MERRY_FOPEN_READ_PERMISSION_GRP_)
      *res_flag |= __FILE_CREATE_RGRP;
    if (flags & _MERRY_FOPEN_READ_PERMISSION_OTH_)
      *res_flag |= __FILE_CREATE_ROTH;
    if (flags & _MERRY_FOPEN_WRITE_PERMISSION_USR_)
      *res_flag |= __FILE_CREATE_WUSR;
    if (flags & _MERRY_FOPEN_WRITE_PERMISSION_GRP_)
      *res_flag |= __FILE_CREATE_WGRP;
    if (flags & _MERRY_FOPEN_WRITE_PERMISSION_OTH_)
      *res_flag |= __FILE_CREATE_WOTH;
    if (flags & _MERRY_FOPEN_EXECUTE_PERMISSION_USR_)
      *res_flag |= __FILE_CREATE_XUSR;
    if (flags & _MERRY_FOPEN_EXECUTE_PERMISSION_GRP_)
      *res_flag |= __FILE_CREATE_XGRP;
    if (flags & _MERRY_FOPEN_EXECUTE_PERMISSION_OTH_)
      *res_flag |= __FILE_CREATE_XOTH;
  }
  return RET_SUCCESS;
}

mret_t merry_close_file(MerryFile *file, MerryErrorStack *st) {
  merry_check_ptr(file);
  if (file->interface_t != INTERFACE_FILE) {
    PUSH(st, "Invalid Operation", "\"close\" on a non-file interface",
         "Closing a File");
    merry_error_stack_fatality(st);
    return RET_FAILURE;
  }
#ifdef _USE_LINUX_
  close(file->file.fd);
#else
// not yet
#endif
  merry_interface_destroy(file);
  return RET_SUCCESS;
}
