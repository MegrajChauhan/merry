#include <merry_interface.h>

mresult_t merry_interface_init(MerryInterface **interface, minterface_t type) {
  if (!interface)
    return MRES_INVALID_ARGS;
  *interface = (MerryInterface *)malloc(sizeof(MerryInterface));
  if (!(*interface)) {
    return MRES_SYS_FAILURE;
  }
  (*interface)->interface_t = type;
  return MRES_SUCCESS;
}

void merry_interface_destroy(MerryInterface *interface) {
  if (!interface)
    return;
  free(interface);
}
