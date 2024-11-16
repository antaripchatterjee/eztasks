#include "helper.h"
#include "task.h"
#include "group.h"
#include <stdio.h>
#include <stdlib.h>

taskgroup_t* taskgroup__new(uint64_t maxOutBufCount) {
    taskgroup_t* tg = (taskgroup_t*) malloc(sizeof(taskgroup_t));
    ezt_taskgroup__init(tg, maxOutBufCount);
    if(!EZT_TG_INITIATED(tg)) {
        free(tg);
        tg = (taskgroup_t*) NULL;
    }
    return tg;
}

void ezt_taskgroup__init(taskgroup_t *tg, uint64_t maxOutBufCount) {
    if(tg) {
        tg->_taskCount = 0ULL;
        tg->_lastTaskId = EZ_ZEROTID;
        tg->_taskQueue = EZT_QNIL;
        tg->_outBufs = (taskbuf_t*) (
            maxOutBufCount == 0 ? NULL : malloc(sizeof(taskbuf_t) * maxOutBufCount)
        );
        if(tg->_outBufs) {
            tg->_maxOutBufCount = maxOutBufCount;
            for(uint64_t i = 0; i < maxOutBufCount; i++) {
                tg->_outBufs[i].buffer = (char*) NULL;
                tg->_outBufs[i].size = 0;
            }
        } else {
            tg->_maxOutBufCount = 0;
        }
    }
}

void ezt_taskgroup__clean(taskgroup_t *tg) {
    if (tg) {
        while (tg->_taskQueue) {
            task_t *task = ezt_task__dequeue(tg);
            if (task) {
                ezt_task__free(task);
            }
        }
        if(tg->_outBufs) {
            for(uint64_t i = 0; i < tg->_maxOutBufCount; i++) {
                if(tg->_outBufs[i].buffer){
                    free(tg->_outBufs[i].buffer);
                }
            }
            free(tg->_outBufs);
        }
        ezt_taskgroup__init(tg, 0);
    }
}

void taskgroup__extend(taskgroup_t *otg, taskgroup_t *etg) {
    if (otg && etg) {
        while (etg->_taskQueue) {
            task_t *task = ezt_task__dequeue(etg);
            if (task) {
                etg->_taskCount--;
                ezt_task__add_to(task, otg);
            }
        }
    }
}


void ezt_taskgroup__gather(taskgroup_t* tg, uint64_t taskCount, task_t* taskList[]) {
    if(tg && taskList) {
        bool success = true;
        for(uint64_t i = 0; success && i < taskCount; i++) {
            success = ezt_task__add_to(taskList[i], tg) != EZ_ZEROTID;
        }
        if(!success) {
            ezt_taskgroup__clean(tg);
        }
    }
}

uint64_t ezt_taskgroup__await(taskgroup_t *tg, uint64_t maxAwaitCount) {
    // await for at least 'maxAwaitCount' number of tasks if greater than 0;
    // When maxAwaitCount is 0, means await for all tasks.
    if(!tg) {
        return 0;
    }

    uint64_t awaitedCount = 0;
    uint64_t _maxAwaitCount = maxAwaitCount == 0 
        || maxAwaitCount > tg->_taskCount ? tg->_taskCount : maxAwaitCount;

    while (tg->_taskQueue != EZT_QNIL && _maxAwaitCount > 0) {
        task_t *task = ezt_task__dequeue(tg);
        taskstatus_t status;
        if (task) {
            tg->_taskCount--;
            tasktime_t taskExecTime = ezt_task__get_exec_time(task);
            if(task->_timeoutMs != EZT_NO_TIMEOUT_MS && taskExecTime >= task->_timeoutMs) {
                status = TS_TIMEDOUT;
            } else {
                task->_state._iterCount++;
                uint64_t _startedAt = task->_startedAt;
                status = task->_taskFn(task, taskExecTime);
                task->_startedAt = _startedAt;
            }

            if (status == TS_INPROGRESS || status == TS_COMPLETED || status == TS_TIMEDOUT) {
                if(task->_children) {
                    taskgroup__extend(tg, task->_children);
                    free(task->_children);
                    task->_children = (taskgroup_t*) NULL;
                }
            }
            if (status == TS_INPROGRESS) {
                if (ezt_task__enqueue(tg, task)) {
                    tg->_taskCount++;
                }
            } else if (status == TS_TIMEDOUT) {
                if(task->_onTimeout != EZT_NO_TIMEOUT_ACTION) {
                    task->_onTimeout(task, taskExecTime);
                    ezt_task__free(task);
                }
                awaitedCount++;
            } else if (status == TS_COMPLETED) {
                if(task->_id <= tg->_maxOutBufCount) {
                    taskbuf_t _temp_taskbuf;
                    copy_taskbuf(&_temp_taskbuf, task->_outBuf);
                    tg->_outBufs[task->_id-1] = _temp_taskbuf;                 
                }
                ezt_task__free(task);
                awaitedCount++;
            }
            if(maxAwaitCount == 0) {
                _maxAwaitCount = tg->_taskCount;
            }
        }
    }
    return awaitedCount;
}
