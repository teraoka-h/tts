#ifndef TASK_H
#define TASK_H

#include <coroutine>
#include <exception>
#include <string>
#include <queue>
#include "awaiter.hpp"
#include "task_types.hpp"

using namespace std;

namespace tts 
{

struct TaskPromise;

struct Task {
  std::coroutine_handle<TaskPromise> handler;

  Task(std::coroutine_handle<TaskPromise> h): handler{h} {}
  Task(Task&& t): handler{t.handler} { t.handler = nullptr; }

  ~Task() = default;
};

struct TaskPromise {
  Task get_return_object() { 
    return Task {
      std::coroutine_handle<TaskPromise>::from_promise(*this)
    }; 
  }
  std::suspend_always initial_suspend() { return {}; }
  TaskFinalAwaiter final_suspend() noexcept { return TaskFinalAwaiter{}; }
  void return_void() {};
  void unhandled_exception() { std::terminate(); }
};

}

template <typename... Args>
struct std::coroutine_traits<tts::Task, Args...> {
  using promise_type = tts::TaskPromise;
};

#endif