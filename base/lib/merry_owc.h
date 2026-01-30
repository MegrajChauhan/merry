// #ifndef _MERRY_OWC_
// #define _MERRY_OWC_
// 
// // One-way channel
// #include <merry_config.h>
// #include <merry_pipe.h>
// #include <merry_platform.h>
// #include <merry_results.h>
// #include <merry_types.h>
// #include <merry_utils.h>
// 
// typedef MerryPipe MerryOWC;
// 
// /**
//  * When two processes are communicating via MerryOWC, a copy is given to each of
//  * the process. One process should only listen by closing it's write line while
//  * the other should only speak by closing the read line. If both processes need
//  * to speak and listen, two OWC can be created. Or, in our case, we will create
//  * a TWC or Two-way channel
//  */
// 
// #define merry_owc_init(err_st) merry_open_merrypipe(err_st)
// #define merry_owc_only_listen(owc) merry_pipe_close_write_end(owc)
// #define merry_owc_only_speak(owc) merry_pipe_close_read_end(owc)
// #define merry_owc_destroy(owc) merry_destroy_pipe(owc)
// #define merry_owc_reopen(owc, err_st) merry_merrypipe_reopen(owc, err_st)
// #define merry_owc_become_neutral(owc) merry_pipe_close_both_ends(owc)
// 
// mresult_t merry_owc_speak(MerryOWC *owc, mbptr_t data, msize_t len);
// 
// mresult_t merry_owc_listen(MerryOWC *owc, mbptr_t buf, msize_t n);
// 
// #endif
