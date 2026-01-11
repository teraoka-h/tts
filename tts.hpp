#ifndef TTS_H
#define TTS_H

#include "task.hpp"
#include "sched.hpp"
#include "awaiter.hpp"
#include <string>

namespace tts 
{
  // TaskID create_task(std::string name, Task&& task);
  // TaskID create_task(std::string name, Task (*task)());

template <typename TaskFunc>
TaskID create_task(std::string name, TaskFunc&& task) {
  return Scheduler::instance().registerTask(name, task());
}

void start_scheduler();

// awaiter api
inline TaskYieldAwaiter yield() {
  return {};
}

inline TaskSleepAwaiter sleep(uint64_t ms) {
  return {};
}

}

#endif 