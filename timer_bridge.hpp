#include <fcntl.h>
#include <poll.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/ioctl.h>

#include <string>

#include "task_types.hpp"
#include "tts_ioctl.h"

namespace tts
{

class TimerBridge {
 private:
  int fd_;

 public:
  TimerBridge(std::string path = "/dev/tts_timer");
  ~TimerBridge();

  bool addRequest(task_id_t id, uint64_t ns);
  bool hasExpiredIDs() const;
  void wait(int32_t timeout_ms = -1);
  size_t readExpiredIDs(task_id_t *buf, size_t max_ids = MAX_TASK_NUM);
};

}