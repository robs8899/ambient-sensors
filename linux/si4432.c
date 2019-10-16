#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <time.h>

#include <sys/ioctl.h>
#include <linux/spi/spidev.h>

#include "util.h"
#include "gpio.h"
#include "spi.h"

#define SI4432_IRQ_PIN          25   // BCM2835 GPIO pin connected to the IRQ pin
#define SI4432_SDN_PIN          24   // BCM2835 GPIO pin connected to the SDN pin

#define SI4432_CHANNEL_MAX      2    // Upper channel number
#define SI4432_CHWAIT_TIME      60000 // Max. millis for listening on a channel
#define SI4432_PACKET_SIZE      58   // Fixed packet size

#define SI4432_STAT1            0x03 // Interrupt/Status 1

#define SI4432_STAT1_IFFERR     0x80 // FIFO Underflow/Overflow Error
#define SI4432_STAT1_ITXFFAFULL 0x40 // TX FIFO Almost Full
#define SI4432_STAT1_ITXFFAEM   0x20 // TX FIFO Almost Empty
#define SI4432_STAT1_IRXFFAFULL 0x10 // RX FIFO Almost Full
#define SI4432_STAT1_IEXT       0x08 // External Interrupt
#define SI4432_STAT1_IPKSENT    0x04 // Packet Sent Interrupt
#define SI4432_STAT1_IPKVALID   0x02 // Valid Packet Received
#define SI4432_STAT1_ICRCERROR  0x01 // CRC Error

#define SI4432_STAT2            0x04 // Interrupt/Status 2

#define SI4432_STAT2_ISWDET     0x80 // Sync Word Detected
#define SI4432_STAT2_IPREAVAL   0x40 // Valid Preamble Detected
#define SI4432_STAT2_IPREAINVAL 0x20 // Invalid Preamble Detected
#define SI4432_STAT2_IRSSI      0x10 // RSSI threshold exceeded
#define SI4432_STAT2_IWUT       0x08 // Wake-Up-Timer
#define SI4432_STAT2_ILBD       0x04 // Low Battery Detect
#define SI4432_STAT2_ICHIPRDY   0x02 // Chip Ready (XTAL)
#define SI4432_STAT2_IPOR       0x01 // Power-on-Reset (POR)

#define SI4432_IEN1             0x05 // Interrupt Enable 1

#define SI4432_IEN1_ENFFERR     0x80 // Enable FIFO Underflow/Overflow
#define SI4432_IEN1_ENTXFFAFULL 0x40 // Enable TX FIFO Almost Full
#define SI4432_IEN1_ENTXFFAEM   0x20 // Enable TX FIFO Almost Empty
#define SI4432_IEN1_ENRXFFAFULL 0x10 // Enable RX FIFO Almost Full
#define SI4432_IEN1_ENEXT       0x08 // Enable External Interrupt
#define SI4432_IEN1_ENPKSENT    0x04 // Enable Packet Sent
#define SI4432_IEN1_ENPKVALID   0x02 // Enable Valid Packet Received
#define SI4432_IEN1_ENCRCERROR  0x01 // Enable CRC Error

#define SI4432_IEN2             0x06 // Interrupt Enable 2

#define SI4432_IEN2_ENSWDET     0x80 // Enable Sync Word Detected
#define SI4432_IEN2_ENPREAVAL   0x40 // Enable Valid Preamble Detected
#define SI4432_IEN2_ENPREAINVAL 0x20 // Enable Invalid Preamble Detected
#define SI4432_IEN2_ENRSSI      0x10 // Enable RSSI threshold
#define SI4432_IEN2_ENWUT       0x08 // Enable Wake-Up Timer
#define SI4432_IEN2_ENLBD       0x04 // Enable Low Battery Detect
#define SI4432_IEN2_ENCHIPRDY   0x02 // Enable Chip Ready (XTAL)
#define SI4432_IEN2_ENPOR       0x01 // Enable POR

#define SI4432_CTRL1            0x07 // Operating Mode and Function Control 1

#define SI4432_CTRL1_SWRES      0x80 // Software Register Reset Bit.
#define SI4432_CTRL1_ENLBD      0x40 // Enable Low Battery Detect.
#define SI4432_CTRL1_ENWT       0x20 // Enable Wake-Up-Timer.
#define SI4432_CTRL1_X32KSEL    0x10 // 32,768 kHz Crystal Oscillator Select.
#define SI4432_CTRL1_TXON       0x08 // TX on in Manual Transmit Mode.
#define SI4432_CTRL1_RXON       0x04 // RX on in Manual Receiver Mode.
#define SI4432_CTRL1_PLLON      0x02 // TUNE Mode (PLL is ON).
#define SI4432_CTRL1_XTON       0x01 // READY Mode (Xtal is ON).

#define SI4432_CTRL2            0x08 // Operating Mode and Function Control 2

#define SI4432_CTRL2_RXMPK      0x10 // RX Multi Packet.
#define SI4432_CTRL2_AUTOTX     0x08 // Automatic Transmission.
#define SI4432_CTRL2_ENLDM      0x04 // Enable Low Duty Cycle Mode.
#define SI4432_CTRL2_FFCLRRX    0x02 // RX FIFO Reset/Clear.
#define SI4432_CTRL2_FFCLRTX    0x01 // TX FIFO Reset/Clear.

