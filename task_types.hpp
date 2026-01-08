#ifndef TASK_TYPES_H
#define TASK_TYPES_H

#include <cstdint>

namespace tts {
  using TaskID = int32_t;
  constexpr TaskID NameDuplicationErr = -1;

  enum class TaskState {
    Ready,
    Running,
    Yield,
    Sleeping,
    Finished,
  };

  struct Task;
  struct TaskControlBlock;
}

#endif
