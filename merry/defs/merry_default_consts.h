#ifndef _MERRY_DEFAULT_CONSTS_
#define _MERRY_DEFAULT_CONSTS_

/*--------Constants that are constant forever--------*/
#define MERRY_ERROR_STACK_DEPTH 5

/*--------Constants that are Configurable--------*/
// Page len: The number of bytes in a page(each page is 1MB is len)
#define _MERRY_PAGE_LEN_ 1048576
#define _MERRY_STACK_PAGE_LEN_ _MERRY_PAGE_LEN_

#define _MERRY_BYTES_PER_PAGE_ _MERRY_PAGE_LEN_
#define _MERRY_WORDS_PER_PAGE_ 524288
#define _MERRY_DWORD_PER_PAGE_ 262144
#define _MERRY_QWORD_PER_PAGE_ 131072

// 1 address represents 8 bytes
#define _MERRY_ADDRESSABLE_ADDRESSES_ON_STACK_ _MERRY_QWORD_PER_PAGE_

// #define _MERRY_EXECUTING_STATE_INIT_CAP_ 10
// #define _MERRY_WILD_REQUEST_QUEUE_LEN_ 10
// #define _MERRY_DEAD_PAGES_BUFFER_LEN_ 20

#endif
