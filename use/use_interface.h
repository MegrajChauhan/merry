#ifndef _USE_INTERFACE_
#define _USE_INTERFACE_

#include <use_defs.h>

struct MerryInterface;

typedef struct MerryInterface Interface;

extern result_t interface_init(Interface **interface, interface_t type)
    _ALIAS_(merry_interface_init);

extern void interface_destroy(Interface *interface)
    _ALIAS_(merry_interface_destroy);

#endif
