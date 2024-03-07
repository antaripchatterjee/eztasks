#include <stdio.h>
#include <stdlib.h>
#include <eztasks.h>



taskstatus_t my_task_mul(task_t *task) {
  static int i;
  printf("Task mul got id: %llu\n", task->_id);
  int input[2];
  read_task_input(task, input);
  int output = input[0] * input[1];
  printf("mul[%d]: %d\n", i, output);
  i++;
  return i == 100 ? TS_COMPLETED : TS_INPROGRESS;
}


taskid_t mul(taskgroup_t *tg, int i, int j) {
  int _calc_input[2] = { i, j };
  task_t* task = create_new_task(
    _calc_input, // input buffer pointer
    sizeof(int[2]), // input buffer size
    0, // output buffer pointer
    &my_task_mul // task function
  );
  taskid_t tid = group_task_into(tg, task);
  return tid;
}


taskstatus_t my_task_add(task_t *task)
{
  printf("Task add got id: %llu\n", task->_id);
  int input[2];
  read_task_input(task, input);
  task->_children = create_new_group();
  mul(task->_children, input[0], input[1]);
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


taskid_t add(taskgroup_t *tg, int i, int j) {
  int _calc_input[2] = { i, j };
  task_t* task = create_new_task(
    _calc_input, // input buffer pointer
    sizeof(int[2]), // input buffer size
    sizeof(int), // output buffer pointer
    &my_task_add // task function
  );
  taskid_t tid = group_task_into(tg, task);
  return tid;
}

taskid_t sub(taskgroup_t *tg, int i, int j) {
  int _calc_input[2] = { i, j };
  task_t* task = create_new_task(
    _calc_input, // input buffer pointer
    sizeof(int[2]), // input buffer size
    sizeof(int), // output buffer pointer
    &my_task_sub // task function
  );
  taskid_t tid = group_task_into(tg, task);
  return tid;
}

taskstatus_t task_sleep(task_t* task) {
  return TS_INPROGRESS;
}

taskid_t async_sleep(taskgroup_t *tg, double ms) {
  task_t* task = create_new_task(NULL, 0, 0, &task_sleep);
  taskid_t tid = group_task_into(tg, task);
  if(tid) {
    set_task_timeout(task, (tasknum_t) ms, EZT_NO_TIMEOUT_ACTION);
  }
  return tid;
}


int main(int argc, char const *argv[])
{
  taskgroup_t tg;
  init_group(&tg);
  taskid_t add_id = add(&tg, 10, 15);
  taskid_t sub_id = sub(&tg, 17, 9);
  async_sleep(&tg, 4500);
  taskbuflist_t outputs = await_group(&tg);
  int* add_res = EZTASK_OUTPUT(int, outputs, add_id);
  int* sub_res = EZTASK_OUTPUT(int, outputs, sub_id);
  printf("add: %d, sub:%d\n", add_res ? *add_res : 0 , sub_res ? *sub_res : 0);
  free(outputs.bufList);
  clean_group(&tg);
  return 0;
}
