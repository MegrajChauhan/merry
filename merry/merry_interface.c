#include <merry_interface.h>

MerryInterface *merry_interface_init(minterface_t type) {
  MerryInterface *interface = (MerryInterface *)malloc(sizeof(MerryInterface));
  if (!interface) {
    return RET_NULL;
  }
  interface->interface_t = type;
  return interface;
}

void merry_interface_destroy(MerryInterface *interface) {
  merry_check_ptr(interface);
  free(interface);
}
