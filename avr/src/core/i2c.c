#include <stdint.h>
#include <avr/io.h>
#include <util/twi.h>

#include "core.h"

#define TWI_CLOCK 400000 // I2C clock
static uint8_t i2c_address;

// ---------------------- TWI private ----------------------

static int twi_start() {

  TWCR = (1<<TWINT) | (1<<TWEN) | (1<<TWSTA); // START condition
  while (!(TWCR & (1<<TWINT))); // wait for completion

  // check status with masked prescaler bits
  if (TW_STATUS != TW_START && TW_STATUS != TW_REP_START)
    return -1; // no success!
  return 0;

}

static int twi_stop() {

  TWCR = (1<<TWINT) | (1<<TWEN) | (1<<TWSTO); // STOP condition
  while (TWCR & (1<<TWSTO)); // wait for completion
  return 0;

}

static int twi_slar() {

  TWDR = (i2c_address << 1) | TW_READ; // load slave address
  TWCR = (1<<TWINT) | (1<<TWEN); // start transmission
  while (!(TWCR & (1<<TWINT))); // wait for completion

  if (TW_STATUS != TW_MR_SLA_ACK) { // check ACK
    TWCR = (1<<TWINT) | (1<<TWSTO); // recover from bus error
    return -3; // return ack error
  }

  return 0;

}

static int twi_slaw() {

  TWDR = i2c_address << 1 | TW_WRITE; // load slave address
  TWCR = (1<<TWINT) | (1<<TWEN); // start transmission
  while (!(TWCR & (1<<TWINT))); // wait for completion

  if (TW_STATUS != TW_MT_SLA_ACK) { // check ACK
    TWCR = (1<<TWINT) | (1<<TWSTO); // recover from bus error
    return -3; // return ack error
  }

  return 0;

}

static int twi_rxa(uint8_t *data) {

  TWCR = (1<<TWINT) | (1<<TWEN) | (1<<TWEA); // read with ACK
  while(!(TWCR & (1<<TWINT))); // wait for completion
  *data = TWDR;
  return 0;

}

static int twi_rxn(uint8_t *data) {

  TWCR = (1<<TWINT) | (1<<TWEN); // read with NACK
  while(!(TWCR & (1<<TWINT))); // wait for completion
  *data = TWDR;
  return 0;

}

static int twi_tx(uint8_t data) {

  TWDR = data; // load data
  TWCR = (1<<TWINT) | (1<<TWEN); // start transmission
  while (!(TWCR & (1<<TWINT))); // wait for completion

  if (TW_STATUS != TW_MT_DATA_ACK) { // check ACK
    TWCR = (1<<TWINT) | (1<<TWSTO); // recover from bus error
    return -3; // return ack error
  }

  return 0;

}

// ---------------------- I2C public -----------------------

void i2c_init() {

  TWSR = 0; // prescaler x1
  TWBR = ((F_CPU/TWI_CLOCK)-16)/2; // clock in Hz

}

void i2c_done() {

  TWCR = 0; // disable TWI

}

void i2c_addr(uint8_t addr) {

  // we need an extra variable because
  // TWDR is used for both, address and data.
  i2c_address = addr;

}

int i2c_read(uint8_t *buf, int len) {

  int ret;

  if ((ret = twi_start())) return ret; // start condition
  if ((ret = twi_slar())) return ret; // send slave address for read
  while (--len) if ((ret = twi_rxa(buf++))) return ret; // receive bytes
  if ((ret = twi_rxn(buf))) return ret; // receive last byte
  return twi_stop(); // stop condition

}

int i2c_write(const uint8_t *buf, int len) {

  int ret;

  if ((ret = twi_start())) return ret; // start condition
  if ((ret = twi_slaw())) return ret; // send slave address for write
  while (len--) if ((ret = twi_tx(*buf++))) return ret; // send bytes
  return twi_stop(); // stop condition

}

int i2c_read_reg(uint8_t reg, uint8_t *buf, int len) {

  int ret;

  if ((ret = twi_start())) return ret; // start condition
  if ((ret = twi_slaw())) return ret; // send slave address for write
  if ((ret = twi_tx(reg))) return ret; // send register address
  if ((ret = twi_start())) return ret; // send repeated start condition
  if ((ret = twi_slar())) return ret; // send slave address for read
  while (--len) if ((ret = twi_rxa(buf++))) return ret; // receive bytes with ACK
  if ((ret = twi_rxn(buf))) return ret; // receive last byte with NACK
  return twi_stop(); // stop condition

}

int i2c_write_reg(uint8_t reg, const uint8_t *buf, int len) {

  int ret;

  if ((ret = twi_start())) return ret; // start condition
  if ((ret = twi_slaw())) return ret; // send slave address for write
  if ((ret = twi_tx(reg))) return ret; // send register address
  while (len--) if ((ret = twi_tx(*buf++))) return ret; // send bytes
  return twi_stop(); // stop condition

}

int i2c_write_pad(uint8_t cmd, uint8_t data, int len) {

  int ret;

  if ((ret = twi_start())) return ret; // start condition
  if ((ret = twi_slaw())) return ret; // send slave address for write
  if ((ret = twi_tx(cmd))) return ret; // send command
  while (len--) if ((ret = twi_tx(data))) return ret; // send bytes
  return twi_stop(); // stop condition

}

//int i2c_read_addr(uint16_t addr, uint8_t *buf, int len) {
//
//  int ret;
//  uint8_t msb, lsb;
//
//  msb = addr >> 8;
//  lsb = addr & 0xFF;
//
//  if ((ret = twi_start())) return ret; // start condition
//  if ((ret = twi_slaw())) return ret; // send slave address for write
//  if ((ret = twi_tx(msb))) return ret; // send 16 bit MSB address
//  if ((ret = twi_tx(lsb))) return ret; // send 16 bit LSB address
//  if ((ret = twi_start())) return ret; // send repeated start condition
//  if ((ret = twi_slar())) return ret; // send slave address for read
//  while (--len) if ((ret = twi_rxa(buf++))) return ret; // receive bytes with ACK
//  if ((ret = twi_rxn(buf))) return ret; // receive last byte with NACK
//  return twi_stop(); // stop condition
//
//}
//
//int i2c_write_addr(uint16_t addr, const uint8_t *buf, int len) {
//
//  int i=0, ret;
//  uint8_t msb, lsb;
//
//  msb = addr >> 8;
//  lsb = addr & 0xFF;
//
//  if ((ret = twi_start())) return ret; // start condition
//  if ((ret = twi_slaw())) return ret; // send slave address for write
//  if ((ret = twi_tx(msb))) return ret; // send 16 bit MSB address
//  if ((ret = twi_tx(lsb))) return ret; // send 16 bit LSB address
//  while (len--) if ((ret = twi_tx(buf[i++]))) return ret; // send bytes
//  return twi_stop(); // stop condition
//
//}
//
//int i2c_poll_ack() {
//
//  int ret, max = 1000;
//
//  do { // Wait for ACK with timeout of 100ms
//    if ((ret = twi_start())) return ret; // start condition
//    if (!(ret = twi_slaw())) return twi_stop(); // stop condition and return
//    wait(100);
//  } while (max--);
//
//  return -3; // ACK timeout
//
//}
