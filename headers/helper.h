#ifndef __EZTASKS_UTIL_H__
#define __EZTASKS_UTIL_H__

#include "types.h"

#ifdef __cplusplus
extern "C"
{
#endif // __cplusplus


taskbuf_t   to_taskbuf          (void* _ptr, taskint_t size);
taskbuf_t   zero_taskbuf        (taskint_t size);
taskbuf_t   empty_taskbuf       (void);
void        copy_taskbuf        (taskbuf_t* dest, taskbuf_t src);

int         enqueue_task        (taskgroup_t *tg, task_t *task);
task_t*     dequeue_task        (taskgroup_t *tg);

void*       consume_task_output (taskbuf_t* outbufs, taskid_t tid, void* dest);


#ifdef __cplusplus
}
#endif // __cplusplus

#endif // __EZTASKS_UTIL_H__