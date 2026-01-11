#ifndef TTS_IOCTL_H
#define TTS_IOCTL_H

#include <linux/ioctl.h>

struct ioctl_sleep_task {
  uint32_t task_id;
  uint64_t sleep_ms;
};

#define TTS_IOCTL_MAGIC 0xAA

#define TTS_ADD_SLEEP_TASK_CMD _IOW(TTS_IOCTL_MAGIC, 1, struct ioctl_sleep_task)

#endif // TTS_IOCTL_H