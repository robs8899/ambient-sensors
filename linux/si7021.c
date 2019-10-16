#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdint.h>
#include <string.h> 
#include <time.h>
#include <unistd.h>

#include <sys/ioctl.h>
//#include <dev/iicbus/iic.h>
#include <linux/i2c.h>
#include <linux/i2c-dev.h>

#include "util.h"

#define SI7021_ADDR            0x40

#define SI7021_M_RH_HOLD_MM    0xE5
#define SI7021_M_RH_NOHOLD_MM  0xF5
#define SI7021_M_T_HOLD_MM     0xE3
#define SI7021_M_T_NOHOLD_MM   0xF3
#define SI7021_M_T_PREV_M      0xE0
#define SI7021_RESET           0xFE
#define SI7021_W_USER_REG      0xE6
#define SI7021_R_USER_REG      0xE7

struct sht_sample {
  uint16_t rh;
  int16_t t;
};

static int sht_read(char *dev, struct sht_sample *s) {

  int fd, err, i;
  uint8_t reg, crc, buf[3];

  // struct iic_msg msg[2];
  // struct iic_rdwr_data data;
  
  struct i2c_msg msg[2];
  struct i2c_rdwr_ioctl_data data;

  fd = open(dev, O_RDWR);

  if (fd == -1) {
    fprintf(stderr, "error opening device %s: %s\n",
        dev, strerror(errno));
    return -1;
  }

  // read RH with "No Hold Master Mode"

  reg = SI7021_M_RH_NOHOLD_MM;
  // msg[0].slave = SI7021_ADDR << 1;
  // msg[0].flags = IIC_M_WR;
  msg[0].addr = SI7021_ADDR;
  msg[0].flags = 0;
  msg[0].len = 1;
  msg[0].buf = &reg;

  // msg[1].slave = SI7021_ADDR << 1;
  // msg[1].flags = IIC_M_RD;
  msg[1].addr = SI7021_ADDR;
  msg[1].flags = I2C_M_RD;
  msg[1].len = 3;
  msg[1].buf = buf;

  data.nmsgs = 1;
  data.msgs = msg;
  
  // err = ioctl(fd, I2CRDWR, &data);
  err = ioctl(fd, I2C_RDWR, &data);

  if (err < 0) {
    fprintf(stderr, "sht_read M_RH_NOHOLD_MM error %s: %s\n",
        dev, strerror(errno));
    close(fd);
    return -1;
  }

  i = 10;

  do {

    sleep_wait(10);

    // msg[0].slave = SI7021_ADDR << 1;
    // msg[0].flags = IIC_M_RD;
	msg[0].addr = SI7021_ADDR;
    msg[0].flags = I2C_M_RD;
    msg[0].len = 3;
    msg[0].buf = buf;

    data.nmsgs = 1;
    data.msgs = msg;
	
  // err = ioctl(fd, I2CRDWR, &data);
  err = ioctl(fd, I2C_RDWR, &data);

  } while (err && i--);

  if (err < 0) {
    fprintf(stderr, "sht_read M_RH_NOHOLD_MM timeout error %s: %s\n",
        dev, strerror(errno));
    close(fd);
    return -1;
  }

  s->rh = (uint16_t)buf[0] << 8 | (buf[1] & 0xFC);
  crc = buf[2];

  // read T from previous RH measurement, crc is not available

  reg = SI7021_M_T_PREV_M;
  // msg[0].slave = SI7021_ADDR << 1;
  // msg[0].flags = IIC_M_WR;
  msg[0].addr = SI7021_ADDR;
  msg[0].flags = 0;
  msg[0].len = 1;
  msg[0].buf = &reg;

  // msg[1].slave = SI7021_ADDR << 1;
  // msg[1].flags = IIC_M_RD;
  msg[1].addr = SI7021_ADDR;
  msg[1].flags = I2C_M_RD;
  msg[1].len = 2;
  msg[1].buf = buf;

  data.nmsgs = 2;
  data.msgs = msg;
  
  // err = ioctl(fd, I2CRDWR, &data);
  err = ioctl(fd, I2C_RDWR, &data);

  if (err < 0) {
    fprintf(stderr, "sht_read M_T_PREV_M error %s: %s\n",
        dev, strerror(errno));
    close(fd);
    return -1;
  }

  s->t = (uint16_t)buf[0] << 8 | (buf[1] & 0xFC);

  close(fd);
  return 0;

}

static int sht_conv(struct sht_sample *s) {

  s->rh = 12500 * s->rh / 65536 - 600;
  s->t = 17572 * s->t / 65536 - 4685;
  return 0;

}

int main(int argc, char** argv) {

	int err;
    char fn[0x10], ts[0x20], fp[0x100], lb[0x100];
	
	time_t raw_time;
	struct tm *tm_ptr;
	struct opt opts;

	struct sht_sample sample;
	
	printf("Si7021 Sensor Logger Ver. 0.1\n");
	
	opts.dev_name = DEFAULT_I2C_BUS;
	opts.log_dst = DEFAULT_LOG_DST;
	opts.log_int = DEFAULT_LOG_INT;

	err = get_opts(argc, argv, &opts);
	if (err < 0) return -1;

	printf("running on device %s, logging to '%s', interval: %u\n",
		opts.dev_name, opts.log_dst, opts.log_int);
	
  while (1) {

    interval_wait(opts.log_int);

    raw_time = time(NULL);
    tm_ptr = localtime(&raw_time);
    strftime(fn, sizeof(fn), "%Y%m%d.log", tm_ptr);
    strftime(ts, sizeof(ts), "%F %T %Z", tm_ptr);
	
    err = sht_read(opts.dev_name, &sample);
    if (err < 0) continue;
    sht_conv(&sample);
	
    sprintf(lb, "%s, %0.2f, %0.2f\n", ts, 0.01f * sample.t, 0.01f * sample.rh); // log entry
	sprintf(fp, "%s%s", opts.log_dst, fn); // build full file path
	printf("%s <- %s", fp, lb); // write to console
	err = write_log(fp, lb); // write to log file
	if (err < 0) return -1;
	
  }
	
}
