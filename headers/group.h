#ifndef __EZTASKS_GROUP_H__
#define __EZTASKS_GROUP_H__

#include "types.h"

#ifdef __cplusplus
extern "C"
{
#endif // __cplusplus

taskgroup_t*    create_new_group    (void);
void            init_group          (taskgroup_t *tg);
void            clean_group         (taskgroup_t *tg);
void            gather_tasks        (taskgroup_t* tg, taskint_t taskCount, task_t* taskList[]);
taskbuf_t       await_any           (taskgroup_t *tg);
void            await_group         (taskgroup_t *tg);
void            extend_group        (taskgroup_t *otg, taskgroup_t *etg);


#ifdef __cplusplus
}
#endif // __cplusplus
#endif // __EZTASKS_GROUP_H__