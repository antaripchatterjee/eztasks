#ifndef __EZTASKS_TYPES_H__
#define __EZTASKS_TYPES_H__

#include <stdint.h>
#include <stdbool.h>


typedef long double             f64_t;
typedef double                  tasktime_t;
typedef struct _taskbuf_t       taskbuf_t;
typedef uintmax_t               taskid_t;
typedef struct _task_t          task_t;
typedef struct _taskqueue_t     taskqueue_t;
typedef struct _taskgroup_t     taskgroup_t;
typedef struct _taskstate_t     taskstate_t;
typedef enum _taskstatus_t      taskstatus_t;


typedef taskstatus_t (*taskfn_t) (task_t*, tasktime_t);
typedef void (*taskcallback_t) (task_t *, tasktime_t);


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
    uint64_t       _iter_count;
    uint16_t        _state_no;
    void*           _state_ptr;
};

struct _task_t {
    taskid_t        _id;
    taskbuf_t       _inBuf;
    taskbuf_t       _outBuf;
    taskfn_t        _taskFn;
    taskstate_t     _state;
    taskgroup_t*    _children;
    uint64_t        _startedAt;
    tasktime_t      _timeoutMs;
    taskcallback_t  _onTimeout;
};

struct _taskqueue_t {
    task_t*         _task;
    taskqueue_t*    _next;
};

struct _taskgroup_t {
    taskqueue_t*    _task_queue;
    uint64_t       _task_count;
    taskid_t        _last_task_id;
    taskbuf_t*      _outbufs;
    uint64_t       _max_outbuf_count;
};

#endif // __EZTASKS_TYPES_H__