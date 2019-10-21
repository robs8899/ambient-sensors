#include <stdint.h>
#include "util.h"

char *dtoa(char *buf, uint16_t val, int dig) {

  buf[dig--] = 0;
  if(dig < 0) return buf;

  do {
    buf[dig] = (val % 10) + 0x30;
    val /= 10;
  } while (dig--);

  return buf;

}
