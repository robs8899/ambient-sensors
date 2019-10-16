#include <errno.h>
#include <fcntl.h>
#include <poll.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "util.h"
#include "gpio.h"

#define CB_SIZE 40

static const char *sfsGpioExport    = "/sys/class/gpio/export";
static const char *sfsGpioUnExport  = "/sys/class/gpio/unexport";
static const char *sfsGpioDirection = "/sys/class/gpio/gpio%u/direction";
static const char *sfsGpioValue     = "/sys/class/gpio/gpio%u/value";
static const char *sfsGpioEdge      = "/sys/class/gpio/gpio%d/edge";
static const char *errOpening       = "error opening %s: %s\n";
static const char *errReading       = "error reading from %s: %s\n";
static const char *errWriting       = "error writing to %s: %s\n";
static const char *errPolling       = "error polling %s: %s\n";

int gpio_mode(int pin, int mode) {
	
	int err, fd, len;
	char cb[CB_SIZE];
	
	errno = 0;
	
	if ((fd = open(sfsGpioUnExport, O_WRONLY)) < 0) {
		fprintf(stderr, errOpening, sfsGpioUnExport, strerror(errno));
		return -1;
	}
	
	len = sprintf(cb, "%d", pin);
	err = write(fd, cb, len);
	close(fd);
		
	if (err < 0) {
		fprintf(stderr, errWriting, sfsGpioUnExport, strerror(errno));
		// return -1; // ignore this error
	}
	
	sleep_wait(100);
	errno = 0;

	if ((fd = open(sfsGpioExport, O_WRONLY)) < 0) {
		fprintf(stderr, errOpening, sfsGpioExport, strerror(errno));
		return -1;
	}
	
	len = sprintf(cb, "%d", pin);
	err = write(fd, cb, len);
	close(fd);
		
	if (err < 0) {
		fprintf(stderr, errWriting, sfsGpioExport, strerror(errno));
		return -1;
	}

	sleep_wait(100);
	len = sprintf(cb, sfsGpioDirection, pin);
	errno = 0;

	if ((fd = open(cb, O_WRONLY)) < 0) {
		fprintf(stderr, errOpening, cb, strerror(errno));
		return -1;
	}
	
	switch (mode) {
	  case GPIO_MODE_IN: err = write(fd, "in", 2); break;
	  case GPIO_MODE_OUT: err = write(fd, "out", 3); break;
	}

	close(fd);
		
	if (err < 0) {
		fprintf(stderr, errWriting, cb, strerror(errno));
		return -1;
	}

	return 0;
	
}

int gpio_edge(int pin, int edge) {
	
	int err, fd;
	char cb[CB_SIZE];

	sprintf(cb, sfsGpioEdge, pin);
	errno = 0;
	
	if ((fd = open(cb, O_WRONLY)) < 0) {
		fprintf(stderr, errOpening, cb, strerror(errno));
		return -1;
	}

	switch (edge) {
	  case GPIO_EDGE_NONE: err = write(fd, "none", 4); break;
	  case GPIO_EDGE_RISING: err = write(fd, "rising", 6); break;
	  case GPIO_EDGE_FALLING: err = write(fd, "falling", 7); break;
	  case GPIO_EDGE_BOTH: err = write(fd, "both", 4); break;
	  default: err = 0;
	}
	
	close(fd);
		
	if (err < 0) {
		fprintf(stderr, errWriting, cb, strerror(errno));
		return -1;
	}

	return 0;
	
}

int gpio_set(int pin, int level) {
	
	int err, fd;
	char cb[CB_SIZE];

	sprintf(cb, sfsGpioValue, pin);
	errno = 0;
	
	if ((fd = open(cb, O_WRONLY)) < 0) {
		fprintf(stderr, errOpening, cb, strerror(errno));
		return -1;
	}

	switch (level) {
	  case GPIO_LEVEL_LO: err = write(fd, "0", 1); break;
	  case GPIO_LEVEL_HI: err = write(fd, "1", 1); break;
	  default: err = 0;
	}
	
	close(fd);
		
	if (err < 0) {
		fprintf(stderr, errWriting, cb, strerror(errno));
		return -1;
	}

	return 0;
	
}

int gpio_get(int pin) {
	
	int err, fd;
	char cb[CB_SIZE], val[2] = "\0\0";

	sprintf(cb, sfsGpioValue, pin);
	errno = 0;

	if ((fd = open(cb, O_RDONLY)) < 0) {
		fprintf(stderr, errOpening, cb, strerror(errno));
		return -1;
	}

	err = read(fd, val, 1);
	close(fd);

	if (err < 0) {
		fprintf(stderr, errReading, cb, strerror(errno));
		return -1;
	}

	return atoi(val);

}

int gpio_poll(int pin, int timeout) {
	
	int err, fd;
	char cb[CB_SIZE];
	
	struct pollfd pd[1];
	
	sprintf(cb, sfsGpioValue, pin);
	errno = 0;

	if ((fd = open(cb, O_RDONLY)) < 0) {
		fprintf(stderr, errOpening, cb, strerror(errno));
		return -1;
	}
	
	lseek(fd, 0, SEEK_SET);
	err = read(fd, cb, CB_SIZE); // clear pending interrupts
	
	memset((void*)pd, 0, sizeof(pd));
	pd[0].fd = fd;
	pd[0].events = POLLPRI;

	err = poll(pd, 1, timeout); // poll with specified timeout
	
	if (err < 0) {
		fprintf(stderr, errPolling, cb, strerror(errno));
		close(fd);
		return -1;
	}
	
	if (pd[0].revents & POLLPRI) {
		lseek(fd, 0, SEEK_SET);
		err = read(fd, cb, CB_SIZE); // clear interrupts
		close(fd);
		return 0;
	}
	
	close(fd);
	return -1;
	
}
