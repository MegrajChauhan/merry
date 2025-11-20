#include <merry_core_connection.h>

mresult_t merry_connect_a_core(MerryContext *ctx, MerryCore *core, mstr_t core_name, mbool_t wait_for_conn) {
    if (!ctx || !core || !core_name)
    	return MRES_INVALID_ARGS;

    if (!ctx->ctx_expecting_connection)
    	return MRES_NOT_ALLOWED;
	// These set of functions are never called by the user directly
	// Hence no need for extra checks here currently
	
__repeat:
	mresult_t res = merry_socket_accept_conn(&ctx->listening_port, &core->comms);
	if (res < 0) {
		if (wait_for_conn)
		  goto __repeat;
		else 
		  return res;
	}

	// We will never have blocking socket ports
	// In order to remove the overhead of having threads owned by Merry, we won't use
	// helper threads hence the use of this
	merry_non_blocking_socket(&core->comms);
	
	// Now we must verify that the core is indeed who we want
	// We will try for a while before giving up
	// we will try 10 times with a gap of 10 microseconds each
	mcommsop_t OPCODE;
	if ((res = merry_socket_recv(&core->comms, &OPCODE, 1)) != MRES_SUCCESS) {
		CLOSESOCKET(core->comms);
		return res;
	}
	if (OPCODE != COMMS_AUTH) {
		CLOSESOCKET(core->comms);
		return MRES_AUTH_FAILED;	
	}
	
    union {
    	mbyte_t verification[8];
    	mqword_t val;
    } msg;
    if ((res = merry_socket_recv(&core->comms, msg.verification, 8)) != MRES_SUCCESS) {
		CLOSESOCKET(core->comms);
		return res;
	}
    if (msg.val != core->verification) {
       	mcommsret_t ret = RET_INVALID_AUTH;
       	merry_socket_send(core->comms, &ret , 1);
       	CLOSESOCKET(core->comms);
       	return MRES_AUTH_FAILED;
    }
    return merry_perform_identification(ctx, core, core_name);
}

mresult_t merry_perform_identification(MerryContext *ctx, MerryCore *core, mstr_t core_name) {
	// The next thing Merry needs is the name of the core
	// The protocol is:
	// Send the length of the name in the first byte
	// Then the actual name
	
	// we will use the same waiting protocol
	mresult_t res;
	mcommsop_t OPCODE;
	if ((res = merry_socket_recv(&core->comms, &OPCODE, 1)) != MRES_SUCCESS) {
		CLOSESOCKET(core->comms);
		return res;
	}
	if (OPCODE != COMMS_IDENTIFYING) {
	   	mcommsret_t ret = RET_INVALID_AUTH;
	   	merry_socket_send(core->comms, &ret , 1);
	   	CLOSESOCKET(core->comms);
	   	return MRES_AUTH_FAILED;
	}

	// we now require the length of the name
	mbyte_t len;
	if ((res = merry_socket_recv(&core->comms, &len, 1)) != MRES_SUCCESS) {
		CLOSESOCKET(core->comms);
		return res;
	}

	mbyte_t sent_name[len+1];
	sent_name[len] = 0;
	if ((res = merry_socket_recv(&core->comms, sent_name, len)) != MRES_SUCCESS) {
		CLOSESOCKET(core->comms);
		return res;
	}

	if (strncmp(core_name, sent_name, len) == 0) {
		core->verified = mtrue;
		core->connection_active = mtrue;
		core->remote_connection = mfalse;
		return MRES_SUCCESS;
	}
	
	CLOSESOCKET(core->comms);
	return MRES_AUTH_FAILED
}
