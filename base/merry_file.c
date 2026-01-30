#include <merry_file.h>

mresult_t merry_open_file(MerryFile **file, mstr_t file_path, mstr_t modes,
                          int flags) {
  if (!file || !file_path || !modes) return MRES_INVALID_ARGS;
  mresult_t res = merry_interface_init(file, INTERFACE_TYPE_FILE);
  if (!(*file)) {
    return res;
  }
  // Figure out the opening mode
  int mode, flag;
  if (merry_figure_out_file_modes(modes, flags, &mode, &flag) == RET_FAILURE) {
    merry_interface_destroy(*file);
    return MRES_INVALID_ARGS;
  }

  // With modes and flags, we can continue

#ifdef _USE_LINUX_
  (*file)->file.fd = open(file_path, mode, flag);
  if ((*file)->file.fd == -1) {
    merry_interface_destroy(*file);
    return MRES_SYS_FAILURE;
  }
#else
// not yet
#endif
  (*file)->file.flags.file_opened = 1;
  (*file)->file.flags.append = 0;
  (*file)->file.flags.read = 0;
  (*file)->file.flags.write = 0;
  if (modes[0] == _MERRY_FOPEN_APPEND_[0])
    (*file)->file.flags.append = 1;
  else if (modes[0] == _MERRY_FOPEN_WRITE_[0])
    (*file)->file.flags.write = 1;
  else if (strcmp(modes, _MERRY_FOPEN_READ_WRITE_) == 0) {
    (*file)->file.flags.read = 1;
    (*file)->file.flags.write = 1;
  } else if (modes[0] == _MERRY_FOPEN_READ_[0])
    (*file)->file.flags.read = 1;
  return MRES_SUCCESS;
}

mret_t merry_figure_out_file_modes(mstr_t modex, int flags, int *res_mode,
                                   int *res_flag) {
  if (modex[0] == _MERRY_FOPEN_APPEND_[0]) {
    *res_mode = __FILE_MODE_APPEND;
    *res_mode |= __FILE_MODE_WRITE;
  } else if (modex[0] == _MERRY_FOPEN_WRITE_[0]) {
    *res_mode = __FILE_MODE_WRITE;
  } else if (strcmp(modex, _MERRY_FOPEN_READ_WRITE_) == 0) {
    *res_mode = __FILE_MODE_READ_WRITE;
  } else if (modex[0] == _MERRY_FOPEN_READ_[0]) {
    *res_mode = __FILE_MODE_READ;
  } else
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

mresult_t merry_close_file(MerryFile *file) {
  if (!file) return MRES_INVALID_ARGS;
  if (file->interface_t != INTERFACE_TYPE_FILE) {
    return MRES_UNEXPECTED;
  }
  if (!file->file.flags.file_opened)
    return MRES_CONFIGURATION_INVALID;
#ifdef _USE_LINUX_
  close(file->file.fd);
#else
// not yet
#endif
  file->file.flags.file_opened = mfalse;
  return MRES_SUCCESS;
}

mresult_t merry_destroy_file(MerryFile *file) {
  if (!file) return MRES_INVALID_ARGS;
  if (file->interface_t != INTERFACE_TYPE_FILE) {
    return MRES_UNEXPECTED;
  }
  if (file->file.flags.file_opened) {
    merry_close_file(file);
  }
  merry_interface_destroy(file);
  return MRES_SUCCESS;
}

mresult_t merry_file_size(MerryFile *file, msize_t *res) {
  if (!file || !res) return MRES_INVALID_ARGS;
  if (file->interface_t != INTERFACE_TYPE_FILE)
    return MRES_UNEXPECTED;
  if (!file->file.flags.file_opened)
    return MRES_CONFIGURATION_INVALID;
#ifdef _USE_LINUX_
  msqword_t old_pos = lseek(file->file.fd, 0, SEEK_CUR);
  *res = lseek(file->file.fd, 0, SEEK_END);
  lseek(file->file.fd, 0, old_pos);
#endif
  return MRES_SUCCESS;
}

mresult_t merry_file_seek(MerryFile *file, mqptr_t _res, msqword_t off,
                          msize_t whence) {
  if (!file) return MRES_INVALID_ARGS;
  if (file->interface_t != INTERFACE_TYPE_FILE)
    return MRES_UNEXPECTED;
  if (!file->file.flags.file_opened)
    return MRES_CONFIGURATION_INVALID;
  if (whence != SEEK_CUR && whence != SEEK_END && whence != SEEK_SET)
    return MRES_INVALID_ARGS;
#ifdef _USE_LINUX_
  mqword_t len;
  if ((len = lseek(file->file.fd, off, whence)) == -1)
    return MRES_SYS_FAILURE;
  if (_res)
    *_res = len;
#endif
  return MRES_SUCCESS;
}

mresult_t merry_file_tell(MerryFile *file, msize_t *off) {
 if (!file || !off) return MRES_INVALID_ARGS;
 if (file->interface_t != INTERFACE_TYPE_FILE)
    return MRES_UNEXPECTED;
  if (!file->file.flags.file_opened)
    return MRES_CONFIGURATION_INVALID;
#ifdef _USE_LINUX_
  if ((msqword_t)(*off = lseek(file->file.fd, 0, SEEK_CUR)) == -1)
    return MRES_SYS_FAILURE;
#endif
  return MRES_SUCCESS;
}

mresult_t merry_file_read(MerryFile *file, mqptr_t _res, mbptr_t buf,
                          msize_t num_of_bytes) {
  if (!file || !buf) return MRES_INVALID_ARGS;  
  if (file->interface_t != INTERFACE_TYPE_FILE)
    return MRES_UNEXPECTED;
  if (!file->file.flags.file_opened || !file->file.flags.read)
    return MRES_CONFIGURATION_INVALID;
  if (!num_of_bytes)
    return MRES_SUCCESS;

  msqword_t len = read(file->file.fd, buf, num_of_bytes);
  if (len == -1)
    return MRES_SYS_FAILURE;

  if (_res)
    *_res = len;
  return MRES_SUCCESS;
}

mresult_t merry_file_write(MerryFile *file, mqptr_t _res, mbptr_t buf,
                           msize_t num_of_bytes) {
  if (!file || !buf) return MRES_INVALID_ARGS;
  if (file->interface_t != INTERFACE_TYPE_FILE)
    return MRES_UNEXPECTED;
  if (!file->file.flags.file_opened ||
      (!file->file.flags.write && !file->file.flags.append))
    return MRES_CONFIGURATION_INVALID;
  if (!num_of_bytes)
    return MRES_SUCCESS;

  msqword_t len = write(file->file.fd, buf, num_of_bytes);
  if (len == -1)
    return MRES_SYS_FAILURE;
  if (_res)
    *_res = len;
  return MRES_SUCCESS;
}
