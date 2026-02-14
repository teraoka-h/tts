#ifndef SCHED_H
#define SCHED_H

#include <coroutine>
#include <queue>
#include <unordered_map>
#include <array>
#include <memory>

#include "task_types.hpp"
#include "timer_bridge.hpp"

namespace tts 
{

class TaskIDAllocator {
 private:
  task_id_t next_id{0};
  std::queue<task_id_t> free_ids_;
  
 public:
  task_id_t allocate();
  void free(task_id_t id);
};

class SleepTimerHandler {
 private:
  int fd_;
};

// スケジューラは singleton
class Scheduler {
  using HandlerAddr = void*;

 private:
  Scheduler() = default;
  ~Scheduler() = default;

  uint8_t registered_tasks_{0};
  task_id_t running_task_id_;
  std::array<std::unique_ptr<TaskControlBlock>, MAX_TASK_NUM> tcb_list_;
  std::unordered_map<std::string, task_id_t> name_to_id_;
  std::unordered_map<HandlerAddr, task_id_t> handler_to_id_;
  std::queue<TaskControlBlock*> ready_queue_;
  std::queue<TaskControlBlock*> finish_queue_;
  TaskIDAllocator id_allocator_;
  TimerBridge kernel_timer_;

  void enqueueReady(task_id_t id);

  TaskControlBlock& getTCBFromHandler(std::coroutine_handle<> h) {
    task_id_t id = handler_to_id_.at(h.address());
    return *(tcb_list_.at(id));
  }

  TaskControlBlock& getTCBFromName(std::string name) {
    task_id_t id = name_to_id_.at(name);
    return *(tcb_list_.at(id));
  }

  bool allTaskFinished() {
    return (registered_tasks_ == finish_queue_.size());
  }

 public:
  Scheduler(const Scheduler&) = delete;
  Scheduler& operator=(const Scheduler&) = delete;
  Scheduler(const Scheduler&&) = delete;
  Scheduler& operator=(Scheduler&&) = delete;

  static Scheduler& instance() {
    static Scheduler instance;
    return instance;
  }

  task_id_t getTaskID(std::string task_name) {
    return name_to_id_[task_name];
  }

  task_id_t getTaskID(std::coroutine_handle<> h) {
    return handler_to_id_[h.address()];
  }

  TaskState getTaskState(task_id_t id) {
    return tcb_list_.at(id).get()->state;
  }

  task_id_t registerTask(std::string name, Task&& task);
  void enqueueReady(std::coroutine_handle<> h);
  void enqueueFinish(std::coroutine_handle<> h);
  bool requestSleep(std::coroutine_handle<> h, uint64_t sleep_ns);
  bool requestSuspend(task_id_t id);
  bool requestResume(task_id_t id);
  void removeReady(std::coroutine_handle<> h);
  void cleanTask(std::coroutine_handle<> h);
  void run();
};

}

#endif