#ifndef _MERRY_FILE_DEFS_
#define _MERRY_FILE_DEFS_

// File opening modes
#include <merry_platform.h>

// It is important to note that these are flags that merry will use internally
// but the user programs will be given different flags
// Not every flag can be implemented because we might want to give some
// workaround to them

#ifdef _USE_LINUX_

//// Opening modes
#define __FILE_MODE_APPEND O_APPEND
// #define __FILE_MODE_ASYNC O_ASYNC // Not supported because we will give our
// own way of doing asynchronous IO
#define __FILE_MODE_CLOEXEC O_CLOEXEC
#define __FILE_MODE_CREATE O_CREAT
// #define __FILE_MODE_NONBLOCK O_NONBLOCK // Merry will implement its own
// behavior
#define __FILE_MODE_READ O_RDONLY
#define __FILE_MODE_WRITE O_WRONLY
#define __FILE_MODE_READ_WRITE O_RDWR

//// File creation Modes
#define __FILE_CREATE_RWXU S_IRWXU // Read write execute for user
#define __FILE_CREATE_RUSR S_IRUSR // Read for user
#define __FILE_CREATE_WUSR S_IWUSR // write for user
#define __FILE_CREATE_XUSR S_IXUSR // execute for user

// Here group doesn't refer to the groups Graves uses
#define __FILE_CREATE_RWXG S_IRWXG // Read write execute for group
#define __FILE_CREATE_RGRP S_IRGRP // Read for group
#define __FILE_CREATE_WGRP S_IWGRP // write for group
#define __FILE_CREATE_XGRP S_IXGRP // execute for group

#define __FILE_CREATE_RWXO S_IRWXO // Read write execute for others
#define __FILE_CREATE_ROTH S_IROTH // Read for others
#define __FILE_CREATE_WOTH S_IWOTH // write for others
#define __FILE_CREATE_XOTH S_IXOTH // execute for others

#else
// not yet
#endif

// In Merry's case, we will have a separate section to pass on more flags
#define _MERRY_FOPEN_READ_ "r"
#define _MERRY_FOPEN_WRITE_ "w"
#define _MERRY_FOPEN_READ_WRITE_ "rw"
#define _MERRY_FOPEN_APPEND_ "a"

/*
 * structure: 8-bits and different flags
 * bit-0: create new file if not exists
 * bit-1: reserve
 * bit-2: read permission for user
 * bit-3: write permission for user
 * bit-4: execute permission for user
 * bit-5: read permission for group
 * bit-6: write permission for group
 * bit-7: execute permission for group
 * bit-8: read permission for others
 * bit-9: write permission for others
 * bit-10: execute permission for others
 * */
#define _MERRY_FOPEN_CREATE_ 0x01
// #define _MERRY_FOPEN_NONBLOCK_ 0x02

// permissions for new files
#define _MERRY_FOPEN_READ_PERMISSION_USR_ 0x04
#define _MERRY_FOPEN_WRITE_PERMISSION_USR_ 0x08
#define _MERRY_FOPEN_EXECUTE_PERMISSION_USR_ 0x10

#define _MERRY_FOPEN_READ_PERMISSION_GRP_ 0x20
#define _MERRY_FOPEN_WRITE_PERMISSION_GRP_ 0x40
#define _MERRY_FOPEN_EXECUTE_PERMISSION_GRP_ 0x80

#define _MERRY_FOPEN_READ_PERMISSION_OTH_ 0x100
#define _MERRY_FOPEN_WRITE_PERMISSION_OTH_ 0x200
#define _MERRY_FOPEN_EXECUTE_PERMISSION_OTH_ 0x400
#endif
