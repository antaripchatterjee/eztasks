#include "helper.h"
#include "task.h"
#include "macros.h"


#include <stdlib.h>
#include <string.h>

taskbuf_t to_taskbuf(void* _ptr, uint64_t size) {
    taskbuf_t taskbuf = zero_taskbuf(size);
    memmove(taskbuf.buffer, _ptr, size);
    return taskbuf;
}

taskbuf_t empty_taskbuf() {
    taskbuf_t taskbuf = {
        .buffer = (char*) NULL,
        .size = (uint64_t) 0
    };
    return taskbuf;
}

taskbuf_t zero_taskbuf(uint64_t size) {
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

void* consume_task_output(taskbuf_t* outbufs, taskid_t tid, void* dest) {
    if(tid && outbufs) {
        if(dest && outbufs[tid-1].buffer) {
            memmove(dest, outbufs[tid-1].buffer, outbufs[tid-1].size);
            free(outbufs[tid-1].buffer);
            outbufs[tid-1] = empty_taskbuf();
            return dest;
        }
    }
    return NULL;
}
