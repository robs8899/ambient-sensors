#include <stdint.h>
#include "util.h"

char *hex_dmp(char *buf, uint8_t *bytes, int size) {

  int i, k;
  uint8_t n;

  i = k = 0;

  while (k < size) {

    if (k > 0) buf[i++] = 0x20;

    n = (bytes[k] & 0xF0) >> 4;
    buf[i++] = n < 0x0A ? 0x30 + n : 0x37 + n;

    n = bytes[k] & 0x0F;
    buf[i++] = n < 0x0A ? 0x30 + n : 0x37 + n;

    k++;

  }

  buf[i++] = 0x20;
  k = 0;

  while (k < size) {
    buf[i++] = bytes[k] > 0x20 &&
        bytes[k] < 0x7F ? bytes[k] : 0x2E;
    k++;
  }

  buf[i] = 0;
  return buf;

}
