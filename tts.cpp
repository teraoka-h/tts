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

// task utilities
TaskState get_task_state(std::string task_name) {
  Scheduler& sched = Scheduler::instance();
  return sched.getTaskState(sched.getTaskID(task_name));
}

// task operating
bool task_suspend(std::string task_name) {
  Scheduler& sched = Scheduler::instance(); 
  task_id_t id = sched.getTaskID(task_name);

  return sched.requestSuspend(id);
}

bool task_resume(std::string task_name) {
  Scheduler& sched = Scheduler::instance();
  task_id_t id = sched.getTaskID(task_name);

  return sched.requestResume(id);
}

}