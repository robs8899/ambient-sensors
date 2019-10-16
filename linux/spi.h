
int spi_init(char *dev);
int spi_done();
int spi_reg_read(uint8_t reg, uint8_t *data);
int spi_reg_write( uint8_t reg, uint8_t data);
int spi_burst_read(uint8_t reg, uint8_t *data, int len);
int spi_burst_write(uint8_t reg, uint8_t *data, int len);
