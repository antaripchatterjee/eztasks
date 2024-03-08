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
taskbuflist_t   await_group         (taskgroup_t *tg);
void            extend_group        (taskgroup_t *otg, taskgroup_t *etg);


#ifdef __cplusplus
}
#endif // __cplusplus
#endif // __EZTASKS_GROUP_H__