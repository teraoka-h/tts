#include <stdexcept>

#include "timer_bridge.hpp"

namespace tts
{

TimerBridge::TimerBridge(std::string path) {
  fd_ = open(path.c_str(), O_RDWR);
  if (fd_ < 0) {
    throw std::runtime_error("[ERROR] Fail to open timer module (" + path + ")");
  }
}

TimerBridge::~TimerBridge() {
  if (fd_ >= 0) {
    close(fd_);
  }
}

bool TimerBridge::addRequest(task_id_t id, uint64_t ns) {
  if (id > MAX_TASK_NUM) {
    return false;
  }

  ioctl_sleep_req_arg req = { .task_id = id, .sleep_ns = ns };

  int ret = ioctl(fd_, TTS_SLEEP_REQ_CMD, &req);
  if (ret < 0) {
    return false;
  }

  return true;
}

bool TimerBridge::hasExpiredIDs() const {
  int count;
  int ret = ioctl(fd_, TTS_GET_EXPIRED_COUNT_CMD, &count);

  if (ret < 0) {
    return false;
  }

  return (count > 0) ? true : false;
}

void TimerBridge::wait(int32_t timeout_ms) {
  pollfd fds = { .fd = fd_, .events = POLLIN };
  poll(&fds, 1, timeout_ms);
}

size_t TimerBridge::readExpiredIDs(task_id_t *buf, size_t max_ids) {
  int bytes = read(fd_, buf, max_ids);
  return (bytes < 0) ? 0 : bytes;
}

}