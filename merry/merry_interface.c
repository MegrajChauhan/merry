#include <merry_interface.h>

MerryInterface *merry_interface_init(minterface_t type, MerryErrorStack *st) {
  MerryInterface *interface = (MerryInterface *)malloc(sizeof(MerryInterface));
  if (!interface) {
    PUSH(st, "Memory Allocation Failure",
         "Failed to allocate memory for interface", "Creating Interface");
    merry_error_stack_fatality(st);
    return RET_NULL;
  }
  interface->interface_t = type;
  return interface;
}

void merry_interface_destroy(MerryInterface *interface) {
  merry_check_ptr(interface);
  free(interface);
}
