#include <cstdio>
#include "tts.hpp"

using namespace tts;

Task task1() {
  for (int i = 1; i <= 2; i++) {
    std::printf("[task1] num: %d\n", i);
    usleep(300000);
    co_await yield();
  }
}

Task task2() {
  for (int i = 1; i <= 4; i++) {
    std::printf("[task2] num: %d\n", i);
    usleep(300000);
    co_await yield();
  }
}

Task task3() {
  for (int i = 1; i <= 6; i++) {
    std::printf("[task3] num: %d\n", i);
    usleep(300000);
    co_await yield();
  }
}

int main() {
  create_task("task1", task1);
  create_task("task2", task2);
  create_task("task3", task3);
  create_task("task_lambda", []() -> Task {
    for (int i = 1; i <= 8; i++) {
      std::printf("[task_lambda] num: %d\n", i);
      usleep(300000);
      co_await yield();
    }
  });

  start_scheduler();

  return 0;
}