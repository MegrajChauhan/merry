#include <merry_file.h>

mresult_t merry_open_file(MerryFile *file, mstr_t file_path,
                                int modes, int flags) {
#ifdef _USE_LINUX_
  (*file) = open(file_path, modes, flags);
  if ((*file) == -1) {
    return MRES_SYS_FAILURE;
  }
#else
// not yet
#endif
  return MRES_SUCCESS;
}

mresult_t merry_close_file(MerryFile *file) {
  merry_check_ptr(file);
#ifdef _USE_LINUX_
  close(file->file.fd);
#else
// not yet
#endif
  return INTERFACE_SUCCESS;
}

mresult_t merry_file_size(MerryFile *file, msize_t *res) {
  merry_check_ptr(file);
#ifdef _USE_LINUX_
  msqword_t old_pos = lseek(*file, 0, SEEK_CUR);
  *res = lseek(*file, 0, SEEK_END);
  lseek(*file, 0, old_pos);
#endif
  return INTERFACE_SUCCESS;
}

mresult_t merry_file_seek(MerryFile *file, msqword_t off,
                                msize_t whence) {
  merry_check_ptr(file);
  if (whence != SEEK_CUR && whence != SEEK_END && whence != SEEK_SET)
    return INTERFACE_INVALID_ARGS;
#ifdef _USE_LINUX_
  if (( lseek(*file, off, whence)) == -1)
    return INTERFACE_HOST_FAILURE;
#endif
  return INTERFACE_SUCCESS;
}

mresult_t merry_file_tell(MerryFile *file, msize_t *off) {
  merry_check_ptr(file);
  merry_check_ptr(off);
#ifdef _USE_LINUX_
  if ((msqword_t)(*off = lseek(*file, 0, SEEK_CUR)) == -1)
    return INTERFACE_HOST_FAILURE;
#endif
  return INTERFACE_SUCCESS;
}

mresult_t merry_file_read(MerryFile *file, mbptr_t buf,
                                msize_t num_of_bytes) {
  merry_check_ptr(file);
  merry_check_ptr(buf);
  if (!num_of_bytes)
    return INTERFACE_SUCCESS;

  read(*file, buf, num_of_bytes);
  return INTERFACE_SUCCESS;
}

mresult_t merry_file_write(MerryFile *file, mbptr_t buf,
                                 msize_t num_of_bytes) {
  merry_check_ptr(file);
  merry_check_ptr(buf);
  if (!num_of_bytes)
    return INTERFACE_SUCCESS;

  write(*ile, buf, num_of_bytes);
  return INTERFACE_SUCCESS;
}
