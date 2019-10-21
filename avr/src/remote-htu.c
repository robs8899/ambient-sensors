#include <stdint.h>
#include <string.h>
#include <avr/io.h>
#include <avr/pgmspace.h>

#include "core/core.h"
#include "util/util.h"
#include "dev/si4432.h"
#include "dev/si7021.h"

static const ezr_reg_val ezr_init_seq[] PROGMEM = {

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

    // {0x75, 0x53}, {0x76, 0x60}, {0x77, 0x40}, // 433.85 MHz -> channel 0
    {0x75, 0x53}, {0x76, 0x61}, {0x77, 0x80}, // 433.90 MHz -> channel 1
    // {0x75, 0x53}, {0x76, 0x62}, {0x77, 0xC0}, // 433.95 MHz -> channel 2
    // {0x75, 0x53}, {0x76, 0x64}, {0x77, 0x00}, // 434.00 MHz -> channel 3

    {0x05, 0x04},                              // Interrupt/Status 1: enable packet sent
    {0x06, 0x00},                              // Interrupt/Status 2: disable all
    {0x09, 0x55},                              // XTAL load capacitance: xtalshft[7], xlc[6:0]
    {0x0B, 0x12}, {0x0C, 0x15}, {0x0D, 0x00},  // XL4432-D01-SMT: GPIO0/GPIO1 is for the RF switch

    // {0x6D, 0x1C}, // TX Power: reserved[7:4]: x001, lna_sw[3]: 1, txpow[2:0]: 100 -> 11dBm
    {0x6D, 0x19}, // TX Power: reserved[7:4]: X001, lna_sw[3]: 1, txpow[2:0]: 001 -> 2dBm

    {0x7D, 0x07}, // TX FIFO Control - Almost Empty Threshold: txfaethr[5:0]: x+1 = 8 bytes
    {0x7E, 0x39}, // RX FIFO Control - Almost Full Threshold: rxafthr[5:0]: x+1 = 58 bytes

};

static const char *fstr = "NODE=$,VBATT=$,RELH=$,TEMP=$,FAILS=$";
static const char *node = "002";
static const char *fill = "*";

static struct sht_sample sample;
static char cb[SI4432_FIXP_SIZE+1];
static char t1[0x08], t2[0x08], t3[0x08], t4[0x08];
static int vbatt, fails;

int main() {

  int c, i;
  uint8_t r;
  ezr_reg_val rv;

  i2c_init(); // initialize I2C
  spi_init(); // initialize SPI
  uart_init(); // // initialize UART
  adc_mux(2); // ADC multiplexer select PC2

  uart_puts("Sensor Node V. 1.00\r\n");

  ezr_init(1, 1, 0); // NIRQ at PC1, SDN at PC0
  ezr_startup(); // switch SDN low, to enable the chip
  c = ezr_poll(SI4432_STATUS_1_IGNORE, SI4432_STATUS_2_ICHIPRDY, 100); // wait for CHIPRDY
  if (c < 0) uart_puts("CHIPRDY time out!\r\n");

  for (i = 0; i < (sizeof(ezr_init_seq) / sizeof(ezr_reg_val)); i++) {
    memcpy_P(&rv, ezr_init_seq + i, sizeof(ezr_reg_val));
    ezr_set(rv.reg, rv.val);  // send init sequence
  }

  uart_puts("Entering main loop...\r\n");

  do {

    if (sht_read(&sample) < 0) {
      uart_puts("sht_read: error\r\n");
      sleep(WDP_1000MS);
      fails++;
      continue;
    }

    if (sht_conv(&sample) < 0) {
      uart_puts("sht_conv: error\r\n");
      sleep(WDP_1000MS);
      fails++;
      continue;
    }

    adc_get(); // ADC enable/warm up after WDT-Sleep
    wait(1000); // wait for stable bandgap voltage reference

    // measure battery level in mV on 11:1 voltage divider with 1.1V VREF
    vbatt = (int32_t)adc_get() * 11000 * 11 / 10 / 1024;

    // format the results
    fmt(cb, fstr, node, stoa(t1, vbatt, 4, 3), stoa(t2, sample.rh, 4, 2),
        stoa(t3, sample.t, 4, 2), dtoa(t4, fails, 2));
    while (strlen(cb) < SI4432_FIXP_SIZE) strcat(cb, fill);

    // print to serial console
    uart_puts("TX: "); uart_puts(cb); uart_puts("\r\n");

    r = ezr_get(SI4432_REG_OPMODE_CTRL_2); // prepare the TX FIFO
    ezr_set(SI4432_REG_OPMODE_CTRL_2, r | 0x01); // TX FIFO reset/clear...
    ezr_set(SI4432_REG_OPMODE_CTRL_2, r & ~0x01); // must be set, then cleared
    ezr_tx(cb, SI4432_FIXP_SIZE); // fill up the FIFO
    ezr_set(SI4432_REG_OPMODE_CTRL_1, SI4432_OPMODE_CTRL_1_TXON); // enable TX

    c = ezr_poll(SI4432_STATUS_1_IPKSENT, SI4432_STATUS_2_IGNORE, 1000); // wait for packet sent
    if (c < 0) uart_puts("PKSENT time out!\r\n");

    for (i=0; i<7; i++) sleep(WDP_8000MS);

  } while (1);

  return 0;

}
