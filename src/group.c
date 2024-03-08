#include "helper.h"
#include "task.h"
#include "group.h"

#include <stdlib.h>

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

void extend_group(taskgroup_t *otg, taskgroup_t *etg) {
    if (otg && etg) {
        while (etg->_task_queue) {
        task_t *task = dequeue_task(etg);
            if (task) {
                etg->_task_count--;
                task->_id = otg->_last_task_id + 1;
                group_task_into(otg, task);
            }
        }
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
