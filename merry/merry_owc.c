#include <merry_owc.h>

mret_t merry_owc_speak(MerryOWC *owc, mbptr_t data, msize_t len,
                       MerryErrorStack *st) {
  // The OWC will speak for you
  // Send len bytes
  merry_check_ptr(owc);
  merry_check_ptr(data);
  if (surelyF(!len))
    return RET_SUCCESS;

  if (surelyF(owc->_wclosed)) {
    PUSH(st, "Misconfigured OWC",
         "OWC couldn't speak because it was misconfigured", "OWC Speaking");
    // What should we do? Since we decided to be strict we should treat this
    // seriously..?
    merry_error_stack_fatality(st);
    return RET_FAILURE; // maybe the channel was configured to be a listener for
                        // this owner
  }
  if (write(owc->_write_fd, (mptr_t)data, len) == -1) {
    merry_error_stack_errno(st);
    PUSH(st, NULL, "Failed to write to OWC", "OWC Speaking");
    return RET_FAILURE;
  }
  return RET_SUCCESS;
}

mret_t merry_owc_listen(MerryOWC *owc, mbptr_t buf, msize_t n,
                        MerryErrorStack *st) {
  // The OWC will listen for you
  // Send len bytes
  merry_check_ptr(owc);
  merry_check_ptr(buf);
  if (surelyF(!n))
    return RET_SUCCESS;
  if (surelyF(owc->_rclosed)) {
    PUSH(st, "Misconfigured OWC",
         "OWC couldn't listen because it was misconfigured", "OWC Listening");
    merry_error_stack_fatality(st);
    return RET_FAILURE; // maybe the channel was configured to be a speaker for
                        // this owner
  }

  if (read(owc->_read_fd, (mptr_t)buf, n) == -1) {
    merry_error_stack_errno(st);
    PUSH(st, NULL, "Failed to read from OWC", "OWC Listening");
    return RET_FAILURE;
  }

  return RET_SUCCESS;
}
