#ifndef _MERRY_CORE_CONNECTION_
#define _MERRY_CORE_CONNECTION_

#include <merry_core.h>
#include <merry_socket.h>
#include <merry_backend.h>
#include <merry_communication_protocol.h>

mresult_t merry_connect_a_core(MerryContext *ctx, MerryCore *core, mstr_t core_name, mbool_t wait_for_conn);

mresult_t merry_perform_identification(MerryContext *ctx, MerryCore *core, mstr_t core_name);

#endif
