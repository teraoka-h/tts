#include "tts.hpp"

namespace tts {
  TaskID create_task(std::string name, Task&& task) {
    return Scheduler::instance().registerTask(name, std::move(task));
  }

  void start_scheduler() {
    Scheduler::instance().run();
  }

}