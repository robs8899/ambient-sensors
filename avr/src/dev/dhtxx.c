#include <stdint.h>
#include "../core/core.h"
#include "dhtxx.h"

static int dht_port;
static int dht_pin;

void dht_init(int port, int pin) {

  dht_port = port;
  dht_pin = pin;

  gpio_prt(dht_port); // set up the i/o port
  gpio_pue(dht_pin); // enable pull up
  gpio_inp(dht_pin); // release the bus

}

int dht_read_raw(int type, uint8_t *buffer) {

  int state, last_state, i, k;
  uint32_t ct, dt, lt, st;

  gpio_prt(dht_port); // set the i/o port
  gpio_out(dht_pin); // acquire the bus
  gpio_clr(dht_pin); // send start signal

  // DHT11: 18ms min., DHT22: 1ms typ., 20ms max.
  if (type == DHT11_TYPE) wait(20000);
  else wait(1000);

  gpio_pue(dht_pin); // enable pull up
  gpio_inp(dht_pin); // release the bus
  last_state = state = 1;

  i = k = 0; // initialize bit counter and low state counter
  st = lt = timer_micros(); // set start time and low state time

  do {

    ct = timer_micros(); // read current time

    if ((state = gpio_lvl(dht_pin)) != last_state) {

      last_state = state;

      if (!state) { // detect falling edge

        if((dt = ct - lt) < 0) dt += 1000000; // timer overflow

        if (i > 1) { // ignore first two transitions

          buffer[k/8] <<= 1;
          if (dt > 100+5) buffer[k/8] |= 1;
          k++;

        }

        lt = ct;
        i++; // next bit

      }

    }

    if((dt = ct - st) < 0) dt += 1000000;

  } while (dt < 10000 && i < 42); // sampling time is typ. 4000 us.

  if (i < 42) return -1; // incomplete sequence
  return 0; // success!

}

void dht_read(int type, int *rh, int *temp, int *fails) {

  int i, k;
  uint8_t buffer[5], chk;

  *rh = 0;
  *temp = 0;
  *fails = 0;

  for (i = 0; i < DHT_MAX_ATTEMPTS; i++) {

    if (dht_read_raw(type, buffer) == 0) {

      chk = 0;
      for (k = 0; k < 4; k++) chk += buffer[k];

      if (chk == buffer[4]) {

        if (type == DHT11_TYPE) { // DHT11

          *rh   = buffer[0] * 10;
          *temp = buffer[2] * 10;

        } else { // DHT22

          *rh   = (buffer[0] << 8 | buffer[1]);
          *temp = (buffer[2] << 8 | buffer[3]);

        }

        return; // success!

      }

    }

    (*fails) ++;
    // minimum sampling interval must be greater than 2s.
    for (k=0; k<1000; k++) wait(2000);

  }

}

