#include <errno.h>
#include <fcntl.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

#include <sys/ioctl.h>
#include <linux/i2c.h>
#include <linux/i2c-dev.h>

#include "i2c.h"

static uint8_t bytes[0x20];
static struct i2c_msg msg[2];
static struct i2c_rdwr_ioctl_data data;

int i2c_read(int fd, uint16_t addr, uint8_t reg, uint8_t *buf, int len) {

  msg[0].addr = addr;
  msg[0].flags = 0;
  msg[0].len = 1;
  msg[0].buf = &reg;

  msg[1].addr = addr;
  msg[1].flags = I2C_M_RD;
  msg[1].len = len;
  msg[1].buf = buf;

  data.nmsgs = 2;
  data.msgs = msg;

  int err = ioctl(fd, I2C_RDWR, &data);

  if (err < 0) {
    fprintf(stderr, "i2c_read - error %d: %s\n",
        errno, strerror(errno));
  }
  
  return err;
  
}

int i2c_write(int fd, uint16_t addr, uint8_t reg, uint8_t *buf, int len) {

  bytes[0] = reg;
  for (int i=0; i<len; i++) bytes[i+1] = buf[i];

  msg[0].addr = addr;
  msg[0].flags = 0;
  msg[0].len = len+1;
  msg[0].buf = bytes;

  data.nmsgs = 1;
  data.msgs = msg;

  int err = ioctl(fd, I2C_RDWR, &data);

  if (err < 0) {
    fprintf(stderr, "i2c_write - error %d: %s\n",
        errno, strerror(errno));
  }

  return err;
  
}