#define SI4432_FIFO             0x7F // Operating Mode and Function Control 2

typedef struct {
	uint8_t reg;
	uint8_t val;
} reg_val_t;

typedef struct {
	int32_t rssi;
	int32_t afc;
	uint8_t pga;
	uint8_t lna;
} rf_stat_t;

static const reg_val_t si4432_init_seq[] = {

    // RB: 2.4 kbps, FD: 2.4 kHz, CF: 9.5 kHz / SF: 62.4 kHz,
    // AFC enabled, AFC Limiter: ± 8 * 625Hz = 5kHz
    // PH: enabled, preabmle: 64 nibbles, MSB first, CRC:CCIT,
    // sync word: 3&2 (0xB388), no header, data: 58 bytes

    {0x1C, 0x31}, {0x1D, 0x44}, {0x1E, 0x0A}, {0x1F, 0x03},
    {0x20, 0xD0}, {0x21, 0x00}, {0x22, 0x9D}, {0x23, 0x49},
    {0x24, 0x01}, {0x25, 0x3D}, {0x2A, 0x08}, {0x30, 0x8C},
    {0x32, 0x00}, {0x33, 0x0A}, {0x34, 0x40}, {0x35, 0x42},
    {0x36, 0xB3}, {0x37, 0x88}, {0x3E, 0x3A}, {0x58, 0x80},
    {0x69, 0x60}, {0x6E, 0x13}, {0x6F, 0xA9}, {0x70, 0x20},

    {0x71, 0x23}, {0x72, 0x04}, // GFSK/FIFO-Mode, FD=2500 (625*4)

    {0x75, 0x53}, {0x76, 0x60}, {0x77, 0x40}, // 433.85 MHz -> channel 0
    // {0x75, 0x53}, {0x76, 0x61}, {0x77, 0x80}, // 433.90 MHz -> channel 1
    // {0x75, 0x53}, {0x76, 0x62}, {0x77, 0xC0}, // 433.95 MHz -> channel 2
    // {0x75, 0x53}, {0x76, 0x64}, {0x77, 0x00}, // 434.00 MHz -> channel 3

	{0x79, 0x00}, // Frequency Hopping Channel Select (initial value)
	{0x7A, 0x05}, // Frequency Hopping Step Size (10kHz steps)
	
    {0x05, 0x00},                              // Interrupt/Status 1: disable all
    {0x06, 0x00},                              // Interrupt/Status 2: disable all
    {0x09, 0x55},                              // XTAL load capacitance: xtalshft[7], xlc[6:0]
    {0x0B, 0x12}, {0x0C, 0x15}, {0x0D, 0x00},  // XL4432-D01-SMT: GPIO0/GPIO1 is for the RF switch

    // {0x6D, 0x1C}, // TX Power: reserved[7:4]: x001, lna_sw[3]: 1, txpow[2:0]: 100 -> 11dBm
    {0x6D, 0x19}, // TX Power: reserved[7:4]: X001, lna_sw[3]: 1, txpow[2:0]: 001 -> 2dBm

    {0x7D, 0x07}, // TX FIFO Control - Almost Empty Threshold: txfaethr[5:0]: x+1 = 8 bytes
    {0x7E, 0x39}, // RX FIFO Control - Almost Full Threshold: rxafthr[5:0]: x+1 = 58 bytes

};

static int si4432_fifo_read(char *rx) {

	if (spi_burst_read(SI4432_FIFO, (uint8_t *)rx,
		SI4432_PACKET_SIZE) < 0) return -1;

    // pad trimming and termination
	int i = SI4432_PACKET_SIZE;
	do { rx[i] = 0;
	} while (rx[--i] == '*' && i > 0);
	return 0;
	
}

static int si4432_rf_stat(rf_stat_t *st) {
	
	uint8_t reg1, reg2;
	
	if (spi_reg_read(0x26, &reg1) <0) return -1;
	st->rssi = 500 * ((int)reg1 - 244);
	
	if (spi_reg_read(0x2B, &reg1) < 0) return -1;
	if (spi_reg_read(0x2C, &reg2) < 0) return -1;
	st->afc = (int32_t)156250 * ((((int16_t)reg1 << 2) | ((int16_t)reg2 & 0xC0)) >> 6);
	
	if (spi_reg_read(0x69, &reg1) < 0) return -1;
	st->pga = 3 * (reg1 & 0x0F);
	st->lna = (reg1 & 0x10) ? 25 : 5;
	
	return 0;

}

static char* build_ts(char *buf, int max, char *fmt) {

	time_t raw_time;
	struct tm *tm_ptr;

	raw_time = time(NULL);
	tm_ptr = localtime(&raw_time);
	strftime(buf, max, fmt, tm_ptr);
	return buf;

}

