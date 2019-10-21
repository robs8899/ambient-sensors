#define SSD1306_ADDR 0x3C

int ssd1306_init();
int ssd1306_clr();
int ssd1306_cle();
int ssd1306_nln();

int ssd1306_putc(char c);
int ssd1306_puts(const char *s);
void ssd1306_set(int x, int y, int font_id);
