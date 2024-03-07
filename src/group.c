#include "util.h"
#include "task.h"
#include "group.h"

#include <string.h>
#include <stdlib.h>
#include <time.h>

taskgroup_t* create_new_group() {
    taskgroup_t* tg = (taskgroup_t*) malloc(sizeof(taskgroup_t));
    init_group(tg);
    return tg;
}

void init_group(taskgroup_t *tg) {
    if(tg) {
        tg->_task_count = 0ULL;
        tg->_last_task_id = 0ULL;
        tg->_task_queue = EZT_QNIL;
        tg->_initiated_at = (taskint_t) clock();
        tg->_timeoutMs = EZT_NO_TIMEOUT;
    }
}

void clean_group(taskgroup_t *tg) {
    if (tg) {
        while (tg->_task_queue) {
            task_t *task = dequeue_task(tg);
            if (task) {
                free_task(task);
            }
        }
        init_group(tg);
    }
}

taskbuflist_t await_group(taskgroup_t *tg)
{
    if(!tg || !(tg->_task_count) || !(tg->_task_queue)) {
        return (taskbuflist_t) {
            .bufList = (taskbuf_t*) NULL,
            .length = 0ULL
        };
    }
    taskint_t initial_task_count = tg->_task_count;
    taskbuflist_t task_bufs = {
        .bufList = (taskbuf_t*) malloc(sizeof(taskbuf_t) * initial_task_count),
        .length = initial_task_count
    };
    if(!(task_bufs.bufList)) {
        while(tg->_task_queue) {
            task_t* task = dequeue_task(tg);
            if(task) {
                free_task(task);
            }
        }
        return (taskbuflist_t) {
            .bufList = (taskbuf_t*) NULL,
            .length = 0ULL
        };
    }
    while (tg->_task_queue) {
        task_t *task = dequeue_task(tg);
        if (task) {
            tg->_task_count--;
            if(is_task_timedout(task)) {
                if(task->_onTimeout != EZT_NO_TIMEOUT_ACTION) {
                    task->_onTimeout(task);
                    free_task(task);
                } 
            } else {
                task->_state._iter_count++;
                taskstatus_t status = task->_taskFn(task);
                if (status == TS_INPROGRESS || status == TS_COMPLETED) {
                    if(task->_children) {
                        extend_group(tg, task->_children);
                        free(task->_children);
                        task->_children = (taskgroup_t*) NULL;
                    }
                }
                if (status == TS_INPROGRESS) {
                    if (enqueue_task(tg, task))
                    {
                        tg->_task_count++;
                    }
                } else if (status == TS_COMPLETED) {
                    if(task->_id <= task_bufs.length) {
                        taskbuf_t _temp_taskbuf;
                        copy_taskbuf(&_temp_taskbuf, task->_outBuf);
                        task_bufs.bufList[task->_id-1] = _temp_taskbuf;                 
                    }
                    free_task(task);
                }
            }
        }
    }
    return task_bufs;
}

taskbool_t is_group_timedout(taskgroup_t* tg) {
    if(!tg) return true;
    if(tg->_timeoutMs == EZT_NO_TIMEOUT) {
        return false;
    }
    tasknum_t groupElapsedTime = (((tasknum_t)((taskint_t) clock() - 
        tg->_initiated_at))*1000)/CLOCKS_PER_SEC;
    if(groupElapsedTime >= tg->_timeoutMs) {
        return true;
    }
    return false;
}

void set_group_timeout(taskgroup_t *tg, tasknum_t timeoutMs) {
    if(tg) {
        tg->_timeoutMs = timeoutMs;
    }
}