#ifndef AWAITER_H
#define AWAITER_H

#include <coroutine>
#include "sched.hpp"
#include "debug.hpp"

namespace tts {
  struct TaskYieldAwaiter {
    bool await_ready() noexcept { return false; }
    bool await_suspend(std::coroutine_handle<> h) noexcept {
      LOG_PRINT("[awaiter(y)] suspend and enqueue ready\n");

      Scheduler::instance().enqueueReady(h);
      return true;
    }
    void await_resume() noexcept {}
  };

  struct TaskSleepAwaiter {
    bool await_ready() noexcept { return false; }
    bool await_suspend(std::coroutine_handle<> h) noexcept {
      LOG_PRINT("[awaiter(s)] sleep and euqueue block\n");
      return true;
    }
    void await_resume() noexcept {}
  };

  struct TaskFinalAwaiter {
    bool await_ready() noexcept { return false; }
    bool await_suspend(std::coroutine_handle<> h) noexcept {
      LOG_PRINT("[awaiter(f)] suspend and enqueue finish\n");

      Scheduler::instance().enqueueFinish(h);
      return true;
    }
    void await_resume() noexcept {}
  };
}
#endif