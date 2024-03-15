#ifndef __EZTASKS_TYPES_H__
#define __EZTASKS_TYPES_H__

#include <stdint.h>
#include <stdbool.h>

typedef uint64_t                taskint_t;
typedef long double             tasknum_t;
typedef double                  tasktime_t;
typedef bool                    taskbool_t;
typedef struct _taskbuf_t       taskbuf_t;
typedef unsigned long long      taskid_t;
typedef struct _task_t          task_t;
typedef struct _taskqueue_t     taskqueue_t;
typedef struct _taskgroup_t     taskgroup_t;
typedef struct _taskstate_t     taskstate_t;
typedef enum _taskstatus_t      taskstatus_t;


typedef taskstatus_t (*taskfn_t) (task_t*);
typedef void (*tasktimeoufn_t) (task_t *, tasktime_t);


enum _taskstatus_t {
    TS_FAILED = -1,
    TS_PENDING,
    TS_INPROGRESS,
    TS_TIMEDOUT,
    TS_COMPLETED
};

struct _taskbuf_t {
    char*           buffer;
    uint64_t        size;
};


struct _taskstate_t {
    taskint_t       _iter_count;
    void*           _state_ptr;
};

struct _task_t {
    taskid_t        _id;
    taskbuf_t       _inBuf;
    taskbuf_t       _outBuf;
    taskfn_t        _taskFn;
    taskstate_t     _state;
    taskgroup_t*    _children;
    taskint_t       _startedAt;
    tasknum_t       _timeoutMs;
    tasktimeoufn_t  _onTimeout;
};

struct _taskqueue_t {
    task_t*         _task;
    taskqueue_t*    _next;
};

struct _taskgroup_t {
    taskqueue_t*    _task_queue;
    taskint_t       _task_count;
    taskid_t        _last_task_id;
    taskbuf_t*      _outbufs;
};

#endif // __EZTASKS_TYPES_H__