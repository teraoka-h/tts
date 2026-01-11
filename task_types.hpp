#ifndef TASK_TYPES_H
#define TASK_TYPES_H

#include <cstdint>

namespace tts 
{

using TaskID = uint32_t;
static constexpr TaskID MAX_TASK_NUM = 10;
constexpr TaskID NameDuplicationErr = (MAX_TASK_NUM + 1);

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
