#ifndef __EZTASKS_MACROS_H__
#define __EZTASKS_MACROS_H__


#define EZT_TG_INITIATED(TG_PTR)            ((TG_PTR)->_maxOutBufCount)

#define EZT_TASK_COUNT(_TASKS)              (sizeof(_TASKS)/sizeof(task_t*))
#define EZT_TASK_ID(T)                      ezt_task__id(T)

#define EZ_ZEROTID                          ((taskid_t) 0)
#define EZT_NO_TIMEOUT_MS                   ((tasktime_t) 0)
#define EZT_NO_TIMEOUT_ACTION               ((taskcallback_t) NULL)
#define EZTASK_OUTPUT(_T, _OUTBUFS, _TID)   ((_T*) consume_task_output(_OUTBUFS, _TID, (_T[]){0}))
#define EZT_QNIL                            ((taskqueue_t *)NULL)
// #define EZT_NO_TIMEOUT_COND                 ((bool*) NULL)


#endif // __EZTASKS_MACROS_H__