extern volatile uint8_t *gpio_pin_reg;
extern volatile uint8_t *gpio_ddr_reg;
extern volatile uint8_t *gpio_prt_reg;

/** set up the current GPIO port, on AVR: 0=B, 1=C, 2=D **/
void gpio_port(int num);

#define gpio_prt(prt) (gpio_port(prt))
#define gpio_inp(pin) (*gpio_ddr_reg &= ~(1<<pin))
#define gpio_out(pin) (*gpio_ddr_reg |=  (1<<pin))
#define gpio_pud(pin) (*gpio_prt_reg &= ~(1<<pin))
#define gpio_pue(pin) (*gpio_prt_reg |=  (1<<pin))
#define gpio_clr(pin) (*gpio_prt_reg &= ~(1<<pin))
#define gpio_set(pin) (*gpio_prt_reg |=  (1<<pin))
#define gpio_lvl(pin) (*gpio_pin_reg & (1<<pin))

void i2c_init();
void i2c_done();
void i2c_addr(uint8_t addr);
int i2c_read(uint8_t *buf, int len);
int i2c_write(const uint8_t *buf, int len);
int i2c_read_reg(uint8_t reg, uint8_t *buf, int len);
int i2c_write_reg(uint8_t reg, const uint8_t *buf, int len);
int i2c_write_pad(uint8_t cmd, uint8_t data, int len);
//int i2c_read_addr(uint16_t addr, uint8_t *buf, int len);
//int i2c_write_addr(uint16_t addr, const uint8_t *buf, int len);
//int i2c_poll_ack();

void spi_init();
void spi_done();
void spi_cs(int cs);
void spi_ds(int ds);
uint8_t spi_trx(uint8_t data);
void spi_rxn(uint8_t *buf, uint16_t cnt);
void spi_txn(const uint8_t *buf, uint16_t cnt);
int spi_rdy();

void uart_init();
void uart_done();
char uart_getc();
void uart_putc(char c);
void uart_gets(char *cb);
void uart_puts(char *cb);

void adc_mux(int pin);
uint16_t adc_get();

#define WDP_8000MS       (1<<WDP3) | (1<<WDP0)
#define WDP_4000MS       (1<<WDP3)
#define WDP_2000MS       (1<<WDP2) | (1<<WDP1) | (1<<WDP0)
#define WDP_1000MS       (1<<WDP2) | (1<<WDP1)
#define WDP_0500MS       (1<<WDP2) | (1<<WDP0)
#define WDP_0250MS       (1<<WDP2)
#define WDP_0125MS       (1<<WDP1) | (1<<WDP0)
#define WDP_0064MS       (1<<WDP1)
#define WDP_0032MS       (1<<WDP0)
#define WDP_0016MS       0

void sleep(uint16_t wdp);
inline void wait(uint16_t micros) __attribute__((always_inline));
inline void wait(uint16_t micros) {
  uint16_t cnt = (micros * (F_CPU / 800000UL)) / 6;
  do __asm__ volatile ("nop"); while (--cnt);
}

void timer_init();
uint32_t timer_micros();
