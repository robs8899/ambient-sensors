#include <stdint.h>
#include <avr/pgmspace.h>
#include "../core/core.h"
#include "si4432.h"

static int ezr_port;
static int nirq_pin;
static int sdn_pin;

void ezr_init(int port, int nirq, int sdn) {

  ezr_port = port;
  nirq_pin = nirq;
  sdn_pin = sdn;

  gpio_prt(ezr_port); // set the i/o port
  gpio_pue(nirq_pin); // enable pull up
  gpio_inp(nirq_pin); // set as input
  gpio_out(sdn_pin); // set as output
  gpio_set(sdn_pin); // device down

  wait(1000); // ensure a minimum down time

}

void ezr_startup() {

  gpio_prt(ezr_port); // set the i/o port
  gpio_clr(sdn_pin); // device up

}

void ezr_shutdown() {

  gpio_prt(ezr_port); // set the i/o port
  gpio_set(sdn_pin); // device down
  wait(1000); // ensure a minimum down time

}

uint8_t ezr_get(uint8_t reg) {

  uint8_t ret;

  spi_cs(SI4432_NSEL_PIN);
  spi_trx(reg);
  ret = spi_trx(0xFF);
  spi_ds(SI4432_NSEL_PIN);
  return ret;

}

uint8_t ezr_set(uint8_t reg, uint8_t val) {

  uint8_t ret;

  spi_cs(SI4432_NSEL_PIN);
  spi_trx(0x80 | reg);
  ret = spi_trx(val);
  spi_ds(SI4432_NSEL_PIN);
  return ret;

}

/* Reads data from FIFO, len must be
 * less than available FIFO size. */
void ezr_rx(void *data, int len) {

  spi_cs(SI4432_NSEL_PIN);
  spi_trx(SI4432_REG_FIFO);
  spi_rxn(data, len);
  spi_ds(SI4432_NSEL_PIN);

}

/* Writes data to FIFO, len must be
 * less than available FIFO size. */
void ezr_tx(void *data, int len) {

  spi_cs(SI4432_NSEL_PIN);
  spi_trx(0x80 | SI4432_REG_FIFO);
  spi_txn(data, len);
  spi_ds(SI4432_NSEL_PIN);

}

int ezr_poll(uint8_t s1, uint8_t s2, int max) {

  uint8_t r1, r2;
  int i = max;

  gpio_prt(ezr_port); // set the i/o port

  do {
    if (!gpio_lvl(nirq_pin)) { // check NIRQ state
      r1 = ezr_get(SI4432_REG_STATUS_1);
      r2 = ezr_get(SI4432_REG_STATUS_2);
      if (((r1 & s1) == s1) &&
          ((r2 & s2) == s2))
        return i;
    }
    wait(1000);
  } while (--i > 0);

  return -1; // time out

}
