#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <math.h>
#include <time.h>
#include <unistd.h>

#include "util.h"
#include "i2c.h"
#include "bme280.h"

// sensor calibration data

uint16_t dig_T1;
int16_t  dig_T2;
int16_t  dig_T3;
uint16_t dig_P1;
int16_t  dig_P2;
int16_t  dig_P3;
int16_t  dig_P4;
int16_t  dig_P5;
int16_t  dig_P6;
int16_t  dig_P7;
int16_t  dig_P8;
int16_t  dig_P9;
uint8_t  dig_H1;
uint16_t dig_H2;
uint8_t  dig_H3;
int16_t  dig_H4;
int16_t  dig_H5;
int8_t   dig_H6;

static int bme280_startup(char *dev) {

  int fd, err;
  uint8_t buf[26];

  fd = open(dev, O_RDWR);

  if (fd < 0) {
    fprintf(stderr, "error opening device - %s: %s\n",
        dev, strerror(errno));
    return -1;
  }

  // --- write ctrl_hum ---

  buf[0] = BME280_CTRL_HUM_VAL;
  err = i2c_write(fd, BME280_ADDR, BME280_CTRL_HUM, buf, 1);

  if (err < 0) {
    fprintf(stderr, "bme280 write ctrl_hum error - %s: %s\n",
        dev, strerror(errno));
    close(fd);
    return -1;
  }

  // --- write ctrl_meas ---

  buf[0] = BME280_CTRL_MEAS_VAL;
  err = i2c_write(fd, BME280_ADDR, BME280_CTRL_MEAS, buf, 1);

  if (err < 0) {
    fprintf(stderr, "bme280 write ctrl_meas error - %s: %s\n",
        dev, strerror(errno));
    close(fd);
    return -1;
  }

  // --- write config ---

  buf[0] = BME280_CONFIG_VAL;
  err = i2c_write(fd, BME280_ADDR, BME280_CONFIG, buf, 1);

  if (err < 0) {
    fprintf(stderr, "bme280 write config error - %s: %s\n",
        dev, strerror(errno));
    close(fd);
    return -1;
  }

  // --- read calibration #1 ---

  err = i2c_read(fd, BME280_ADDR, BME280_CALIB00, buf, 26);

  if (err < 0) {
    fprintf(stderr, "bme280 read calibration error - %s: %s\n",
        dev, strerror(errno));
    close(fd);
    return -1;
  }

  dig_T1 = (uint16_t)buf[ 0] | buf[ 1] << 8;
  dig_T2 = (int16_t )buf[ 2] | buf[ 3] << 8;
  dig_T3 = (int16_t )buf[ 4] | buf[ 5] << 8;
  dig_P1 = (uint16_t)buf[ 6] | buf[ 7] << 8;
  dig_P2 = (int16_t )buf[ 8] | buf[ 9] << 8;
  dig_P3 = (int16_t )buf[10] | buf[11] << 8;
  dig_P4 = (int16_t )buf[12] | buf[13] << 8;
  dig_P5 = (int16_t )buf[14] | buf[15] << 8;
  dig_P6 = (int16_t )buf[16] | buf[17] << 8;
  dig_P7 = (int16_t )buf[18] | buf[19] << 8;
  dig_P8 = (int16_t )buf[20] | buf[21] << 8;
  dig_P9 = (int16_t )buf[22] | buf[23] << 8;
  dig_H1 = (uint8_t )buf[25];

  // --- read calibration #2 ---

  err = i2c_read(fd, BME280_ADDR, BME280_CALIB26, buf, 25);

  if (err < 0) {
    fprintf(stderr, "bme280 read calibration error - %s: %s\n",
        dev, strerror(errno));
    close(fd);
    return -1;
  }

  dig_H2 = (int16_t)buf[0] | buf[1] << 8;
  dig_H3 = (uint8_t)buf[2];
  dig_H4 = (int16_t)buf[3] << 4 | (0x0F & buf[4]);
  dig_H5 = (int16_t)buf[5] << 4 | (0x0F & (buf[4] >> 4));
  dig_H6 = (int8_t )buf[6];

  close(fd);
  return 0;

}

static int bme280_readout(char *dev, int32_t *rp, int32_t *rt, int32_t *rh) {

  int fd, err;
  uint8_t buf[8];

  fd = open(dev, O_RDWR);

  if (fd < 0) {
    fprintf(stderr, "error opening device - %s: %s\n",
        dev, strerror(errno));
    return -1;
  }

  err = i2c_read(fd, BME280_ADDR, BME280_PRESS_MSB, buf, 8);

  if (err < 0) {
    fprintf(stderr, "bme280 readout error - %s: %s\n",
        dev, strerror(errno));
    close(fd);
    return -1;
  }

  sleep_wait(10);

  *rp = (buf[0] << 12) | (buf[1] << 4) | (buf[2] >> 4);
  *rt = (buf[3] << 12) | (buf[4] << 4) | (buf[5] >> 4);
  *rh = (buf[6] << 8) | buf[7];

  close(fd);
  return 0;

}

// --- Original code from BST-BME280-DS002-13 ---

#define BME280_S32_t signed long
#define BME280_U32_t unsigned long
#define BME280_S64_t signed long long

BME280_S32_t t_fine;

// Returns temperature in DegC, resolution is 0.01 DegC. Output value of "5123" equals 51.23 DegC.
// t_fine carries fine temperature as global value

