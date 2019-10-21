#include <stdint.h>
#include <avr/pgmspace.h>

#include "../core/core.h"
#include "ssd1306f.h"
#include "ssd1306.h"

// --------------------- private API ----------------------

#define DISPLAY_START_COL   0
#define DISPLAY_START_ROW   0
#define DISPLAY_END_COL     127
#define DISPLAY_END_ROW     7
#define DISPLAY_WIDTH       128
#define DISPLAY_HEIGHT      8

static int display_x = 0;
static int display_y = 0;
static int page_offset = 0;

static const font_info *display_font = DEFAULT_FONT;

static const uint8_t display_init[] PROGMEM = {

    0xAE,          // display off
    0x00 | 0x00,   // lower column start address, for page addressing mode only
    0x10 | 0x00,   // higher column start address, for page addressing mode only
    0xB0,          // page start address, for page addressing mode only
    0x40 | 0x00,   // display start line register from 0-63
    0x20, 0x00,    // set memory addressing mode - A[1:0] = 00b, horizontal addressing mode - SSD1306 only
    0xA1,          // segment re-map - A0/A1: column address 0/127 is mapped to SEG0
    0xA4,          // entire display ON (A4h/A5h), A4: enable display outputs according to the GDDRAM
    0xA6,          // normal/inverse display: normal: A6 / invert: A7
    0x81, 0x80,    // contrast control - the contrast value
    0xA8, 0x3F,    // Set Set Multiplex Ratio to N+1 MUX - N=A[5:0]: from 16MUX to 64MUX, RESET=111111b (i.e. 63d, 64MUX)
    0xD3, 0x00,    // Set Display Offset - 0x00
    0xD5, 0x80,    // set clock divide ratio - A[7:4]=1000b, A[3:0]=0000b - 0x80 is default after reset
    0xD9, 0x42,    // dis-charge / pre-charge period - A[3:0]: Phase 1 / A[7:4]: Phase 2 (2/2 after RESET)
    0xC8,          // COM Output Scan Direction: remapped mode. Scan from COM[N-1] to COM0
    0xDA, 0x12,    // COM Pins Hardware Configuration - A[4]=1 / A[5]=0 - Alt. COM pin configuration / Disable COM Left/Right re-map
    0xDB, 0x30,    // V COMH deselect level - A[6:4]=011b=30h ~0.83 x VCC
    0x8D, 0x14,    // enable charge pump - SSD1306 only
    0xAF,          // display ON

};

// ------------- private API -------------

static int ssd1306_cmd(const uint8_t *seq, int len) {

  int i;
  uint8_t cmd[2];

  cmd[0] = 0x00;
  i2c_addr(SSD1306_ADDR);
  for (i = 0; i < len; i++) {
    cmd[1] = seq[i];
    if (i2c_write(cmd, 2) < 0) return -1;
  }

  return 0;

}

/**
 * Set display start address in page mode:
 * 0x00 | lower nibble of the 8-bit column start address
 * 0x10 | higher nibble of the 8-bit column start address for the display data RAM
 * 0xB0 | page start address from 0 to 7 in GDDRAM
 */
static int ssd1306_addr(int x, int y, int o) {

  uint8_t b[3];
  x += 2;

  b[0] = 0x00 + (x & 0x0F);
  b[1] = 0x10 + ((x & 0xF0) >> 4);
  b[2] = 0xB0 + ((y + o) & 0x07);

  if (ssd1306_cmd(b, 3) < 0) return -1;
  return 0;

}

/**
 * Set display start line offset.
 * Specifies the mapping of the display start line
 * to one of COM0~COM63.
 */
static int ssd1306_offs(int o) {

  uint8_t b[2];

  b[0] = 0xD3;
  b[1] = o & 0x3F;

  if (ssd1306_cmd(b, 2) < 0) return -1;
  return 0;

}

static int ssd1306_data(const uint8_t *seq, int len) {

  i2c_addr(SSD1306_ADDR);
  if (i2c_write_reg(0x40, seq, len) < 0) return -1;
  return 0;

}

