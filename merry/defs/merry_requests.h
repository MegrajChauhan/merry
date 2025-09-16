#ifndef _MERRY_REQUESTS_
#define _MERRY_REQUESTS_

/*
 * Request generally refers to just:
 * 1) Programs request to graves that it wants to be fulfilled
 * 2) Core requests that reports something to graves
 *
 * It is good to note that the approach is not perfect and has many
 * flaws. When a lot of requests are queued, each core has to wait for
 * a while to see its request fufilled which isn't ideal.
 *
 * This is why I have made a change in this attempt:
 * 1) Not all requests are fulfilled by Graves.
 * 2) Cores will fulfill what they can themselves
 *
 * Graves will fulfill only:
 * 1) Direct syscall requests under controlled environment.
 * 2) Direct core requests suggesting error
 * 3) Core requests to interact with other cores
 * 4) Core requests to modify its memory or share memory
 * 5) And more if necessary
 *
 * Previously, while Graves handled everything such as file IO, file
 * handling and so on, that is now going to be passed to the cores.
 *
 * */

typedef enum mgreq_t mgreq_t;

enum mgreq_t {
  KILL_SELF, // RES: A core killing itself
             // DESC: If a core has done what it can and wants to
             //       terminate itself gracefully

};

#endif
