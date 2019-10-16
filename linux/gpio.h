
#define GPIO_MODE_IN    0
#define GPIO_MODE_OUT   1

#define GPIO_LEVEL_LO   0
#define GPIO_LEVEL_HI   1

#define GPIO_EDGE_NONE    0
#define GPIO_EDGE_RISING  1
#define GPIO_EDGE_FALLING 2
#define GPIO_EDGE_BOTH    3

int gpio_mode(int pin, int mode);
int gpio_edge(int pin, int edge);
int gpio_set(int pin, int level);
int gpio_get(int pin);
int gpio_poll(int pin, int timeout);