static int ssd1306_fill(const uint8_t data, int len) {

  i2c_addr(SSD1306_ADDR);
  if (i2c_write_pad(0x40, data, len) < 0) return -1;
  return 0;

}

// --------------------- public API ----------------------

int ssd1306_init() {

  int i;
  uint8_t cmd;

  for (i = 0; i < sizeof(display_init); i++) {
    memcpy_P(&cmd, display_init + i, 1);
    if (ssd1306_cmd(&cmd, 1) < 0) return -1;
  }

  return 0;

}

int ssd1306_clr() {

  int i;

  display_x = 0;
  display_y = 0;

  for (i = DISPLAY_START_ROW; i < DISPLAY_HEIGHT; i++) {
    if (ssd1306_addr(DISPLAY_START_COL, i, 0) < 0) return -1;
    if (ssd1306_fill(0x00, DISPLAY_WIDTH) < 0) return -1;
  }

  return 0;

}

int ssd1306_cle() {

  int i;

  if (display_x >= DISPLAY_END_COL) return -1;

  for (i = 0; i < display_font->height; i++) {
    if (ssd1306_addr(display_x, display_y+i, page_offset) < 0) return -1;
    if (ssd1306_fill(0x00, (DISPLAY_WIDTH - display_x)) < 0) return -1;
  }

  return 0;

}

int ssd1306_nln() {

  int i, new_offset, scroll_lines, scroll_offset;

  display_x = 0;

  if (display_y + display_font->height <= DISPLAY_END_ROW) {

    display_y += display_font->height;
    return 0;

  } else {

    new_offset = (page_offset + display_font->height) & 0x07;

    // clear the line at new offset
    for (i = 0; i < display_font->height; i++) {
      if (ssd1306_addr(display_x, display_y + i, new_offset) < 0) return -1;
      if (ssd1306_fill(0x00, (DISPLAY_WIDTH - display_x)) < 0) return -1;
    }

    // line by line smooth scrolling
    scroll_lines = display_font->height * 8;
    scroll_offset = page_offset * 8;

    for (i = 0; i < scroll_lines; i++) {
      if (ssd1306_offs(++scroll_offset & 0x3F) < 0) return -1;
      wait(5000);
    }

    page_offset = new_offset;
    return 0;

  }

}

int ssd1306_putc(char c) {

  int i;
  font_entry e; // the entry in the font table
  uint8_t d[0x20]; // up to 32 bytes for the font bitmap!

  if (c < display_font->first || c > display_font->last) c = '?';

  memcpy_P(&e, display_font->table + (c - display_font->first), sizeof(e));
  memcpy_P(d, display_font->bitmap + e.offset, display_font->height * e.width);
  if (display_x + e.width >= DISPLAY_END_COL) ssd1306_nln();

  for (i = 0; i < display_font->height; i++) {
    if (ssd1306_addr(display_x, display_y+i, page_offset) < 0) return -1;
    if (ssd1306_data(d + e.width*i, e.width) < 0) return -1;
  }

  display_x += e.width;

  for (i = 0; i < display_font->spacing; i++) d[i] = 0x00;
  if (display_x + display_font->spacing >= DISPLAY_END_COL) ssd1306_nln();

  for (i = 0; i < display_font->height; i++) {
    if (ssd1306_addr(display_x, display_y+i, page_offset) < 0) return -1;
    if (ssd1306_data(d, display_font->spacing) < 0) return -1;
  }

  display_x += display_font->spacing;

  return 0;

}

int ssd1306_puts(const char *s) {

  int i = 64; // max.
  while (*s !=0 && i--)
    if (ssd1306_putc(*s++) < 0) return -1;
  return 0;

}

void ssd1306_set(int x, int y, int font_id) {

  display_x = x;
  display_y = y;

  switch (font_id) {

#if USE_SMALL_FONT
  case SMALL_FONT_ID:
    display_font = &small_font_info;
    return;
#endif
#if USE_BOLD_FONT
  case BOLD_FONT_ID:
    display_font = &bold_font_info;
    return;
#endif
#if USE_BIG_FONT
  case BIG_FONT_ID:
    display_font = &big_font_info;
    return;
#endif
  default:
    display_font = DEFAULT_FONT;
    return;

  }

}
