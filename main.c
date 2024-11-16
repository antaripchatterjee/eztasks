#include <stdio.h>
#include <stdlib.h>
#include <eztasks.h>

#define TASK_COUNT 2
#define BUFFER_SIZE 2048

#define STATE_READING 0
#define STATE_WRITE_OUT 1


taskstatus_t task_read_file(task_t* task, tasktime_t execTime) {
  FILE* fp = NULL;
  char buffer[BUFFER_SIZE] = { 0 };
  ezt_task__read_in(task, &fp);

  size_t bytesRead = fread(buffer, sizeof(char), BUFFER_SIZE, fp);
  if(bytesRead > 0) {

    return TS_INPROGRESS;
  }
  return TS_COMPLETED;
}

task_t* async_read_file(FILE* fp) {
  task_t* task = ezt_task__new(&fp, sizeof(FILE**), 0, &task_read_file);
  return task;
}


taskstatus_t task_wait(task_t* task, tasktime_t execTime) {
  tasktime_t ms;
  ezt_task__read_in(task, &ms);
  if(execTime >= ms) return TS_COMPLETED;
  return TS_INPROGRESS;
}

task_t* async_wait(double ms) {
  task_t* task = ezt_task__new((tasktime_t[]) { ms }, sizeof(tasktime_t), 0, &task_wait);
  return task;
}



int main(int argc, char const *argv[])
{
  taskgroup_t tg;
  FILE* fp = fopen("test.txt", "r");
  if(fp == NULL) {
    perror("fopen");
    return -1;
  }

  ezt_taskgroup__init(&tg, TASK_COUNT);
  if(!EZT_TG_INITIATED(&tg)) {
    fclose(fp);
    perror("ezt_taskgroup__init");
    return -1;
  }

  task_t* t_sleep = async_wait(4500);
  task_t* t_read = async_read_file(fp);

  task_t* tasks[TASK_COUNT] = { t_read, t_sleep };
  
  ezt_taskgroup__gather(&tg, TASK_COUNT, tasks);

  if(!EZT_TG_INITIATED(&tg)) {
    perror("ezt_taskgroup__gather");
    fclose(fp);
    ezt_task__free(t_read);
    ezt_task__free(t_sleep);
    return -1;
  }
  
  ezt_taskgroup__await(&tg, 0); // wait for all
  
  fclose(fp);
  ezt_taskgroup__clean(&tg);
  return 0;
}
