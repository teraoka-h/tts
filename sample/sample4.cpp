#include <cstdio>
#include "../tts.hpp"
#include <chrono>

using namespace tts;
using namespace std;

Task task1() {

  for (int i = 1; i <= 3; i++) { 
    printf("[task1] num: %d\n", i);

    chrono::time_point start = chrono::high_resolution_clock::now();
    co_await sleep_ms(5);
    chrono::time_point ts = chrono::high_resolution_clock::now();

    int delay_us = chrono::duration_cast<chrono::microseconds>(ts - start).count();
    printf("[task1] delay=%d(us)\n", delay_us);
  }
}

Task task2() {

  for (int i = 1; i <= 3; i++) {
    printf("[task2] num: %d\n", i);

    chrono::time_point start = chrono::high_resolution_clock::now();
    co_await sleep_ms(2);
    chrono::time_point ts = chrono::high_resolution_clock::now();

    int delay_us = chrono::duration_cast<chrono::microseconds>(ts - start).count();
    printf("[task2] delay=%d(us)\n", delay_us);
  }
}

int main() {
  create_task("task1", task1);
  create_task("task2", task2);

  start_scheduler();

  return 0;
}