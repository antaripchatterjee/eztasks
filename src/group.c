#include "helper.h"
#include "task.h"
#include "group.h"

#include <stdlib.h>

taskgroup_t* taskgroup__new(taskint_t maxOutBufCount) {
    taskgroup_t* tg = (taskgroup_t*) malloc(sizeof(taskgroup_t));
    ezt_taskgroup__init(tg, maxOutBufCount);
    if(!EZT_TG_INITIATED(tg)) {
        free(tg);
        tg = (taskgroup_t*) NULL;
    }
    return tg;
}

void ezt_taskgroup__init(taskgroup_t *tg, taskint_t maxOutBufCount) {
    if(tg) {
        tg->_task_count = 0ULL;
        tg->_last_task_id = EZ_ZEROTID;
        tg->_task_queue = EZT_QNIL;
        tg->_outbufs = (taskbuf_t*) (
            maxOutBufCount == 0 ? NULL : malloc(sizeof(taskbuf_t) * maxOutBufCount)
        );
        if(tg->_outbufs) {
            tg->_max_outbuf_count = maxOutBufCount;
            for(taskint_t i = 0; i < maxOutBufCount; i++) {
                tg->_outbufs[i].buffer = (char*) NULL;
                tg->_outbufs[i].size = 0;
            }
        } else {
            tg->_max_outbuf_count = 0;
        }
    }
}

void ezt_taskgroup__clean(taskgroup_t *tg) {
    if (tg) {
        while (tg->_task_queue) {
            task_t *task = dequeue_task(tg);
            if (task) {
                ezt_task__free(task);
            }
        }
        if(tg->_outbufs) {
            free(tg->_outbufs);
        }
        ezt_taskgroup__init(tg, 0);
    }
}

void taskgroup__extend(taskgroup_t *otg, taskgroup_t *etg) {
    if (otg && etg) {
        while (etg->_task_queue) {
            task_t *task = dequeue_task(etg);
            if (task) {
                etg->_task_count--;
                ezt_task__add_to(task, otg);
            }
        }
    }
}


void ezt_taskgroup__gather(taskgroup_t* tg, taskint_t taskCount, task_t* taskList[]) {
    if(tg && taskList) {
        taskbool_t success = true;
        for(taskint_t i = 0; success && i < taskCount; i++) {
            success = ezt_task__add_to(taskList[i], tg) != EZ_ZEROTID;
        }
        if(success) {
            
        }
    }
}

taskint_t ezt_taskgroup__await(taskgroup_t *tg, taskint_t maxAwaitCount) {
    // await for at least 'maxAwaitCount' number of tasks if greater than 0;
    // When maxAwaitCount is 0, means await for all tasks.
    if(!tg) {
        return 0;
    }

    taskint_t awaitedCount = 0;
    taskint_t _maxAwaitCount = maxAwaitCount == 0 
        || maxAwaitCount > tg->_task_count ? tg->_task_count : maxAwaitCount;

    while (tg->_task_queue && awaitedCount < _maxAwaitCount) {
        task_t *task = dequeue_task(tg);
        taskstatus_t status;
        if (task) {
            tg->_task_count--;
            tasktime_t taskExecTime;
            if((taskExecTime = ezt_task__is_timeout(task)) > 0) {
                status = TS_TIMEDOUT;
            } else {
                task->_state._iter_count++;
                status = task->_taskFn(task);
            }
            if (status == TS_INPROGRESS || status == TS_COMPLETED || status == TS_TIMEDOUT) {
                if(task->_children) {
                    taskgroup__extend(tg, task->_children);
                    free(task->_children);
                    task->_children = (taskgroup_t*) NULL;
                }
            }
            if (status == TS_INPROGRESS) {
                if (enqueue_task(tg, task)) {
                    tg->_task_count++;
                }
            } else if (status == TS_TIMEDOUT) {
                if(task->_onTimeout != EZT_NO_TIMEOUT_ACTION) {
                    task->_onTimeout(task, taskExecTime);
                    ezt_task__free(task);
                }
                awaitedCount++;
            } else if (status == TS_COMPLETED) {
                if(task->_id <= tg->_max_outbuf_count) {
                    taskbuf_t _temp_taskbuf;
                    copy_taskbuf(&_temp_taskbuf, task->_outBuf);
                    tg->_outbufs[task->_id-1] = _temp_taskbuf;                 
                }
                ezt_task__free(task);
                awaitedCount++;
            }
            if(maxAwaitCount == 0) {
                _maxAwaitCount = tg->_task_count;
            }
        }
    }
    return awaitedCount;
}
