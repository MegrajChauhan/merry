#include <regr_core/lib/fs/rbc_file.h>

mresult_t rbc_file_open(RBCFile **file, mstr_t fpath, mstr_t mode, int flags) {
  merry_check_ptr(fpath);
  if (merry_is_path_a_directory(fpath)) {
    MFATAL("RBC<LIB:fs>",
           "The given file path either doesn't exist or is most likely a "
           "directory: PATH=%s",
           fpath);
    return MRES_INVALID_ARGS;
  }
  *file = (RBCFile *)malloc(sizeof(RBCFile));
  if (!file) {
    MFATAL("RBC<LIB:fs>", "Failed to allocate memory for FILE: PATH=%s", fpath);
    return MRES_SYS_FAILURE;
  }
  if (((*file)->buf = malloc(_RBC_FILE_BUF_LEN_)) == NULL) {
    MFATAL("RBC<LIB:fs>", "Failed to allocate memory for FILE BUF: PATH=%s",
           fpath);
    free(*file);
    return MRES_SYS_FAILURE;
  }
  mresult_t res;
  if ((res = merry_open_file(&(*file)->file, fpath, mode, flags)) !=
      MRES_SUCCESS) {
    MFATAL("RBC<LIB:fs>", "Failed to open file: PATH=%s", fpath);
    free((*file)->buf);
    free(*file);
    return res;
  }
  (*file)->BP = 0;
  (*file)->usable_bytes_in_buffer = 0;
  merry_file_tell((*file)->file, &(*file)->actual_file_off);
  return MRES_SUCCESS;
}

mresult_t rbc_file_reopen(RBCFile *file, mstr_t fpath, mstr_t mode, int flags) {
  merry_check_ptr(file);
  merry_check_ptr(fpath);
  if (merry_is_path_a_directory(fpath)) {
    MFATAL("RBC<LIB:fs>",
           "The given file path either doesn't exist or is most likely a "
           "directory: PATH=%s",
           fpath);
    return MRES_INVALID_ARGS;
  }
  mresult_t res;
  if ((res = merry_open_file(&file->file, fpath, mode, flags)) !=
      MRES_SUCCESS) {
    MFATAL("RBC<LIB:fs>", "Failed to open file: PATH=%s", fpath);
    free(file->buf);
    free(file);
    return res;
  }
  file->BP = 0;
  file->usable_bytes_in_buffer = 0;
  merry_file_tell(file->file, &file->actual_file_off);
  return MRES_SUCCESS;
}

mresult_t rbc_fseek(RBCFile *file, msqword_t off, msize_t whence) {
  merry_check_ptr(file);
  merry_check_ptr(file->file);
  // If there is anything in the buffer, write it back
  if (!file->file->file.flags.file_opened)
    return MRES_MISCONFIGURED_INVALID;
  if (file->file->file.flags.read && !file->file->file.flags.write &&
      !file->file->file.flags.append) {
    file->BP = 0;
    file->usable_bytes_in_buffer = 0;
    merry_file_seek(file->file, file->actual_file_off, SEEK_SET);
  } else if ((!file->file->file.flags.read && file->file->file.flags.write) ||
             file->file->file.flags.append) {
    mresult_t res;
    if (file->BP != 0) {
      res = rbc_file_flush(file); // this will update the fields
      if (res != MRES_SUCCESS)
        return res;
    }
  } else if (file->file->file.flags.read && file->file->file.flags.write) {
    // Read/Write mode
    // usable_bytes_in_buffer refers to usable bytes for Read
    // and hence using that we can check what the state of the
    // buffer is
    if (file->last_oper == _RBC_LAST_OPER_WRITE_) {
      // The buffer is in write state
      mresult_t res;
      if (file->BP != 0) {
        res = rbc_file_flush(file); // this will update the fields
        if (res != MRES_SUCCESS)
          return res;
      }
    } else {
      // The buffer is in read state
      file->BP = 0;
      file->usable_bytes_in_buffer = 0;
      merry_file_seek(file->file, file->actual_file_off, SEEK_SET);
    }
  } else
    return MRES_MISCONFIGURED_INVALID;
  // update actual_file_off as well
  mresult_t ret = merry_file_seek(file->file, off, whence);
  if (ret != MRES_SUCCESS)
    return ret;
  file->actual_file_off = file->file->file.res;
  return MRES_SUCCESS;
}

