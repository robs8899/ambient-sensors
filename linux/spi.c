#include <errno.h>
#include <fcntl.h>
#include <poll.h>
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <sys/ioctl.h>
#include <linux/spi/spidev.h>

#include "util.h"
#include "spi.h"

#define SPI_BUFFER_SIZE       0x40        // SPI transfer buffer size
#define SPI_DEV_MODE          SPI_MODE_0  // SPI mode for SPI_IOC_WR_MODE
#define SPI_DEV_CLOCK_SPEED   1000000     // 1MHz SPI clock speed
#define SPI_DEV_BITS_PER_WORD 8           // SPI transfers with 8 bits per word
#define SPI_DEV_TRANS_DELAY   0           // wait before deselecting before the next transfer
#define SPI_DEV_CS_CHANGE     0           // deselect before starting the next transfer

static int fd;
static uint8_t trx[SPI_BUFFER_SIZE];
static struct spi_ioc_transfer ioc;

int spi_init(char *dev) {
	
	errno = 0;

	if ((fd = open(dev, O_RDWR)) < 0) {
		fprintf(stderr, "error opening device %s: %s\n",
			dev, strerror(errno));
		return -1;
	}
	
	memset(&ioc, 0, sizeof(ioc));
	ioc.tx_buf        = (intptr_t)trx;
	ioc.rx_buf        = (intptr_t)trx;
	ioc.speed_hz      = SPI_DEV_CLOCK_SPEED;
	ioc.delay_usecs   = SPI_DEV_TRANS_DELAY;
	ioc.bits_per_word = SPI_DEV_BITS_PER_WORD;
	ioc.cs_change     = SPI_DEV_CS_CHANGE;
	
	return 0;

}

int spi_done() {

	errno = 0;

	if (close(fd) < 0) {
		fprintf(stderr, "error closing fd %d: %s\n",
			fd, strerror(errno));
		return -1;
	}
	
	return 0;

}

int spi_transfer(int len) {

	ioc.len = len;
	errno = 0;

	if (ioctl(fd, SPI_IOC_MESSAGE(1), &ioc) < 0) {
		fprintf(stderr, "spi_transfer error: %s\n", strerror(errno));
		return -1;
	}

	return 0;

}

int spi_reg_read(uint8_t reg, uint8_t *data) {

	trx[0] = reg;
	trx[1] = 0xFF;
	if (spi_transfer(2) < 0) return -1;
	*data = trx[1];
	return 0;
	
}

int spi_reg_write(uint8_t reg, uint8_t val) {
	
	trx[0] = reg;
	trx[1] = val;
	if (spi_transfer(2) < 0) return -1;
	return 0;
	
}

int spi_burst_read(uint8_t reg, uint8_t *data, int len) {
	
	trx[0] = reg;
	memset(trx+1, 0xFF, len);
	if (spi_transfer(len+1) < 0) return -1;
	memcpy(data, trx+1, len);
	return 0;
	
}

int spi_burst_write(uint8_t reg, uint8_t *data, int len) {

	trx[0] = reg;
	memcpy(data, trx+1, len);
	if (spi_transfer(len+1) < 0) return -1;
	return 0;
	
}
