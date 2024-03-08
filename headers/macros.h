#ifndef __EZTASKS_MACROS_H__
#define __EZTASKS_MACROS_H__


#define EZT_NO_TIMEOUT ((tasknum_t) 0)
#define EZT_NO_TIMEOUT_ACTION ((tasktimeoufn_t) NULL)
#define EZTASK_OUTPUT(_T, _OUTBUFS, _TID) ((_T*) consume_task_output(_OUTBUFS, _TID, (_T[]){0}))
#define EZT_QNIL ((taskqueue_t *)NULL)


#endif // __EZTASKS_MACROS_H__