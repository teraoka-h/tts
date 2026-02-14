#ifndef TASK_TYPES_H
#define TASK_TYPES_H

#include <cstdint>
#include <coroutine>
#include "tts_config.h"

namespace tts 
{

using task_id_t = uint8_t;
constexpr task_id_t NameDuplicationErr = (MAX_TASK_NUM + 1);

enum class TaskState {
  Ready,
  Running,
  Blocked,
  Suspended,
  Finished,
};

struct Task;

struct TaskControlBlock {
  task_id_t id;
  TaskState state;
  std::coroutine_handle<> handler;

  TaskControlBlock(task_id_t id_, TaskState state_, std::coroutine_handle<> handler_):
    id(id_), state(state_), handler(handler_) 
    {

    }

  ~TaskControlBlock() {
    std::printf("[tcb] destructor.\n");
    if (handler) {
      handler.destroy();
    }
  }
};

}

#endif
