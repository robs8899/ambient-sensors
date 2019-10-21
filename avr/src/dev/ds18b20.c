#include <stdint.h>

#include "../core/core.h"
#include "ds18b20.h"

static int ow_port;
static int ow_pin;

static uint8_t crc8_lookup[] = {

    0, 94,188,226, 97, 63,221,131,194,156,126, 32,163,253, 31, 65,
    157,195, 33,127,252,162, 64, 30, 95,  1,227,189, 62, 96,130,220,
    35,125,159,193, 66, 28,254,160,225,191, 93,  3,128,222, 60, 98,
    190,224,  2, 92,223,129, 99, 61,124, 34,192,158, 29, 67,161,255,
    70, 24,250,164, 39,121,155,197,132,218, 56,102,229,187, 89,  7,
    219,133,103, 57,186,228,  6, 88, 25, 71,165,251,120, 38,196,154,
    101, 59,217,135,  4, 90,184,230,167,249, 27, 69,198,152,122, 36,
    248,166, 68, 26,153,199, 37,123, 58,100,134,216, 91,  5,231,185,
    140,210, 48,110,237,179, 81, 15, 78, 16,242,172, 47,113,147,205,
    17, 79,173,243,112, 46,204,146,211,141,111, 49,178,236, 14, 80,
    175,241, 19, 77,206,144,114, 44,109, 51,209,143, 12, 82,176,238,
    50,108,142,208, 83, 13,239,177,240,174, 76, 18,145,207, 45,115,
    202,148,118, 40,171,245, 23, 73,  8, 86,180,234,105, 55,213,139,
    87,  9,235,181, 54,104,138,212,149,203, 41,119,244,170, 72, 22,
    233,183, 85, 11,136,214, 52,106, 43,117,151,201, 74, 20,246,168,
    116, 42,200,150, 21, 75,169,247,182,232, 10, 84,215,137,107, 53

};

// ------------- private API -------------

static int ow_reset() {

  // master reset pulse for 480 us min.
  // presence pulse after 15-60 us, line pulled down for 60-240 us
  // tests showed: pulse detected for waits between 35 and 150 us

  gpio_prt(ow_port);
  gpio_out(ow_pin); // pin as output
  gpio_clr(ow_pin); // master reset pulse
  wait(500); // master tx: 480μs minimum

  gpio_pue(ow_pin); // enable pull up
  gpio_inp(ow_pin); // release the bus
  wait(75); // DS18B20 waits 15-60μs

  if(!gpio_lvl(ow_pin)) {
    wait(500); // master rx: 480μs minimum
    return 0;
  }

  // no presence pulse
  return -1;

}

static int ow_read_bit() {

  uint8_t value;

  gpio_prt(ow_port);
  gpio_out(ow_pin);
  gpio_clr(ow_pin);
  wait(2); // should be >= 1 us

  gpio_pue(ow_pin); // enable pull up
  gpio_inp(ow_pin); // release the bus
  wait(5); // sampling time is within 15 us after master pull down

  value = gpio_lvl(ow_pin);
  wait(80); // rest of time slot + recovery between slots

  return value ? 1 : 0;

}

static void ow_write_bit(int value) {

  gpio_prt(ow_port);
  gpio_out(ow_pin);
  gpio_clr(ow_pin);
  wait(2); // should be >=1 us

  if (value) {

    gpio_set(ow_pin); // write one
    wait(120); // wait until end of time slot

  } else {

    wait(120); // wait until end of time slot

  }

  gpio_pue(ow_pin); // enable pull up
  gpio_inp(ow_pin); // release the bus
  wait(20); // recovery between slots

}

static uint8_t ow_read_byte() {

  int i;
  uint8_t mask = 1, data = 0;

  for(i = 0; i < 8; i++) {

    if(ow_read_bit()) data |= mask;
    mask <<= 1;

  }

  return data;

}

static void ow_write_byte(uint8_t value) {

  int i;
  uint8_t mask = 1;

  for(i = 0; i < 8; i++) {

    ow_write_bit(value & mask);
    mask <<= 1;

  }

}

// ------------- public API -------------

void ow_init(int port, int pin) {

  ow_port = port;
  ow_pin = pin;

  gpio_prt(ow_port); // set up the i/o port
  gpio_pue(ow_pin); // enable pull up
  gpio_inp(ow_pin); // release the bus

}

/*
  This is a slightly modified version of the
  search algortithm in Maxims APPLICATION NOTE 187
 */
