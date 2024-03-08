#include "task.h"
#include "helper.h"

#include <stdlib.h>
#include <string.h>
#include <time.h>


void free_task (task_t* task) {
    if(task) {
        if(task->_inBuf.buffer) {
            free(task->_inBuf.buffer);
        }
        if(task->_outBuf.buffer) {
            free(task->_outBuf.buffer);
        }
        if(task->_state._data_ptr) 
        {
            free(task->_state._data_ptr);
        }
        free(task);
    }
}

task_t* create_new_task (
    void* inBufPtr, 
    taskint_t inBufSize, 
    taskint_t outBufSize, 
    taskfn_t taskFn
) {
    task_t *new_task = (task_t *)malloc(sizeof(task_t));
    if (new_task)
    {
        memset(new_task, 0, sizeof(task_t));
        taskbuf_t inBuf = (inBufPtr && inBufSize) ? 
            to_taskbuf(inBufPtr, inBufSize) : empty_taskbuf();
        taskbuf_t outBuf = outBufSize ? 
            zero_taskbuf(outBufSize) : empty_taskbuf();
        new_task->_id = (taskint_t) 0;
        new_task->_inBuf = inBuf;
        new_task->_outBuf = outBuf;
        new_task->_taskFn = taskFn;
        new_task->_children = (taskgroup_t *) NULL;
        new_task->_state = (taskstate_t) {
            ._iter_count = 0UL,
            ._data_size = 0UL,
            ._data_ptr = NULL
        };
        new_task->_startedAt = 0UL;
        new_task->_timeoutMs = EZT_NO_TIMEOUT;
        new_task->_onTimeout = EZT_NO_TIMEOUT_ACTION;
    }
    return new_task;
}



taskid_t group_task_into(taskgroup_t *tg, task_t *task)
{
    if (tg && task) {
        if (enqueue_task(tg, task)) {
            tg->_task_count++;
            task->_startedAt = (taskint_t) clock();
            task->_id = ++(tg->_last_task_id);
            return task->_id;
        }
        free_task(task);
    }
    return (taskid_t) 0;
}

void read_task_input(task_t *task, void *input)
{
    if (task->_id && task->_inBuf.buffer && input) {
        memmove(input, task->_inBuf.buffer, task->_inBuf.size);
    }
}

void write_task_output(task_t *task, void *output)
{
    if (task->_id && task->_outBuf.buffer && output) {
        memmove(task->_outBuf.buffer, output, task->_outBuf.size);
    }
}

void set_task_timeout(task_t* task, tasknum_t timeoutMs, tasktimeoufn_t onTimeout) {
    if(task) {
        task->_timeoutMs = timeoutMs;
        task->_onTimeout = onTimeout;
    }
}

taskbool_t is_task_timedout(task_t* task) {
    if(!task) return true; 
    if(task->_timeoutMs == EZT_NO_TIMEOUT) {
        return false;
    }
    tasknum_t taskElapsedTime = (((tasknum_t)((taskint_t) clock() - 
        task->_startedAt))*1000)/CLOCKS_PER_SEC;
    if(taskElapsedTime >= task->_timeoutMs) {
        return true;
    }
    return false;
}

void* consume_task_output(taskbuflist_t taskBufs, taskid_t tid, void* dest) {
    if(tid && tid <= taskBufs.length && taskBufs.bufList) {
        if(dest && taskBufs.bufList[tid-1].buffer) {
            memmove(dest, taskBufs.bufList[tid-1].buffer, taskBufs.bufList[tid-1].size);
            free(taskBufs.bufList[tid-1].buffer);
            taskBufs.bufList[tid-1] = empty_taskbuf();
            return dest;
        }
    }
    return NULL;
}