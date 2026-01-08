#ifndef SCHED_H
#define SCHED_H

#include <coroutine>
#include <queue>
#include <unordered_map>
#include <array>
#include <memory>
#include "task_types.hpp"

namespace tts {

  static constexpr TaskID MAX_TASK_NUM = 10;

  class TaskIDAllocator {
   private:
    TaskID next_id{0};
    std::queue<TaskID> free_ids_;
   
   public:
    TaskID allocate();
    void free(TaskID id);
  };

  // スケジューラは singleton
  class Scheduler {
    using HandlerAddr = void*;

   private:
    Scheduler() = default;
    ~Scheduler() = default;

    std::array<std::unique_ptr<TaskControlBlock>, MAX_TASK_NUM> tcb_list_;
    std::unordered_map<std::string, TaskID> name_to_id_;
    std::unordered_map<HandlerAddr, TaskID> handler_to_id_;
    std::queue<std::coroutine_handle<>> ready_queue_;
    std::queue<std::coroutine_handle<>> finish_queue_;
    TaskIDAllocator id_allocator_;

    TaskControlBlock& getTCBFromHandler(std::coroutine_handle<> h) {
      TaskID id = handler_to_id_.at(h.address());
      return *(tcb_list_[id]);
    }

    TaskControlBlock& getTCBFromName(std::string name) {
      TaskID id = name_to_id_.at(name);
      return *(tcb_list_[id]);
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

    TaskID registerTask(std::string name, Task&& task);
    void enqueueReady(std::coroutine_handle<> h);
    void enqueueFinish(std::coroutine_handle<> h);
    void removeReady(std::coroutine_handle<> h);
    void cleanTask(std::coroutine_handle<> h);
    void run();
  };
}

#endif