#ifndef I2C_H_
#define I2C_H_

int i2c_read(int fd, uint16_t addr, uint8_t reg, uint8_t *buf, int len);
int i2c_write(int fd, uint16_t addr, uint8_t reg, uint8_t *buf, int len);

#endif /* I2C_H_ */

