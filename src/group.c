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
        tg->_last_task_id = EZ_ZEROTID;
        tg->_task_queue = EZT_QNIL;
        tg->_outbufs = (taskbuf_t*) NULL;
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
        if(tg->_outbufs) {
            free(tg->_outbufs);
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
                group_into(task, otg);
            }
        }
    }
}


void gather_tasks(taskgroup_t* tg, taskint_t taskCount, task_t* taskList[]) {
    if(tg && taskList) {
        taskbool_t success = true;
        for(taskint_t i = 0; success && i < taskCount; i++) {
            success = group_into(taskList[i], tg) != EZ_ZEROTID;
        }
        if(success) {
            
        }
    }
}

taskbuf_t await_any(taskgroup_t *tg) {
    // await_any waits for only one of the task from the group
    taskbuf_t _temp_taskbuf = empty_taskbuf();
    volatile taskbool_t completed = false;
    while (tg && tg->_task_queue && !completed) {
        task_t *task = dequeue_task(tg);
        if (task) {
            tg->_task_count--;
            if(completed) {
                free_task(task);
                continue;
            }
            tasktime_t taskExecTime;
            taskstatus_t status;
            if((taskExecTime = is_timeout(task)) > 0) {
                status = TS_TIMEDOUT;
            } else {
                task->_state._iter_count++;
                status = task->_taskFn(task);
            }
            if (status == TS_INPROGRESS) {
                if(task->_children) {
                    extend_group(tg, task->_children);
                    free(task->_children);
                    task->_children = (taskgroup_t*) NULL;
                }
                if (enqueue_task(tg, task)) {
                    tg->_task_count++;
                }
            } else if (status == TS_COMPLETED || status == TS_TIMEDOUT) {
                if(status == TS_TIMEDOUT && task->_onTimeout != EZT_NO_TIMEOUT_ACTION) {
                    task->_onTimeout(task, taskExecTime);
                }
                copy_taskbuf(&_temp_taskbuf, task->_outBuf);              
                free_task(task);
                completed = true;
            }
        }
    }
    return _temp_taskbuf;
}

void await_group(taskgroup_t *tg) {
    // await group waits for all the tasks of the group
    if(!tg || !(tg->_task_count) || !(tg->_task_queue)) {
        return;
    }
    taskint_t initial_task_count = tg->_task_count;
    tg->_outbufs = (taskbuf_t*) malloc(sizeof(taskbuf_t) * initial_task_count);
    if(!(tg->_outbufs)) {
        while(tg->_task_queue) {
            task_t* task = dequeue_task(tg);
            if(task) {
                free_task(task);
            }
        }
        return;
    }
    while (tg->_task_queue) {
        task_t *task = dequeue_task(tg);
        if (task) {
            tg->_task_count--;
            tasktime_t taskExecTime;
            if((taskExecTime = is_timeout(task)) > 0) {
                if(task->_onTimeout != EZT_NO_TIMEOUT_ACTION) {
                    task->_onTimeout(task, taskExecTime);
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
                    if (enqueue_task(tg, task)) {
                        tg->_task_count++;
                    }
                } else if (status == TS_COMPLETED) {
                    if(task->_id <= initial_task_count) {
                        taskbuf_t _temp_taskbuf;
                        copy_taskbuf(&_temp_taskbuf, task->_outBuf);
                        tg->_outbufs[task->_id-1] = _temp_taskbuf;                 
                    }
                    free_task(task);
                }
            }
        }
    }
}
