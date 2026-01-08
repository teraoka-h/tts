#include <cstdio>
#include "tts.hpp"

using namespace tts;


Task task1() {
  for (int i = 1; i <= 3; i++) {
    std::printf("[task1] num: %d\n", i);
    usleep(300000);
    co_await tts::yield();
  }
}

Task task2() {
  for (int i = 1; i <= 6; i++) {
    std::printf("[task2] num: %d\n", i);
    usleep(300000);
    co_await tts::yield();
  }
}

Task task3() {
  for (int i = 1; i <= 9; i++) {
    std::printf("[task3] num: %d\n", i);
    usleep(300000);
    co_await tts::yield();
  }
}

int main() {
  tts::create_task("task1", task1());
  tts::create_task("task2", task2());
  tts::create_task("task3", task3());

  tts::start_scheduler();

  return 0;
}