#ifndef __EZTASKS_TASK_H__
#define __EZTASKS_TASK_H__

#include "macros.h"
#include "types.h"

#define TASKCOUNT(_TASKS) (sizeof(_TASKS)/sizeof(task_t*))
#define EZ_ZEROTID ((taskid_t) 0)
#define EZ_TID(T) get_task_id(T)

#ifdef __cplusplus
extern "C"
{
#endif // __cplusplus


void        free_task           (task_t* task);
task_t*     create_new_task     (void* inBufPtr, taskint_t inBufSize, taskint_t outBufSize, taskfn_t taskFn);
taskid_t    get_task_id         (task_t* task);
taskid_t    group_into          (task_t *task, taskgroup_t *tg);
void        set_task_timeout    (task_t* task, taskdec_t timeoutMs, tasktimeoufn_t onTimeout);
tasktime_t  is_timeout          (task_t* task);
void        read_task_input     (task_t *task, void *input);
void        write_task_output   (task_t *task, void *output);


#ifdef __cplusplus
}
#endif // __cplusplus

#endif // __EZTASKS_TASK_H__