#include "sched.hpp"
#include "task.hpp"

#include "debug.hpp"

namespace tts 
{

TaskID TaskIDAllocator::allocate() {
  TaskID id;
  if (!free_ids_.empty()) {
    id = free_ids_.front();
    free_ids_.pop();
  }
  else {
    id = next_id++;
  }

  return id;
}

void TaskIDAllocator::free(TaskID id) {
  free_ids_.push(id);
}

TaskID Scheduler::registerTask(std::string name, Task&& task) {
  if (name_to_id_.contains(name)) {
    std::printf("[sched] ERR: duplication task name: %s\n", name.c_str());
    task.handler = nullptr;
    return NameDuplicationErr;
  }

  std::unique_ptr<TaskControlBlock> tcb;
  TaskID id = id_allocator_.allocate();

  tcb = std::make_unique<TaskControlBlock>(
    id, TaskState::Ready, std::move(task.handler)
  );

  HandlerAddr addr = tcb->handler.address();

  // 各マップに登録
  name_to_id_[name] = id;
  handler_to_id_[addr] = id;
  tcb_list_[id] = std::move(tcb);

  LOG_PRINTF("[sched] register task: %s(id=%d) addr:%p\n", name.c_str(), id, addr);
  enqueueReady(tcb_list_[id]->handler);
  return id;
}

void Scheduler::enqueueReady(std::coroutine_handle<> h) {
  TaskControlBlock& tcb = getTCBFromHandler(h);

  tcb.state = TaskState::Ready;
  ready_queue_.push(&tcb);

  LOG_PRINTF("[sched] enqueue ready task(id=%d)\n", (int)(tcb.id));
}

void Scheduler::enqueueFinish(std::coroutine_handle<> h) {
  TaskControlBlock& tcb = getTCBFromHandler(h);

  tcb.state = TaskState::Finished;
  finish_queue_.push(&tcb);

  // std::printf("[sched] enqueue ready task(id=%d)\n", (int)(tcb.id));
  LOG_PRINTF("[sched] enqueue ready task(id=%d)\n", (int)(tcb.id));
}

void Scheduler::removeReady(std::coroutine_handle<> h) {
  TaskControlBlock& tcb = getTCBFromHandler(h);

  if (tcb.state == TaskState::Ready) {
    tcb.state = TaskState::Finished;
  }
}

void Scheduler::run() {
  LOG_PRINT("[sched] run.\n");

  while (!ready_queue_.empty()) {
    TaskControlBlock* tcb = ready_queue_.front();
    ready_queue_.pop();

    if (tcb->state != TaskState::Ready) {
      std::printf("[sched] task is not ready\n");
      continue;
    }

    LOG_PRINTF("[sched] resume task(id=%d)\n", (int)(tcb.id));
    tcb->state = TaskState::Running;
    tcb->handler.resume();
  }
}

}