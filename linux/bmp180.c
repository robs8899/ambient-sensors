#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <math.h>
#include <time.h>
#include <unistd.h>

#include <sys/ioctl.h>
//#include <dev/iicbus/iic.h>
#include <linux/i2c.h>
#include <linux/i2c-dev.h>

#include "util.h"

#define BMP180_ADDR      0x77

#define BMP180_COEFF     0xAA
#define BMP180_CHPID     0xD0
#define BMP180_RESET     0xE0
#define BMP180_MCTRL     0xF4
#define BMP180_OUT_MSB   0xF6
#define BMP180_OUT_LSB   0xF7
#define BMP180_OUT_XLSB  0xF8
#define BMP180_CMD_UT    0x2E
#define BMP180_CMD_UP    0x34

struct bmp_coeff {

  int16_t ac1;
  int16_t ac2;
  int16_t ac3;
  uint16_t ac4;
  uint16_t ac5;
  uint16_t ac6;
  int16_t b1;
  int16_t b2;
  int16_t mb;
  int16_t mc;
  int16_t md;

};

struct bmp_raw {

  uint8_t oss; // oversampling: 1, 2 or 3
  int32_t ut; // uncompensated temperature
  uint32_t up; // uncompensated pressure

};

struct bmp_comp {

  int32_t ct; // compensated temperature, scaled with 10
  int32_t cp; // compensated pressure, scaled with 100

};

static int bmp_read_coeff(char *dev, struct bmp_coeff *coeff) {

  int fd, err;
  uint8_t reg, buf[22];

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

  reg = BMP180_COEFF;
  // msg[0].slave = BMP180_ADDR << 1;
  // msg[0].flags = IIC_M_WR;
  msg[0].addr = BMP180_ADDR;
  msg[0].flags = 0;
  msg[0].len = 1;
  msg[0].buf = &reg;

  // msg[1].slave = BMP180_ADDR << 1;
  // msg[1].flags = IIC_M_RD;
  msg[1].addr = BMP180_ADDR;
  msg[1].flags = I2C_M_RD;
  msg[1].len = 22;
  msg[1].buf = buf;

  data.nmsgs = 2;
  data.msgs = msg;

  // err = ioctl(fd, I2CRDWR, &data);
  err = ioctl(fd, I2C_RDWR, &data);

  if (err < 0) {
    fprintf(stderr, "bmp_read_coeff error %s: %s\n",
        dev, strerror(errno));
    close(fd);
    return -1;
  }

  coeff->ac1 = (int16_t )buf[ 0] << 8 | buf[ 1];
  coeff->ac2 = (int16_t )buf[ 2] << 8 | buf[ 3];
  coeff->ac3 = (int16_t )buf[ 4] << 8 | buf[ 5];
  coeff->ac4 = (uint16_t)buf[ 6] << 8 | buf[ 7];
  coeff->ac5 = (uint16_t)buf[ 8] << 8 | buf[ 9];
  coeff->ac6 = (uint16_t)buf[10] << 8 | buf[11];
  coeff->b1  = (int16_t )buf[12] << 8 | buf[13];
  coeff->b2  = (int16_t )buf[14] << 8 | buf[15];
  coeff->mb  = (int16_t )buf[16] << 8 | buf[17];
  coeff->mc  = (int16_t )buf[18] << 8 | buf[19];
  coeff->md  = (int16_t )buf[20] << 8 | buf[21];

  close(fd);
  return 0;

}

static int bmp_read_raw(char *dev, struct bmp_raw *raw) {

  int fd, err;
  uint8_t reg, buf[3];

  // struct iic_msg msg[2];
  // struct iic_rdwr_data data;

  struct i2c_msg msg[2];
  struct i2c_rdwr_ioctl_data data;

  if(raw->oss < 1 || raw->oss > 3) {
    fprintf(stderr, "invalid oss value: %u\n", raw->oss);
    return -1;
  }

  fd = open(dev, O_RDWR);

  if (fd == -1) {
    fprintf(stderr, "error opening device %s: %s\n",
        dev, strerror(errno));
    return -1;
  }

  // send command for uncompensated temperature

  buf[0] = BMP180_MCTRL;
  buf[1] = BMP180_CMD_UT;

  // msg[0].slave = BMP180_ADDR << 1;
  // msg[0].flags = IIC_M_WR;
  msg[0].addr = BMP180_ADDR;
  msg[0].flags = 0;
  msg[0].len = 2;
  msg[0].buf = buf;

  data.nmsgs = 1;
  data.msgs = msg;

  // err = ioctl(fd, I2CRDWR, &data);
  err = ioctl(fd, I2C_RDWR, &data);

  if (err < 0) {
    fprintf(stderr, "bmp_read_raw MCTRL/UT error %s: %s\n",
        dev, strerror(errno));
    close(fd);
    return -1;
  }

  sleep_wait(10); // max. 4.5ms according to the data sheet

  // read registers at 0xF6 ff.

  reg = BMP180_OUT_MSB;
  // msg[0].slave = BMP180_ADDR << 1;
  // msg[0].flags = IIC_M_WR;
  msg[0].addr = BMP180_ADDR;
  msg[0].flags = 0;
  msg[0].len = 1;
  msg[0].buf = &reg;

  // msg[1].slave = BMP180_ADDR << 1;
  // msg[1].flags = IIC_M_RD;
  msg[1].addr = BMP180_ADDR;
  msg[1].flags = I2C_M_RD;
  msg[1].len = 2;
  msg[1].buf = buf;

  data.nmsgs = 2;
  data.msgs = msg;

  // err = ioctl(fd, I2CRDWR, &data);
  err = ioctl(fd, I2C_RDWR, &data);

  if (err < 0) {
    fprintf(stderr, "bmp_read_raw OUT_MSB error %s: %s\n",
        dev, strerror(errno));
    close(fd);
    return -1;
  }

  raw->ut = (uint32_t)buf[0] << 8 | (uint32_t)buf[1];

  // send command for uncompensated pressure

  buf[0] = BMP180_MCTRL;
  buf[1] = BMP180_CMD_UP + (raw->oss << 6);

  // msg[0].slave = BMP180_ADDR << 1;
  // msg[0].flags = IIC_M_WR;
  msg[0].addr = BMP180_ADDR;
  msg[0].flags = 0;
  msg[0].len = 2;
  msg[0].buf = buf;

  data.nmsgs = 1;
  data.msgs = msg;

  // err = ioctl(fd, I2CRDWR, &data);
  err = ioctl(fd, I2C_RDWR, &data);

  if (err < 0) {
    fprintf(stderr, "bmp_read_raw MCTRL/UP error %s: %s\n",
        dev, strerror(errno));
    close(fd);
    return -1;
  }

  sleep_wait(26); // max. 25.5ms at oss=3

  // read registers at 0xF6 ff.

  reg = BMP180_OUT_MSB;
  // msg[0].slave = BMP180_ADDR << 1;
  // msg[0].flags = IIC_M_WR;
  msg[0].addr = BMP180_ADDR;
  msg[0].flags = 0;
  msg[0].len = 1;
  msg[0].buf = &reg;

  // msg[1].slave = BMP180_ADDR << 1;
  // msg[1].flags = IIC_M_RD;
  msg[1].addr = BMP180_ADDR;
  msg[1].flags = I2C_M_RD;
  msg[1].len = 3;
  msg[1].buf = buf;

  data.nmsgs = 2;
  data.msgs = msg;

  // err = ioctl(fd, I2CRDWR, &data);
  err = ioctl(fd, I2C_RDWR, &data);

  if (err < 0) {
    fprintf(stderr, "bmp_read_raw OUT_MSB error %s: %s\n",
        dev, strerror(errno));
    close(fd);
    return -1;
  }

  raw->up = (((uint32_t)buf[0] << 16) | ((uint32_t)buf[1] << 8) |
      (uint32_t)buf[2]) >> (8 - raw->oss);

  close(fd);
  return 0;

}

