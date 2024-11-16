#ifndef __EZTASKS_TASK_H__
#define __EZTASKS_TASK_H__

#include "macros.h"
#include "types.h"


#ifdef __cplusplus
extern "C"
{
#endif // __cplusplus


task_t*     ezt_task__new           (void* inBufPtr, uint64_t inBufSize, uint64_t outBufSize, taskfn_t taskFn, uint64_t stateBufMinSize);
void        ezt_task__free          (task_t* task);
taskid_t    ezt_task__id            (task_t* task);
taskid_t    ezt_task__add_to        (task_t *task, taskgroup_t *tg);
void        ezt_task__set_timeout   (task_t* task, tasktime_t timeoutMs, taskcallback_t onTimeout);
tasktime_t  ezt_task__get_exec_time (task_t* task);
void        ezt_task__read_in       (task_t *task, void *input);
void        ezt_task__write_out     (task_t *task, void *output);
int         ezt_task__enqueue       (taskgroup_t *tg, task_t *task);
task_t*     ezt_task__dequeue       (taskgroup_t *tg);
uint16_t    ezt_task__get_state     (task_t* task);
void        ezt_task__put_state     (task_t* task, uint16_t state);
int         ezt_task__update_state  (task_t* task, void* buffer, uint64_t size);
void        ezt_task__append_state  (task_t* task, void* buffer, uint64_t size);


#ifdef __cplusplus
}
#endif // __cplusplus

#endif // __EZTASKS_TASK_H__