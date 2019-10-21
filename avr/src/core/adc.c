#include <stdint.h>
#include <avr/io.h>

#include "core.h"

// #define ADCREFS (1<<REFS0) // Voltage Reference is AVCC
#define ADCREFS (1<<REFS0) | (1<<REFS1) // Internal 1.1V Voltage Reference

void adc_mux(int pin) {

  switch (pin) {

    case 1: // ADC1
      ADMUX = ADCREFS | (1<<MUX0);
      PORTC &= ~(1<<PORTC1);
      DDRC &= ~(1<<DDC1);
      break;

    case 2: // ADC2
      ADMUX = ADCREFS | (1<<MUX1);
      PORTC &= ~(1<<PORTC2);
      DDRC &= ~(1<<DDC2);
      break;

    case 3: // ADC3
      ADMUX = ADCREFS | (1<<MUX1) | (1<<MUX0);
      PORTC &= ~(1<<PORTC3);
      DDRC &= ~(1<<DDC3);
      break;

    case 4: // ADC4
      ADMUX = ADCREFS | (1<<MUX2);
      PORTC &= ~(1<<PORTC4);
      DDRC &= ~(1<<DDC4);
      break;

    case 5:  // ADC5
      ADMUX = ADCREFS | (1<<MUX2) | (1<<MUX0);
      PORTC &= ~(1<<PORTC5);
      DDRC &= ~(1<<DDC5);
      break;

    case 6: // ADC6
      ADMUX = ADCREFS | (1<<MUX2) | (1<<MUX1);
      PORTC &= ~(1<<PORTC6);
      DDRC &= ~(1<<DDC6);
      break;

    case 7: // ADC7 - no digital pin!
      ADMUX = ADCREFS | (1<<MUX2) | (1<<MUX1) | (1<<MUX0);
      break;

    default: // ADC0
      ADMUX = ADCREFS;
      PORTC &= ~(1<<PORTC0);
      DDRC &= ~(1<<DDC0);

  }

}

uint16_t adc_get() {

  // enable, start, clear interrupt flag and set prescaler
  ADCSRA = (1<<ADEN) | (1<<ADSC) | (1<<ADIF) | (1<<ADPS2)| (1<<ADPS1);
  while (ADCSRA & (1<<ADSC)); // check if conversion is in progress
  return ADCW;

}
