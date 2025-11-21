#ifndef _MERRY_ACCEPTOR_COMMANDS_
#define _MERRY_ACCEPTOR_COMMANDS_

typedef enum maccop_t maccop_t;
typedef enum maccconf_t maccconf_t;

enum maccop_t {
	ACC_RUN_INDEFINITELY,
	ACC_RUN_TILL_FULL,
	ACC_RUN_TILL_CUSTOM
};

enum maccconf_t {
	ACC_NO_ACCEPT_WHEN_FULL,
	ACC_TERMINATE_REQUESTS_WHEN_FULL
};


#endif
