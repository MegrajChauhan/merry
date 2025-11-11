#ifndef _USE_REQUEST_
#define _USE_REQUEST_

#include <merry_graves_request_queue.h>
#include <use_defs.h>
#include <use_utils.h>

extern result_t SEND_REQUEST(GravesRequest *creq) _ALIAS_(merry_SEND_REQUEST);

extern result_t SEND_REQUEST_async(GravesRequest *creq)
    _ALIAS_(merry_SEND_REQUEST_async);

#endif
