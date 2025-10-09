#include <merry_twc.h>

mret_t merry_twc_init(MerryTWC *party_1, MerryTWC *party_2,
                      MerryErrorStack *st) {
  merry_check_ptr(party_1);
  merry_check_ptr(party_2);

  MerryOWC *l_channel = merry_owc_init(st);
  if (!l_channel) {
    PUSH(st, NULL, "Failed to create a TWC channel", "Creating TWC");
    return RET_FAILURE;
  }

  MerryOWC *s_channel = merry_owc_init(st);
  if (!s_channel) {
    PUSH(st, NULL, "Failed to create a TWC channel", "Creating TWC");
    return RET_FAILURE;
  }

  // Use that to initialize the speaking and listening channels
  party_1->listening_channel = l_channel;
  party_2->listening_channel = s_channel;
  party_1->speaking_channel = s_channel;
  party_2->speaking_channel = l_channel;

  // Not configured yet

  return RET_SUCCESS;
}

void merry_twc_config(MerryTWC *twc) {
  // After two processes have been created, this may be used on party_1 and
  // party_2 to configure for comms
  merry_check_ptr(twc);
  merry_owc_only_listen(twc->listening_channel);
  merry_owc_only_speak(twc->speaking_channel);
}

mret_t merry_twc_send(MerryTWC *twc, mbptr_t data, msize_t len,
                      MerryErrorStack *err_st) {
  merry_check_ptr(twc);
  merry_check_ptr(data);
  return merry_owc_speak(twc->speaking_channel, data, len, err_st);
}

mret_t merry_twc_receive(MerryTWC *twc, mbptr_t buf, msize_t n,
                         MerryErrorStack *err_st) {
  merry_check_ptr(twc);
  merry_check_ptr(buf);
  return merry_owc_listen(twc->listening_channel, buf, n, err_st);
}

void merry_twc_close_channel(MerryTWC *twc) {
  merry_check_ptr(twc);
  merry_owc_destroy(twc->listening_channel);
  merry_owc_destroy(twc->speaking_channel);
  // twc was never allocated by us so we don't free it
}
