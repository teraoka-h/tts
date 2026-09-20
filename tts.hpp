#ifndef TTS_H
#define TTS_H

#include "task.hpp"
#include "sched.hpp"
#include "awaiter.hpp"
#include <string>
#include <variant>

namespace tts 
{

// scheduling
void start_scheduler();

// task operationg 
template <typename TaskFunc>
task_id_t task_create(std::string name, TaskFunc&& task, TaskPriority priority = TaskPriority::Normal) {
  return Scheduler::instance().registerTask(name, task(), priority);
}

bool task_suspend(std::string task_name);
bool task_suspend(task_id_t task_id);
bool task_resume(std::string task_name);
bool task_resume(task_id_t task_id);
bool task_abort_sleep(std::string task_name);
bool task_abort_sleep(task_id_t task_id);

// task utilities
TaskState task_get_state(std::string task_name);
task_id_t task_get_id(std::string task_name);

// awaiter api
inline TaskYieldAwaiter yield() {
  return {};
}

inline TaskSuspendAwaiter suspend() {
  return {};
}

inline TaskSleepAwaiter sleep_ms(uint64_t ms) {
  return TaskSleepAwaiter { .sleep_ns = (ms * 1000000) };
}

}

#endif 