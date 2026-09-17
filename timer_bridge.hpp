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
  void abortSleepTimer(task_id_t id);
  bool hasExpiredIDs() const;
  void wait(int32_t timeout_ms = -1);
  expired_bitmap_t readExpiredIDMap();
};

}