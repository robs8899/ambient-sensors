#include <stdint.h>
#include "../core/core.h"
#include "bmp180.h"

#define BMP180_COEFF     0xAA
#define BMP180_CHPID     0xD0
#define BMP180_RESET     0xE0
#define BMP180_MCTRL     0xF4
#define BMP180_OUT_MSB   0xF6
#define BMP180_OUT_LSB   0xF7
#define BMP180_OUT_XLSB  0xF8

#define BMP180_CMD_UT    0x2E
#define BMP180_CMD_UP    0x34

int bmp_read_coeff(struct bmp_coeff *coeff) {

  uint8_t buf[22];

  i2c_addr(BMP180_ADDR);

  // there are 11 calibration registers, each with 16bit size
  if (i2c_read_reg(BMP180_COEFF, buf, 22) < 0) return -1;

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
  
  return 0;

}

int bmp_read_raw(struct bmp_raw *raw) {

  uint8_t buf[3];

  raw->ut = 0;
  raw->up = 0;

  if(raw->oss < 1 || raw->oss > 3) return -1;

  i2c_addr(BMP180_ADDR);

  // send command for uncompensated temperature
  buf[0] = BMP180_CMD_UT;
  if (i2c_write_reg(BMP180_MCTRL, buf, 1) < 0) return -2;
  wait(5000); // 4.5ms according to the data sheet

  // read registers at 0xF6 ff.
  if (i2c_read_reg(BMP180_OUT_MSB, buf, 2) < 0) return -3;
  raw->ut = (uint32_t)buf[0] << 8 | (uint32_t)buf[1];

  // send command for uncompensated pressure
  buf[0] = BMP180_CMD_UP + (raw->oss << 6);
  if (i2c_write_reg(BMP180_MCTRL, buf, 1) < 0) return -4;
  wait(26000); // max. 25.5ms at oss=3

  // read registers at 0xF6 ff.
  if (i2c_read_reg(BMP180_OUT_MSB, buf, 3) < 0) return -5;
  raw->up = (((uint32_t)buf[0] << 16) | ((uint32_t)buf[1] << 8) |
      (uint32_t)buf[2]) >> (8 - raw->oss);

  return 0;

}

int bmp_comp(const struct bmp_coeff *coeff,
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
