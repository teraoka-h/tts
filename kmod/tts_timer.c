#include <linux/types.h>
#include <linux/device.h>
#include <linux/kernel.h>
#include <linux/slab.h> // kalloc, kfree
#include <linux/module.h>
#include <linux/fs.h>
#include <linux/miscdevice.h>
#include <linux/poll.h>
#include <linux/version.h>
#include <linux/errno.h>
#include <linux/printk.h>

// for timer
#include <linux/hrtimer.h>
#include <linux/ktime.h>

#include "../tts_ioctl.h"

MODULE_LICENSE("Dual BSD/GPL");

struct proc_timer_ctx;

struct task_timer {
  struct hrtimer timer;
  task_id_t task_id;
  struct proc_timer_ctx *ctx;
};

struct proc_timer_ctx {
  struct task_timer timers[MAX_TASK_NUM];
  expired_bitmap_t expired_bitmap;
  spinlock_t lock;
  wait_queue_head_t wq;
};

/* Module Finctions */
static enum hrtimer_restart timer_callback(struct hrtimer *t) {
  // hrtimerに対応したタスク対応タイマー構造体の逆引き
  struct task_timer *ttimer = container_of(t, struct task_timer, timer);
  struct proc_timer_ctx* ctx = ttimer->ctx;

  // pr_info("tts_timer: ttimer callback (task_id=%d)", ttimer->task_id);

  set_bit(ttimer->task_id, &(ctx->expired_bitmap));
  // 割り込み可能状態な対応プロセスを実行状態へ
  wake_up_interruptible(&(ctx->wq));

  // pr_info("  bitmap: %0x\n", ctx->expired_bitmap);

  // タイマーを再度起動させない
  return HRTIMER_NORESTART;
}

static struct proc_timer_ctx* proc_timer_ctx_create(void) {
  struct proc_timer_ctx *ctx = kzalloc(sizeof(struct proc_timer_ctx), GFP_KERNEL);

  spin_lock_init(&(ctx->lock));
  init_waitqueue_head(&(ctx->wq));
  ctx->expired_bitmap = 0;

  for (size_t i = 0; i < MAX_TASK_NUM; i++) {
    ctx->timers[i].task_id = i;
    ctx->timers[i].ctx = ctx;
    hrtimer_setup(&(ctx->timers[i].timer), timer_callback, CLOCK_MONOTONIC, HRTIMER_MODE_REL);
  }

  return ctx;
}

static void proc_timer_ctx_destroy(struct proc_timer_ctx *ctx) {
  for (size_t i = 0; i < MAX_TASK_NUM; i++) {
    hrtimer_cancel(&(ctx->timers[i].timer));
  }

  kfree(ctx);
}

static int tts_timer_open(struct inode *inode, struct file *file) {
  // pr_info("tts_timer: (pid=%d) open\n", current->pid);

  struct proc_timer_ctx *ctx = proc_timer_ctx_create();
  if (ctx == NULL) {
    return -ENOMEM;
  }

  file->private_data = ctx;
  return 0;
}

