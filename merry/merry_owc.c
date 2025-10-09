#include <merry_owc.h>

minterfaceRet_t merry_owc_speak(MerryOWC *owc, mbptr_t data, msize_t len) {
  // The OWC will speak for you
  // Send len bytes
  merry_check_ptr(owc);
  merry_check_ptr(data);
  if (owc->interface_t != INTERFACE_TYPE_PIPE)
    return INTERFACE_TYPE_INVALID;

  if (surelyF(owc->cpipe._wclosed)) {
    return INTERFACE_MISCONFIGURED; // maybe the channel was configured to be a
                                    // listener for this owner
  }
  if (surelyF(!len))
    return INTERFACE_SUCCESS;
  if (write(owc->cpipe._write_fd, (mptr_t)data, len) == -1) {
    return INTERFACE_HOST_FAILURE;
  }
  return INTERFACE_SUCCESS;
}

minterfaceRet_t merry_owc_listen(MerryOWC *owc, mbptr_t buf, msize_t n) {
  // The OWC will listen for you
  // Send len bytes
  merry_check_ptr(owc);
  merry_check_ptr(buf);
  if (owc->interface_t != INTERFACE_TYPE_PIPE)
    return INTERFACE_TYPE_INVALID;
  if (surelyF(owc->cpipe._rclosed)) {
    return INTERFACE_MISCONFIGURED; // maybe the channel was configured to be a
                                    // speaker for this owner
  }
  if (surelyF(!n))
    return INTERFACE_SUCCESS;

  if (read(owc->cpipe._read_fd, (mptr_t)buf, n) == -1) {
    return INTERFACE_HOST_FAILURE;
  }

  return INTERFACE_SUCCESS;
}
