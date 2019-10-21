#include <avr/interrupt.h>

#include "core.h"

static volatile uint16_t timer_msb;

// Timer/Counter1 Overflow vector
ISR(TIMER1_OVF_vect) {

  timer_msb++;

}

void timer_init() {

  cli();

  timer_msb = 0;
  TCNT1 = 0; // Timer/Counter 1 value (16bit access)
  TCCR1A = 0; // Normal mode (COM1A1, COM1A0, COM1B1, COM1B0, WGM11, WGM10)
  TCCR1B = 1 << CS11; // enable with CS12,CS11,CS10 => 010 => clk / 8 for F_CPU = 8000000L
  TIMSK1 = 1 << TOIE1; // Overflow Interrupt Enable

  sei();

}

uint32_t timer_micros() {

  // TODO must be the half on F_CPU = 16000000L
  return (uint32_t)timer_msb << 16 | TCNT1;

}