int main(int argc, char** argv) {

	int i, err, pass, ch;
    char fn[0x10], ts[0x20], fp[0x100], lb[0x100];
	char packet[SI4432_PACKET_SIZE+1];
	uint8_t flags[2];
	rf_stat_t stat1, stat2;
	struct opt opts;
	
	printf("Si4432 Sensor Logger Ver. 0.1\n");
	
	opts.dev_name = DEFAULT_SPI_DEV;
	opts.log_dst = DEFAULT_LOG_DST;
	opts.log_int = DEFAULT_LOG_INT;

	err = get_opts(argc, argv, &opts);
	if (err < 0) return -1;

	printf("running on device %s, logging to '%s', interval: %u\n",
		opts.dev_name, opts.log_dst, opts.log_int);

	// ------------- GPIO STUFF ---------------
	
	gpio_mode(SI4432_SDN_PIN, GPIO_MODE_OUT);
	gpio_mode(SI4432_IRQ_PIN, GPIO_MODE_IN);
	gpio_edge(SI4432_IRQ_PIN, GPIO_EDGE_FALLING);
	
	gpio_set(SI4432_SDN_PIN, GPIO_LEVEL_HI); // shutdown
	sleep_wait(100); // ensure minimum down time
	
	gpio_set(SI4432_SDN_PIN, GPIO_LEVEL_LO); // startup
	sleep_wait(25); // awaiting POR reset time (typ: 16ms)
	
	// -------------- SPI STUFF ---------------

	err = spi_init(opts.dev_name);
	if (err < 0) return -1;
	
	// -------- Si4432 initialization ---------
	for (i = 0; i < sizeof(si4432_init_seq) / sizeof(reg_val_t); i++) {
		if (spi_reg_write(si4432_init_seq[i].reg | 0x80, si4432_init_seq[i].val) < 0)
			return -1;
	}
	
	 // enable valid packet interrupt, enable CRC error interrupt, enable syncword interrupt
	err = spi_reg_write(SI4432_IEN1  | 0x80, SI4432_IEN1_ENPKVALID | SI4432_IEN1_ENCRCERROR);
	err = spi_reg_write(SI4432_IEN2  | 0x80, SI4432_IEN2_ENSWDET);

	pass = 0; // initial internal state
	ch = 99; // any high value to start at 0

	while (1) {

		if (pass == 0) {
			
			if (++ch > SI4432_CHANNEL_MAX) ch = 0; // next channel
			
			if (ch == 0) {
				
				build_ts(ts, sizeof(ts), "%F %T %Z");
				printf("%s - entering sleep mode...\n", ts);
				
				err = spi_reg_write(SI4432_CTRL1 | 0x80, SI4432_CTRL1_XTON); // ready mode
				interval_wait(opts.log_int); // awaiting interval
				
			}

			err = spi_reg_write(0x79 | 0x80, ch); // switch the channel
			err = spi_reg_write(SI4432_CTRL1 | 0x80, SI4432_CTRL1_RXON); // enable manual RX
	        sleep_wait(25); // awaiting response time (max. 16.8ms)
			
			build_ts(ts, sizeof(ts), "%F %T %Z");
			printf("%s - rx enabled on channel %d\n", ts, ch);
			
			// get initial RF status
			memset(&stat1, 0, sizeof(stat1));
			err = si4432_rf_stat(&stat1);
			
		}
		
		// any ISM node must respond within CHWAIT_TIME!
		err = gpio_poll(SI4432_IRQ_PIN, SI4432_CHWAIT_TIME); // waiting for interrupt
		err = spi_burst_read(SI4432_STAT1, flags, 2); // read and clear status

		if (flags[1] & SI4432_STAT2_ISWDET) {
			
			// updated AFC is available at this time
			memset(&stat2, 0, sizeof(stat2));
			err = si4432_rf_stat(&stat2);
			pass = 1;
			
		}

		if (flags[0] & SI4432_STAT1_IPKVALID) {
			memset(packet, 0, sizeof(packet));
			err = si4432_fifo_read(packet);
			pass = 2;
		}

		if (flags[0] & SI4432_STAT1_ICRCERROR) {
			printf("crc error!\n");
			pass = 0;
		}

		if (pass < 2) continue;
		pass = 0;

		build_ts(ts, sizeof(ts), "%F %T %Z");
		build_ts(fn, sizeof(fn), "%Y%m%d");

		sprintf(lb, "%s,%s\n", ts, packet); // prepend timestamp, append new line
		sprintf(fp, "%s/%03u/%s.log", opts.log_dst, ch, fn); // build full log file path

		printf("rx: %s", lb); // write log buffer to console
		printf("wr: %s\n", fp); // write log path to console
		
		err = write_log(fp, lb); // write to log file
		if (err < 0) return -1;
		
		printf("rf status 1: rssi=%.1fdBm, afc=%.2fHz, pga=%ddB, lna=%ddB\n", 
			0.001 * stat1.rssi, 0.001 * stat1.afc, stat1.pga, stat1.lna);

		printf("rf status 2: rssi=%.1fdBm, afc=%.2fHz, pga=%ddB, lna=%ddB\n", 
			0.001 * stat2.rssi, 0.001 * stat2.afc, stat2.pga, stat2.lna);
			
	}

}