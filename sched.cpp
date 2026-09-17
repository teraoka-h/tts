#include "sched.hpp"
#include "task.hpp"
#include "tts_ioctl.h"

#include "debug.hpp"

#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <unistd.h>
#include <errno.h>

namespace tts 
{

task_id_t TaskIDAllocator::allocate() {
  task_id_t id;
  if (!free_ids_.empty()) {
    id = free_ids_.front();
    free_ids_.pop();
  }
  else {
    id = next_id++;
  }

  return id;
}

void TaskIDAllocator::free(task_id_t id) {
  free_ids_.push(id);
}

task_id_t Scheduler::registerTask(std::string name, Task&& task) {
  if (name_to_id_.contains(name)) {
    std::printf("[sched] ERR: duplication task name: %s\n", name.c_str());
    task.handler.destroy();
    return NameDuplicationErr;
  }

  task_id_t id = id_allocator_.allocate();
  std::unique_ptr<TaskControlBlock> tcb;

  tcb = std::make_unique<TaskControlBlock>(
    id, TaskState::Ready, std::move(task.handler)
  );

  // 各マップに登録
  name_to_id_[name] = id;
  handler_to_id_[tcb->handler.address()] = id;
  tcb_list_[id] = std::move(tcb);
  num_tasks_++;

  return id;
}

void Scheduler::enqueueReady(task_id_t id) {
  TaskControlBlock& tcb = *(tcb_list_.at(id));

  tcb.state = TaskState::Ready;
  ready_queue_.push(&tcb);

  LOG_PRINTF("[sched] enqueue ready task(id=%d)\n", (int)(tcb.id));
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

  LOG_PRINTF("[sched] enqueue ready task(id=%d)\n", (int)(tcb.id));
}

bool Scheduler::requestSuspend(task_id_t id) {
  TaskControlBlock& tcb = *(tcb_list_[id]);
  if (tcb.state == TaskState::Suspended) {
    return false;
  }

  LOG_PRINTF("[sched] set suspend task(id=%d)\n", (int)(tcb.id));
  tcb.state = TaskState::Suspended;
  return true;
}

bool Scheduler::requestResume(task_id_t id) {
  TaskControlBlock& tcb = *(tcb_list_[id]);
  if (tcb.state != TaskState::Suspended) {
    return false;
  }

  tcb.state = TaskState::Ready;
  enqueueReady(tcb.handler);
  return true;
}

bool Scheduler::requestSleep(std::coroutine_handle<> h, uint64_t sleep_ns) {
  TaskControlBlock& tcb = getTCBFromHandler(h);

  if (tcb.state != TaskState::Running) {
    return false;
  }

  // 秒数が0なら単にyield
  if (sleep_ns == 0) {
    enqueueReady(h);
  }
  else {
    if (!kernel_timer_.addRequest(tcb.id, sleep_ns)) {
      return false;
    }
  }

  tcb.state = TaskState::Blocked;
  return true;
}

void Scheduler::abortSleep(task_id_t id) {
  TaskControlBlock& tcb = *(tcb_list_.at(id));

  // Blocked 状態でなければスキップ
  if (tcb.state != TaskState::Blocked) {
    return;
  }

  kernel_timer_.abortSleepTimer(id);

  if (tcb.state == TaskState::Blocked) {
    tcb.state = TaskState::Ready;
    ready_queue_.push(&tcb);
  }
}

void Scheduler::removeReady(std::coroutine_handle<> h) {
  TaskControlBlock& tcb = getTCBFromHandler(h);

  if (tcb.state == TaskState::Ready) {
    tcb.state = TaskState::Finished;
  }
}

void Scheduler::run() {
  LOG_PRINT("[sched] run.\n");

  // すべてのタスクを Ready にする
  for (task_id_t i = 0; i < num_tasks_; i++) {
    enqueueReady(i);
  }

  // 全タスクが Finished になるまでスケジューリング
  while (!allTaskFinished()) {

    // Ready Queue が空なら，スリープ中のタイマーの経過を待つ
    if (ready_queue_.empty()) {
      if (!kernel_timer_.hasExpiredIDs()) {
        kernel_timer_.wait();
      }
    }

    // スリープが終了したタスクを Ready Queue に push
    if (kernel_timer_.hasExpiredIDs()) { 
      expired_bitmap_t bitmap = kernel_timer_.readExpiredIDMap();

      for (int i = 0; i < num_tasks_; i++) {
        // ビットが立っていないIDをスキップ
        if (((bitmap >> i) & 0x01) == 0) {
          continue;
        }

        TaskControlBlock& tcb = *(tcb_list_.at(i));
        
        if (tcb.state == TaskState::Blocked) {  
          tcb.state = TaskState::Ready;
          ready_queue_.push(&tcb);
        }
      }
    }

    // Ready Queue からタスクを取り出す
    TaskControlBlock* tcb = ready_queue_.front();
    ready_queue_.pop();

    // Ready 状態でないタスクはスキップ (suspend されたタスクなど)
    if (tcb->state != TaskState::Ready) {
      continue;
    }

    // タスクを Run にして，コルーチンを再開
    LOG_PRINTF("[sched] resume task(id=%d)\n", (int)(tcb->id));
    tcb->state = TaskState::Running;
    running_task_id_ = tcb->id;
    tcb->handler.resume();
  }
}

}