int ow_search_next(uint8_t *rom_code, struct ow_search_state *search_state) {

  uint8_t crc8, id_bit_number, id_bit, cmp_id_bit;
  uint8_t last_zero_bit, rom_byte_number;
  uint8_t rom_byte_mask, search_direction;

  // initialize for search
  crc8 = 0;
  id_bit_number = 1;
  last_zero_bit = 0;
  rom_byte_number = 0;
  rom_byte_mask = 1;

  // if the last call was not the last one
  if (!search_state->is_last_device) {

    if (ow_reset() < 0) return -1;
    ow_write_byte(OW_SEARCH_ROM);

    do {

      // read a bit and its complement
      id_bit = ow_read_bit();
      cmp_id_bit = ow_read_bit();

      // check for no devices on 1-wire
      if ((id_bit == 1) && (cmp_id_bit == 1)) break;

      if (id_bit != cmp_id_bit) {

        // all devices coupled have 0 or 1
        // bit write value for search
        search_direction = id_bit;

      } else {

        // if this discrepancy is before the last discrepancy
        // on a previous next then pick the same as last time

        if (id_bit_number < search_state->last_discrepancy) {

          search_direction = ((rom_code[rom_byte_number] & rom_byte_mask) > 0);

        } else {

          // if equal to last pick 1, if not then pick 0
          search_direction = (id_bit_number == search_state->last_discrepancy);

        }

        // if 0 was picked then record its position in LastZero
        if (search_direction == 0) {

          last_zero_bit = id_bit_number;
          // check for last discrepancy in family
          if (last_zero_bit < 9)
            search_state->last_family_discrepancy = last_zero_bit;

        }

      }

      // set or clear the bit in the ROM byte
      if (search_direction == 1) rom_code[rom_byte_number] |= rom_byte_mask;
      else rom_code[rom_byte_number] &= ~rom_byte_mask;

      // serial number search direction write bit
      ow_write_bit(search_direction);

      // next bit
      id_bit_number ++;
      rom_byte_mask <<= 1;

      if (rom_byte_mask == 0) {

        // ROM byte complete, go to the next one
        crc8 = crc8_lookup[rom_code[rom_byte_number] ^ crc8];
        rom_byte_number ++;
        rom_byte_mask = 1;

      }

    } while(rom_byte_number < 8); // all ROM bytes 0-7

    if (id_bit_number > 64 && crc8 == 0) {

      // search was successful
      search_state->last_discrepancy = last_zero_bit;
      if (search_state->last_discrepancy == 0)
        search_state->is_last_device = 1;
      return 0;

    }

  }

  return -1;

}

int ow_search_all(uint8_t (*rom_codes)[OW_ROM_SIZE], int *num_devices) {

  int dev_index, search_result, i, k;
  struct ow_search_state search_state;

  search_state.last_discrepancy = 0;
  search_state.last_family_discrepancy = 0;
  search_state.is_last_device = 0;
  *num_devices = dev_index = 0;

  for (i = 0; i < OW_MAX_ATTEMPTS; i++) {

    for (k = 0; k < OW_MAX_DEVICES; k++) {

      search_result = ow_search_next(
          rom_codes[dev_index], &search_state);

      if (search_result < 0) {

        // search failed trying again
        search_state.last_discrepancy = 0;
        search_state.last_family_discrepancy = 0;
        search_state.is_last_device = 0;
        *num_devices = dev_index = 0;

        break;

      }

      dev_index++;

      if(search_state.is_last_device) {

        *num_devices = dev_index;
        return 0;

      }

    }

  }

  // OW_MAX_ATTEMPTS reached: too many errors!
  return -1;

}

/*
  If device_index is less than zero, we send a SKIP_ROM to
  reach all sensors with a CONVERT_T command.
 */
int ow_send_convert(uint8_t *rom_code) {

  int i;

  if (ow_reset() < 0) return -1;

  ow_write_byte(OW_MATCH_ROM);
  for (i = 0; i < OW_ROM_SIZE; i++) ow_write_byte(rom_code[i]);
  ow_write_byte(OW_CONVERT_T);

  for (i = 0; i < 750; i++) wait(1000); // 750ms for 12bit conversation time!
  // while(!ow_read_bit()); // conversion in progress...
  return 0;

}

int ow_send_convert_all() {

  int i;

  if (ow_reset() < 0) return -1;
  ow_write_byte(OW_SKIP_ROM);
  ow_write_byte(OW_CONVERT_T);

  for (i = 0; i < 750; i++) wait(1000); // 750ms for 12bit conversation time!
  // while(!ow_read_bit()); // conversion in progress...
  return 0;

}

int ow_read_scratch_pad(uint8_t *rom_code, uint8_t *scratch_pad) {

  int i, crc8;

  if (ow_reset() < 0) return -1;
  ow_write_byte(OW_MATCH_ROM);
  for (i = 0; i < OW_ROM_SIZE; i++) ow_write_byte(rom_code[i]);
  ow_write_byte(OW_READ_SCRATCHPAD);

  crc8 = 0;
  for(i = 0; i < 9; i++) {
    scratch_pad[i] = ow_read_byte(ow_pin);
    crc8 = crc8_lookup[scratch_pad[i] ^ crc8];
  }

  return (crc8 == 0) ? 0 : -1;

}

int ow_decode_temp(uint8_t *scratch_pad) {

  return ((int32_t)625 * (scratch_pad[1] << 8 | scratch_pad[0])) / 100;

}

int ow_decode_alm1(uint8_t *scratch_pad) {

  return scratch_pad[2];

}

int ow_decode_alm2(uint8_t *scratch_pad) {

  return scratch_pad[3];

}

int ow_decode_rsln(uint8_t *scratch_pad) {

  return 9 + ((scratch_pad[4] & 0x60) >> 5);

}
