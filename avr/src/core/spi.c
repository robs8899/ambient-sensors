#include <stdint.h>
#include <avr/io.h>

#include "core.h"

#define SPI_MASTER_MODE0 (1<<MSTR)                         // CPOL=0, CPHA=0 - mode 0
#define SPI_MASTER_MODE1 (1<<MSTR) | (1<<CPHA)             // CPOL=0, CPHA=1 - mode 1
#define SPI_MASTER_MODE2 (1<<MSTR) | (1<<CPOL)             // CPOL=1, CPHA=0 - mode 2
#define SPI_MASTER_MODE3 (1<<MSTR) | (1<<CPOL) | (1<<CPHA) // CPOL=1, CPHA=1 - mode 3

#define SPI_CLKD128      (1<<SPR0)| (1<<SPR1)
#define SPI_CLKD64       (1<<SPR1)
#define SPI_CLKD16       (1<<SPR0)
#define SPI_CLKD4        0

#define SPI_MODE         SPI_MASTER_MODE2 // mode 0 for Si4432, mode 2 for AD9833
#define SPI_CLKD         SPI_CLKD4 // SPI clock at fo/4 (x2 if double speed)

void spi_init() {

  DDRB  &= ~(1<<PB4); // MISO as input
  PORTB |= (1 << PB4); // enable pull up

  DDRB  |=  (1<<PB1) | (1<<PB2) | (1<<PB3) | (1<<PB5); // PB1 as CS1, PB2 as CS0, MOSI, SCK as output
  PORTB |=  (1<<PB1) | (1<<PB2); // deselect PB1/CS1 and PB2/CS0, must be select/deselect with spi_cs()/spi_ds()

  SPCR = (1<<SPE) | SPI_MODE | SPI_CLKD; // Enable with MSB first, defined mode and clock div
  SPSR = (1<<SPI2X); // enable double SPI speed

}

void spi_done() {

  SPCR = 0; // disable SPI

}

void spi_cs(int cs) {

  switch (cs) { // 0 is PB2, 1 is PB1
    case 0: PORTB |=  (1<<PB1); PORTB &= ~(1<<PB2); break;
    case 1: PORTB &= ~(1<<PB1); PORTB |=  (1<<PB2); break;
  }

}

void spi_ds(int ds) {

  switch (ds) {
    case 0: PORTB |= (1<<PB2); break;
    case 1: PORTB |= (1<<PB1); break;
  }

}

uint8_t spi_trx(uint8_t data) {

  SPDR = data;
  while (!(SPSR & (1<<SPIF)));
  return SPDR;

}

void spi_rxn(uint8_t *buf, uint16_t cnt) {

  while (cnt--) {
    SPDR = 0xFF;
    while (!(SPSR & (1<<SPIF)));
    *buf++ = SPDR;
  };

}

void spi_txn(const uint8_t *buf, uint16_t cnt) {

  while (cnt--) {
    SPDR = *buf++;
    while (!(SPSR & (1<<SPIF)));
  }

}

int spi_rdy() {

  int t = 1000;

  do { // Wait for ready with timeout of 100ms
    if (spi_trx(0xFF) == 0xFF) break;
    wait(100);
  } while (t--);

  return t ? 1 : 0;

}