static long tts_timer_ioctl(struct file *file, unsigned int cmd, unsigned long arg) {
  // pr_info("tts_timer: (pid=%d) ioctl\n", current->pid);

  struct proc_timer_ctx *ctx = (struct proc_timer_ctx *)(file->private_data);

  switch (cmd) {
    case TTS_SLEEP_REQ_CMD: {
      ioctl_sleep_req_arg req_arg;
      int ret = copy_from_user(&req_arg, (ioctl_sleep_req_arg __user *)arg, sizeof(req_arg));
      if (ret) {
        pr_info("tts_timer: failed to copy user data\n");
        return -EFAULT;
      }

      if (req_arg.task_id >= MAX_TASK_NUM) {
        pr_info("tts_timer: task_id is lager than MAX\n");
        return -EINVAL;
      }

      task_id_t task_id = req_arg.task_id;
      uint64_t  sleep_ns = req_arg.sleep_ns;
      struct hrtimer *timer = &(ctx->timers[task_id].timer);
      ktime_t sleep_time = ktime_set(0, sleep_ns);

      // pr_info("  Add timer (task_id=%d, ns=%lu)\n", task_id, sleep_ns);
      hrtimer_start(timer, sleep_time, HRTIMER_MODE_REL);
      break;
    }
    case TTS_HAS_EXPIRED_CMD: {
      uint8_t has_expired;
      has_expired = (READ_ONCE(ctx->expired_bitmap) != 0) ? 1 : 0;
      
      // pr_info("  has_expired: %d\n", has_expired);

      int ret = copy_to_user((uint8_t __user *)arg, &has_expired, sizeof(has_expired));
      if(ret) {
        pr_info("tts_timer: failed copy_to_user\n");
        return -EFAULT;
      }

      break;
    }
    case TTS_ABORT_SLEEP_CMD: {
      task_id_t task_id;
      int ret = copy_from_user(&task_id, (task_id_t __user *)arg, sizeof(task_id));
      if (ret) {
        pr_info("tts_timer: failed copy_from_user\n");
        return -EFAULT;
      }

      if (task_id >= MAX_TASK_NUM) {
        pr_info("tts_timer: task_id is lager than MAX\n");
        return -EINVAL;
      }

      struct proc_timer_ctx *ctx = (struct proc_timer_ctx *)(file->private_data);
      struct hrtimer *timer = &(ctx->timers[task_id].timer);

      hrtimer_cancel(timer);

      if (test_bit(task_id, &(ctx->expired_bitmap))) {
        clear_bit(task_id, &(ctx->expired_bitmap));
      }

      break;
    }
    case TTS_GET_EXPIRED_BITMAP_CMD: {
      expired_bitmap_t bitmap = xchg(&(ctx->expired_bitmap), 0);

      int ret = copy_to_user((expired_bitmap_t __user *)arg, &bitmap, sizeof(bitmap));
     
      // pr_info("  expired bitmap: %0x\n", READ_ONCE(bitmap));

      if (ret) {
        pr_info("tts_timer: failed copy_from_user\n");
        return -EFAULT;
      }

      break;
    }
  }

  return 0;
}

static __poll_t tts_timer_poll(struct file *file, struct poll_table_struct *ptable) {
  // pr_info("tts_timer: (pid=%d) poll\n", current->pid);

  __poll_t mask = 0;
  struct proc_timer_ctx *ctx = (struct proc_timer_ctx *)(file->private_data);

  poll_wait(file, &(ctx->wq), ptable);

  if (READ_ONCE(ctx->expired_bitmap) != 0) {
    mask |= (POLLIN | POLLRDNORM);
  }
  
  return mask;
}

static ssize_t tts_timer_read(struct file *file, char __user *buf, size_t count, loff_t *ppos) {
  // pr_info("tts_timer: (pid=%d) read\n", current->pid);

  return 0;
}

static int tts_timer_release(struct inode *inode, struct file *file) {
  // pr_info("tts_timer: (pid=%d) release\n", current->pid);

  proc_timer_ctx_destroy((struct proc_timer_ctx *)(file->private_data));
  return 0;
}

static struct file_operations fops = {
  .owner = THIS_MODULE,
  .open  = tts_timer_open,
  .unlocked_ioctl = tts_timer_ioctl,
  .poll = tts_timer_poll,
  .read = tts_timer_read,
  .release = tts_timer_release,
};

static struct miscdevice misc_dev = {
  .minor = MISC_DYNAMIC_MINOR,
  .name = "tts_timer",
  .fops = &fops,
};

static int tts_timer_init(void) {
  pr_info("tts_timer: init.\n");
  int ret = misc_register(&misc_dev);
  if (ret) {
    pr_err("tts_timer: register failed (%d).\n", ret);
    return ret;
  }

  return 0;
}

static void tts_timer_exit(void) {
  pr_info("tts_timer: exit.\n");
  misc_deregister(&misc_dev);
}

module_init(tts_timer_init);
module_exit(tts_timer_exit);