mresult_t rbc_ftell(RBCFile *file, msize_t *off) {
  merry_check_ptr(file);
  merry_check_ptr(file->file);
  // If there is anything in the buffer, write it back
  if (!file->file->file.flags.file_opened)
    return MRES_MISCONFIGURED_INVALID;
  if (!file->file->file.flags.read && file->file->file.flags.write) {
    mresult_t res;
    if (file->BP != 0) {
      res = rbc_file_flush(file);
      if (res != MRES_SUCCESS)
        return res;
    }
  } else if (file->file->file.flags.read && file->file->file.flags.write) {
    if (file->last_oper == _RBC_FMODE_WRITE_) {
      mresult_t res;
      if (file->BP != 0) {
        res = rbc_file_flush(file);
        if (res != MRES_SUCCESS)
          return res;
      }
    }
  }
  return merry_file_tell(file->file, off);
}

_MERRY_ALWAYS_INLINE_ mresult_t rbc_fread(RBCFile *file, mbptr_t buf,
                                          msize_t num_of_bytes) {
  merry_check_ptr(file);
  merry_check_ptr(file->file);
  merry_check_ptr(buf);
  if (!file->file->file.flags.file_opened)
    return MRES_MISCONFIGURED_INVALID;
  if (!file->file->file.flags.read)
    return MRES_MISCONFIGURED_INVALID;
  if (num_of_bytes == 0) {
    file->file->file.res = 0;
    return MRES_SUCCESS;
  }
  if (file->file->file.flags.write) {
    // Opened for read and write
    if (file->last_oper == _RBC_LAST_OPER_WRITE_) {
      // Last operation was a write so reset it
      // for read mode again
      mresult_t res;
      if (file->BP != 0) {
        res = rbc_file_flush(file);
        if (res != MRES_SUCCESS)
          return res;
      }
      // rbc_file_flush resets the fields
    }
  }
  // Now the read could be gigantic(bigger than our buffer even)
  // For such cases, we actually won't buffer anything
  file->last_oper = _RBC_LAST_OPER_READ_;
  mresult_t ret = MRES_SUCCESS;
  if (file->usable_bytes_in_buffer) {
    if (num_of_bytes > file->usable_bytes_in_buffer) {
      memcpy(buf, file->buf + file->BP, file->usable_bytes_in_buffer);
      num_of_bytes -= file->usable_bytes_in_buffer;
      ret = merry_file_read(file->file, buf + file->usable_bytes_in_buffer,
                            num_of_bytes);
      if (ret != MRES_SUCCESS)
        return ret;
      // We won't populate the buffer here with multiple
      // reads. The reason being: num_of_bytes may as
      // well be n*_RBC_FILE_BUF_LEN_ and it won't be
      // intelligent to handle that
      file->file->file.res += file->usable_bytes_in_buffer;
      file->actual_file_off += num_of_bytes;
      file->usable_bytes_in_buffer = 0;
      file->BP = 0;
    } else {
      memcpy(buf, file->buf + file->BP, num_of_bytes);
      file->file->file.res = num_of_bytes; // how many bytes was read
      file->BP += num_of_bytes;
      file->usable_bytes_in_buffer -= num_of_bytes;
    }
  } else {
    // we don't have anything in the buffer
    if (num_of_bytes > _RBC_FILE_BUF_LEN_) {
      // In such case, we won't even try buffering
      // since the num_of_bytes may as well be
      // n*_RBC_FILE_BUF_LEN_ which won't look beautiful with
      // a very large number of reads
      ret = merry_file_read(file->file, buf, num_of_bytes);
      if (ret != MRES_SUCCESS)
        return ret;
      file->actual_file_off += file->file->file.res;
    } else {
      // We will buffer the read and give the required data to
      // the buffer
      ret = merry_file_read(file->file, file->buf, _RBC_FILE_BUF_LEN_);
      if (ret != MRES_SUCCESS)
        return ret;
      file->actual_file_off += file->file->file.res;
      // The file might not have had the requested number
      // of bytes in the first place
      file->usable_bytes_in_buffer = file->file->file.res;
      file->BP = 0;
      if (file->usable_bytes_in_buffer < num_of_bytes) {
        memcpy(buf, file->buf, file->usable_bytes_in_buffer);
        file->BP = 0;
        file->usable_bytes_in_buffer = 0;
      } else {
        memcpy(buf, file->buf, num_of_bytes);
        file->BP = num_of_bytes;
        file->usable_bytes_in_buffer -= num_of_bytes;
        file->file->file.res = num_of_bytes;
      }
    }
  }

  return ret;
}

