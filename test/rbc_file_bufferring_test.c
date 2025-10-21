#include <regr_core/lib/fs/rbc_file.h>

int READ_TEST(mstr_t str) {
  MLOG("", "\n------------------------READ--------------------", NULL);
  RBCFile *file = rbc_file_open(str, _MERRY_FOPEN_READ_, 0);
  if (!file) {
    MLOG("Test(Read)", "Failed to open file: err=%s", strerror(errno));
    return 1;
  }

  minterfaceRet_t res;

  mbyte_t BUF[10] = {0};

  // we will read chunks of 8 bytes and at last seek back
  // to read the entire file
  msize_t file_len = 0;
  res = rbc_fseek(file, 0, SEEK_END);
  if (res != INTERFACE_SUCCESS) {
    MLOG("Test(Read)", "Failed to seek to end of file: err=%s",
         strerror(errno));
    rbc_file_destroy(file);
    return 1;
  }

  res = rbc_ftell(file, &file_len);
  if (res != INTERFACE_SUCCESS) {
    MLOG("Test(Read)", "Failed to read the size of file: err=%s",
         strerror(errno));
    rbc_file_destroy(file);
    return 1;
  }

  MLOG("Test(Read)", "File size=%zu", file_len);
  res = rbc_fseek(file, 0, SEEK_SET);
  if (res != INTERFACE_SUCCESS) {
    MLOG("Test(Read)", "Failed to seek to start of file: err=%s",
         strerror(errno));
    rbc_file_destroy(file);
    return 1;
  }

  for (msize_t i = 0; i < (file_len / 8); i++) {
    res = rbc_fread(file, BUF, 8);
    if (res != INTERFACE_SUCCESS) {
      MLOG("Test(Read)", "Failed to read from file: err=%s", strerror(errno));
      rbc_file_destroy(file);
      return 1;
    }
    MLOG("Test(Read)", "Read from file: %s", BUF);
  }
  // We won't read any leftover bytes
  res = rbc_fseek(file, 0, SEEK_SET);
  if (res != INTERFACE_SUCCESS) {
    MLOG("Test(Read)", "Failed to seek to start of file: err=%s",
         strerror(errno));
    rbc_file_destroy(file);
    return 1;
  }
  mbyte_t another_buf[file_len + 1];
  memset(another_buf, 0, file_len + 1);
  res = rbc_fread(file, another_buf, file_len);
  if (res != INTERFACE_SUCCESS) {
    MLOG("Test(Read)", "Failed to read from file: err=%s", strerror(errno));
    rbc_file_destroy(file);
    return 1;
  }
  MLOG("Test(Read)", "FILE CONTENTS: \n%s", another_buf);

  rbc_file_destroy(file);
  return 0;
}

int WRITE_TEST(mstr_t path) {
  MLOG("", "\n------------------------WRITE--------------------", NULL);
  RBCFile *file =
      rbc_file_open(path, _MERRY_FOPEN_WRITE_,
                    _MERRY_FOPEN_CREATE_ | _MERRY_FOPEN_READ_PERMISSION_USR_ |
                        _MERRY_FOPEN_WRITE_PERMISSION_USR_);
  if (!file) {
    MLOG("Test(Write)", "Failed to open file: err=%s", strerror(errno));
    return 1;
  }

  minterfaceRet_t res;

  // We will ask for number of bytes from the user and then read
  // it to write that in chunks
  msize_t len;
  printf("Test(Write): Enter the number of bytes to write: ");
  // We will read in chunks of 8 and hence if len is not a multiple
  // of 8 then SORRY
  scanf("%zu", &len);
  mbyte_t buf[9] = {0};

  for (msize_t i = 0; i < (len / 8); i++) {
    printf("Test(Write): >>> ");
    scanf("%s", buf);
    res = rbc_fwrite(file, buf, 8);
    if (res != INTERFACE_SUCCESS) {
      MLOG("Test(Write)", "Failed to write to file: err=%s", strerror(errno));
      rbc_file_destroy(file);
      return 1;
    }
  }

  rbc_file_destroy(file);
  return 0;
}

int APPEND_TEST(mstr_t path) {
  MLOG("", "\n------------------------APPEND--------------------", NULL);
  RBCFile *file =
      rbc_file_open(path, _MERRY_FOPEN_APPEND_,
                    _MERRY_FOPEN_CREATE_ | _MERRY_FOPEN_READ_PERMISSION_USR_ |
                        _MERRY_FOPEN_WRITE_PERMISSION_USR_);
  if (!file) {
    MLOG("Test(Append)", "Failed to open file: err=%s", strerror(errno));
    return 1;
  }

  minterfaceRet_t res;

  // Same as write
  msize_t len;
  printf("Test(Append): Enter the number of bytes to append: ");
  scanf("%zu", &len);
  mbyte_t buf[9] = {0};

  for (msize_t i = 0; i < (len / 8); i++) {
    printf("Test(Append): >>> ");
    scanf("%s", buf);
    res = rbc_fwrite(file, buf, 8);
    if (res != INTERFACE_SUCCESS) {
      MLOG("Test(Append)", "Failed to append to file: err=%s", strerror(errno));
      rbc_file_destroy(file);
      return 1;
    }
  }

  rbc_file_destroy(file);
  return 0;
}

