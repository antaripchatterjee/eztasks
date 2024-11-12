#include <stdio.h>
#include <stdlib.h>
#include <eztasks.h>

#define TASK_COUNT 3

void task_sleep_on_timeout(task_t* task, tasktime_t execTime) {
  printf("Time taken by task %llu is %lf milliseconds\n", task->_id, execTime);
}

taskstatus_t task_sleep(task_t* task) {
  // printf("task async_sleep[%llu] called for %lld times\n", task->_id, task->_state._iter_count);
  return TS_INPROGRESS;
}

task_t* async_sleep(double ms) {
  task_t* task = ezt_task__new(NULL, 0, 0, &task_sleep);
  ezt_task__set_timeout(task, (taskdec_t) ms, &task_sleep_on_timeout);
  return task;
}


taskstatus_t my_task_mul(task_t *task) {
  printf("Task mul got id: %llu\n", task->_id);
  int input[2];
  ezt_task__read_in(task, input);
  int output = input[0] * input[1];
  // taskgroup_t tg;
  // ezt_taskgroup__init(&tg);
  // task_t* tasks[2] = {
  //   async_sleep(9000),
  //   async_sleep(2500)
  // };

  // taskid_t tids[EZT_TASK_COUNT(tasks)] = {EZ_ZEROTID};
  // ezt_taskgroup__gather(&tg, EZT_TASK_COUNT(tasks), tasks, tids);
  // ezt_taskgroup__await_any(&tg);
  // ezt_taskgroup__clean(&tg);
  printf("mul: %d\n", output);
  return TS_COMPLETED;
}


task_t* mul(int i, int j) {
  int _calc_input[2] = { i, j };
  task_t* task = ezt_task__new(
    _calc_input, // input buffer pointer
    sizeof(int[2]), // input buffer size
    0, // output buffer pointer
    &my_task_mul // task function
  );
  return task;
}


taskstatus_t my_task_add(task_t *task)
{
  printf("Task add got id: %llu\n", task->_id);
  int input[2];
  ezt_task__read_in(task, input);
  // task->_children = taskgroup__new();
  // task_t* tasks[] = {
  //   mul(input[0], input[1])
  // };
  // taskid_t tids[EZT_TASK_COUNT(tasks)] = {EZ_ZEROTID};
  // ezt_taskgroup__gather(task->_children, EZT_TASK_COUNT(tasks), tasks);
  // taskid_t mul_id = tids[0];
  // if(!mul_id) perror("Could not group task 'mul'");
  int output = input[0] + input[1];
  ezt_task__write_out(task, &output);
  return TS_COMPLETED;
}

taskstatus_t my_task_sub(task_t *task)
{
  printf("Task sub got id: %llu\n", task->_id);
  int input[2];
  ezt_task__read_in(task, input);
  int output = input[0] - input[1];
  ezt_task__write_out(task, &output);
  return TS_COMPLETED;
}


task_t* add(int i, int j) {
  int _calc_input[2] = { i, j };
  task_t* task = ezt_task__new(
    _calc_input, // input buffer pointer
    sizeof(int[2]), // input buffer size
    sizeof(int), // output buffer pointer
    &my_task_add // task function
  );
  return task;
}

task_t* sub(int i, int j) {
  int _calc_input[2] = { i, j };
  task_t* task = ezt_task__new(
    _calc_input, // input buffer pointer
    sizeof(int[2]), // input buffer size
    sizeof(int), // output buffer pointer
    &my_task_sub // task function
  );
  return task;
}


int main(int argc, char const *argv[])
{
  taskgroup_t tg;

  ezt_taskgroup__init(&tg, TASK_COUNT);
  if(!EZT_TG_INITIATED(&tg)) {
    perror("Could not initiate task group 'tg'");
    return -1;
  }

  task_t* t_add = add(10, 15);
  task_t* t_sub = sub(17, 9);
  task_t* t_sleep = async_sleep(4500);
  task_t* tasks[TASK_COUNT] = { t_add, t_sub, t_sleep };
  
  ezt_taskgroup__gather(&tg, TASK_COUNT, tasks);

  if(!EZT_TG_INITIATED(&tg)) {
    perror("Could not gather tasks.");
    ezt_task__free(t_add);
    ezt_task__free(t_sub);
    ezt_task__free(t_sleep);
    return -1;
  }

  
  taskint_t awaitedCount = ezt_taskgroup__await(&tg, 0); // wait for all
  
  ezt_taskgroup__clean(&tg);
  return 0;
}
