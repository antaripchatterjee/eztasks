#ifndef __EZTASKS_UTIL_H__
#define __EZTASKS_UTIL_H__

#include "types.h"

#ifdef __cplusplus
extern "C"
{
#endif // __cplusplus


taskbuf_t   to_taskbuf          (void* _ptr, uint64_t size);
taskbuf_t   zero_taskbuf        (uint64_t size);
taskbuf_t   empty_taskbuf       (void);
void        copy_taskbuf        (taskbuf_t* dest, taskbuf_t src);


void*       consume_task_output (taskbuf_t* outbufs, taskid_t tid, void* dest);
uint64_t    round_up_to_1024    (uint64_t n);


#ifdef __cplusplus
}
#endif // __cplusplus

#endif // __EZTASKS_UTIL_H__