static int bmp_conv(const struct bmp_coeff *coeff,
    const struct bmp_raw *raw, struct bmp_comp *comp) {

  int32_t x1, x2, x3, b3, b5, b6;
  uint32_t b4, b7;

  comp->ct = 0;
  comp->cp = 0;

  if(raw->oss < 1 || raw->oss > 3) return -1;

  x1 = (raw->ut - coeff->ac6) * coeff->ac5 >> 15;
  x2 = ((int32_t)coeff->mc << 11) / (x1 + coeff->md);
  b5 = x1 + x2;
  comp->ct = (b5 + 8) >> 4;

  b6 = b5 - 4000;
  x1 = (coeff->b2 * (b6 * b6) >> 12) >> 11;
  x2 = (coeff->ac2 * b6) >> 11;
  x3 = x1 + x2;
  b3 = ((((int32_t)coeff->ac1 * 4 + x3) << raw->oss) + 2) >> 2;

  x1 = (coeff->ac3 * b6) >> 13;
  x2 = (coeff->b1 * ((b6 * b6) >> 12)) >> 16;
  x3 = ((x1 + x2) + 2) >> 2;
  b4 = (coeff->ac4 * (x3 + 32768)) >> 15;

  b7 = ((raw->up - b3) * (50000 >> raw->oss));
  if (b7 < 0x80000000) comp->cp = (b7 << 1) / b4;
  else comp->cp = (b7 / b4) << 1;

  x1 = (comp->cp >> 8) * (comp->cp >> 8);
  x1 = (x1 * 3038) >> 16;
  x2 = (-7357 * comp->cp) >> 16;
  comp->cp += (x1 + x2 + 3791) >> 4;

  return 0;

}

int main(int argc, char** argv) {

  int err;
  char fn[0x10], ts[0x20], fp[0x100], lb[0x100];

  time_t raw_time;
  struct tm *tm_ptr;
  struct opt opts;

  struct bmp_coeff coeff;
  struct bmp_raw raw;
  struct bmp_comp comp;

  printf("BMP180 Sensor Logger Ver. 0.1\n");

  opts.dev_name = DEFAULT_I2C_BUS;
  opts.log_dst = DEFAULT_LOG_DST;
  opts.log_int = DEFAULT_LOG_INT;

  err = get_opts(argc, argv, &opts);
  if (err < 0) return -1;

  printf("running on device %s, logging to '%s', interval: %u\n",
      opts.dev_name, opts.log_dst, opts.log_int);

  err = bmp_read_coeff(opts.dev_name, &coeff);
  if (err < 0) return -1;

  raw.oss = 3;

  while (1) {

    interval_wait(opts.log_int);

    raw_time = time(NULL);
    tm_ptr = localtime(&raw_time);
    strftime(fn, sizeof(fn), "%Y%m%d.log", tm_ptr);
    strftime(ts, sizeof(ts), "%F %T %Z", tm_ptr);

    err = bmp_read_raw(opts.dev_name, &raw);
    if (err < 0) continue;
    bmp_conv(&coeff, &raw, &comp);

    // calulate sea level pressure
    comp.cp = (double)comp.cp/pow(1.0-(288.0/44330.0), 5.255);

    sprintf(lb, "%s, %0.2f, %0.2f\n", ts, 0.1f * comp.ct, 0.01f * comp.cp); // log entry
    sprintf(fp, "%s%s", opts.log_dst, fn); // build full file path
    printf("%s <- %s", fp, lb); // write to console
    err = write_log(fp, lb); // write to log file
    if (err < 0) return -1;

  }

}
