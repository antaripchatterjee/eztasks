#include <stdio.h>
#include <stdlib.h>
#include <eztasks.h>

void task_sleep_on_timeout(task_t* task, tasktime_t execTime) {
  printf("Time taken by task %llu is %lf milliseconds\n", task->_id, execTime);
}

taskstatus_t task_sleep(task_t* task) {
  // printf("task async_sleep[%llu] called for %lld times\n", task->_id, task->_state._iter_count);
  return TS_INPROGRESS;
}

task_t* async_sleep(double ms) {
  task_t* task = create_new_task(NULL, 0, 0, &task_sleep);
  set_task_timeout(task, (tasknum_t) ms, &task_sleep_on_timeout);
  return task;
}


taskstatus_t my_task_mul(task_t *task) {
  printf("Task mul got id: %llu\n", task->_id);
  int input[2];
  read_task_input(task, input);
  int output = input[0] * input[1];
  // taskgroup_t tg;
  // init_group(&tg);
  // task_t* tasks[2] = {
  //   async_sleep(9000),
  //   async_sleep(2500)
  // };

  // taskid_t tids[TASKCOUNT(tasks)] = {EZ_ZEROTID};
  // gather_tasks(&tg, TASKCOUNT(tasks), tasks, tids);
  // await_any(&tg);
  // clean_group(&tg);
  printf("mul: %d\n", output);
  return TS_COMPLETED;
}


task_t* mul(int i, int j) {
  int _calc_input[2] = { i, j };
  task_t* task = create_new_task(
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
  read_task_input(task, input);
  // task->_children = create_new_group();
  // task_t* tasks[] = {
  //   mul(input[0], input[1])
  // };
  // taskid_t tids[TASKCOUNT(tasks)] = {EZ_ZEROTID};
  // gather_tasks(task->_children, TASKCOUNT(tasks), tasks);
  // taskid_t mul_id = tids[0];
  // if(!mul_id) perror("Could not group task 'mul'");
  int output = input[0] + input[1];
  write_task_output(task, &output);
  return TS_COMPLETED;
}

taskstatus_t my_task_sub(task_t *task)
{
  printf("Task sub got id: %llu\n", task->_id);
  int input[2];
  read_task_input(task, input);
  int output = input[0] - input[1];
  write_task_output(task, &output);
  return TS_COMPLETED;
}


task_t* add(int i, int j) {
  int _calc_input[2] = { i, j };
  task_t* task = create_new_task(
    _calc_input, // input buffer pointer
    sizeof(int[2]), // input buffer size
    sizeof(int), // output buffer pointer
    &my_task_add // task function
  );
  return task;
}

task_t* sub(int i, int j) {
  int _calc_input[2] = { i, j };
  task_t* task = create_new_task(
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
  init_group(&tg);
  task_t* t_add = add(10, 15);
  task_t* t_sub = sub(17, 9);
  task_t* t_sleep = async_sleep(4500);
  task_t* tasks[] = { t_add, t_sub, t_sleep };
  gather_tasks(&tg, TASKCOUNT(tasks), tasks);
  if(!get_task_id(t_add)) perror("Could not group task 'add'");
  if(!get_task_id(t_sub)) perror("Could not group task 'sub'");
  if(!get_task_id(t_sleep)) perror("Could not group task 'async_sleep'");
  await_group(&tg);
  // int* add_res = EZTASK_OUTPUT(int, tg._outbufs, add_id);
  // int* sub_res = EZTASK_OUTPUT(int, tg._outbufs, sub_id);
  // printf("add: %d, sub:%d\n", add_res ? *add_res : 0 , sub_res ? *sub_res : 0);
  clean_group(&tg);
  return 0;
}
