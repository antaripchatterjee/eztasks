#ifndef __EZTASKS_TASK_H__
#define __EZTASKS_TASK_H__

#include "macros.h"
#include "types.h"


#ifdef __cplusplus
extern "C"
{
#endif // __cplusplus


task_t*     ezt_task__new           (void* inBufPtr, taskint_t inBufSize, taskint_t outBufSize, taskfn_t taskFn);
void        ezt_task__free          (task_t* task);
taskid_t    ezt_task__id            (task_t* task);
taskid_t    ezt_task__add_to        (task_t *task, taskgroup_t *tg);
void        ezt_task__set_timeout   (task_t* task, taskdec_t timeoutMs, taskcallback_t onTimeout);
tasktime_t  ezt_task__get_exec_time (task_t* task);
void        ezt_task__read_in       (task_t *task, void *input);
void        ezt_task__write_out     (task_t *task, void *output);
int         ezt_task__enqueue       (taskgroup_t *tg, task_t *task);
task_t*     ezt_task__dequeue       (taskgroup_t *tg);


#ifdef __cplusplus
}
#endif // __cplusplus

#endif // __EZTASKS_TASK_H__