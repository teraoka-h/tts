#include <linux/types.h>
#include <linux/device.h>
#include <linux/kernel.h>
#include <linux/slab.h> // kmalloc, kfree
#include <linux/version.h>
#include <linux/module.h>
#include <linux/fs.h>
#include <linux/printk.h>

#include <linux/miscdevice.h>

#include "tts_ioctl.h"

MODULE_LICENSE("Dual BSD/GPL");

/* Module Finctions */

static long tts_timer_ioctl(struct file *file, unsigned int cmd, unsigned long arg)
{
  int ret = 0;
  return ret;
}

static struct file_operations fops = {
  .owner = THIS_MODULE,
  .unlocked_ioctl = tts_timer_ioctl,
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
