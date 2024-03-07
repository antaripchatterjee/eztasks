#ifndef __EZTASKS_TASK_H__
#define __EZTASKS_TASK_H__

#include "macros.h"
#include "types.h"

#ifdef __cplusplus
extern "C"
{
#endif // __cplusplus


void        free_task           (task_t* task);
task_t*     create_new_task     (
                                    void* inBufPtr, 
                                    taskint_t inBufSize, 
                                    taskint_t outBufSize, 
                                    taskfn_t taskFn
                                );
taskid_t    group_task_into     (taskgroup_t *tg, task_t *task);
void        set_task_timeout    (task_t* task, tasknum_t timeoutMs, tasktimeoufn_t onTimeout);
taskbool_t  is_task_timedout    (task_t* task);
void        read_task_input     (task_t *task, void *input);
void        write_EZTASK_OUTPUT   (task_t *task, void *output);
void*       consume_EZTASK_OUTPUT (taskbuflist_t taskBufs, taskid_t tid, void* dest);

#ifdef __cplusplus
}
#endif // __cplusplus

#endif // __EZTASKS_TASK_H__