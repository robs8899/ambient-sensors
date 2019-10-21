#include <stdint.h>
#include "util.h"

char *patt(char *buf, uint8_t byte) {

  int i = 8, k = 0;

  buf[i--] = 0;
  do { buf[i--] = byte & (1 << k++) ? 'X' : '-';
  } while (k < 8);
  return buf;

}
