#ifndef __EZTASKS_MACROS_H__
#define __EZTASKS_MACROS_H__


#define EZT_NO_TIMEOUT_MS                   ((tasktime_t) 0)
#define EZT_NO_TIMEOUT_ACTION               ((tasktimeoufn_t) NULL)
#define EZTASK_OUTPUT(_T, _OUTBUFS, _TID)   ((_T*) consume_task_output(_OUTBUFS, _TID, (_T[]){0}))
#define EZT_QNIL                            ((taskqueue_t *)NULL)
#define EZT_NO_TIMEOUT_COND                 ((taskbool_t*) NULL)


#endif // __EZTASKS_MACROS_H__