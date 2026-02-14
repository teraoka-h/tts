#ifndef TTS_H
#define TTS_H

#include "task.hpp"
#include "sched.hpp"
#include "awaiter.hpp"
#include <string>
#include <variant>

namespace tts 
{

template <typename TaskFunc>
task_id_t create_task(std::string name, TaskFunc&& task) {
  return Scheduler::instance().registerTask(name, task());
}

// scheduling
void start_scheduler();

// task operationg 
bool task_suspend(std::string task_name);
bool task_resume(std::string task_name);

// task utilities
TaskState get_task_state(std::string task_name);

//  api
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