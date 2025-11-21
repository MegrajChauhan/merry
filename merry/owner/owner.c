#include <merry_type.h>
#include <merry_logger.h>
#include <merry_file.h>
#include <merry_utils.h>
#include <stdio.h>
#include <string,h>
#include <errno.h>

_MERRY_INTERNAL_ MerryFile lck;

mcstr_t mowner_get_env(mcstr_t name, char *envp[]) {
	msize_t len = strlen(name);
	for (msize_t i = 0; envp[i] != NULL; i++) {
		if (strncmp(envp[i], name, len) == 0 && envp[i][len] == '=')
			return envp[i] + len + 1;
	}
	return NULL;
}

mresult_t mowner_get_lock() {
	mresult_t res = merry_file_open(&lck, "./mowner.lck", __FILE_MODE_READ | __FILE_MODE_CREATE, __FILE_CREATE_RWXU);
	if (res != MRES_SUCCESS) 
		return res;

	// lock the file
	if ((res = merry_lock_file(lck)) != MRES_SUCCESS)
		return res;
	return MRES_SUCCESS;
}

int main(int argc, char **argv, char *envp[]) {
	MLOG("OWNER", "Owner started running....", NULL);
	mcstr_t OWNER_AUTH = mowner_get_env("OAUTH", envp);
	mcstr_t CTX_AUTH = mowner_get_env("CAUTH", envp);
	msize_t ctx_port = 8080; // just some default for now
	msize_t core_port = 8000; // just some default for now

	if (!OWNER_AUTH || !CTX_AUTH) {
		// We will definitely need a logger
		MFATAL("OWNER", "Authentication not provided. Cannot continue", NULL);
		return -1;
	}
	MLOG("OWNER", "Received the authentication passwords...", NULL);
	
	// Parse the arguments here
	// Get the other data here
	MLOG("OWNER", "Preparing lock.....", NULL);
	mresult_t res = mowner_get_lock();
	if (res != MRES_SUCCESS) {
		MFATAL("OWNER", "Couldn't obtain lock for the lock file.", NULL);
		if (res == MRES_SYS_FAILURE) {
			MFATAL("OWNER", "Possibly the system failed: %s", strerr(errno));
		}
	}
	MLOG("OWNER", "Acquired lock.....", NULL);

	// We have the environment variables, cmd arguments and the file lock	
	MLOG("OWNER", "Starting Executor...", NULL);
		
	return 0;
}
