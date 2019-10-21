#include <stdint.h>
#include "util.h"

char *hex_str(char *buf, uint8_t *bytes, int size) {

  int i, k;
  uint8_t n;

  i = k = 0;

  do {

    n = (bytes[k] & 0xF0) >> 4;
    buf[i++] = n < 0x0A ? 0x30 + n : 0x37 + n;

    n = bytes[k] & 0x0F;
    buf[i++] = n < 0x0A ? 0x30 + n : 0x37 + n;

  } while (++k < size);

  buf[i] = 0;
  return buf;

}
