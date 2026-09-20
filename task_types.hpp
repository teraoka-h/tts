#ifndef TASK_TYPES_H
#define TASK_TYPES_H

#include <cstdint>
#include <coroutine>
#include "tts_config.h"

namespace tts 
{

// タスクの状態
enum class TaskState {
  Ready,
  Running,
  Blocked,
  Suspended,
  Finished,
};

// タスクの優先度
enum class TaskPriority {
  Low,
  Normal,
  High,
  Count,
};

using task_id_t = uint8_t;
constexpr task_id_t NameDuplicationErr = (MAX_TASK_NUM + 1);

struct Task;

struct TaskControlBlock {
  task_id_t id;
  TaskState state;
  TaskPriority priority;
  std::coroutine_handle<> handler;

  TaskControlBlock(task_id_t id_, TaskState state_, TaskPriority priority_, std::coroutine_handle<> handler_):
    id(id_), state(state_), priority(priority_), handler(handler_) 
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
