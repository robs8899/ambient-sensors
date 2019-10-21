#define BMP180_ADDR  0x77

struct bmp_coeff {

  int16_t ac1;
  int16_t ac2; 
  int16_t ac3; 
  uint16_t ac4;
  uint16_t ac5;
  uint16_t ac6;
  int16_t b1; 
  int16_t b2;
  int16_t mb;
  int16_t mc;
  int16_t md;

};

struct bmp_raw {

  uint8_t oss; // oversampling: 1, 2 or 3
  int32_t ut; // uncompensated temperature
  uint32_t up; // uncompensated pressure

};

struct bmp_comp {

  int32_t ct; // compensated temperature, scaled with 10
  int32_t cp; // compensated pressure, scaled with 100

};

int bmp_read_coeff(struct bmp_coeff *coeff);
int bmp_read_raw(struct bmp_raw *raw);
int bmp_comp(const struct bmp_coeff *coeff,
    const struct bmp_raw *raw, struct bmp_comp *comp);
