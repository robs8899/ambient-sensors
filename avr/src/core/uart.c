#include <stdint.h>
#include <avr/io.h>

#include "core.h"

void uart_init() {

  uint16_t ubrr_val = 8; // 115200 fixed baud rate on 8MHz with U2Xn enabled
  UBRR0H = ubrr_val >> 8; // UBRR0[11:8] - 4bit MSB part
  UBRR0L = ubrr_val & 0xFF; // UBRR0[7:0] - 8bit LSB part

  UCSR0A  = (1<< U2X0); // enable asynchronous double speed
  UCSR0B  = (1<<RXEN0) | (1<<TXEN0); // enable rx/tx
  UCSR0C  = (1<<UCSZ01) | (1<<UCSZ00); // asynchronous mode, 8N1

}

void uart_done() {

  UCSR0B = 0; // disable USART

}

char uart_getc() {

  while (!(UCSR0A & (1<<RXC0))); // wait for receive complete
  return (char)UDR0;

}

void uart_putc(char c) {

  while (!(UCSR0A & (1<<UDRE0))); // wait for UDRn ready to be written
  UDR0 = (uint8_t)c;

  while (!(UCSR0A & (1<<TXC0))); // wait for transmit complete
  UCSR0A |= (1<<TXC0); // cleared by writing a one

}

void uart_gets(char *buf) {

  int i = 0xFF; // max
  *buf = uart_getc();
  while (*buf != 0x0D && *buf != 0x0A && i--)
    *++buf = uart_getc();
  *buf = 0;

}

void uart_puts(char *buf) {

  int i = 0xFF; // max
  while (*buf !=0 && i--)
    uart_putc(*buf++);

}
