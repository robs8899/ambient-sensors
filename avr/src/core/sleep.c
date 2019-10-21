#include <stdint.h>
#include <avr/interrupt.h>
#include <avr/sleep.h>
#include <avr/wdt.h>

#include "core.h"

ISR (WDT_vect) { // the watchdog interrupt
  wdt_disable(); // disable the watchdog
}

void sleep(uint16_t wdp) {

  ADCSRA = 0; // disable the ADC
  MCUSR = 0; // clear the MCU Status Register
  WDTCSR = (1<<WDCE) | (1<<WDE); // start timed sequence
  WDTCSR = (1<<WDIE) | wdp; // watchdog interrupt enable with prescaler

  set_sleep_mode(SLEEP_MODE_STANDBY); // set sleep mode
  sleep_bod_disable(); // disable BOD before going to sleep

  cli();
  wdt_reset();
  sei();

  sleep_enable();
  sleep_cpu();
  sleep_disable();

}
