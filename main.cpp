#include <coroutine>
#include <cstdio>
#include <exception>

struct Task {
  struct promise_type;

  using handle = std::coroutine_handle<promise_type>;
  handle h;

  Task(handle h): h{h} {}
  Task(Task&& t): h{t.h} { t.h = nullptr; }

  ~Task() {
    if (h) {
      h.destroy();
    }
  }

  void resume() {
    if (h && !h.done()) {
      h.resume();
    }
  }

  bool done() const {
    return (!h || h.done());
  }
};

struct Task::promise_type {
  Task get_return_object() {
    return Task {
      std::coroutine_handle<promise_type>::from_promise(*this)
    };
  }

  std::suspend_always initial_suspend() { 
    std::printf("[promise] initial suspend\n");
    return {}; 
  }
  std::suspend_always final_suspend() noexcept { 
    std::printf("[promise] final suspend\n");
    return {}; 
  }

  void return_void() {};
  void unhandled_exception() {
    std::terminate();
  }
};

Task coro() {
  for (int i = 0; i < 10; i++) {
    std::printf("num: %d\n", i);
    co_await std::suspend_always{};
  }
}

int main() {
  auto t = coro();
  while (!t.done()) {
    std::printf("[INFO] task resumed\n");
    t.resume();
  }
}