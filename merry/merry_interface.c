#include <merry_interface.h>

mresult_t merry_interface_init(MerryInterface **interface, minterface_t type) {
  *interface = (MerryInterface *)malloc(sizeof(MerryInterface));
  if (!(*interface)) {
    return MRES_SYS_FAILURE;
  }
  (*interface)->interface_t = type;
  return MRES_SUCCESS;
}

void merry_interface_destroy(MerryInterface *interface) {
  merry_check_ptr(interface);
  free(interface);
}
