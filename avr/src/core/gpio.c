#include <stdint.h>
#include <avr/io.h>
#include <avr/pgmspace.h>

#include "core.h"

const uint16_t PROGMEM gpio_pins[] = {(uint16_t) &PINB,  (uint16_t) &PINC,  (uint16_t) &PIND};
const uint16_t PROGMEM gpio_ddrs[] = {(uint16_t) &DDRB,  (uint16_t) &DDRC,  (uint16_t) &DDRD};
const uint16_t PROGMEM gpio_prts[] = {(uint16_t) &PORTB, (uint16_t) &PORTC, (uint16_t) &PORTD};

volatile uint8_t *gpio_pin_reg;
volatile uint8_t *gpio_ddr_reg;
volatile uint8_t *gpio_prt_reg;

void gpio_port(int num) {
  gpio_pin_reg = (volatile uint8_t *)pgm_read_word(gpio_pins + num);
  gpio_ddr_reg = (volatile uint8_t *)pgm_read_word(gpio_ddrs + num);
  gpio_prt_reg = (volatile uint8_t *)pgm_read_word(gpio_prts + num);
}
