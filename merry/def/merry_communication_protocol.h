#ifndef _MERRY_COMMUNICATION_PROTOCOL_
#define _MERRY_COMMUNICATION_PROTOCOL_

typedef enum mcommsop_t mcommsop_t;
typedef enum mcommsret_t mcommsret_t;
typedef enum mcommsmerryreq_t mcommsmerryreq_t;

enum mcommsret_t {
	RET_NOT_AUTHENTICATED,
	RET_INVALID_AUTH,
};

enum mcommsmerryreq_t {
	REQ_IDENTIFY, // send name of the core
};

enum mcommsop_t {
	COMMS_AUTH,
	COMMS_IDENTIFYING,
};

#endif
