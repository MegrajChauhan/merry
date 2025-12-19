#ifndef _USE_INTERFACE_
#define _USE_INTERFACE_

#include <use_defs.h>
#include <use_utils.h>

struct MerryInterface;

typedef struct MerryInterface Interface;

result_t interface_init(Interface **interface, interface_t type);

void interface_destroy(Interface *interface);

#endif
