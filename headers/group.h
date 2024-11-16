#ifndef __EZTASKS_GROUP_H__
#define __EZTASKS_GROUP_H__

#include "types.h"
#include "macros.h"

#ifdef __cplusplus
extern "C"
{
#endif // __cplusplus

taskgroup_t*    ezt_taskgroup__new          (taskint_t maxOutBufCount);
void            ezt_taskgroup__init         (taskgroup_t *tg, taskint_t maxOutBufCount);
void            ezt_taskgroup__clean        (taskgroup_t *tg);
void            ezt_taskgroup__gather       (taskgroup_t* tg, taskint_t taskCount, task_t* taskList[]);
taskint_t       ezt_taskgroup__await        (taskgroup_t *tg, taskint_t maxAwaitCount);
void            ezt_taskgroup__extend       (taskgroup_t *otg, taskgroup_t *etg);


#ifdef __cplusplus
}
#endif // __cplusplus
#endif // __EZTASKS_GROUP_H__