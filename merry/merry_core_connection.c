#include <merry_core_connection.h>

mresult_t merry_connect_a_core(MerryContext *ctx, MerryCore *core, mstr_t core_name, mbool_t wait_for_conn) {
    if (!ctx || !core || !name)
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
	for (msize_t i = 0; i < 10; i++) {
		if (merry_socket_recv(&core->comms, &OPCODE, 1) != MRES_SUCCESS)
		    usleep(10);
		else {
			if (OPCODE == COMMS_AUTH)
			   goto __authenticate;
			break; // invalid first message
		}
	}
	CLOSESOCKET(core->comms);
	return MRES_AUTH_FAILED;
__authenticate:
    // The next read must return 8 bytes that verifies the core
    // again, we will try 10 times
    union {
    	mbyte_t verification[8];
    	mqword_t val;
    } msg;
    for (msize_t i = 0; i < 10; i++) {
    	if (merry_socket_recv(&core->comms, msg.verification, 8) != MRES_SUCCESS)
        	usleep(10);
    	else {
    	    if (msg.val != core->verification) {
    	    	mcommsret_t ret = RET_INVALID_AUTH;
    	    	merry_socket_send(core->comms, &ret , 1);
    	    	CLOSESOCKET(core->comms);
    	    	return MRES_AUTH_FAILED;
    	    }
    	    // now perform the identification
    	    return merry_perform_identification(ctx, core, core_name);
   	 	}
    }
    CLOSESOCKET(core->comms);
	return MRES_AUTH_FAILED;
}

mresult_t merry_perform_identification(MerryContext *ctx, MerryCore *core, mstr_t core_name) {
	// So this is indeed a merry spawned core
	core->remote_connection = mfalse;

	// The next thing Merry needs is the name of the core
	// The protocol is:
	// Send the length of the name in the first byte
	// Then the actual name
	
	// we will use the same waiting protocol
	mcommsop_t OPCODE;
	for (msize_t i = 0; i < 10; i++) {
	    	if (merry_socket_recv(&core->comms, &OPCODE, 1) != MRES_SUCCESS)
	        	usleep(10);
	    	else {
	    	    if (OPCODE != COMMS_IDENTIFYING) {
	    	    	mcommsret_t ret = RET_INVALID_AUTH;
	    	    	merry_socket_send(core->comms, &ret , 1);
	    	    	CLOSESOCKET(core->comms);
	    	    	return MRES_AUTH_FAILED;
	    	    }
	    	    goto __identify;
	   	 	}
	    }
	    CLOSESOCKET(core->comms);
	    return MRES_AUTH_FAILED;
__identify:
	// we now require the length of the name
	mbyte_t len;
	for (msize_t i = 0; i < 10; i++) {
	   	if (merry_socket_recv(&core->comms, &len, 1) != MRES_SUCCESS)
	       	usleep(10);
    	else {
    		mbyte_t name[len];
    		
   	 	}
    }	
	CLOSESOCKET(core->comms);
	return MRES_AUTH_FAILED
}
