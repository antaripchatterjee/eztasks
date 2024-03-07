#include "util.h"
#include "task.h"
#include "macros.h"


#include <stdlib.h>
#include <string.h>

taskbuf_t to_taskbuf(void* _ptr, taskint_t size) {
    taskbuf_t taskbuf = zero_taskbuf(size);
    memmove(taskbuf.buffer, _ptr, size);
    return taskbuf;
}

taskbuf_t empty_taskbuf() {
    taskbuf_t taskbuf = {
        .buffer = (char*) NULL,
        .size = (taskint_t) 0
    };
    return taskbuf;
}

taskbuf_t zero_taskbuf(taskint_t size) {
    taskbuf_t taskbuf = {
        .buffer = (char*) malloc(sizeof(char) * size),
        .size = size
    };
    memset(taskbuf.buffer, 0, size);
    return taskbuf;
}

void copy_taskbuf(taskbuf_t* dest, taskbuf_t src) {
    *dest = to_taskbuf(src.buffer, src.size);
}

int enqueue_task(taskgroup_t *tg, task_t *task) {
    if (tg->_task_queue == EZT_QNIL) {
        tg->_task_queue = (taskqueue_t *) malloc(sizeof(taskqueue_t));
        if (tg->_task_queue) {
            tg->_task_queue->_task = task;
            tg->_task_queue->_next = EZT_QNIL;
        }
        return 1;
    } else {
        taskqueue_t *_last = tg->_task_queue;
        taskqueue_t *_temp;
        do {
            _temp = _last->_next;
            if (_temp)
            {
                _last = _temp;
            }
        } while (_temp);
        _last->_next = (taskqueue_t *) malloc(sizeof(taskqueue_t));
        if (_last->_next) {
            _last->_next->_task = task;
            _last->_next->_next = EZT_QNIL;
        }
        return 1;
    }
    return 0;
}

task_t *dequeue_task(taskgroup_t *tg) {
    task_t *next_task = (task_t *)NULL;
    if (tg && tg->_task_queue) {
        next_task = tg->_task_queue->_task;
        taskqueue_t *_temp = tg->_task_queue;
        tg->_task_queue = _temp->_next;
        free(_temp);
    }
    return next_task;
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

