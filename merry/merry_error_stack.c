#include <merry_error_stack.h>

void PUSH(MerryErrorStack *st, mstr_t cause, mstr_t msg, mstr_t cont) {
  merry_check_ptr(st);

  // Any of the messages can be NULL too!
  // We will try to be as dynamic as possible with error Display
  if (st->SP == (MERRY_ERROR_STACK_DEPTH))
    merry_unreachable(
        "Error Stack Depth Reached[STACK OWNER: id=%zu, uid=%zu, "
        "gid=%zu][Error Push Attempt: cause=%s, msg=%s, context=%s]",
        st->owner_id, st->owner_uid, st->owner_gid, cause, msg, cont);

  st->entries[st->SP].context = cont;
  st->entries[st->SP].cause = cause;
  st->entries[st->SP].message = msg;
  st->SP++;
}

void ERROR(MerryErrorStack *st) {
  merry_check_ptr(st);

  merry_log("ERROR DUMP:", NULL);
  if (st->SP == 0) {
    if (st->owner_id != (mqword_t)-1)
      merry_unreachable(
          "Empty Error Dump[STACK OWNER: id=%zu, uid=%zu, gid=%zu]",
          st->owner_id, st->owner_uid, st->owner_gid);
    else
      merry_unreachable("Empty Error Dump[STACK OWNER: id=COMPONENT, "
                        "uid=COMPONENT, gid=COMPONENT]",
                        NULL);
  }
  if (st->owner_id != (mqword_t)-1)
    merry_msg("Belongs to: id=%zu, uid=%zu, gid=%zu", st->owner_id,
              st->owner_uid, st->owner_gid);
  else
    merry_msg("Belongs to: id=COMPONENT, uid=COMPONENT, gid=COMPONENT", NULL);

  for (msize_t i = 0; i < st->SP; i++) {
    MerryErrorStackEntry entry = st->entries[i];
    if (entry.context)
      fprintf(stderr, "\nWhile '%s':\n\t", entry.context);
    if (entry.cause)
      fprintf(stderr, "(Due to '%s'): ", entry.cause);
    if (entry.message) // This is always present actually
      fprintf(stderr, "%s\n", entry.message);
  }
}
