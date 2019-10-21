#include <stdint.h>
#include <string.h>
#include "util.h"

char *stoa(char *buf, int32_t val, uint8_t prc, uint8_t scl) {

  int m, i, p;
  char t[0x10];

  *buf = 0;

  if (val < 0) {

    strcat(buf, "-");
    m = val >> 0x1F;
    val ^= m;
    val -= m;

  } else strcat(buf, "+");

  p = 1;
  for (i=0; i < scl; i++) p *= 10;
  strcat(buf, dtoa(t, val/p, prc-scl));
  strcat(buf, ".");
  strcat(buf, dtoa(t, val%p, scl));

  return buf;

}
