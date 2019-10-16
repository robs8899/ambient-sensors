/*
 * Copyright (C) 2017 Robert Schoch
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL AUTHOR OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 */

#include <errno.h>
#include <fcntl.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
#include <sys/stat.h>

#include "util.h"

int next_timepoint(int interval) {
	
	int c;
	time_t raw_time;
	struct tm *tm_ptr;
	
	raw_time = time(NULL);
	tm_ptr = localtime(&raw_time);

	c = tm_ptr->tm_sec + tm_ptr->tm_min*60 + tm_ptr->tm_hour*3600; // number of seconds of current day
	return interval - (c % interval); // calculate remaining seconds to next round time
	
}

void interval_wait(int interval) {
	
	sleep(next_timepoint(interval));
	return;

}

void sleep_wait(long millis) {

	struct timespec ts;

	ts.tv_sec  = (time_t)(millis / 1000L);
	ts.tv_nsec = (millis % 1000L) * 1000000L;
	nanosleep(&ts, NULL);

}

int get_opts(int argc, char** argv, struct opt *opts) {
	
  int opt, val, err;

  errno = 0;
  err = 0;
  
  while ((opt = getopt(argc, argv, "d:f:i:")) != -1 && err == 0) {

    switch(opt) {
      case 'f':
        opts->dev_name = optarg;
        break;
	case 'd':
        opts->log_dst = optarg;
        break;
      case 'i':
        val = strtoul(optarg, 0, 10);
        if (val != 0) {
			opts->log_int = val;
		} else {
          fprintf(stderr, "invalid argument for option -- %c\n", opt);
          err = 1;
        }
        break;
      default:
        err = 1;
    }

  }

  if (err) {
    fprintf(stderr, "usage: [-f <device>][-d <path>][-i <interval>]\n");
    return -1;
  }

  return 0;
	
}

int write_log(char *path, char *entry) {
	
  int fd, result;
	
	fd = open(path, O_APPEND | O_CREAT | O_WRONLY,
		S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH);
		
	if (fd == -1) {
		fprintf(stderr, "error opening file %s: %s\n",
			path, strerror(errno));
		return -1;
	}

	result = write(fd, entry, strlen(entry));
	
	if (result == -1) {
		fprintf(stderr, "error writing file %s: %s\n",
		path, strerror(errno));
		return -1;
	}

	result = close(fd);
	
	if (result == -1) {
		fprintf(stderr, "error closing file %s: %s\n",
		path, strerror(errno));
		return -1;
	}
	
	return 0;

}
