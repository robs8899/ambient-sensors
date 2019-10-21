#include <stdint.h>
#include "../core/core.h"
#include "si7021.h"

#define SI7021_M_RH_HOLD_MM    0xE5
#define SI7021_M_RH_NOHOLD_MM  0xF5
#define SI7021_M_T_HOLD_MM     0xE3
#define SI7021_M_T_NOHOLD_MM   0xF3
// #define SI7021_M_T_PREV_M      0xE0 // Si7021 only!
#define SI7021_RESET           0xFE
#define SI7021_W_USER_REG      0xE6
#define SI7021_R_USER_REG      0xE7

int sht_read(struct sht_sample *s) {

  uint8_t buf[3];
  uint8_t crc;

  i2c_addr(SI7021_ADDR);

  // Read RH with "Hold Master Mode"
  if (i2c_read_reg(SI7021_M_RH_HOLD_MM, buf, 3) < 0) return -1;
  s->rh = (uint16_t)buf[0] << 8 | (buf[1] & 0xFC);
  crc = buf[2]; // TODO check if valid

  // Read T from previous RH measurement (Si7021 only!)
  // if (i2c_read_reg(SI7021_M_T_PREV_M, buf, 2) < 0) return -1;
  // s->t = (uint16_t)buf[0] << 8 | (buf[1] & 0xFC);

  // Read T with "Hold Master Mode"
  if (i2c_read_reg(SI7021_M_T_HOLD_MM, buf, 3) < 0) return -1;
  s->t = (uint16_t)buf[0] << 8 | (buf[1] & 0xFC);
  crc = buf[2]; // TODO check if valid

  return 0;

}

int sht_conv(struct sht_sample *s) {

  s->rh = 12500L * s->rh / 65536 - 600;
  s->t = 17572L * s->t / 65536 - 4685;
  return 0;

}

int sht_read_ureg(uint8_t *ur) {

  i2c_addr(SI7021_ADDR);

  // Read the User Register
  if (i2c_read_reg(SI7021_R_USER_REG, ur, 1) < 0) return -1;
  return 0;

}

int sht_write_ureg(uint8_t *ur) {

  i2c_addr(SI7021_ADDR);

  // Write the User Register
  if (i2c_write_reg(SI7021_W_USER_REG, ur, 1) < 0) return -1;
  return 0;

}

int sht_reset(void) {

  uint8_t reset = SI7021_RESET;
  i2c_addr(SI7021_ADDR);

  if (i2c_write(&reset, 1) < 0) return -1;
  return 0;

}
