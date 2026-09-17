#include "tts.hpp"

namespace tts 
{
  // TaskID create_task(std::string name, Task&& task) {
  //   return Scheduler::instance().registerTask(name, std::move(task));
  // }

  // TaskID create_task(std::string name, Task (*task)()) {
  //   return Scheduler::instance().registerTask(name, task());
  // }

// scheduling
void start_scheduler() {
  Scheduler::instance().run();
}

/* task utilities */

TaskState task_get_state(std::string task_name) {
  Scheduler& sched = Scheduler::instance();
  return sched.getTaskState(sched.getTaskID(task_name));
}

task_id_t task_get_id(std::string task_name) {
  return Scheduler::instance().getTaskID(task_name);
}


/* task operating */
bool task_suspend(std::string task_name) {
  Scheduler& sched = Scheduler::instance(); 
  task_id_t id = sched.getTaskID(task_name);

  return sched.requestSuspend(id);
}

bool task_suspend(task_id_t task_id) {
  return Scheduler::instance().requestSuspend(task_id);
}

bool task_resume(std::string task_name) {
  Scheduler& sched = Scheduler::instance();
  task_id_t id = sched.getTaskID(task_name);

  return sched.requestResume(id);
}

bool task_resume(task_id_t task_id) {
  return Scheduler::instance().requestResume(task_id);
}

bool task_abort_sleep(std::string task_name) {
  Scheduler& sched = Scheduler::instance();
  task_id_t id = sched.getTaskID(task_name);

  sched.abortSleep(id);
  return true;
}

bool task_abort_sleep(task_id_t task_id) {
  Scheduler::instance().abortSleep(task_id);
  return true;
}

}