int READ_WRITE_TEST(mstr_t path) {
  MLOG("", "\n------------------------REST/WRITE--------------------", NULL);
  RBCFile *file =
      rbc_file_open(path, _MERRY_FOPEN_READ_WRITE_,
                    _MERRY_FOPEN_CREATE_ | _MERRY_FOPEN_READ_PERMISSION_USR_ |
                        _MERRY_FOPEN_WRITE_PERMISSION_USR_);
  if (!file) {
    MLOG("Test(RW)", "Failed to open file: err=%s", strerror(errno));
    return 1;
  }

  // Here we will perform chunk reads much like above but not for the
  // full file and then follow up with writes and then again reads
  minterfaceRet_t res;

  mbyte_t BUF[10] = {0};

  msize_t file_len = 0;
  res = rbc_fseek(file, 0, SEEK_END);
  if (res != INTERFACE_SUCCESS) {
    MLOG("Test(RW)", "Failed to seek to end of file: err=%s", strerror(errno));
    rbc_file_destroy(file);
    return 1;
  }

  res = rbc_ftell(file, &file_len);
  if (res != INTERFACE_SUCCESS) {
    MLOG("Test(RW)", "Failed to read the size of file: err=%s",
         strerror(errno));
    rbc_file_destroy(file);
    return 1;
  }

  MLOG("Test(Read)", "File size=%zu", file_len);
  rbc_fseek(file, 0, SEEK_SET);

  // We will read 1 chunk of 8 bytes and then write afterwards
  msize_t first_read = 8;
  if (file_len < 8)
    first_read = file_len;

  res = rbc_fread(file, BUF, first_read);
  if (res != INTERFACE_SUCCESS) {
    MLOG("Test(RW)", "Failed to read from file: err=%s", strerror(errno));
    rbc_file_destroy(file);
    return 1;
  }
  MLOG("Test(RW)", "Read from file: %s", BUF);

  // Now we will write some chunksmsize_t len;
  msize_t len;
  printf("Test(RW): Enter the number of chunks(len=8) to write: ");
  scanf("%zu", &len);
  mbyte_t buf[9] = {0};

  for (msize_t i = 0; i < (len); i++) {
    printf("Test(RW): >>> ");
    scanf("%s", buf);
    res = rbc_fwrite(file, buf, 8);
    if (res != INTERFACE_SUCCESS) {
      MLOG("Test(RW)", "Failed to write to file: err=%s", strerror(errno));
      rbc_file_destroy(file);
      return 1;
    }
  }

  // Now we will try to read some bytes
  // Since we will be at the end of the file, we should get EOF
  res = rbc_fread(file, BUF, 8);
  if (res != INTERFACE_SUCCESS) {
    MLOG("Test(RW)", "Failed to read from file: err=%s", strerror(errno));
    rbc_file_destroy(file);
    return 1;
  }
  MLOG("Test(RW)", "Read from file: %s", BUF);

  // Now we will seek to the start and read the whole file
  res = rbc_fseek(file, 0, SEEK_END);
  if (res != INTERFACE_SUCCESS) {
    MLOG("Test(RW)", "Failed to seek to end of file: err=%s", strerror(errno));
    rbc_file_destroy(file);
    return 1;
  }

  res = rbc_ftell(file, &file_len);
  if (res != INTERFACE_SUCCESS) {
    MLOG("Test(RW)", "Failed to read the size of file: err=%s",
         strerror(errno));
    rbc_file_destroy(file);
    return 1;
  }

  MLOG("Test(RW)", "File size=%zu", file_len);
  res = rbc_fseek(file, 0, SEEK_SET);
  if (res != INTERFACE_SUCCESS) {
    MLOG("Test(RW)", "Failed to seek to start of file: err=%s",
         strerror(errno));
    rbc_file_destroy(file);
    return 1;
  }
  mbyte_t another_buf[file_len + 1];
  memset(another_buf, 0, file_len + 1);
  res = rbc_fread(file, another_buf, file_len);
  if (res != INTERFACE_SUCCESS) {
    MLOG("Test(RW)", "Failed to read from file: err=%s", strerror(errno));
    rbc_file_destroy(file);
    return 1;
  }
  MLOG("Test(RW)", "FILE CONTENTS: \n%s", another_buf);

  rbc_file_destroy(file);
  return 0;
}

int main() {
  if (READ_TEST("./tc_test_9.c"))
    return 0;
  if (WRITE_TEST("./rbc_write_test.txt"))
    return 0;
  if (READ_TEST("./rbc_write_test.txt"))
    return 0;
  if (APPEND_TEST("./rbc_write_test.txt"))
    return 0;
  if (READ_TEST("./rbc_write_test.txt"))
    return 0;
  if (READ_WRITE_TEST("./rbc_write_test.txt"))
    return 0;
  return 0;
}
