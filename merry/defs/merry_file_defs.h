#ifndef _MERRY_FILE_DEFS_
#define _MERRY_FILE_DEFS_

// This will include the flags that the user programs can infact use

// In Merry's case, we will have a separate section to pass on more flags
#define _MERRY_FOPEN_READ_ "r"
#define _MERRY_FOPEN_WRITE_ "w"
#define _MERRY_FOPEN_READ_WRITE_ "rw"
#define _MERRY_FOPEN_APPEND_ "a"

/*
 * structure: 8-bits and different flags
 * bit-0: create new file if not exists
 * bit-1: blocking(0) and non-blocking(1)
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
#define _MERRY_FOPEN_NONBLOCK_ 0x02

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
