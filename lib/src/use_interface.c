#include <use_interface.h>
#include <interface/merry_interface.h>

_ALWAYS_INLINE_ result_t interface_init(Interface **interface, interface_t type) {
	return merry_interface_init(interface, type);
}

_ALWAYS_INLINE_ void interface_destroy(Interface *interface) {
	return merry_interface_destroy(interface);
}