BME280_S32_t bme280_comp_t(BME280_S32_t adc_T) {

  BME280_S32_t var1, var2, T;
  var1  = ((((adc_T>>3) - ((BME280_S32_t)dig_T1<<1))) * ((BME280_S32_t)dig_T2)) >> 11;
  var2  = (((((adc_T>>4) - ((BME280_S32_t)dig_T1)) * ((adc_T>>4) - ((BME280_S32_t)dig_T1))) >> 12) * ((BME280_S32_t)dig_T3)) >> 14;
  t_fine = var1 + var2;
  T  = (t_fine * 5 + 128) >> 8;
  return T;

}

// Returns pressure in Pa as unsigned 32 bit integer in Q24.8 format (24 integer bits and 8 fractional bits).
// Output value of "24674867" represents 24674867/256 = 96386.2 Pa = 963.862 hPa

BME280_U32_t bme280_comp_p(BME280_S32_t adc_P) {

  BME280_S64_t var1, var2, p;
  var1 = ((BME280_S64_t)t_fine) - 128000;
  var2 = var1 * var1 * (BME280_S64_t)dig_P6;
  var2 = var2 + ((var1*(BME280_S64_t)dig_P5)<<17);
  var2 = var2 + (((BME280_S64_t)dig_P4)<<35);var1 = ((var1 * var1 * (BME280_S64_t)dig_P3)>>8) + ((var1 * (BME280_S64_t)dig_P2)<<12);
  var1 = (((((BME280_S64_t)1)<<47)+var1))*((BME280_S64_t)dig_P1)>>33;
  if(var1 == 0) {
    return 0; // avoid exception caused by division by zero
  }
  p = 1048576-adc_P;
  p = (((p<<31)-var2)*3125)/var1;var1 = (((BME280_S64_t)dig_P9) * (p>>13) * (p>>13)) >> 25;
  var2 =(((BME280_S64_t)dig_P8) * p) >> 19;
  p = ((p + var1 + var2) >> 8) + (((BME280_S64_t)dig_P7)<<4);
  return(BME280_U32_t)p;
}

// Returns humidity in %RH as unsigned 32 bit integer in Q22.10 format (22 integer and 10 fractional bits).
// Output value of "47445" represents 47445/1024 = 46.333 %RH

BME280_U32_t bme280_comp_h(BME280_S32_t adc_H) {

  BME280_S32_t v_x1_u32r;
  v_x1_u32r = (t_fine - ((BME280_S32_t)76800));
  v_x1_u32r = (((((adc_H << 14) - (((BME280_S32_t)dig_H4) << 20) - (((BME280_S32_t)dig_H5) * v_x1_u32r)) + ((BME280_S32_t)16384)) >> 15) * (((((((v_x1_u32r * ((BME280_S32_t)dig_H6)) >> 10) * (((v_x1_u32r * ((BME280_S32_t)dig_H3)) >> 11) + ((BME280_S32_t)32768))) >> 10) + ((BME280_S32_t)2097152)) * ((BME280_S32_t)dig_H2) + 8192) >> 14));
  v_x1_u32r = (v_x1_u32r - (((((v_x1_u32r >> 15) * (v_x1_u32r >> 15)) >> 7) * ((BME280_S32_t)dig_H1)) >> 4));
  v_x1_u32r = (v_x1_u32r < 0 ? 0 : v_x1_u32r); v_x1_u32r = (v_x1_u32r > 419430400 ? 419430400 : v_x1_u32r);
  return (BME280_U32_t)(v_x1_u32r>>12);

}

int main(int argc, char** argv) {

  int err;
  char fn[0x10], ts[0x20], fp[0x100], lb[0x100];

  time_t raw_time;
  struct tm *tm_ptr;
  struct opt opts;

  int32_t rp, rt, rh; // raw values
  int32_t cp, ct, ch; // compensated values

  printf("BME280 Sensor Logger Ver. 0.1\n");

  opts.dev_name = DEFAULT_I2C_BUS;
  opts.log_dst = DEFAULT_LOG_DST;
  opts.log_int = DEFAULT_LOG_INT;

  err = get_opts(argc, argv, &opts);
  if (err < 0) return -1;

  printf("running on device %s, logging to '%s', interval: %u\n",
      opts.dev_name, opts.log_dst, opts.log_int);

  err = bme280_startup(opts.dev_name);
  if (err < 0) return -1;

  while (1) {

    interval_wait(opts.log_int);

    raw_time = time(NULL);
    tm_ptr = localtime(&raw_time);
    strftime(fn, sizeof(fn), "%Y%m%d.log", tm_ptr);
    strftime(ts, sizeof(ts), "%F %T %Z", tm_ptr);

    // --- data readout ---
    err = bme280_readout(opts.dev_name, &rp, &rt, &rh);
    if (err < 0) return -1;

    ct = bme280_comp_t(rt);
    cp = bme280_comp_p(rp);
    ch = bme280_comp_h(rh);

    // calulate sea level pressure
    cp = (double)cp/pow(1.0-(288.0/44330.0), 5.255);

    sprintf(lb, "%s, %0.2f, %0.2f, %0.2f\n", ts, (float)cp/256/100, (float)ct/100, (float)ch/1024); // log entry
    sprintf(fp, "%s%s", opts.log_dst, fn); // build full file path
    printf("%s <- %s", fp, lb); // write to console
    err = write_log(fp, lb); // write to log file
    if (err < 0) return -1;

  }

}
