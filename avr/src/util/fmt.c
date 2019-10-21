#include <stdarg.h>
#include <stdint.h>
#include <string.h>
#include "util.h"

char *fmt(char *buf, const char *fmt, ...) {

  int i;
  va_list ap;

  i = 0;
  *buf = 0;
  va_start(ap, fmt);

  do {

    if (fmt[i] == '$') strcat(buf, va_arg(ap, char *));
    else strncat(buf, fmt + i, 1);

  } while (fmt[i++] != 0);

  va_end(ap);
  return buf;

}
