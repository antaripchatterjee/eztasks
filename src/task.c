#include "task.h"
#include "helper.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>


void ezt_task__free (task_t* task) {
    if(task) {
        if(task->_inBuf.buffer) {
            free(task->_inBuf.buffer);
        }
        if(task->_outBuf.buffer) {
            free(task->_outBuf.buffer);
        }
        free(task);
    }
}

task_t* ezt_task__new (void* inBufPtr,  taskint_t inBufSize, taskint_t outBufSize, taskfn_t taskFn) {
    task_t *new_task = (task_t *)malloc(sizeof(task_t));
    if (new_task)
    {
        memset(new_task, 0, sizeof(task_t));
        taskbuf_t inBuf = (inBufPtr && inBufSize) ? 
            to_taskbuf(inBufPtr, inBufSize) : empty_taskbuf();
        taskbuf_t outBuf = outBufSize ? 
            zero_taskbuf(outBufSize) : empty_taskbuf();
        new_task->_id = EZ_ZEROTID;
        new_task->_inBuf = inBuf;
        new_task->_outBuf = outBuf;
        new_task->_taskFn = taskFn;
        new_task->_children = (taskgroup_t *) NULL;
        new_task->_state = (taskstate_t) {
            ._iter_count = 0UL,
            ._state_ptr = NULL
        };
        new_task->_startedAt = 0UL;
        new_task->_timeoutMs = EZT_NO_TIMEOUT_MS;
        new_task->_onTimeout = EZT_NO_TIMEOUT_ACTION;
    }
    return new_task;
}

taskid_t ezt_task__id(task_t* task) {
    if(task) {
        return task->_id;
    }
    return EZ_ZEROTID;
}



taskid_t ezt_task__add_to(task_t *task, taskgroup_t *tg)
{
    if (tg && task) {
        if (enqueue_task(tg, task)) {
            tg->_task_count++;
            task->_startedAt = (taskint_t) clock();
            task->_id = ++(tg->_last_task_id);
            return task->_id;
        }
        ezt_task__free(task);
    }
    return EZ_ZEROTID;
}

void ezt_task__read_in(task_t *task, void *input)
{
    if (task->_id && task->_inBuf.buffer && input) {
        memmove(input, task->_inBuf.buffer, task->_inBuf.size);
    }
}

void ezt_task__write_out(task_t *task, void *output)
{
    if (task->_id && task->_outBuf.buffer && output) {
        memmove(task->_outBuf.buffer, output, task->_outBuf.size);
    }
}

void ezt_task__set_timeout(task_t* task, taskdec_t timeoutMs, tasktimeoufn_t onTimeout) {
    if(task) {
        task->_timeoutMs = timeoutMs;
        task->_onTimeout = onTimeout;
    }
}

tasktime_t ezt_task__is_timeout(task_t* task) {
    if(!task) return (tasktime_t) 1;
    tasktime_t taskExecTime = (((tasktime_t)((taskint_t) clock() - 
        task->_startedAt))*1000)/CLOCKS_PER_SEC;
    if(task->_timeoutMs == EZT_NO_TIMEOUT_MS) {
        return EZT_NO_TIMEOUT_MS;
    }
    if(taskExecTime >= task->_timeoutMs) {
        return taskExecTime;
    }
    return EZT_NO_TIMEOUT_MS;
}