mresult_t rbc_fwrite(RBCFile *file, mbptr_t buf, msize_t num_of_bytes) {
  merry_check_ptr(file);
  merry_check_ptr(file->file);
  merry_check_ptr(buf);
  if (!file->file->file.flags.file_opened)
    return MRES_MISCONFIGURED_INVALID;
  if (!file->file->file.flags.write && !file->file->file.flags.append)
    return MRES_MISCONFIGURED_INVALID;
  if (num_of_bytes == 0) {
    file->file->file.res = 0;
    return MRES_SUCCESS;
  }
  if (file->file->file.flags.read) {
    // Opened for read and write
    if (file->last_oper == _RBC_LAST_OPER_READ_) {
      // Last operation was a read so reset it
      // for write mode again
      file->BP = 0;
      file->usable_bytes_in_buffer = 0;
    }
  }
  // Now the write could be gigantic(bigger than our buffer even)
  // For such cases, we actually won't buffer anything
  file->last_oper = _RBC_LAST_OPER_WRITE_;
  mresult_t ret = MRES_SUCCESS;
  if ((msize_t)(_RBC_FILE_BUF_LEN_ - file->BP) >= num_of_bytes) {
    // We can fit it all in the buffer
    memcpy(file->buf + file->BP, buf, num_of_bytes);
    file->actual_file_off += num_of_bytes;
    file->BP += num_of_bytes;
  } else {
    // We can't fit it all in the buffer.
    // Now either num_of_bytes is a huge number
    // (eg. n*_RBC_FILE_BUF_LEN_) which doesn't feel good to
    // handle so we won't buffer such cases.
    // Here we will need two calls to merry_file_write since
    // we will have to first flush the buffer and then write the
    // data
    if ((ret = rbc_file_flush(file)) != MRES_SUCCESS)
      return ret;
    // maybe now we can fit it?
    if (num_of_bytes <= _RBC_FILE_BUF_LEN_) {
      memcpy(file->buf, buf, num_of_bytes);
      file->BP = num_of_bytes;
    } else {
      // rbc_file_flush will reset the fields obviously
      ret = merry_file_write(file->file, buf, num_of_bytes);
      if (ret != MRES_SUCCESS)
        return ret;
    }
    file->actual_file_off += num_of_bytes;
  }
  return ret;
}

_MERRY_ALWAYS_INLINE_ mresult_t rbc_file_close(RBCFile *file) {
  // Just close the file and not free the resources(for re-use)
  merry_check_ptr(file);
  if (!file->file->file.flags.file_opened)
    return MRES_MISCONFIGURED_INVALID;
  mresult_t ret = MRES_SUCCESS;
  if (file->BP != 0 || file->usable_bytes_in_buffer != 0)
    ret = rbc_file_flush(file);
  if (ret != MRES_SUCCESS)
    return ret;
  ret = merry_close_file(file->file);
  return ret;
}

void rbc_file_destroy(RBCFile *file) {
  merry_check_ptr(file);
  merry_check_ptr(file->file);
  if (file->file->file.flags.file_opened)
    rbc_file_close(file);
  merry_destroy_file(file->file);
  free(file->buf);
  free(file);
}

mresult_t rbc_file_flush(RBCFile *file) {
  merry_check_ptr(file);
  merry_check_ptr(file->file);
  if (!file->file->file.flags.file_opened)
    return MRES_MISCONFIGURED_INVALID;
  if (!file->file->file.flags.write && !file->file->file.flags.append)
    return MRES_MISCONFIGURED_INVALID;
  if (file->file->file.flags.read) {
    // in r/w mode
    if (file->last_oper == _RBC_LAST_OPER_READ_) {
      file->usable_bytes_in_buffer = 0;
      return MRES_SUCCESS; // we can't do anything here
    }
  }
  // now it means we are in write or append mode so
  mresult_t ret = merry_file_write(file->file, file->buf, file->BP);
  if (ret != MRES_SUCCESS)
    return ret;
  file->BP = 0;
  return ret;
}
