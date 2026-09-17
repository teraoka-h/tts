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

void TimerBridge::abortSleepTimer(task_id_t id) {
  if (id > MAX_TASK_NUM) {
    return;
  }

  int ret = ioctl(fd_, TTS_ABORT_SLEEP_CMD, &id);
  if (ret < 0) {
    return;
  }
}

bool TimerBridge::hasExpiredIDs() const {
  uint8_t has_expired;
  int ret = ioctl(fd_, TTS_HAS_EXPIRED_CMD, &has_expired);

  if (ret < 0) {
    return false;
  }

  return (has_expired == 1);
}

void TimerBridge::wait(int32_t timeout_ms) {
  pollfd fds = { .fd = fd_, .events = POLLIN };
  poll(&fds, 1, timeout_ms);
}

expired_bitmap_t TimerBridge::readExpiredIDMap() {
  expired_bitmap_t bitmap;
  
  int ret = ioctl(fd_, TTS_GET_EXPIRED_BITMAP_CMD, &bitmap);
  if (ret < 0) {
    return 0;
  }

  return bitmap; 
}

}