#define SI7021_ADDR  0x40

struct sht_sample {
  uint16_t rh;
  int16_t t;
};

int sht_read(struct sht_sample *s);
int sht_conv(struct sht_sample *s);
int sht_read_ureg(uint8_t *ur);
int sht_write_ureg(uint8_t *ur);
int sht_reset(void);
