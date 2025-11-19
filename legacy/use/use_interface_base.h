#ifndef _USE_INTERFACE_BASE_
#define _USE_INTERFACE_BASE_

#include <merry_interface.h>
#include <use_defs.h>
#include <use_utils.h>

typedef MerryInterface Interface;

extern result_t interface_init(Interface **interface, interface_t type)
    _ALIAS_(merry_interface_init);

extern void interface_destroy(Interface *interface)
    _ALIAS_(merry_interface_destroy);

#endif
