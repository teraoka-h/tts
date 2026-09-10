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
task_id_t task_create(std::string name, TaskFunc&& task) {
  return Scheduler::instance().registerTask(name, task());
}

bool task_suspend(std::string task_name);
bool task_resume(std::string task_name);
bool task_abort_sleep(std::string task_name);

// task utilities
TaskState get_task_state(std::string task_name);

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