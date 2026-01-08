#ifndef TTS_H
#define TTS_H

#include "task.hpp"
#include "sched.hpp"
#include "awaiter.hpp"
#include <string>

namespace tts {
  TaskID create_task(std::string name, Task&& task);
  void start_scheduler();

  // awaiter api
  inline TaskYieldAwaiter yield() {
    return {};
  }
}

#endif 