// #include <merry_owc.h>
//
// mresult_t merry_owc_speak(MerryOWC *owc, mbptr_t data, msize_t len) {
//   // The OWC will speak for you
//   // Send len bytes
//   merry_check_ptr(owc);
//   merry_check_ptr(data);
//   if (owc->interface_t != INTERFACE_TYPE_PIPE)
//     return MRES_CONFIGURATION_INVALID;
//
//   if (surelyF(owc->cpipe._wclosed)) {
//     return MRES_NOT_ALLOWED; // maybe the channel was configured to be a
//                              // listener for this owner
//   }
//   if (surelyF(!len))
//     return MRES_SUCCESS;
//   if (write(owc->cpipe._write_fd, (mptr_t)data, len) == -1) {
//     return MRES_SYS_FAILURE;
//   }
//   return MRES_SUCCESS;
// }
//
// mresult_t merry_owc_listen(MerryOWC *owc, mbptr_t buf, msize_t n) {
//   // The OWC will listen for you
//   // Send len bytes
//   merry_check_ptr(owc);
//   merry_check_ptr(buf);
//   if (owc->interface_t != INTERFACE_TYPE_PIPE)
//     return MRES_CONFIGURATION_INVALID;
//   if (surelyF(owc->cpipe._rclosed)) {
//     return MRES_NOT_ALLOWED; // maybe the channel was configured to be a
//                              // speaker for this owner
//   }
//   if (surelyF(!n))
//     return MRES_SUCCESS;
//
//   if (read(owc->cpipe._read_fd, (mptr_t)buf, n) == -1) {
//     return MRES_SYS_FAILURE;
//   }
//
//   return MRES_SUCCESS;
// }
